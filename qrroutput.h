#ifndef QRROUTPUT_H
#define QRROUTPUT_H

#include <QString>

typedef unsigned long XID;
typedef XID RROutput;
typedef struct _XRROutputInfo XRROutputInfo;
typedef unsigned short Connection;

class QRRScreenRessources;
class QRRCrtc;

class QRROutput
{
public:
    ~QRROutput(void);

    QString name;
    int physicalWidth;
    int physicalHeight;
    Connection connection;
    QRRCrtc* crtc(void) const;

    QString display(void) const;

    inline bool enabled(void) const {return mEnabled;}
    bool enable(void);
    bool disable(void);
private:
    QRROutput(QRRScreenRessources* parent, RROutput id, XRROutputInfo* info);

    QRRScreenRessources* mParent;
    RROutput mId;
    XRROutputInfo* mInfo;
    bool mEnabled;

    friend class QRRScreenRessources;
};

#endif // QRROUTPUT_H
