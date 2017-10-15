cmake_minimum_required(VERSION 3.0)


include(trainServer/cmake/HunterGate.cmake)
HunterGate(
        URL "https://github.com/ruslo/hunter/archive/v0.16.15.tar.gz"
        SHA1 "6974c2150fc0d3b09de3ad1efcbf15d360647ffa"
)

project(OpenHabAI)

add_subdirectory(frontend)
add_subdirectory(trainServer)