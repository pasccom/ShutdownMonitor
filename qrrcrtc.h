#ifndef QRRCRTC_H
#define QRRCRTC_H

#include <QString>
#include <QRect>

typedef unsigned short Rotation;
typedef unsigned long XID;
typedef XID RRMode;
typedef XID RROutput;
typedef struct _XRRCrtcInfo XRRCrtcInfo;

class QRRScreenRessources;

/*!
 * \brief Internal representation for XrandR CRTC
 *
 * Instances of this class represent a CRTC (Cathode Ray Tube Controller).
 * In particular, they give the position of the CRTC buffer in the screen.
 */
class QRRCrtc
{
public:
    /*!
     * \brief Destructor
     *
     * Releases the internal CRTC information.
     */
    ~QRRCrtc(void);

    int x;                  /*!< The x coordinate of the top left point of the CRTC on the screen */
    int y;                  /*!< The y coordinate of the top left point of the CRTC on the screen */
    unsigned int width;     /*!< The width of the CRTC */
    unsigned int height;    /*!< The height of the CRTC */
    RRMode mode;            /*!< The mode of the CRTC */
    Rotation rotation;      /*!< The rotation of the CRTC */

    /*!
     * \brief Associated outputs
     *
     * Return the list of the outputs associated with this CRTC.
     * \note There may be mutiple outputs associated with one CRTC.
     * In that case the output is duplicated.
     * \return The list of the associated outputs
     */
    QList<RROutput> outputs() const;
    /*!
     * \brief User-friendly name of this CRTC
     *
     * Returns a user-friendly name for the CRTC.
     * \return A user-friendly name for the CRTC.
     */
    QString display(void) const;
    /*!
     * \brief CRTC rectangle
     *
     * Returns the rectangle that this CRTC spans on the screen.
     * \return The rectange that this CRTC spans on the screen.
     * \sa x, y, width, height
     */
    inline QRect rect(void) const {return QRect(x, y, width, height);}
private:
    /*!
     * \brief Constructor
     *
     * Initialize the class with the given information.
     * \param parent The parent screen resources.
     * \param info The CRTC information from XrandR.
     */
    QRRCrtc(QRRScreenRessources* parent, XRRCrtcInfo* info);

    QRRScreenRessources* mParent;   /*!< The parent screen resources */
    XRRCrtcInfo* mInfo;             /*!< The CRTC information */

    friend class QRRScreenRessources;
};

#endif // QRRCRTC_H
