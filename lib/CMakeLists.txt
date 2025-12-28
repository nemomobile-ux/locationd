cmake_minimum_required(VERSION 3.16)
project(locationd_lib)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core DBus Network Positioning)

# Папка с XML-файлами для адаптеров
set(XML_DIR ${CMAKE_CURRENT_SOURCE_DIR}/xml)


set(SRCS
    pluginmanager.cpp
    accuracy.cpp
    satinfofull.cpp
    geoclue1service.cpp
    positionadaptor.cpp
    satelliteadaptor.cpp
    velocityadaptor.cpp
)

set(HEADERS
    pluginmanager.h
    ilocationprovider.h
    accuracy.h
    satinfofull.h
    geoclue1service.h
    positionadaptor.h
    satelliteadaptor.h
    velocityadaptor.h
)

# Создание shared библиотеки
add_library(location SHARED
    ${SRCS}
    ${HEADERS}
)

# Пути include (чтобы найти сгенерированные *_adaptor.h)
target_include_directories(location PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

# Линковка с Qt
target_link_libraries(location Qt6::Core Qt6::DBus Qt6::Positioning Qt6::Network)

# Установка
install(TARGETS location
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
