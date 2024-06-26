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

#ifndef KSCREENOUTPUT_H
#define KSCREENOUTPUT_H

#include "qoutput.h"

#include <KScreen/Output>
#include <QString>
#include <QRect>

class KScreenResources;

/*!
 * \brief Internal representation for KScreen output
 *
 * Instances of this class represent an output (monitor, ...).
 */
class KScreenOutput : public QOutput
{
public:
    /*!
     * \brief User-friendly name of this output
     *
     * Returns a user-friendly name for the output.
     * \return A user-friendly name for the output.
     */
    QString display(void) const;

private:
    /*!
     * \brief Constructor
     *
     * Initialize the class with the given information.
     * \param parent The parent screen resources.
     * \param output The output information from Kscreen.
     */
    KScreenOutput(KScreenResources* parent, const KScreen::OutputPtr& output);
    /*!
     * \brief Update the output
     *
     * This function actualizes the properties of the output.
     * \note Currently only the connection state is updated.
     * \param output The output from KScreen
     */
    void update(const KScreen::OutputPtr& output);

    KScreen::OutputPtr mOutput; /*!< The actual KScreen output */
    QRect mRect;                /*!< The output rect on the screen from the original configuration */
    uint32_t mPriority;         /*!< The output priority from the original configuration */

    friend class KScreenResources;
};

#endif // KSCREENOUTPUT_H
