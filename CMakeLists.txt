# CMake minimum version
cmake_minimum_required(VERSION 3.27)

# Project configuration
project(ShutdownMonitor VERSION 2.0.1 LANGUAGES CXX)

# Options
option(CONSOLE_UI "Include command line interface" ON)
option(SYSTRAY_UI "Include system tray interface" ON)
option(X11_BACKEND "Include X11 backend" ON)
option(KSCREEN5_BACKEND "Include KScreen5 backend" ON)

# C++ configuration
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt configuration
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

if (X11_BACKEND)
    find_package(Qt5 COMPONENTS Widgets X11Extras LinguistTools REQUIRED)
else()
    find_package(Qt5 COMPONENTS Widgets LinguistTools REQUIRED)
endif()

# Target shutdownmonitor
add_executable(shutdownmonitor
    qscreenresources.cpp
    #qscreenresourcesfactory.cpp
    qoutput.cpp
    main.cpp
)
target_include_directories(shutdownmonitor PRIVATE "${CMAKE_SOURCE_DIR}")
target_link_libraries(shutdownmonitor Qt5::Widgets)
target_compile_definitions(shutdownmonitor PRIVATE QT_DISABLE_DEPRECATED_UP_TO=0x050F00)

# Console interface
if (CONSOLE_UI)
    message("Include command-line interface")
    target_compile_definitions(shutdownmonitor PRIVATE SHUTDOWN_MONITOR_CONSOLE)
endif()

# System tray interface
if (SYSTRAY_UI)
    message("Include system tray interface")
    target_compile_definitions(shutdownmonitor PRIVATE SHUTDOWN_MONITOR_SYSTRAY)
    target_sources(shutdownmonitor PRIVATE
        shutdownmonitor.qrc
    )
endif()

# Initialize backends
set(BACKEND_INCLUDES "")
set(BACKEND_INSERT "")

# KScreen5 backend
if (KSCREEN5_BACKEND)
    find_package(KF5Screen REQUIRED)

    message("Include KScreen5 backend")
    list(APPEND BACKEND_INCLUDES "kscreenresources.h")
    list(APPEND BACKEND_INSERT "KScreenResources")
    target_link_libraries(shutdownmonitor KF5::Screen)

    target_sources(shutdownmonitor PRIVATE
        kscreenresources.cpp
        kscreenoutput.cpp
    )
endif()

# X11 backend
if (X11_BACKEND)
    #find_package(Qt5 COMPONENTS X11Extras REQUIRED)

    message("Include X11 backend")
    list(APPEND BACKEND_INCLUDES "xrrscreenresources.h")
    list(APPEND BACKEND_INSERT "XRandRScreenResources")
    target_link_libraries(shutdownmonitor Qt5::X11Extras Xrandr X11)

    target_sources(shutdownmonitor PRIVATE
        xrrscreenresources.cpp
        xrroutput.cpp
        xrrcrtc.cpp
    )
endif()

# Check backends
if((NOT BACKEND_INCLUDES) OR (NOT BACKEND_INSERT))
    message(FATAL_ERROR "No backend has been enabled")
endif()

# Generate backends
list(TRANSFORM BACKEND_INCLUDES PREPEND "#include \"")
list(TRANSFORM BACKEND_INCLUDES APPEND "\"")
list(JOIN BACKEND_INCLUDES "\n" INCLUDE_BACKENDS)
list(TRANSFORM BACKEND_INSERT REPLACE "^(.+)$" "qMakePair(\\1::name, &\\1::create)")
list(TRANSFORM BACKEND_INSERT PREPEND "availableBackends.append(")
list(TRANSFORM BACKEND_INSERT APPEND ")\;")
list(JOIN BACKEND_INSERT "\n    " INSERT_BACKENDS)
configure_file(qscreenresourcesfactory.cpp.in qscreenresourcesfactory.cpp)
target_sources(shutdownmonitor PRIVATE ${CMAKE_BINARY_DIR}/qscreenresourcesfactory.cpp)

# Translations
set(TRANSLATIONS
    shutdownmonitor_fr.ts
    shutdownmonitor_en.ts
)

qt5_create_translation(LUPDATE_VAR ${CMAKE_SOURCE_DIR} ${TRANSLATIONS} OPTIONS -extensions cpp)
add_custom_target(lupdate DEPENDS ${LUPDATE_VAR})
qt5_add_translation(LRELEASE_VAR ${TRANSLATIONS})
add_custom_target(lrelease ALL DEPENDS ${LRELEASE_VAR})

# Doxygen
find_package(Doxygen)
doxygen_add_docs(doc ${CMAKE_SOURCE_DIR} ALL CONFIG_FILE "${CMAKE_SOURCE_DIR}/Doxyfile")
doxygen_add_docs(dev_doc ${CMAKE_SOURCE_DIR} CONFIG_FILE "${CMAKE_SOURCE_DIR}/Doxyfile.internal")

# Install
install(TARGETS shutdownmonitor)
install(FILES "$<LIST:TRANSFORM,${TRANSLATIONS},REPLACE,\\.ts$,.qm>" DESTINATION "share/shutdownmonitor/translations/")
