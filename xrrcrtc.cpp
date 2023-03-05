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

#include "xrrcrtc.h"

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
