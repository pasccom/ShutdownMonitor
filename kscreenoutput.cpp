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

#include "kscreenoutput.h"
#include "kscreenresources.h"

#include <QtDebug>

KScreenOutput::KScreenOutput(KScreenResources* parent, const KScreen::OutputPtr& output)
    : QOutput(parent), mOutput(output)
{
    physicalWidth = !output.isNull() ? output->sizeMm().width() : 0;
    physicalHeight = !output.isNull() ? output->sizeMm().height() : 0;
    name = !output.isNull() ? output->name() : QString();
    if (output.isNull())
        connection = QOutput::Connection::Unknown;
    else if (output->isConnected())
        connection = QOutput::Connection::Connected;
    else
        connection = QOutput::Connection::Disconnected;

    mRect = !output.isNull() ? QRect(mOutput->pos(), mOutput->size()) : QRect();
    mPriority = !output.isNull() ? output->priority() : 0;

    mEnabled = !output.isNull() ? output->isEnabled() : false;
}

void KScreenOutput::update(const KScreen::OutputPtr& output)
{
    if (!output.isNull() && output->isEnabled()) {
        if (output->isConnected())
            connection = QOutput::Connection::Connected;
        else
            connection = QOutput::Connection::Disconnected;
    }
}

QString KScreenOutput::display(void) const
{
    if (mOutput.isNull())
        return QString();

    return QString("%1 (%2x%3+%4+%5)").arg(mOutput->name())
                                      .arg(mOutput->size().width())
                                      .arg(mOutput->size().height())
                                      .arg(mOutput->pos().x())
                                      .arg(mOutput->pos().y());
}
