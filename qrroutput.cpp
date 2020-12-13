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

bool QRROutput::enable(bool grab)
{
    return mParent->enableOutput(this, grab);
}

bool QRROutput::disable(bool grab)
{
    return mParent->disableOutput(this, grab);
}

bool QRROutput::toggle(bool grab)
{
    if (mEnabled)
        return mParent->disableOutput(this, grab);
    else
        return mParent->enableOutput(this, grab);

}
