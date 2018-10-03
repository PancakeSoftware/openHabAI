# #####################################################
# ## test-uWebsockets

# c++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# extrutable / Lib
add_executable(
        uWebsockets-example
        uWebsockets-example.cpp)

# Include directories
target_include_directories(uWebsockets-example
        PUBLIC
        ${DEFAULT_INCLUDE_DIRECTORIES})

# Libraries
target_link_libraries(uWebsockets-example
        Boost::filesystem
        Boost::system
        ${DEPENDENCIES_UWEBSOCKETS_LIB})

add_dependencies(uWebsockets-example
        uWebSockets_external)