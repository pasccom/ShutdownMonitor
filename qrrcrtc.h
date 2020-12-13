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

#ifndef QRRCRTC_H
#define QRRCRTC_H

#include <QString>
#include <QList>
#include <QRect>

typedef unsigned short Rotation;
typedef unsigned long XID;
typedef XID RRMode;
typedef XID RROutput;
typedef struct _XRRCrtcInfo XRRCrtcInfo;

class QRRScreenResources;

/*!
 * \brief Internal representation for XrandR CRTC
 *
 * Instances of this class represent a CRTC (Cathode Ray Tube Controller).
 * In particular, they give the position of the CRTC buffer in the screen.
 */
class QRRCrtc
{
public:
    int x;                      /*!< The x coordinate of the top left point of the CRTC on the screen */
    int y;                      /*!< The y coordinate of the top left point of the CRTC on the screen */
    unsigned int width;         /*!< The width of the CRTC */
    unsigned int height;        /*!< The height of the CRTC */
    RRMode mode;                /*!< The mode of the CRTC */
    Rotation rotation;          /*!< The rotation of the CRTC */
    QList<RROutput> outputs;    /*! The list of the associated outputs */

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
    QRRCrtc(QRRScreenResources* parent, XRRCrtcInfo* info);

    QRRScreenResources* mParent;   /*!< The parent screen resources */

    friend class QRRScreenResources;
};

#endif // QRRCRTC_H
