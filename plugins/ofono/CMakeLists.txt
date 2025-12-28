set(PLUGINNAME cell)

set(SRC ${PLUGINNAME}plugin.cpp)
set(HEADERS ${PLUGINNAME}plugin.h
    ${CMAKE_SOURCE_DIR}/lib/ilocationprovider.h)

set(CMAKE_AUTOMOC ON)

qt_add_library(${PLUGINNAME}
    MODULE
    ${SRC}
    ${HEADERS}
)

target_include_directories(${PLUGINNAME} PRIVATE ${CMAKE_SOURCE_DIR}/lib)

target_link_libraries(${PLUGINNAME} PRIVATE
    Qt6::Core
    Qt6::Network
    Qt6::DBus
    Qt6::Positioning)

set_target_properties(${PLUGINNAME} PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins
)

install(TARGETS ${PLUGINNAME}
    LIBRARY DESTINATION /usr/lib/locationd/plugins
)
