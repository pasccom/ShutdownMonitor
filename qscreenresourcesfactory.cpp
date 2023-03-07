/* Copyright 2023 Pascal COMBES <pascom@orange.fr>
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

#include "qscreenresources.h"

#ifdef SHUTDOWN_MONITOR_KSCREEN
#   include "kscreenresources.h"
#endif // SHUTDOWN_MONITOR_KSCREEN

#ifdef SHUTDOWN_MONITOR_X11
#   include "xrrscreenresources.h"
#endif // SHUTDOWN_MONITOR_X11

#include <QtDebug>

void QScreenResources::initBackends(void)
{
#ifdef SHUTDOWN_MONITOR_KSCREEN
    availableBackends.insert(KScreenResources::name, &KScreenResources::create);
#endif // SHUTDOWN_MONITOR_KSCREEN

#ifdef SHUTDOWN_MONITOR_X11
    availableBackends.insert(XRandRScreenResources::name, &XRandRScreenResources::create);
#endif // SHUTDOWN_MONITOR_X11
}
