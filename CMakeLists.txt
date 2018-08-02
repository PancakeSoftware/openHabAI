cmake_minimum_required(VERSION 3.0)


include(trainServer/cmake/HunterGate.cmake)
HunterGate(
        URL "https://github.com/ruslo/hunter/archive/v0.16.15.tar.gz"
        SHA1 "6974c2150fc0d3b09de3ad1efcbf15d360647ffa"
)

project(OpenHabAI)


# Include cmake modules
include(trainServer/cmake/buildHelper.cmake)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "cmake")


# ------------------------------------
# -- Project infos

# Meta information about the project
setMetaProject(
        "OpenHabAI"  "OPENHAB_AI"               # name, name in headerfile,
        "ai for openhab"                        # descrition
        "Joshua Johannson"                      # author
)


# set project name and output dir
project(${META_PROJECT_NAME})


# ------------------------------------
# -- Project options
option(OPTION_TESTS         "do tests." ON)
option(OPTION_BUILD_DEPS    "Build dependencies." ON)
option(OPTION_USE_SEASOCKS  "use seasocks lib for websocket comunication" OFF)


# -----------------------------
# -- set BUILD out dirs
setOutDir(
        build                               # general
        build/bin                           # bin
        build/lib                           # lib
        build/include                       # include
        build/lib                           # archiv
        build/pack                          # package / installer / app
)

# ----------------------------
# c++ options
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wno-unused-variable")

# -----------------------------
# -- set INSTALL out dirs
set(INSTALL_BIN bin)
set(INSTALL_LIB lib)
set(INSTALL_INCLUDE include)
set(INSTALL_COMPONENTS applications libraries headers)



# -----------------------------
# -- default SOURCE dirs for targets
set(SOURCE_DIR source/)
set(TEST_DIR_GLOB ${CMAKE_CURRENT_SOURCE_DIR}/test/)
set(INCLUDE_DIR include/)



# -------------------------------------------------
# -- Dependences
add_subdirectory(trainServer/dependencies)

# -- BOOST
hunter_add_package(Boost COMPONENTS filesystem)
find_package(Boost CONFIG REQUIRED system filesystem)

# -- GTEST
hunter_add_package(GTest)
find_package(GTest CONFIG REQUIRED)

# ------------------------
link_directories(
        ${OUTPUT_LIB}
        ${DEPENDENCIES_TENSORFLOW_DIR}
        ${DEPENDENCIES_MXNET_DIR}
        ${DEPENDENCIES_UWEBSOCKETS_DIR}
)


# default headers, libs
set(DEFAULT_INCLUDE_DIRECTORIES
        ${DEPENDENCIES_JSON_INCLUDE}
        ${DEPENDENCIES_MXNET_INCLUDE}
        ${OUTPUT_INCLUDE}
        ${DEPENDENCIES_EXPRTK_INCLUDE}
        ${DEPENDENCIES_UWEBSOCKETS_INCLUDE}
        )

set(DEFAULT_LIBRARIES
        ${DEPENDENCIES_TENSORFLOW_LIB}
        ${DEPENDENCIES_SEASOCKS_LIB}
        ${DEPENDENCIES_MXNET_LIB}
        ${DEPENDENCIES_UWEBSOCKETS_LIB}
        pthread
        Boost::filesystem
        Boost::system
        )

set(DEFAULT_DEPENDENCIES
        mxnet_external
        uWebSockets_external
        ${DEPENDENCIES_SEASOCKS_LIB_EXTERNAL}
        )

set(DEFAUT_DEFINITIONS
        ${DEPENDENCIES_SEASOCKS_DEFINITIONS})




# --------------------------------------------------
# -- subdirs
add_subdirectory(catflow/cpp)
add_subdirectory(catflow/example)
add_subdirectory(frontend)
add_subdirectory(trainServer)