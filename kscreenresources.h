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

#ifndef KSCREENRESOURCES_H
#define KSCREENRESOURCES_H

#include "qscreenresources.h"
#include <KScreen/Config>

/*!
 * \brief Internal reprsentation for KScreen configuration
 *
 * This class holds the internal representation for a KScreen configuration.
 * It also allows to enable and disable the outputs.
 */
class KScreenResources : public QScreenResources
{
public:
    static QString name;    /*!< Backend name */
    /*!
     * \brief Screen resources factory
     *
     * This method creates a new screen resource instance,
     * if the backend matches.
     * \param backend Selected backend.
     * \return A new screen resources instance if the backend matches,
     * otherwise, \c nullptr.
     */
    static QScreenResources* create(const QString& backend);
    /*!
     * \brief Retrieve KScreen current configuration
     *
     * Uses KScreen API to retrieve current KScreen configuration.
     * \return The current KScreen configuration.
     */
    static KScreenResources* getCurrent();

    /*!
     * \brief Destructor
     *
     * Desallocates the internal data and releases the resources.
     */
    inline virtual ~KScreenResources(void) {}

    /*!
     * \brief Enable the given output
     *
     * Enable the given output.
     * \param output The output to enable.
     * \param grab This parameter is ignored in this implementation.
     * \return Whether this output was successfully enabled.
     * \sa disableOutput()
     */
    bool enableOutput(QOutput* output, bool grab = false);
    /*!
     * \brief Disable the given output
     *
     * Disable the given output.
     * \param output The output to disable.
     * \param grab This parameter is ignored in this implementation.
     * \return Whether this output was successfully disabled.
     * \sa enableOutput()
     */
    bool disableOutput(QOutput* output, bool grab = false);
protected:
    /*!
     * \brief Refresh the cached output list
     *
     * Refresh the QList of output internal representations.
     */
    void refreshOutputs(void);
private:
    /*!
     * \brief Constructor
     *
     * Initialize the class with the given information.
     * \param config A KScreen configuration.
     * \sa getCurrent()
     */
    KScreenResources(const KScreen::ConfigPtr& config);

    /*!
     * \brief Compute total screen
     *
     * Compute the screen rectangle when all outputs are on.
     * \return The total screen rectangle
     * \sa computeScreen()
     */
    QRect computeTotalScreen(void) const;
    /*!
     * \brief Compute screen
     *
     * Compute the screen rectangle when only enabled outputs are on.
     * \return The screen rectangle
     * \sa computeTotalScreen()
     */
    QRect computeScreen(void) const;
    /*!
     * \brief Compute global minimum priority
     *
     * Compute the minimum priority taking into account all outputs.
     * \return The global minimum priority
     * \sa computePriority()
     */
    uint32_t computeTotalPriority(void) const;
    /*!
     * \brief Compute minimum priority
     *
     * Compute the minimum priority taking into account only enabled outputs.
     * \return The minimum priority
     * \sa computeTotalPriority()
     */
    uint32_t computePriority(void) const;
    /*!
     * \brief Update configuration
     *
     * This function updates KScreen configuration.
     * \param offset The offset to shift the top-left corner of the enabled outputs by
     * \param shift The shift for the priorities of the enabled outputs
     * \return Whether the configuration was successfully updated.
     */
    bool updateConfig(const QPoint& offset, uint32_t shift);

    KScreen::ConfigPtr mConfig; /*!< The current KScreen configuration */
};

#endif // KSCREENRESOURCES_H
