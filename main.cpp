#include "qrrscreenresources.h"
#include "qrroutput.h"
#include "qrrcrtc.h"

#include <QX11Info>
#include <QMenu>
#include <QSystemTrayIcon>
#include <QTranslator>
#include <QApplication>
#include <QCommandLineParser>

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
    // Setup application:
    QApplication app(argc, argv);
    QApplication::setApplicationName("ShutdownMonitor");
    QApplication::setApplicationVersion("0.0.1");
    QApplication::setOrganizationName("pascom");

    // Load and install translator for the system locale:
    QTranslator translator(&app);
    translator.load(QLocale(), "shutdownmonitor", "_", app.applicationDirPath());
    if (!app.installTranslator(&translator))
        qWarning() << "Could not install translator";

    // Check that we are under X11:
    if (!QX11Info::isPlatformX11()) {
        qWarning() << QObject::tr("This program only supports X11");
        return -1;
    }

    // Check that system tray is available:
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        qWarning() << QObject::tr("This program requires the system tray");
        return -2;
    }

    // Setup command line arguments parser:
    QCommandLineParser parser;
    parser.addVersionOption();
    parser.addHelpOption();
    parser.setApplicationDescription(QObject::tr("Enable and disable the monitors from a system tray icon"));
    parser.addOption(QCommandLineOption("theme", QObject::tr("The theme to use for the icons. It can be 'light' or 'dark'."), "theme", "light"));
    parser.process(app);

    // Check theme:
    QStringList availableThemes;
    availableThemes << QT_TRANSLATE_NOOP("QObject", "light");
    availableThemes << QT_TRANSLATE_NOOP("QObject",  "dark");
    if (!availableThemes.contains(parser.value("theme"))) {
        qWarning() << QObject::tr("Unsupported theme: %1").arg(parser.value("theme"));
        parser.showHelp(-3);
    }

    // Load screen resources:
    QRRScreenResources* resources = QRRScreenResources::getCurrent(QX11Info::display());

    // Create the system tray menu:
    QMenu menu;
    QIcon  enabledMonitorIcon(QString(":/icons/%1/enabled-monitor.png").arg(parser.value("theme")));
    QIcon disabledMonitorIcon(QString(":/icons/%1/disabled-monitor.png").arg(parser.value("theme")));
    foreach (RROutput outputId, resources->outputs()) {
        QRROutput* output = resources->output(outputId);
        if (output->connection != RR_Connected)
            continue;
        qDebug() << output->display();

        // Create an action for this connected output:
        QAction *action = menu.addAction(output->display());
        action->setIcon(output->enabled() ? enabledMonitorIcon : disabledMonitorIcon);
        action->setData(QVariant::fromValue<RROutput>(outputId));
        QObject::connect(action, &QAction::triggered, [resources, action, &enabledMonitorIcon, &disabledMonitorIcon] {
            QRROutput* output = resources->output(action->data().value<RROutput>());
            if (output->enabled())
                output->disable();
            else
                output->enable();

            action->setIcon(output->enabled() ? enabledMonitorIcon : disabledMonitorIcon);
        });
    }
    menu.addSeparator();

    // Create the theme sub-menu:
    QMenu *themeMenu = menu.addMenu(QIcon::fromTheme("palette-symbolic"), QObject::tr("Theme"));
    foreach (QString theme, availableThemes) {
        QAction* action = themeMenu->addAction(QObject::tr(theme.toLocal8Bit().data()));
        QObject::connect(action, &QAction::triggered, [resources, &menu, &theme, &enabledMonitorIcon, &disabledMonitorIcon] {
            enabledMonitorIcon  = QIcon(QString(":/icons/%1/enabled-monitor.png").arg(theme));
            disabledMonitorIcon = QIcon(QString(":/icons/%1/disabled-monitor.png").arg(theme));

            foreach (QAction* action, menu.actions()) {
                if (action->data().isNull())
                    continue;
                QRROutput* output = resources->output(action->data().value<RROutput>());
                action->setIcon(output->enabled() ? enabledMonitorIcon : disabledMonitorIcon);
            }
        });
    }

    // Add the exit action:
    menu.addAction(QIcon::fromTheme("window-close"), QObject::tr("Quit"), &app, &QApplication::quit);

    // Show the system tray icon:
    QSystemTrayIcon icon(QIcon::fromTheme("monitor"), &app);
    icon.setContextMenu(&menu);
    icon.show();

    // Ensure that the screen resources are deallocated before quitting the application:
    QObject::connect(&app, &QApplication::aboutToQuit, [resources] {
        qDebug() << "Delete screen resources";
        delete resources;
    });

    // Start application event loop:
    return app.exec();
}
