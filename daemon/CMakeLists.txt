set(PROJECT locationd)

set(SRC
    main.cpp
)

set(HEADERS
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
    ${CMAKE_SOURCE_DIR}/daemon/data/locationd.service.in
    ${CMAKE_BINARY_DIR}/daemon/locationd.service
    @ONLY
)


install(TARGETS ${PROJECT} RUNTIME
    DESTINATION ${CMAKE_INSTALL_BINDIR})

install(FILES ${CMAKE_BINARY_DIR}/daemon/locationd.service
        DESTINATION /usr/lib/systemd/system)

install(FILES data/org.freedesktop.Geoclue.conf
        DESTINATION /etc/dbus-1/system.d)
