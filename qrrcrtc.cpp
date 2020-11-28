#include "qrrcrtc.h"

#include <QtDebug>

#include "X11/extensions/Xrandr.h"

QRRCrtc::QRRCrtc(QRRScreenResources *parent, XRRCrtcInfo *info)
    : mParent(parent)
{
    x = info != nullptr ? info->x : 0;
    y = info != nullptr ? info->y : 0;
    width = info != nullptr ? info->width : 0;
    height = info != nullptr ? info->height : 0;
    mode = info != nullptr ? info->mode : None;
    rotation = info != nullptr ? info->rotation : RR_Rotate_0;

    outputs.clear();
    for (int o = 0; o < info->noutput; o++)
        outputs.append(info->outputs[o]);
}

QString QRRCrtc::display(void) const
{
    return QString("%1x%2+%3+%4").arg(width)
                                 .arg(height)
                                 .arg(x)
                                 .arg(y);
}
