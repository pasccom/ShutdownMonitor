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
#include "qoutput.h"

QMap< QString, std::function<QScreenResources*(const QString&)> > QScreenResources::availableBackends;

QStringList QScreenResources::listBackends(void)
{
    QStringList ret;

    if (availableBackends.isEmpty())
        initBackends();

    foreach (QString name, availableBackends.keys())
        ret.append(name);
    return ret;
}

QScreenResources* QScreenResources::create(const QString& backend)
{
    if (availableBackends.isEmpty())
        initBackends();

    foreach (QString name, availableBackends.keys()) {
        auto factory = availableBackends.value(name);
        QScreenResources* ans = factory(backend);
        if (ans != nullptr)
            return ans;
    }
    return nullptr;
}

QScreenResources::~QScreenResources(void)
{
    qDeleteAll(mOutputs);
}

QOutput* QScreenResources::output(QOutputId outputId) const
{
    return mOutputs.value(outputId, nullptr);
}

QOutput* QScreenResources::output(const QString& name) const
{
    foreach (QOutputId outputId, outputs()) {
        QOutput* output = mOutputs.value(outputId, nullptr);
        if (output == nullptr)
            continue;
        if (output->connection != QOutput::Connection::Connected)
            continue;
        if (QString::compare(output->name, name, Qt::CaseSensitive) == 0)
            return output;
    }

    return nullptr;
}

QList<QOutputId> QScreenResources::outputs(bool refresh)
{
    if (mOutputs.isEmpty() || refresh)
        refreshOutputs();

    return mOutputs.keys();
}
