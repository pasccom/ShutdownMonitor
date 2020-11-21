#ifndef QRRSCREENRESSOURCES_H
#define QRRSCREENRESSOURCES_H

#include <QList>
#include <QMap>

typedef unsigned long XID;
typedef XID RRCrtc;
typedef XID RROutput;
typedef struct _XDisplay Display;
typedef struct _XRRScreenResources XRRScreenResources;

class QRROutput;
class QRRCrtc;

class QRRScreenRessources
{
public:
    static QRRScreenRessources* get(Display *display);
    static QRRScreenRessources* getCurrent(Display *display);
    ~QRRScreenRessources(void);

    QRROutput* output(RROutput outputId);
    QList<QRROutput*> outputs(bool refresh = false);
    QRRCrtc* crtc(RRCrtc crtcId);

    bool enableOutput(QRROutput* out);
    bool disableOutput(QRROutput* out);
private:
    QRRScreenRessources(Display* display, XRRScreenResources* ressources);

    void refreshOutputs(void);
    bool actualizeCrtcOrigin(RRCrtc crtcId, const QPoint& newOrigin);

    Display* mDisplay;
    XRRScreenResources* mRessources;
    QList<QRROutput*> mOutputs;
    QMap<RRCrtc, QRRCrtc*> mCrtcs;
};

#endif // QRRSCREENRESSOURCES_H
