#include "qrrscreenressources.h"
#include "qrroutput.h"
#include "qrrcrtc.h"

#include <QtDebug>

#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>

QRRScreenRessources::QRRScreenRessources(Display *display, XRRScreenResources *ressources)
    : mDisplay(display), mRessources(ressources)
{}

QRRScreenRessources::~QRRScreenRessources(void)
{
    qDeleteAll(mOutputs);
    qDeleteAll(mCrtcs);

    if (mRessources != nullptr)
        XRRFreeScreenResources(mRessources);
}

QRRScreenRessources* QRRScreenRessources::get(Display* display)
{
    Window root = DefaultRootWindow(display);
    return new QRRScreenRessources(display, XRRGetScreenResources(display, root));
}

QRRScreenRessources* QRRScreenRessources::getCurrent(Display* display)
{
    Window root = DefaultRootWindow(display);
    return new QRRScreenRessources(display, XRRGetScreenResourcesCurrent(display, root));
}

QRROutput* QRRScreenRessources::output(RROutput outputId)
{
    foreach (QRROutput* output, mOutputs)
        if (output->mId == outputId)
            return output;

    return nullptr;
}

QList<QRROutput*> QRRScreenRessources::outputs(bool refresh)
{
    if (mOutputs.isEmpty() || refresh)
        refreshOutputs();

    return mOutputs;
}

void QRRScreenRessources::refreshOutputs(void)
{
    qDeleteAll(mOutputs);

    for (int o = 0; o < mRessources->noutput; o++)
        mOutputs.append(new QRROutput(this, mRessources->outputs[o], XRRGetOutputInfo(mDisplay, mRessources, mRessources->outputs[o])));
}

QRRCrtc* QRRScreenRessources::crtc(RRCrtc crtcId)
{
    /*for (int c = 0; c < mRessources->ncrtc; c++) {
        XRRCrtcInfo* crtcInfo = XRRGetCrtcInfo(mDisplay, mRessources, mRessources->crtcs[c]);
        qDebug() << c << mRessources->crtcs[c] << crtcInfo->x << crtcInfo->y << crtcInfo->width << crtcInfo->height;
        XRRFreeCrtcInfo(crtcInfo);
    }*/

    if (crtcId == None)
        return nullptr;
    if (!mCrtcs.contains(crtcId))
        mCrtcs.insert(crtcId, new QRRCrtc(this, XRRGetCrtcInfo(mDisplay, mRessources, crtcId)));
    return mCrtcs.value(crtcId);
}

bool QRRScreenRessources::enableOutput(QRROutput* out)
{
    if (out->mEnabled)
        return true;
    if (!mCrtcs.contains(out->mInfo->crtc))
        return false;

    QRect totalScreen;
    QRect newScreen;
    foreach (QRROutput* o, mOutputs) {
        if (!mCrtcs.contains(o->mInfo->crtc))
            continue;
        totalScreen |= mCrtcs.value(o->mInfo->crtc)->rect();
        if ((o == out) || (o->mEnabled))
            newScreen |= mCrtcs.value(o->mInfo->crtc)->rect();
    }

    bool ans = true;
    out->mEnabled = true;
    XGrabServer(mDisplay);
    for (auto it = mCrtcs.constBegin(); it != mCrtcs.constEnd(); it++)
        ans &= actualizeCrtcOrigin(it.key(), it.value()->rect().topLeft() - newScreen.topLeft() + totalScreen.topLeft());
    XUngrabServer(mDisplay);
    return ans;
}

bool QRRScreenRessources::disableOutput(QRROutput* out)
{
    if (!out->mEnabled)
        return true;
    if (!mCrtcs.contains(out->mInfo->crtc))
        return false;

    QRect totalScreen;
    QRect newScreen;
    foreach (QRROutput* o, mOutputs) {
        if (!mCrtcs.contains(o->mInfo->crtc))
            continue;
        totalScreen |= mCrtcs.value(o->mInfo->crtc)->rect();
        if ((o != out) && (o->mEnabled))
            newScreen |= mCrtcs.value(o->mInfo->crtc)->rect();
    }

    bool ans = true;
    out->mEnabled = false;
    XGrabServer(mDisplay);
    for (auto it = mCrtcs.constBegin(); it != mCrtcs.constEnd(); it++)
        ans &= actualizeCrtcOrigin(it.key(), it.value()->rect().topLeft() - newScreen.topLeft() + totalScreen.topLeft());
    XUngrabServer(mDisplay);
    return ans;
}

bool QRRScreenRessources::actualizeCrtcOrigin(RRCrtc crtcId, const QPoint& newOrigin)
{
    QRRCrtc* crtc = mCrtcs.value(crtcId);
    QList<RROutput> crtcOutputs = crtc->outputs();

    for (auto it = crtcOutputs.begin(); it != crtcOutputs.end();) {
        QRROutput* o = output(*it);
        if ((o == nullptr) || !o->mEnabled)
            it = crtcOutputs.erase(it);
        else
            it++;
    }

    Status s;
    QVector<RROutput> outputs = QVector<RROutput>::fromList(crtcOutputs);
    if (outputs.isEmpty())
        s = XRRSetCrtcConfig(mDisplay, mRessources, crtcId, CurrentTime,
                             0, 0, None, RR_Rotate_0, NULL, 0);
    else
        s = XRRSetCrtcConfig(mDisplay, mRessources, crtcId, CurrentTime,
                             newOrigin.x(), newOrigin.y(), crtc->mode, crtc->rotation,
                             outputs.data(), outputs.size());

    if (s != RRSetConfigSuccess)
        return false;

    return true;
}
