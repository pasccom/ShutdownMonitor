#ifndef QRRSCREENRESOURCES_H
#define QRRSCREENRESOURCES_H

#include <QList>
#include <QMap>

typedef unsigned long XID;
typedef XID RRCrtc;
typedef XID RROutput;
typedef struct _XDisplay Display;
typedef struct _XRRScreenResources XRRScreenResources;

class QRROutput;
class QRRCrtc;

/*!
 * \brief Internal reprsentation for XrandR screen resources
 *
 * This class holds the internal representation for XRandR screen resources.
 * It also allows to enable and disable the outputs.
 */
class QRRScreenResources
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
    static QRRScreenResources* get(Display *display);
    /*!
     * \brief Retrieve XRandR screen resources
     *
     * Uses XrandR 1.3 API to retrieve screen resources for the given display.
     * \param display The X display for which to retrieve screen resources.
     * \return The screen resources to the given display.
     * \sa get()
     */
    static QRRScreenResources* getCurrent(Display *display);

    /*!
     * \brief Destructor
     *
     * Desallocates the internal data and releases the resources.
     */
    ~QRRScreenResources(void);

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
    QRROutput* output(RROutput outputId) const;
    /*!
     * \brief Get an output by its name
     *
     * Get a pointer to the output internal representation corresponding to the given name.
     * \param name The desired output name.
     * \return The output internal representation corresponding to the given name.
     */
    QRROutput* output(const QString& name) const;
    /*!
     * \brief Get a CRTC
     *
     * Get a pointer to the corresponding CRTC (Cathode Ray Tube Controller) internal reprsentation.
     * \param crtcId The desired CRTC identifier.
     * \return The CRTC internal representation corresponding to the given identifier.
     */
    QRRCrtc* crtc(RRCrtc crtcId);

    /*!
     * \brief Enable the given output
     *
     * Enable the given output.
     * \param output The output to enable.
     * \return Whether this output was successfully enabled.
     * \sa disableOutput()
     */
    bool enableOutput(QRROutput* output);
    /*!
     * \brief Disable the given output
     *
     * Disable the given output.
     * \param output The output to disable.
     * \return Whether this output was successfully disabled.
     * \sa enableOutput()
     */
    bool disableOutput(QRROutput* output);
private:
    /*!
     * \brief Constructor
     *
     * Initialize the class with the given information.
     * \param display The X display.
     * \param resources The screen resources from XRandR.
     * \sa get(), getCurrent()
     */
    QRRScreenResources(Display* display, XRRScreenResources* resources);

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
    QMap<RROutput, QRROutput*> mOutputs;         /*!< The list of output internal representations */
    QMap<RRCrtc, QRRCrtc*> mCrtcs;      /*!< The map of CRTC internal representations */
};

#endif // QRRSCREENRESOURCES_H
