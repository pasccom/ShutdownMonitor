#include "qrrcrtc.h"

#include <QtDebug>

#include "X11/extensions/Xrandr.h"

QRRCrtc::QRRCrtc(QRRScreenResources *parent, XRRCrtcInfo *info)
    : mParent(parent), mInfo(info)
{
    x = mInfo != nullptr ? mInfo->x : 0;
    y = mInfo != nullptr ? mInfo->y : 0;
    width = mInfo != nullptr ? mInfo->width : 0;
    height = mInfo != nullptr ? mInfo->height : 0;
    mode = mInfo != nullptr ? mInfo->mode : None;
    rotation = mInfo != nullptr ? mInfo->rotation : RR_Rotate_0;
}

QRRCrtc::~QRRCrtc(void)
{
    if (mInfo != nullptr)
        XRRFreeCrtcInfo(mInfo);
}

QList<RROutput> QRRCrtc::outputs() const
{
    QList<RROutput> outputs;

    for (int o = 0; o < mInfo->noutput; o++)
        outputs.append(mInfo->outputs[o]);

    return outputs;
}

QString QRRCrtc::display(void) const
{
    return QString("%1x%2+%3+%4").arg(width)
                                 .arg(height)
                                 .arg(x)
                                 .arg(y);
}
