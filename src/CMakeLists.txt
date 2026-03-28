cmake_minimum_required(VERSION 3.20)
project(my-oatpp-app)

set(CMAKE_CXX_STANDARD 17)

find_package(oatpp 1.3.0 REQUIRED)

add_executable(my-oatpp-app src/main.cpp)

target_link_libraries(my-oatpp-app
    PRIVATE oatpp::oatpp
)