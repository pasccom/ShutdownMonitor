/* Copyright 2020 Pascal COMBES <pascom@orange.fr>
 *
 * This file is part of ShutdownMonitor.
 *
 * ShutdownMonitor is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ShutdownMonitor is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ShutdownMonitor. If not, see <http://www.gnu.org/licenses/>
 */

#include "xrrscreenresources.h"
#include "xrroutput.h"
#include "xrrcrtc.h"

#include <QtDebug>

#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>

XRandRScreenResources::XRandRScreenResources(Display *display, XRRScreenResources *resources)
    : mDisplay(display), mResources(resources)
{}

XRandRScreenResources::~XRandRScreenResources(void)
{
    qDeleteAll(mOutputs);
    qDeleteAll(mCrtcs);

    if (mResources != nullptr)
        XRRFreeScreenResources(mResources);
}

XRandRScreenResources* XRandRScreenResources::get(Display* display)
{
    Window root = DefaultRootWindow(display);
    return new XRandRScreenResources(display, XRRGetScreenResources(display, root));
}

XRandRScreenResources* XRandRScreenResources::getCurrent(Display* display)
{
    Window root = DefaultRootWindow(display);
    return new XRandRScreenResources(display, XRRGetScreenResourcesCurrent(display, root));
}

XRandROutput* XRandRScreenResources::output(RROutput outputId) const
{
    return mOutputs.value(outputId, nullptr);
}

XRandROutput* XRandRScreenResources::output(const QString& name) const
{
    foreach (RROutput outputId, outputs()) {
        XRandROutput* output = mOutputs.value(outputId, nullptr);
        if (output == nullptr)
            continue;
        if (output->connection != RR_Connected)
            continue;
        if (QString::compare(output->name, name, Qt::CaseSensitive) == 0)
            return output;
    }

    return nullptr;
}

QList<RROutput> XRandRScreenResources::outputs(bool refresh)
{
    if (mOutputs.isEmpty() || refresh)
        refreshOutputs();

    return mOutputs.keys();
}

void XRandRScreenResources::refreshOutputs(void)
{
    qDeleteAll(mOutputs);

    for (int o = 0; o < mResources->noutput; o++) {
        XRROutputInfo* info = XRRGetOutputInfo(mDisplay, mResources, mResources->outputs[o]);
        mOutputs.insert(mResources->outputs[o], new XRandROutput(this, info));
        XRRFreeOutputInfo(info);
    }
}

XRandRCrtc* XRandRScreenResources::crtc(RRCrtc crtcId)
{
    if (crtcId == None)
        return nullptr;
    if (!mCrtcs.contains(crtcId)) {
        XRRCrtcInfo* info = XRRGetCrtcInfo(mDisplay, mResources, crtcId);
        mCrtcs.insert(crtcId, new XRandRCrtc(this, info));
        XRRFreeCrtcInfo(info);
    }
    return mCrtcs.value(crtcId);
}

bool XRandRScreenResources::enableOutput(XRandROutput* output, bool grab)
{
    // The output is already enabled:
    if (output->mEnabled)
        return true;
    // The output CRTC should be in the CRTC map:
    if (!mCrtcs.contains(output->mCrtcId))
        return false;

    // Update the CRTCs:
    bool ans = true;
    bool oldOutputState = output->mEnabled;
    output->mEnabled = true;
    QRect totalScreen = computeTotalScreen();
    QRect newScreen = computeScreen();
    if (grab)
        XGrabServer(mDisplay);
    for (auto it = mCrtcs.constBegin(); it != mCrtcs.constEnd(); it++)
        ans &= updateCrtcOrigin(it.key(), it.value()->rect().topLeft() - newScreen.topLeft() + totalScreen.topLeft());
    if (grab)
        XUngrabServer(mDisplay);
    if (!ans)
        output->mEnabled = oldOutputState;
    return ans;
}

bool XRandRScreenResources::disableOutput(XRandROutput* output, bool grab)
{
    // The output is already disabled:
    if (!output->mEnabled)
        return true;
    // The output CRTC should be in the CRTC map:
    if (!mCrtcs.contains(output->mCrtcId))
        return false;

    // Update the CRTCs:
    bool ans = true;
    bool oldOutputState = output->mEnabled;
    output->mEnabled = false;
    QRect totalScreen = computeTotalScreen();
    QRect newScreen = computeScreen();
    if (newScreen.isNull()) {
        output->mEnabled = true;
        return false;
    }
    if (grab)
        XGrabServer(mDisplay);
    for (auto it = mCrtcs.constBegin(); it != mCrtcs.constEnd(); it++)
        ans &= updateCrtcOrigin(it.key(), it.value()->rect().topLeft() - newScreen.topLeft() + totalScreen.topLeft());
    if (grab)
        XUngrabServer(mDisplay);
    if (!ans)
        output->mEnabled = oldOutputState;
    return ans;
}

QRect XRandRScreenResources::computeTotalScreen(void) const
{
    QRect screen;
    foreach (XRandROutput* o, mOutputs) {
        if (!mCrtcs.contains(o->mCrtcId))
            continue;
        screen |= mCrtcs.value(o->mCrtcId)->rect();
    }
    return screen;
}

QRect XRandRScreenResources::computeScreen(void) const
{
    QRect screen;
    foreach (XRandROutput* o, mOutputs) {
        if (!mCrtcs.contains(o->mCrtcId))
            continue;
        if (o->mEnabled)
            screen |= mCrtcs.value(o->mCrtcId)->rect();
    }
    return screen;
}

bool XRandRScreenResources::updateCrtcOrigin(RRCrtc crtcId, const QPoint& newOrigin)
{
    // Get the CRTC internal representation:
    XRandRCrtc* crtc = mCrtcs.value(crtcId);

    // Get the associated enabled outputs:
    QList<RROutput> crtcOutputs = crtc->outputs;
    for (auto it = crtcOutputs.begin(); it != crtcOutputs.end();) {
        XRandROutput* o = output(*it);
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
