# CMake minimum version
cmake_minimum_required(VERSION 3.27)

# Project configuration
project(ShutdownMonitor VERSION 2.0.1 LANGUAGES CXX)

# Options
option(CONSOLE_UI "Include command line interface" ON)
option(SYSTRAY_UI "Include system tray interface" ON)
option(X11_BACKEND "Include X11 backend" ON)
option(KSCREEN5_BACKEND "Include KScreen5 backend" ON)
option(KSCREEN6_BACKEND "Include KScreen6 backend" ON)
set(QT_VERSION 6 CACHE STRING "Qt version to use")

# C++ configuration
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt configuration
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

if (QT_VERSION EQUAL 5)
    set(QT Qt5)
    if (X11_BACKEND)
        find_package(Qt5 COMPONENTS Core X11Extras Widgets LinguistTools REQUIRED)
    else()
        find_package(Qt5 COMPONENTS Core Widgets LinguistTools REQUIRED)
    endif()
elseif (QT_VERSION EQUAL 6)
    set(QT Qt6)
    find_package(Qt6 COMPONENTS Core Widgets LinguistTools REQUIRED)
else()
    message(FATAL_ERROR "Unsupported Qt version ${QT_VERSION}")
endif()
message("Using ${QT}")

add_library(qt_config INTERFACE)
if (NOT (CMAKE_BUILD_TYPE STREQUAL "Debug"))
    target_compile_definitions(qt_config INTERFACE QT_NO_DEBUG_OUTPUT)
endif()

# Target shutdownmonitor
add_executable(shutdownmonitor
    qscreenresources.cpp
    #qscreenresourcesfactory.cpp
    qoutput.cpp
    main.cpp
)
target_include_directories(shutdownmonitor PRIVATE "${CMAKE_SOURCE_DIR}")
target_link_libraries(shutdownmonitor ${QT}::Widgets)
target_link_libraries(shutdownmonitor qt_config)

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

# KScreen6 backend
if (KSCREEN6_BACKEND AND (QT_VERSION EQUAL 6))
    find_package(KF6Screen REQUIRED)

    message("Include KScreen6 backend")
    add_library(backend_kscreen6)
    target_link_libraries(backend_kscreen6 ${QT}::Core)
    target_link_libraries(backend_kscreen6 KF6::Screen)
    target_link_libraries(backend_kscreen6 qt_config)
    target_compile_definitions(backend_kscreen6 PRIVATE KSCREEN_VERSION=6)
    target_sources(backend_kscreen6 PRIVATE
        kscreenresources.cpp
        kscreenoutput.cpp
    )

    list(APPEND BACKEND_INCLUDES "kscreenresources.h")
    list(APPEND BACKEND_INSERT "KScreenResources")
    target_link_libraries(shutdownmonitor backend_kscreen6)
endif()

# KScreen5 backend
if (KSCREEN5_BACKEND AND (QT_VERSION EQUAL 5))
    find_package(KF5Screen REQUIRED)

    message("Include KScreen5 backend")
    add_library(backend_kscreen5)
    target_link_libraries(backend_kscreen5 ${QT}::Core)
    target_link_libraries(backend_kscreen5 KF5::Screen)
    target_link_libraries(backend_kscreen5 qt_config)
    target_compile_definitions(backend_kscreen5 PRIVATE KSCREEN_VERSION=5)
    target_sources(backend_kscreen5 PRIVATE
        kscreenresources.cpp
        kscreenoutput.cpp
    )

    list(APPEND BACKEND_INCLUDES "kscreenresources.h")
    list(APPEND BACKEND_INSERT "KScreenResources")
    target_link_libraries(shutdownmonitor backend_kscreen5)
endif()

# X11 backend
if (X11_BACKEND)
    #find_package(Qt5 COMPONENTS X11Extras REQUIRED)

    message("Include X11 backend")
    add_library(backend_x11)
    if (QT_VERSION EQUAL 5)
        target_link_libraries(backend_x11 ${QT}::X11Extras)
    elseif (QT_VERSION EQUAL 6)
        target_link_libraries(backend_x11 ${QT}::Gui)
    endif()
    target_link_libraries(backend_x11 Xrandr X11)
    target_link_libraries(backend_x11 qt_config)
    target_sources(backend_x11 PRIVATE
        xrrscreenresources.cpp
        xrroutput.cpp
        xrrcrtc.cpp
    )

    list(APPEND BACKEND_INCLUDES "xrrscreenresources.h")
    list(APPEND BACKEND_INSERT "XRandRScreenResources")
    target_link_libraries(shutdownmonitor backend_x11)
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

qt_create_translation(LUPDATE_VAR ${CMAKE_SOURCE_DIR} ${TRANSLATIONS} OPTIONS -extensions cpp)
add_custom_target(lupdate DEPENDS ${LUPDATE_VAR})
qt_add_translation(LRELEASE_VAR ${TRANSLATIONS})
add_custom_target(lrelease ALL DEPENDS ${LRELEASE_VAR})

# Doxygen
find_package(Doxygen)
doxygen_add_docs(doc ${CMAKE_SOURCE_DIR} ALL CONFIG_FILE "${CMAKE_SOURCE_DIR}/Doxyfile")
doxygen_add_docs(dev_doc ${CMAKE_SOURCE_DIR} CONFIG_FILE "${CMAKE_SOURCE_DIR}/Doxyfile.internal")

# Install
install(TARGETS shutdownmonitor)
install(FILES "$<LIST:TRANSFORM,${TRANSLATIONS},REPLACE,\\.ts$,.qm>" DESTINATION "share/shutdownmonitor/translations/")
