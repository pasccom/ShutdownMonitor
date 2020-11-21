#include "qrrscreenressources.h"
#include "qrroutput.h"
#include "qrrcrtc.h"

#include <QX11Info>
#include <QMenu>
#include <QSystemTrayIcon>
#include <QApplication>

#include <QtDebug>

#include <X11/extensions/Xrandr.h>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    if (!QX11Info::isPlatformX11()) {
        qWarning() << "This program only supports X11";
        return -1;
    }

    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        qWarning() << "This program requires the system tray";
        return -2;
    }

    QRRScreenRessources* ressources = QRRScreenRessources::getCurrent(QX11Info::display());

    QMenu menu;
    foreach (QRROutput* output, ressources->outputs()) {
        if (output->connection != RR_Connected)
            continue;
        qDebug() << output->display();

        QAction *action = menu.addAction(output->display());
        action->setIcon(output->enabled() ? QIcon(":/icons/enabled-monitor.png") : QIcon(":/icons/disabled-monitor.png"));
        QObject::connect(action, &QAction::triggered, [action, output] {
            if (output->enabled())
                output->disable();
            else
                output->enable();

            action->setIcon(output->enabled() ? QIcon(":/icons/enabled-monitor.png") : QIcon(":/icons/disabled-monitor.png"));
        });
    }
    menu.addAction(QIcon::fromTheme("window-close"), "Quit", &app, &QApplication::quit);

    QSystemTrayIcon icon(QIcon::fromTheme("monitor"), &app);
    icon.setContextMenu(&menu);
    icon.show();

    QObject::connect(&app, &QApplication::aboutToQuit, [ressources] {
        qDebug() << "Delete screen ressources";
        delete ressources;
    });

    return app.exec();
}
