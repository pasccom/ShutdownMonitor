TARGET = shutdownmonitor    # The name of the executable
TEMPLATE = app              # Make an executable

# Qt configuration:
!equals(QT_MAJOR_VERSION, 5) {
    error("This program can only be compiled under Qt5.")
}

QT += x11extras
QT += widgets
CONFIG += c++11

# Additional libs:
!unix {
    error("This program can only be compiled under unix, as it uses XrandR library")
}
LIBS += -lXrandr -lX11

!equals(CONSOLE, no) {
    message("Include command-line interface")
    DEFINES += SHUTDOWN_MONITOR_CONSOLE
}
!equals(SYSTRAY, no) {
    message("Include system tray interface")
    DEFINES += SHUTDOWN_MONITOR_SYSTRAY
}

# The headers and source files:
HEADERS +=  qrrcrtc.h \
            qrroutput.h \
            qrrscreenresources.h
SOURCES +=  main.cpp \
            qrrcrtc.cpp \
            qrroutput.cpp \
            qrrscreenresources.cpp

# The resources:
contains(DEFINES, SHUTDOWN_MONITOR_SYSTRAY): RESOURCES += shutdownmonitor.qrc

# The translations:
TRANSLATIONS += shutdownmonitor_en.ts \
                shutdownmonitor_fr.ts

# Other files:
OTHER_FILES +=  .gitignore          \
                Doxyfile            \
                Doxyfile.internal

# The directory where to put MOC-generated files :
MOC_DIR = ./.moc

# Cheating dirs :
unix {
    OBJECTS_DIR = ./.obj_unix
    CONFIG(debug, debug|release):DESTDIR = ./debug
    else:DESTDIR = ./release
}

# Translation files update (not handled by Qt):
isEmpty(QMAKE_LUPDATE) {
    win32:QMAKE_LUPDATE = $$[QT_INSTALL_BINS]/lupdate.exe
    else:QMAKE_LUPDATE = $$[QT_INSTALL_BINS]/lupdate
}

lupdate.depends += $$_PRO_FILE_
lupdate.depends += $$SOURCES
lupdate.depends += $$HEADERS
lupdate.commands = $$QMAKE_LUPDATE -no-obsolete \"$$_PRO_FILE_\"

QMAKE_EXTRA_TARGETS += lupdate

# Translation files generation (not handled by Qt):
isEmpty(QMAKE_LRELEASE) {
    win32:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]/lrelease.exe
    else:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]/lrelease
}

lrelease.input = TRANSLATIONS
lrelease.output = ./${DESTDIR}/${QMAKE_FILE_BASE}.qm
lrelease.commands = $$QMAKE_LRELEASE ${QMAKE_FILE_IN} -qm ./${DESTDIR}/${QMAKE_FILE_BASE}.qm
lrelease.CONFIG += no_link

QMAKE_EXTRA_COMPILERS += lrelease
POST_TARGETDEPS += compiler_lrelease_make_all
