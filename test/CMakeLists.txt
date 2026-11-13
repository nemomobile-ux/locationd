find_package(Qt6 REQUIRED COMPONENTS Core DBus)

add_executable(test_location test_location.cpp)
target_link_libraries(test_location PRIVATE Qt6::Core Qt6::DBus)