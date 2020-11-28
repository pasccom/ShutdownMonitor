#include "qrrscreenresources.h"
#include "qrroutput.h"
#include "qrrcrtc.h"

#include <QtDebug>

#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>

QRRScreenResources::QRRScreenResources(Display *display, XRRScreenResources *resources)
    : mDisplay(display), mResources(resources)
{}

QRRScreenResources::~QRRScreenResources(void)
{
    qDeleteAll(mOutputs);
    qDeleteAll(mCrtcs);

    if (mResources != nullptr)
        XRRFreeScreenResources(mResources);
}

QRRScreenResources* QRRScreenResources::get(Display* display)
{
    Window root = DefaultRootWindow(display);
    return new QRRScreenResources(display, XRRGetScreenResources(display, root));
}

QRRScreenResources* QRRScreenResources::getCurrent(Display* display)
{
    Window root = DefaultRootWindow(display);
    return new QRRScreenResources(display, XRRGetScreenResourcesCurrent(display, root));
}

QRROutput* QRRScreenResources::output(RROutput outputId)
{
    foreach (QRROutput* output, mOutputs)
        if (output->mId == outputId)
            return output;

    return nullptr;
}

QList<QRROutput*> QRRScreenResources::outputs(bool refresh)
{
    if (mOutputs.isEmpty() || refresh)
        refreshOutputs();

    return mOutputs;
}

void QRRScreenResources::refreshOutputs(void)
{
    qDeleteAll(mOutputs);

    for (int o = 0; o < mResources->noutput; o++)
        mOutputs.append(new QRROutput(this, mResources->outputs[o], XRRGetOutputInfo(mDisplay, mResources, mResources->outputs[o])));
}

QRRCrtc* QRRScreenResources::crtc(RRCrtc crtcId)
{
    if (crtcId == None)
        return nullptr;
    if (!mCrtcs.contains(crtcId))
        mCrtcs.insert(crtcId, new QRRCrtc(this, XRRGetCrtcInfo(mDisplay, mResources, crtcId)));
    return mCrtcs.value(crtcId);
}

bool QRRScreenResources::enableOutput(QRROutput* out)
{
    // The output is already enabled:
    if (out->mEnabled)
        return true;
    // The output CRTC should be in the CRTC map:
    if (!mCrtcs.contains(out->mCrtcId))
        return false;

    // Update the CRTCs:
    bool ans = true;
    out->mEnabled = true;
    QRect totalScreen = computeTotalScreen();
    QRect newScreen = computeScreen();
    XGrabServer(mDisplay);
    for (auto it = mCrtcs.constBegin(); it != mCrtcs.constEnd(); it++)
        ans &= updateCrtcOrigin(it.key(), it.value()->rect().topLeft() - newScreen.topLeft() + totalScreen.topLeft());
    XUngrabServer(mDisplay);
    return ans;
}

bool QRRScreenResources::disableOutput(QRROutput* out)
{
    // The output is already disabled:
    if (!out->mEnabled)
        return true;
    // The output CRTC should be in the CRTC map:
    if (!mCrtcs.contains(out->mCrtcId))
        return false;

    // Update the CRTCs:
    bool ans = true;
    out->mEnabled = false;
    QRect totalScreen = computeTotalScreen();
    QRect newScreen = computeScreen();
    XGrabServer(mDisplay);
    for (auto it = mCrtcs.constBegin(); it != mCrtcs.constEnd(); it++)
        ans &= updateCrtcOrigin(it.key(), it.value()->rect().topLeft() - newScreen.topLeft() + totalScreen.topLeft());
    XUngrabServer(mDisplay);
    return ans;
}

QRect QRRScreenResources::computeTotalScreen(void) const
{
    QRect screen;
    foreach (QRROutput* o, mOutputs) {
        if (!mCrtcs.contains(o->mCrtcId))
            continue;
        screen |= mCrtcs.value(o->mCrtcId)->rect();
    }
    return screen;
}

QRect QRRScreenResources::computeScreen(void) const
{
    QRect screen;
    foreach (QRROutput* o, mOutputs) {
        if (!mCrtcs.contains(o->mCrtcId))
            continue;
        if (o->mEnabled)
            screen |= mCrtcs.value(o->mCrtcId)->rect();
    }
    return screen;
}

bool QRRScreenResources::updateCrtcOrigin(RRCrtc crtcId, const QPoint& newOrigin)
{
    // Get the CRTC internal representation:
    QRRCrtc* crtc = mCrtcs.value(crtcId);

    // Get the associated enabled outputs:
    QList<RROutput> crtcOutputs = crtc->outputs;
    for (auto it = crtcOutputs.begin(); it != crtcOutputs.end();) {
        QRROutput* o = output(*it);
        if ((o == nullptr) || !o->mEnabled)
            it = crtcOutputs.erase(it);
        else
            it++;
    }

    // Update the CRTC origin or disable it if there is not any associated enabled output:
    Status s;
    QVector<RROutput> outputs = QVector<RROutput>::fromList(crtcOutputs);
    if (outputs.isEmpty())
        s = XRRSetCrtcConfig(mDisplay, mResources, crtcId, CurrentTime,
                             0, 0, None, RR_Rotate_0, NULL, 0);
    else
        s = XRRSetCrtcConfig(mDisplay, mResources, crtcId, CurrentTime,
                             newOrigin.x(), newOrigin.y(), crtc->mode, crtc->rotation,
                             outputs.data(), outputs.size());

    return (s == RRSetConfigSuccess);
}
