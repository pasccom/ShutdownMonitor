#include "qrroutput.h"
#include "qrrcrtc.h"
#include "qrrscreenresources.h"

#include <QtDebug>

#include <X11/extensions/Xrandr.h>

QRROutput::QRROutput(QRRScreenResources *parent, XRROutputInfo *info)
    : mParent(parent)
{
    physicalWidth = info != nullptr ? info->mm_width : 0;
    physicalHeight = info != nullptr ? info->mm_height : 0;
    name = info != nullptr ? QString::fromLocal8Bit(QByteArray(info->name, info->nameLen)) : QString();
    connection = info != nullptr ? info->connection : RR_UnknownConnection;
    mCrtcId = info != nullptr ? info->crtc : None;

    mEnabled = info != nullptr ? mParent->crtc(info->crtc) != nullptr : false;
}

QRRCrtc* QRROutput::crtc(void) const
{
    return mCrtcId != None ? mParent->crtc(mCrtcId): nullptr;
}

QString QRROutput::display(void) const
{
    if (name.isNull() || (crtc() == nullptr))
        return name;

    return QString("%1 (%2)").arg(name)
                             .arg(crtc()->display());
}

bool QRROutput::enable(void)
{
    return mParent->enableOutput(this);
}

bool QRROutput::disable(void)
{
    return mParent->disableOutput(this);
}

bool QRROutput::toggle(void)
{
    if (mEnabled)
        return mParent->disableOutput(this);
    else
        return mParent->enableOutput(this);

}
