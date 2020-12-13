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

#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>

#include <X11/extensions/Xrandr.h>

#ifndef SHUTDOWN_MONITOR_CONSOLE
#   ifndef SHUTDOWN_MONITOR_SYSTRAY
#       error -DSHUTDOWN_MONITOR_CONSOLE or -DSHUTDOWN_MONITOR_SYSTRAY is required
#   endif
#endif

/*!
 * \mainpage Shutdown monitor documentation
 *
 * \section description Description
 * This small program allows to enable and disable the monitors
 * from a system tray icon. The screen configuration is kept unchanged
 * when the monitor are disabled and enabled afterwards.
 *
 * \section systray System tray interface
 * Right click on the system tray icon to make the context menu appear.
 * In the context menu, you can toogle a monitor by clicking on it.
 * A blue monitor \image{inline} html enabled-monitor.png "" is an enabled monitor, while
 * a black monitor \image{inline} html disabled-monitor.png "" is a disabled monitor.
 *
 * \section console Command-line interface
 * Hereafter is a table describing command-line options:
 * | Short | Long form          | Arguments   | Description                             |
 * | ----- | ------------------ | ----------- | --------------------------------------- |
 * | \c -t | \c --toggle-output | \c <output> | The outputs to disable before starting. |
 * | \c -l | \c --list-outputs  |             | List outputs and quit.                  |
 */
#ifdef SHUTDOWN_MONITOR_CONSOLE
QStringList toggleOutputs(QRRScreenResources* resources, QStringList& outputs)
{
    QStringList toggledOutputs;

    foreach (RROutput outputId, resources->outputs()) {
        QRROutput* output = resources->output(outputId);
        if (output == nullptr)
            continue;
        if (output->connection != RR_Connected)
            continue;
        foreach (QString name, outputs) {
            if (QString::compare(output->name, name, Qt::CaseSensitive) == 0) {
                if (output->toggle())
                    toggledOutputs << name;
            }
        }
    }

    return toggledOutputs;
}

static int socketFds[2];
void signalHandler(int signum) {
    Q_UNUSED(signum);
    char a = 'o';
    write(socketFds[0], &a, 1);
}
#endif // SHUTDOWN_MONITOR_CONSOLE

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

    // Setup command line arguments parser:
    QCommandLineParser parser;
    parser.addVersionOption();
    parser.addHelpOption();
    parser.setApplicationDescription(QObject::tr("Enable and disable the monitors from the system tray or the command line."));
#ifdef SHUTDOWN_MONITOR_SYSTRAY
    parser.addOption(QCommandLineOption("theme", QObject::tr("The theme to use for the icons. It can be 'light' or 'dark'."), QObject::tr("theme"), "light"));
#endif // SHUTDOWN_MONITOR_SYSTRAY
#ifdef SHUTDOWN_MONITOR_CONSOLE
    parser.addOption(QCommandLineOption({"t", "toggle-output"}, QObject::tr("The outputs to disable before starting."), QObject::tr("output")));
    parser.addOption(QCommandLineOption({"l", "list-outputs"}, QObject::tr("List outputs and quit.")));
#endif // SHUTDOWN_MONITOR_CONSOLE
    parser.process(app);

    // Load screen resources:
    QRRScreenResources* resources = QRRScreenResources::getCurrent(QX11Info::display());
#ifdef SHUTDOWN_MONITOR_SYSTRAY
    bool done = false;
#else // SHUTDOWN_MONITOR_SYSTRAY
    bool done = true;
#endif // SHUTDOWN_MONITOR_SYSTRAY

