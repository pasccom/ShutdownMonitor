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

#ifndef QOUTPUT_H
#define QOUTPUT_H

#include <QString>

class QScreenResources;

/*!
 * \brief Internal representation for outputs
 *
 * Instances of this class represent an output (monitor, ...).
 */
class QOutput
{
public:
    enum class Connection {
        Unknown = -1,
        Disconnected = 0,
        Connected,
    };

    QString name;               /*!< The output name */
    int physicalWidth;          /*!< The physical width of this output (in mm) */
    int physicalHeight;         /*!< The physical height of this output (in mm) */
    Connection connection;      /*!< The connection state of this output */

    /*!
     * \brief Destructor
     *
     * This destructor does nothing. It is there to enable polymorphism.
     */
    inline virtual ~QOutput(void) {}

    /*!
     * \brief User-friendly name of this output
     *
     * Returns a user-friendly name for the output.
     * \return A user-friendly name for the output.
     */
    virtual QString display(void) const {return name;}

    /*!
     * \brief Is enabled?
     *
     * Returns whether this output is currently enabled.
     * \note Disconnected outputs are disabled, but connected outputs can be disabled.
     * \return Whether this output is enabled.
     * \sa enable(), disable(), toggle()
     */
    inline bool enabled(void) const {return mEnabled;}
    /*!
     * \brief Enable the output
     *
     * Enable the output.
     * \param grab Whether to grab the X display.
     * \return Whether this output was successfully enabled.
     * \sa disable(), enabled()
     */
    bool enable(bool grab = false);
    /*!
     * \brief Disable the output
     *
     * Disable the output.
     * \param grab Whether to grab the X display.
     * \return Whether this output was successfully disabled.
     * \sa enable(), enabled()
     */
    bool disable(bool grab = false);
    /*!
     * \brief Toggle the output
     *
     * Disable the output if it is enabled, or
     * enable the output if it is disabled.
     * \param grab Whether to grab the X display.
     * \return Whether this output was successfully toggled.
     * \sa enable(), disable(), enabled()
     */
    bool toggle(bool grab = false);
protected:
    /*!
     * \brief Constructor
     *
     * Initialize the class with the given information.
     * \param parent The parent screen resources.
     */
    inline QOutput(QScreenResources *parent) :
        mParent(parent), mEnabled(false) {}

    QScreenResources* mParent;    /*!< The parent screen resources */
    bool mEnabled;                /*!< The enabled state for this output */
};

#endif // QOUTPUT_H
