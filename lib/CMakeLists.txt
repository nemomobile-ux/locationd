cmake_minimum_required(VERSION 3.16)
project(locationd_lib)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core DBus Network Positioning)

qt_add_dbus_adaptor(GENERATED_SOURCES xml/gc-iface-master-full.xml locationdaemonclientresolver.h LocationDaemonClientResolver masteradaptor)

qt_add_dbus_adaptor(GENERATED_SOURCES xml/gc-iface-geoclue-full.xml locationdaemonpositionresolver.h LocationDaemonPositionResolver geoclueadaptor)
qt_add_dbus_adaptor(GENERATED_SOURCES xml/gc-iface-master-client-full.xml locationdaemonpositionresolver.h LocationDaemonPositionResolver masterclientdaptor)
qt_add_dbus_adaptor(GENERATED_SOURCES xml/gc-iface-address-full.xml locationdaemonpositionresolver.h LocationDaemonPositionResolver addressadaptor)
qt_add_dbus_adaptor(GENERATED_SOURCES xml/gc-iface-position-full.xml locationdaemonpositionresolver.h LocationDaemonPositionResolver positionadaptor)
qt_add_dbus_adaptor(GENERATED_SOURCES xml/gc-iface-satellite-full.xml locationdaemonpositionresolver.h LocationDaemonPositionResolver satelliteadaptor)


#qt_add_dbus_adaptor(GENERATED_SOURCES xml/gc-iface-geocode-full.xml geoclue1service.h GeoClue1Service geocodeadaptor)
#qt_add_dbus_adaptor(GENERATED_SOURCES xml/gc-iface-reverse-geocode-full.xml geoclue1service.h GeoClue1Service reversegeocodeadaptor)
#qt_add_dbus_adaptor(GENERATED_SOURCES xml/gc-iface-satellite-full.xml geoclue1service.h GeoClue1Service satelliteadaptor)
#qt_add_dbus_adaptor(GENERATED_SOURCES xml/gc-iface-velocity-full.xml geoclue1service.h GeoClue1Service velocityadaptor)


set(SRCS
    pluginmanager.cpp
    geocluetypes.cpp
    geoclue1service.cpp
    locationdaemonclientresolver.cpp
    locationdaemonpositionresolver.cpp
)

set(HEADERS
    pluginmanager.h
    ilocationprovider.h
    geoclue1service.h
    geocluetypes.h
    locationdaemonclientresolver.h
    locationdaemonpositionresolver.h
)

add_library(location SHARED
    ${SRCS}
    ${HEADERS}
    ${GENERATED_SOURCES}
)

target_include_directories(location PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(location Qt6::Core Qt6::DBus Qt6::Positioning Qt6::Network)

install(TARGETS location
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
