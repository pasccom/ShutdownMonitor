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

#ifndef QSCREENRESOURCES_H
#define QSCREENRESOURCES_H

#include <QMap>

typedef unsigned long QOutputId;

class QOutput;

/*!
 * \brief Internal reprsentation for screen resources
 *
 * This class holds the internal representation for screen resources.
 * It also allows to enable and disable the outputs.
 */
class QScreenResources
{
public:
    QString name;   /*!< Name of the backend */

    /*!
     * \brief List available backends
     *
     * This function lists the available backends.
     * \return The list of the available backend names.
     */
    static QStringList listBackends(void);
    /*!
     * \brief Creates screen resources
     *
     * This function creates a new screen resources instance,
     * using the given backend, if any, or the prefered backend.
     * \param backend The name of the preferred backend
     * \return A new screen resource instance.
     */
    static QScreenResources* create(const QString& backend);
    /*!
     * \brief Destructor
     *
     * Desallocates the internal data and releases the resources.
     */
    virtual ~QScreenResources(void);
    /*!
     * \brief Get all outputs
     *
     * Get a QList of the identifiers of all outputs.
     * Refresh the output cache if needed.
     * \param refresh Whether the cached output list should be refreshed.
     * \return The list of the identifiers of all outputs.
     * \sa outputs()
     */
    QList<QOutputId> outputs(bool refresh = false);
    /*!
     * \brief Get all outputs
     *
     * Get a QList of the identifiers of all outputs.
     * \note Do not refresh the output cache.
     * \return The list of the identifiers of all outputs.
     * \sa outputs(bool)
     */
    inline QList<QOutputId> outputs(void) const {return mOutputs.keys();}

    /*!
     * \brief Get an output by its id
     *
     * Get a pointer to the output internal representation corresponding to the given identifier.
     * \param outputId The desired output identifier.
     * \return The output internal representation corresponding to the given identifier.
     */
    QOutput* output(QOutputId outputId) const;
    /*!
     * \brief Get an output by its name
     *
     * Get a pointer to the output internal representation corresponding to the given name.
     * \param name The desired output name.
     * \return The output internal representation corresponding to the given name.
     */
    QOutput* output(const QString& name) const;

    /*!
     * \brief Enable the given output
     *
     * Enable the given output.
     * \param output The output to enable.
     * \param grab Whether to grab the X display.
     * \return Whether this output was successfully enabled.
     * \sa disableOutput()
     */
    virtual bool enableOutput(QOutput* output, bool grab = false) = 0;
    /*!
     * \brief Disable the given output
     *
     * Disable the given output.
     * \param output The output to disable.
     * \param grab Whether to grab the X display.
     * \return Whether this output was successfully disabled.
     * \sa enableOutput()
     */
    virtual bool disableOutput(QOutput* output, bool grab = false) = 0;
protected:
    /*!
     * \brief Constructor
     *
     * Initialize the class with the given backend name.
     * \param name The backend name.
     * \sa create()
     */
    inline QScreenResources(const QString& name) :
        name(name) {}
    /*!
     * \brief Refresh the cached output list
     *
     * Refresh the QList of output internal representations.
     */
    virtual void refreshOutputs(void) = 0;

    QMap<QOutputId, QOutput*> mOutputs; /*!< The list of output internal representations */
private:
    /*! The list of available backends */
    static QList< QPair< QString, std::function<QScreenResources*(const QString&)> > > availableBackends;
    /*!
     * \brief Initialize available backends.
     *
     * This function is in charge of initializing the list of available backends.
     */
    static void initBackends(void);
};

#endif // QSCREENRESOURCES_H
