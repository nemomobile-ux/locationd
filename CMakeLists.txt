cmake_minimum_required(VERSION 3.22)
project(locationd VERSION 1.0 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(QT_MIN_VERSION "6.2.0")
set(CMAKE_INSTALL_PREFIX /usr)

include(GNUInstallDirs)

find_package(Qt6 ${QT_MIN_VERSION} REQUIRED COMPONENTS
    Core
    DBus
    Network
    Positioning)

add_subdirectory(lib)
add_subdirectory(daemon)
add_subdirectory(master)
add_subdirectory(plugins)
