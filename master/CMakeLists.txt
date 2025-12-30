set(PROJECT master)

set(SRC
    main.cpp
)

add_executable(${PROJECT}
    ${SRC}
    ${HEADERS}
)

target_include_directories(${PROJECT} PRIVATE
    ${CMAKE_SOURCE_DIR}/lib
)

target_link_libraries(${PROJECT} PUBLIC
    Qt6::Core
    Qt6::DBus
    Qt6::Network
    Qt6::Positioning
    location
)

configure_file(
    ${CMAKE_SOURCE_DIR}/master/org.freedesktop.Geoclue.Master.service.in
    ${CMAKE_BINARY_DIR}/master/org.freedesktop.Geoclue.Master.service
    @ONLY
)

install(TARGETS ${PROJECT} RUNTIME
    DESTINATION ${CMAKE_INSTALL_LIBEXECDIR}/locationd/)

install(FILES ${CMAKE_BINARY_DIR}/master/org.freedesktop.Geoclue.Master.service
    DESTINATION DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/dbus-1/services)
