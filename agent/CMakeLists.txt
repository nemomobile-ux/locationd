find_package(PkgConfig REQUIRED)
pkg_check_modules(NOTIFICATIONS REQUIRED nemonotifications-qt6)

find_package(Qt6 REQUIRED COMPONENTS Core Gui DBus)

add_executable(locationd-agent main.cpp authorizationagent.cpp authorizationagent.h)
target_include_directories(locationd-agent PRIVATE ${NOTIFICATIONS_INCLUDE_DIRS})
target_link_libraries(locationd-agent PRIVATE Qt6::Core Qt6::Gui Qt6::DBus ${NOTIFICATIONS_LIBRARIES})
target_compile_options(locationd-agent PRIVATE ${NOTIFICATIONS_CFLAGS_OTHER})

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/locationd-agent.service.in
    ${CMAKE_CURRENT_BINARY_DIR}/locationd-agent.service
    @ONLY
)

install(TARGETS locationd-agent RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/locationd-agent.service
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/systemd/user)