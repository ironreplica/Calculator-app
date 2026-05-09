cmake_minimum_required(VERSION 3.20)
project(my-oatpp-app)

set(CMAKE_CXX_STANDARD 17)

# No version number = accepts whatever is installed (1.3.0 in Docker, 1.4.0 on Windows)
find_package(oatpp REQUIRED)

include_directories(include)

add_executable(my-oatpp-app
    calculator_functionality.cpp
    ExpressionTree.cpp
    OatApp.cpp
)

target_link_libraries(my-oatpp-app
    PRIVATE oatpp::oatpp
)