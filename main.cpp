#include "qrrscreenressources.h"
#include "qrroutput.h"
#include "qrrcrtc.h"

#include <QX11Info>
#include <QMenu>
#include <QSystemTrayIcon>
#include <QApplication>

#include <QtDebug>

#include <X11/extensions/Xrandr.h>

/*!
 * \mainpage Shutdown monitor documentation
 *
 * \section Description
 * This small program allows to enable and disable the monitors
 * from a system tray icon. The screen configuration is kept unchanged
 * when the monitor are disabled and enabled afterwards.
 *
 * \section Usage
 * Right click on the system tray icon to make the context menu appear.
 * In the context menu, you can toogle a monitor by clicking on it.
 * A blue monitor \image{inline} html enabled-monitor.png "" is an enabled monitor, while
 * a black monitor \image{inline} html disabled-monitor.png "" is a disabled monitor.
 */

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Check that we are under X11:
    if (!QX11Info::isPlatformX11()) {
        qWarning() << "This program only supports X11";
        return -1;
    }

    // Check that system tray is available:
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        qWarning() << "This program requires the system tray";
        return -2;
    }

    // Load screen ressources:
    QRRScreenRessources* ressources = QRRScreenRessources::getCurrent(QX11Info::display());

    // Create the system tray menu:
    QMenu menu;
    foreach (QRROutput* output, ressources->outputs()) {
        if (output->connection != RR_Connected)
            continue;
        qDebug() << output->display();

        // Create an action for this connected output:
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

    // Add the exit action:
    menu.addAction(QIcon::fromTheme("window-close"), "Quit", &app, &QApplication::quit);

    // Show the system tray icon:
    QSystemTrayIcon icon(QIcon::fromTheme("monitor"), &app);
    icon.setContextMenu(&menu);
    icon.show();

    // Ensure that the screen ressources are deallocated before quitting the application:
    QObject::connect(&app, &QApplication::aboutToQuit, [ressources] {
        qDebug() << "Delete screen ressources";
        delete ressources;
    });

    // Start application event loop:
    return app.exec();
}
