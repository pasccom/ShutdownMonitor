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

#include "qoutput.h"
#include "qscreenresources.h"

bool QOutput::enable(bool grab)
{
    return mParent->enableOutput(this, grab);
}

bool QOutput::disable(bool grab)
{
    return mParent->disableOutput(this, grab);
}

bool QOutput::toggle(bool grab)
{
    if (mEnabled)
        return mParent->disableOutput(this, grab);
    else
        return mParent->enableOutput(this, grab);

}
