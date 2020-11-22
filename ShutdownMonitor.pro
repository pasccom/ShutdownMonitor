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

# The headers and source files:
HEADERS +=  qrrcrtc.h \
            qrroutput.h \
            qrrscreenresources.h
SOURCES +=  main.cpp \
            qrrcrtc.cpp \
            qrroutput.cpp \
            qrrscreenresources.cpp

# The resources:
RESOURCES += shutdownmonitor.qrc

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
