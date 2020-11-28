#ifndef QRROUTPUT_H
#define QRROUTPUT_H

#include <QString>

typedef unsigned long XID;
typedef XID RROutput;
typedef XID RRCrtc;
typedef struct _XRROutputInfo XRROutputInfo;
typedef unsigned short Connection;

class QRRScreenResources;
class QRRCrtc;

/*!
 * \brief Internal representation for XrandR output
 *
 * Instances of this class represent an output (monitor, ...).
 */
class QRROutput
{
public:
    QString name;               /*!< The output name */
    int physicalWidth;          /*!< The physical width of this output (in mm) */
    int physicalHeight;         /*!< The physical height of this output (in mm) */
    Connection connection;      /*!< The connection state of this output */

    /*!
     * \brief Associated CRTC
     *
     * Returns the internal representation of the CRTC (Cathode Ray Tube controler)
     * associated with this controller.
     * \return The associated CRTC.
     */
    QRRCrtc* crtc(void) const;
    /*!
     * \brief User-friendly name of this output
     *
     * Returns a user-friendly name for the output.
     * \return A user-friendly name for the output.
     */
    QString display(void) const;

    /*!
     * \brief Is enabled?
     *
     * Returns whether this output is currently enabled.
     * \note Disconnected outputs are disabled, but connected outputs can be disabled.
     * \return Whether this output is enabled.
     * \sa enable(), disable()
     */
    inline bool enabled(void) const {return mEnabled;}
    /*!
     * \brief Enable the output
     *
     * Enable the output.
     * \return Whether this output was successfully enabled.
     * \sa disable(), enabled()
     */
    bool enable(void);
    /*!
     * \brief Disable the output
     *
     * Disable the output.
     * \return Whether this output was successfully disabled.
     * \sa enable(), disabled()
     */
    bool disable(void);
private:
    /*!
     * \brief Constructor
     *
     * Initialize the class with the given information.
     * \param parent The parent screen resources.
     * \param info The output information from XrandR.
     */
    QRROutput(QRRScreenResources* parent, XRROutputInfo* info);

    QRRScreenResources* mParent;    /*!< The parent screen resources */
    RRCrtc mCrtcId;                 /*!< The id of the associated CRTC */
    bool mEnabled;                  /*!< The enabled state for this output */

    friend class QRRScreenResources;
};

#endif // QRROUTPUT_H
