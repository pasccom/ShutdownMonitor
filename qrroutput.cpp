#include "qrroutput.h"
#include "qrrscreenresources.h"

#include <QtDebug>

#include <X11/extensions/Xrandr.h>

QRROutput::QRROutput(QRRScreenResources *parent, RROutput id, XRROutputInfo *info)
    : mParent(parent), mId(id), mInfo(info)
{
    physicalWidth = mInfo != nullptr ? mInfo->mm_width : 0;
    physicalHeight = mInfo != nullptr ? mInfo->mm_height : 0;
    name = mInfo != nullptr ? QString::fromLocal8Bit(QByteArray(mInfo->name, mInfo->nameLen)) : QString();
    connection = mInfo != nullptr ? mInfo->connection : RR_UnknownConnection;

    mEnabled = mInfo != nullptr ? mParent->crtc(mInfo->crtc) != nullptr : false;
}

QRROutput::~QRROutput(void)
{
    if (mInfo != nullptr)
        XRRFreeOutputInfo(mInfo);
}

QRRCrtc* QRROutput::crtc(void) const
{
    return mInfo != nullptr ? mParent->crtc(mInfo->crtc): nullptr;
}

QString QRROutput::display(void) const
{
    if (name.isNull())
        return name;

    return QString("%1 (%2mm x %3mm)").arg(name)
                                      .arg(physicalWidth)
                                      .arg(physicalHeight);
}

bool QRROutput::enable(void)
{
    return mParent->enableOutput(this);
}

bool QRROutput::disable(void)
{
    return mParent->disableOutput(this);
}

