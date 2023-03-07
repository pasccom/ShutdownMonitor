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

#include "kscreenresources.h"
#include "kscreenoutput.h"

#include <QtDebug>

#include <KScreen/GetConfigOperation>
#include <KScreen/SetConfigOperation>

QString KScreenResources::name = "KScreen";

QScreenResources* KScreenResources::create(const QString& backend)
{
    Q_UNUSED(backend);

    return KScreenResources::getCurrent();
}

KScreenResources *KScreenResources::getCurrent()
{
    KScreen::GetConfigOperation opCurrent(KScreen::ConfigOperation::NoOptions);
    if (!opCurrent.exec()) {
        qWarning() << "Could not retrieve current config:" << opCurrent.errorString();
        return nullptr;
    } else {
        return new KScreenResources(opCurrent.config());
    }
}

KScreenResources::KScreenResources(const KScreen::ConfigPtr& config)
    : QScreenResources(KScreenResources::name), mConfig(config)
{}

void KScreenResources::refreshOutputs(void)
{
    qDeleteAll(mOutputs);

    for (KScreen::OutputPtr output : mConfig->outputs())
        mOutputs.insert(output->id(), new KScreenOutput(this, output));
}

bool KScreenResources::enableOutput(QOutput* output, bool grab)
{
    Q_UNUSED(grab);

    // Cast output to internal type:
    KScreenOutput* kOutput = dynamic_cast<KScreenOutput*>(output);
    if (kOutput == nullptr)
        return false;

    // The output is already enabled:
    if (kOutput->mEnabled)
        return true;

    // Compute output offsets and priority shift:
    bool oldOutputState = kOutput->mEnabled;
    kOutput->mEnabled = true;
    QRect totalScreen = computeTotalScreen();
    QRect newScreen = computeScreen();
    uint32_t totalMinPriority = computeTotalPriority();
    uint32_t newMinPriority = computePriority();
    Q_ASSERT(newMinPriority >= totalMinPriority);

    // Update KScreen configuration:
    bool ans = updateConfig(newScreen.topLeft() - totalScreen.topLeft(), newMinPriority - totalMinPriority);
    if (!ans)
        kOutput->mEnabled = oldOutputState;
    return ans;
}

bool KScreenResources::disableOutput(QOutput* output, bool grab)
{
    Q_UNUSED(grab);

    KScreenOutput* kOutput = dynamic_cast<KScreenOutput*>(output);
    if (kOutput == nullptr)
        return false;

    // The output is already disabled:
    if (!kOutput->mEnabled)
        return true;

    // Compute output offsets and priority shift:
    bool oldOutputState = kOutput->mEnabled;
    kOutput->mEnabled = false;
    QRect totalScreen = computeTotalScreen();
    QRect newScreen = computeScreen();
    if (newScreen.isNull()) {
        kOutput->mEnabled = true;
        return false;
    }
    uint32_t totalMinPriority = computeTotalPriority();
    uint32_t newMinPriority = computePriority();
    Q_ASSERT(newMinPriority >= totalMinPriority);

    // Update KScreen configuration:
    bool ans = updateConfig(newScreen.topLeft() - totalScreen.topLeft(), newMinPriority - totalMinPriority);
    if (!ans)
        kOutput->mEnabled = oldOutputState;
    return ans;
}

QRect KScreenResources::computeTotalScreen(void) const
{
    QRect screen;
    foreach (QOutput* o, mOutputs) {
        KScreenOutput* kOutput = dynamic_cast<KScreenOutput*>(o);
        if (kOutput == nullptr)
            continue;
        if (kOutput->connection != QOutput::Connection::Connected)
            continue;

        screen |= kOutput->mRect;
    }
    return screen;
}

QRect KScreenResources::computeScreen(void) const
{
    QRect screen;
    foreach (QOutput* o, mOutputs) {
        KScreenOutput* kOutput = dynamic_cast<KScreenOutput*>(o);
        if (kOutput == nullptr)
            continue;
        if (kOutput->connection != QOutput::Connection::Connected)
            continue;
        if (kOutput->mEnabled)
            screen |= kOutput->mRect;
    }
    return screen;
}

uint32_t KScreenResources::computeTotalPriority(void) const
{
    bool first = true;
    uint32_t m = 0;
    foreach (QOutput* o, mOutputs) {
        KScreenOutput* kOutput = dynamic_cast<KScreenOutput*>(o);
        if (kOutput == nullptr)
            continue;
        if (kOutput->connection != QOutput::Connection::Connected)
            continue;
        if (first || (kOutput->mPriority < m))
            m = kOutput->mPriority;
        first = false;
    }
    return m;
}

uint32_t KScreenResources::computePriority(void) const
{
    bool first = true;
    uint32_t m = 0;
    foreach (QOutput* o, mOutputs) {
        KScreenOutput* kOutput = dynamic_cast<KScreenOutput*>(o);
        if (kOutput == nullptr)
            continue;
        if (kOutput->connection != QOutput::Connection::Connected)
            continue;
        if (!kOutput->mEnabled)
            continue;
        if (first || (kOutput->mPriority < m))
            m = kOutput->mPriority;
        first = false;
    }
    return m;
}

bool KScreenResources::updateConfig(const QPoint& offset, uint32_t shift)
{
    foreach (KScreen::OutputPtr output, mConfig->outputs()) {
        KScreenOutput* kOutput = dynamic_cast<KScreenOutput*>(mOutputs.value(output->id()));
        if (kOutput == nullptr)
            continue;

        if (output->isEnabled() != kOutput->mEnabled)
            output->setEnabled(kOutput->mEnabled);
        else
            output->setPos(kOutput->mRect.topLeft() - offset);
        if (output->isEnabled())
            output->setPriority(kOutput->mPriority - shift);
    }

    KScreen::SetConfigOperation opSet(mConfig);
    if (opSet.exec())
        return true;

    qWarning() << "Could not set config:" << opSet.errorString();
    return false;
}
