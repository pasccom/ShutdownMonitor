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

#include "xrroutput.h"
#include "xrrcrtc.h"
#include "xrrscreenresources.h"

#include <QtDebug>

#include <X11/extensions/Xrandr.h>

XRandROutput::XRandROutput(XRandRScreenResources *parent, XRROutputInfo *info)
    : QOutput(parent)
{
    physicalWidth = info != nullptr ? info->mm_width : 0;
    physicalHeight = info != nullptr ? info->mm_height : 0;
    name = info != nullptr ? QString::fromLocal8Bit(QByteArray(info->name, info->nameLen)) : QString();
    if (info == nullptr)
        connection = QOutput::Connection::Unknown;
    else if (info->connection == RR_Disconnected)
        connection = QOutput::Connection::Disconnected;
    else if (info->connection == RR_Connected)
        connection = QOutput::Connection::Connected;
    else
        connection = QOutput::Connection::Unknown;
    mCrtcId = info != nullptr ? info->crtc : None;

    mEnabled = info != nullptr ? parent->crtc(info->crtc) != nullptr : false;
}

XRandRCrtc* XRandROutput::crtc(void) const
{
    XRandRScreenResources* parent = dynamic_cast<XRandRScreenResources*>(mParent);
    if (parent == nullptr)
        return nullptr;

    return mCrtcId != None ? parent->crtc(mCrtcId): nullptr;
}

QString XRandROutput::display(void) const
{
    if (name.isNull() || (crtc() == nullptr))
        return name;

    return QString("%1 (%2)").arg(name)
                             .arg(crtc()->display());
}

bool XRandROutput::enable(bool grab)
{
    return mParent->enableOutput(this, grab);
}

bool XRandROutput::disable(bool grab)
{
    return mParent->disableOutput(this, grab);
}

bool XRandROutput::toggle(bool grab)
{
    if (mEnabled)
        return mParent->disableOutput(this, grab);
    else
        return mParent->enableOutput(this, grab);

}
