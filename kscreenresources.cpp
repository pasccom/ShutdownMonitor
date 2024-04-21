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

#define __STR(_x_) #_x_
#define STR(_x_) __STR(_x_)

QString KScreenResources::name = "KScreen" STR(KSCREEN_VERSION);

QScreenResources* KScreenResources::create(bool forceBackend)
{
    Q_UNUSED(forceBackend);

    return KScreenResources::getCurrent();
}

KScreenResources *KScreenResources::getCurrent()
{
    KScreen::ConfigPtr config = getConfig();

    if (config.isNull())
        return nullptr;

    return  new KScreenResources(config);
}

KScreen::ConfigPtr KScreenResources::getConfig(void)
{
    KScreen::GetConfigOperation* opGet = new KScreen::GetConfigOperation(KScreen::ConfigOperation::NoOptions);
    if (opGet->exec())
        return opGet->config();

    qWarning() << QObject::tr("Could not retrieve current config. Error:") << opGet->errorString();
    return KScreen::ConfigPtr();
}

bool KScreenResources::setConfig(const KScreen::ConfigPtr& config)
{
    KScreen::SetConfigOperation* opSet = new KScreen::SetConfigOperation(config);
    if (opSet->exec())
        return true;

    qWarning() << QObject::tr("Could not set config. Error:") << opSet->errorString();
    return false;
}

KScreenResources::KScreenResources(const KScreen::ConfigPtr& config)
    : QScreenResources(KScreenResources::name)
{
    mOutputs.clear();
    refreshOutputs(config);
}


void KScreenResources::refreshOutputs(void)
{
    KScreen::ConfigPtr config = getConfig();
    if (config.isNull())
        return;

    refreshOutputs(config);
}

void KScreenResources::refreshOutputs(const KScreen::ConfigPtr& config)
{
    QList<QOutputId> deletedIds;

    // Update outputs and create new ones:
    for (KScreen::OutputPtr output : config->outputs()) {
        KScreenOutput* kOutput = dynamic_cast<KScreenOutput*>(mOutputs.value(output->id()));
        if (kOutput == nullptr) {
            QMap<QOutputId, QOutput*>::const_iterator outputIt;

            for (outputIt = mOutputs.constBegin(); outputIt != mOutputs.constEnd(); outputIt++) {
                if (QString::compare(output->name(), outputIt.value()->name, Qt::CaseSensitive) == 0) {
                    kOutput = dynamic_cast<KScreenOutput*>(outputIt.value());
                    break;
                }
            }

            if (kOutput != nullptr) {
                // Existing output changed id
                mOutputs.remove(outputIt.key());
                mOutputs.insert(output->id(), kOutput);
            } else {
                // New output
                mOutputs.insert(output->id(), new KScreenOutput(this, output));
                continue;
            }
        }
        // Update output
        kOutput->update(output);
    }

    // Remove deleted outputs
    for (QOutputId outputId: mOutputs.keys()) {
        bool found = false;
        for (KScreen::OutputPtr output : config->outputs()) {
            if (output->id() == outputId) {
                found = true;
                break;
            }
        }
        if (!found)
            deletedIds.append(outputId);
    }

    for (QOutputId outputId : deletedIds) {
        QOutput* output = mOutputs.take(outputId);
        delete output;
    }
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
    KScreen::ConfigPtr config = getConfig();
    if (config.isNull())
        return false;

    foreach (KScreen::OutputPtr output, config->outputs())
        qDebug() << output->isEnabled() << output-> isConnected() << output->name() << output->id() << output->pos() << output->priority();

    refreshOutputs(config);

    foreach (KScreen::OutputPtr output, config->outputs()) {
        KScreenOutput* kOutput = dynamic_cast<KScreenOutput*>(mOutputs.value(output->id()));
        if (kOutput == nullptr) {

            continue;
        }

        if (output->isEnabled() != kOutput->mEnabled)
            output->setEnabled(kOutput->mEnabled);
        else
            output->setPos(kOutput->mRect.topLeft() - offset);
        if (output->isEnabled())
            output->setPriority(kOutput->mPriority - shift);
    }

    foreach (KScreen::OutputPtr output, config->outputs())
        qDebug() << output->isEnabled() << output-> isConnected() << output->name() << output->id() << output->pos() << output->priority();

    return setConfig(config);
}
