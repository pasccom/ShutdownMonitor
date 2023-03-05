/* Copyright 2020 Pascal COMBES <pascom@orange.fr>
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

#ifndef XRRSCREENRESOURCES_H
#define XRRSCREENRESOURCES_H

#include <QMap>

typedef unsigned long XID;
typedef XID RRCrtc;
typedef XID RROutput;
typedef struct _XDisplay Display;
typedef struct _XRRScreenResources XRRScreenResources;

class XRandROutput;
class XRandRCrtc;
class QPoint;
class QRect;

/*!
 * \brief Internal reprsentation for XrandR screen resources
 *
 * This class holds the internal representation for XRandR screen resources.
 * It also allows to enable and disable the outputs.
 */
class XRandRScreenResources
{
public:
    /*!
     * \brief Retrieve XRandR screen resources
     *
     * Uses XrandR 1.2 API to retrieve screen resources for the given display.
     * \param display The X display for which to retrieve screen resources.
     * \return The screen resources to the given display.
     * \sa getCurrent()
     */
    static XRandRScreenResources* get(Display *display);
    /*!
     * \brief Retrieve XRandR screen resources
     *
     * Uses XrandR 1.3 API to retrieve screen resources for the given display.
     * \param display The X display for which to retrieve screen resources.
     * \return The screen resources to the given display.
     * \sa get()
     */
    static XRandRScreenResources* getCurrent(Display *display);

    /*!
     * \brief Destructor
     *
     * Desallocates the internal data and releases the resources.
     */
    ~XRandRScreenResources(void);

    /*!
     * \brief Get all outputs
     *
     * Get a QList of the identifiers of all outputs.
     * Refresh the output cache if needed.
     * \param refresh Whether the cached output list should be refreshed.
     * \return The list of the identifiers of all outputs.
     * \sa outputs()
     */
    QList<RROutput> outputs(bool refresh = false);
    /*!
     * \brief Get all outputs
     *
     * Get a QList of the identifiers of all outputs.
     * \note Do not refresh the output cache.
     * \return The list of the identifiers of all outputs.
     * \sa outputs(bool)
     */
    inline QList<RROutput> outputs(void) const {return mOutputs.keys();}

    /*!
     * \brief Get an output by its id
     *
     * Get a pointer to the output internal representation corresponding to the given identifier.
     * \param outputId The desired output identifier.
     * \return The output internal representation corresponding to the given identifier.
     */
    XRandROutput* output(RROutput outputId) const;
    /*!
     * \brief Get an output by its name
     *
     * Get a pointer to the output internal representation corresponding to the given name.
     * \param name The desired output name.
     * \return The output internal representation corresponding to the given name.
     */
    XRandROutput* output(const QString& name) const;
    /*!
     * \brief Get a CRTC
     *
     * Get a pointer to the corresponding CRTC (Cathode Ray Tube Controller) internal reprsentation.
     * \param crtcId The desired CRTC identifier.
     * \return The CRTC internal representation corresponding to the given identifier.
     */
    XRandRCrtc* crtc(RRCrtc crtcId);

    /*!
     * \brief Enable the given output
     *
     * Enable the given output.
     * \param output The output to enable.
     * \param grab Whether to grab the X display.
     * \return Whether this output was successfully enabled.
     * \sa disableOutput()
     */
    bool enableOutput(XRandROutput* output, bool grab = false);
    /*!
     * \brief Disable the given output
     *
     * Disable the given output.
     * \param output The output to disable.
     * \param grab Whether to grab the X display.
     * \return Whether this output was successfully disabled.
     * \sa enableOutput()
     */
    bool disableOutput(XRandROutput* output, bool grab = false);
private:
    /*!
     * \brief Constructor
     *
     * Initialize the class with the given information.
     * \param display The X display.
     * \param resources The screen resources from XRandR.
     * \sa get(), getCurrent()
     */
    XRandRScreenResources(Display* display, XRRScreenResources* resources);

    /*!
     * \brief Refresh the cached output list
     *
     * Refresh the QList of output internal representations.
     */
    void refreshOutputs(void);
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
     * \brief Update a CRTC origin
     *
     * Update the top left point of a CRTC (Cathode Ray Tube Controller) rectangle.
     * Also update the associated outputs. The CRTC is disabled
     * if there is not any associated enabled output.
     * \param crtcId The identifier of the CRTC to update.
     * \param newOrigin The new coordinates of the CRTC top left point.
     * \return Whether the CRTC origin was successfully updated.
     */
    bool updateCrtcOrigin(RRCrtc crtcId, const QPoint& newOrigin);

    Display* mDisplay;                  /*!< The associated X display */
    XRRScreenResources* mResources;    /*!< The associated screen resources */
    QMap<RROutput, XRandROutput*> mOutputs;         /*!< The list of output internal representations */
    QMap<RRCrtc, XRandRCrtc*> mCrtcs;      /*!< The map of CRTC internal representations */
};

#endif // XRRSCREENRESOURCES_H
