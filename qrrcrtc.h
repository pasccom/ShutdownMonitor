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

class QRRCrtc
{
public:
    ~QRRCrtc(void);

    int x;
    int y;
    unsigned int width;
    unsigned int height;
    RRMode mode;
    Rotation rotation;

    QList<RROutput> outputs() const;
    QString display(void) const;
    inline QRect rect(void) const {return QRect(x, y, width, height);}
private:
    QRRCrtc(QRRScreenRessources* parent, XRRCrtcInfo* info);

    QRRScreenRessources* mParent;
    XRRCrtcInfo* mInfo;

    friend class QRRScreenRessources;
};

#endif // QRRCRTC_H
