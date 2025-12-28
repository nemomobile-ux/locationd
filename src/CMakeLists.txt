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

install(TARGETS ${PROJECT} RUNTIME
    DESTINATION ${CMAKE_INSTALL_BINDIR})
