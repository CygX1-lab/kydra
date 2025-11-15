cmake_minimum_required(VERSION 3.16)
project(kydra)
set(PROJECT_VERSION "0.9.0")
set(PROJECT_VERSION_MAJOR 5)

find_package(ECM REQUIRED NO_MODULE)

set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

# Prefer Qt5 but allow Qt6 if needed
set(QT_MAJOR_VERSION 5)

find_package(Qt5 5.15.0 REQUIRED CONFIG COMPONENTS Widgets Concurrent)
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
