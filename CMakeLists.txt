cmake_minimum_required(VERSION 3.16)
project(kydra VERSION 1.3.3)
set(PROJECT_VERSION "1.3.2")
set(PROJECT_VERSION_MAJOR 5)

find_package(ECM REQUIRED NO_MODULE)

set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

# Prefer Qt5 but allow Qt6 if needed
set(QT_MAJOR_VERSION 5)

find_package(Qt5 5.15.0 REQUIRED CONFIG COMPONENTS Widgets Concurrent Qml Quick)
message(STATUS "Building with Qt5 - Qt6 features will be compiled but may have limited functionality")
set(QT_VERSION_MAJOR 5)
set(KF_VERSION "KF5")

include(KDEInstallDirs)
include(KDECMakeSettings NO_POLICY_SCOPE)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMInstallIcons)
include(ECMMarkAsTest)
include(GenerateExportHeader)

find_package(${KF_VERSION} REQUIRED KIO DBusAddons I18n IconThemes XmlGui)

# Kirigami framework for modern UI
# Note: Requires libkf5kirigami2-dev package to be installed
find_package(KF5Kirigami2)
if(NOT KF5Kirigami2_FOUND)
    message(WARNING "KF5Kirigami2 not found - Kirigami UI will be disabled. Install libkf5kirigami2-dev to enable.")
    set(WITH_KIRIGAMI OFF)
else()
    set(WITH_KIRIGAMI ON)
    add_definitions(-DHAVE_KIRIGAMI)
endif()

find_package(QApt 3.0.2 REQUIRED)
# Skip DebconfKDE to avoid Qt6 dependency conflicts
# find_package(DebconfKDE 1.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(MUON_VERSION ${PROJECT_VERSION})
configure_file(KydraVersion.h.in KydraVersion.h)

add_subdirectory(src)

set_package_properties(QApt PROPERTIES
    DESCRIPTION "Qt wrapper around the libapt-pkg library"
    PURPOSE "Used to support apt-based distribution systems"
    TYPE REQUIRED)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