#ifdef SHUTDOWN_MONITOR_CONSOLE
    // List outputs:
    if (parser.isSet("list-outputs")) {
        foreach (RROutput outputId, resources->outputs()) {
            QRROutput* output = resources->output(outputId);
            if (output == nullptr)
                continue;
            if (output->connection != RR_Connected)
                continue;
            std::cout << qPrintable(output->name) << std::endl;
        }
        done = true;
    }

    // Toggle output:
    QStringList outputs = parser.values("toggle-output");
    if (!outputs.isEmpty()) {
        if (socketpair(AF_UNIX, SOCK_RAW, 0, socketFds) != 0) {
            qWarning() << "Could not create socket pair. Error:" << errno << QString("(%1)").arg(strerror(errno));
        } else {
            struct sigaction sigInt;
            sigInt.sa_handler = signalHandler;
            sigemptyset(&sigInt.sa_mask);
            sigInt.sa_flags = SA_RESTART;

            if (sigaction(SIGINT, &sigInt, 0) != 0) {
                qWarning() << "Could not install signal handler. Error:" << errno << QString("(%1)").arg(strerror(errno));
            } else {
                char buffer;
                QStringList toggledOutputs;

                std::cout << qPrintable(QObject::tr("Press Ctrl+C to restore previous state. "));
                std::cout.flush();
                toggledOutputs = toggleOutputs(resources, outputs);
                read(socketFds[1], &buffer, 1);
                std::cout << std::endl;
                toggleOutputs(resources, toggledOutputs);
            }
        }
        done = true;
    }

    if (done) {
        qDebug() << "Delete screen resources";
        delete resources;
        return 0;
    }
#endif // SHUTDOWN_MONITOR_CONSOLE

#ifdef SHUTDOWN_MONITOR_SYSTRAY
    // Check that system tray is available:
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        qWarning() << QObject::tr("This program requires the system tray");
        return -2;
    }

    // Check theme:
    QStringList availableThemes;
    availableThemes << QT_TRANSLATE_NOOP("QObject", "light");
    availableThemes << QT_TRANSLATE_NOOP("QObject",  "dark");
    if (!availableThemes.contains(parser.value("theme"))) {
        qWarning() << QObject::tr("Unsupported theme: %1").arg(parser.value("theme"));
        parser.showHelp(-3);
    }

    // Create the system tray menu:
    int o = 0;
    QMenu menu;
    QIcon  enabledMonitorIcon(QString(":/icons/%1/enabled-monitor.png").arg(parser.value("theme")));
    QIcon disabledMonitorIcon(QString(":/icons/%1/disabled-monitor.png").arg(parser.value("theme")));
    foreach (RROutput outputId, resources->outputs()) {
        QRROutput* output = resources->output(outputId);
        if (output == nullptr)
            continue;
        if (output->connection != RR_Connected)
            continue;
        qDebug() << output->display();

        // Create an action for this connected output:
        QAction* action;
        if (o < ('Q' - 'A')) {
            action = menu.addAction(QString("%1. %2").arg(QChar(o++ + 'A'))
                                                     .arg(output->display()));
        } else {
            action = menu.addAction(QString("     %1").arg(output->display()));
        }

        action->setIcon(output->enabled() ? enabledMonitorIcon : disabledMonitorIcon);
        action->setData(QVariant::fromValue<RROutput>(outputId));
        QObject::connect(action, &QAction::triggered, [resources, action, &enabledMonitorIcon, &disabledMonitorIcon] {
            QRROutput* output = resources->output(action->data().value<RROutput>());
            output->toggle();
            action->setIcon(output->enabled() ? enabledMonitorIcon : disabledMonitorIcon);
        });
    }
    menu.addSeparator();

    // Create the theme sub-menu:
    QMenu *themeMenu = menu.addMenu(QIcon::fromTheme("palette-symbolic"), QObject::tr("Theme"));
    foreach (QString theme, availableThemes) {
        QAction* themeAction = themeMenu->addAction(QObject::tr(theme.toLocal8Bit().data()));
        themeAction->setData(theme);
        QObject::connect(themeAction, &QAction::triggered, [resources, &menu, themeAction, &enabledMonitorIcon, &disabledMonitorIcon] {
            enabledMonitorIcon  = QIcon(QString(":/icons/%1/enabled-monitor.png").arg(themeAction->data().toString()));
            disabledMonitorIcon = QIcon(QString(":/icons/%1/disabled-monitor.png").arg(themeAction->data().toString()));

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
#endif // SHUTDOWN_MONITOR_SYSTRAY
}
