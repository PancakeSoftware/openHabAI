include(ExternalProject)
include("../cmake/HunterGate.cmake")

# compile dependencies

# -- MXNET -------------------------------------------------
if (OPTION_BUILD_DEPS)
    IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin") # apple
        message(STATUS "[Dependency] will use 'mxnet' on APPLE")
        set(MXNET_USE_BLAS apple)
        set(MXNET_USE_OPENMP 0)
    elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
        message(STATUS "[Dependency] will use 'mxnet' on LINUX")
        set(MXNET_USE_BLAS openblas)
        set(MXNET_USE_OPENMP 1)
    endif()

    ExternalProject_Add(mxnet_external
            GIT_REPOSITORY https://github.com/dmlc/mxnet
            GIT_TAG 1.3.0

            PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/mxnet"
            BUILD_IN_SOURCE 1
            INSTALL_DIR ${OUTPUT}

            CMAKE_ARGS
            -DCMAKE_INSTALL_PREFIX=${OUTPUT}
            -DTESTS=OFF
            -DUSE_CPP_PACKAGE=ON
            -DUSE_BLAS=${MXNET_USE_BLAS}
            -DUSE_CUDA=0
            -DUSE_OPENCV=0
            -DUSE_OPENMP=${MXNET_USE_OPENMP}
            -DUSE_MXNET_LIB_NAMING=1
            )

    add_library(mxnet SHARED IMPORTED)
endif ()

set(DEPENDENCIES_MXNET_LIB mxnet PARENT_SCOPE)
set(DEPENDENCIES_MXNET_DIR ${CMAKE_CURRENT_SOURCE_DIR}/mxnet/src/mxnet_external/ PARENT_SCOPE)
set(DEPENDENCIES_MXNET_INCLUDE
        ${CMAKE_CURRENT_SOURCE_DIR}/mxnet/src/mxnet_external/cpp-package/include
        ${CMAKE_CURRENT_SOURCE_DIR}/mxnet/src/mxnet_external/3rdparty/tvm/nnvm/include
        ${CMAKE_CURRENT_SOURCE_DIR}/mxnet/src/mxnet_external/include PARENT_SCOPE)


# -- SEASOCKS -------------------------------------------------
if (OPTION_USE_SEASOCKS)
    if (OPTION_BUILD_DEPS)
        ExternalProject_Add(seasocks_external
                GIT_REPOSITORY https://github.com/mattgodbolt/seasocks.git
                PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/seasocks"
                #BUILD_IN_SOURCE 1

                CMAKE_ARGS
                "-DCMAKE_INSTALL_PREFIX=${OUTPUT}"
                "-DUNITTESTS=OFF"
                )
        add_library(seasocks SHARED IMPORTED)
    endif ()

    set(DEPENDENCIES_SEASOCKS_LIB seasocks z PARENT_SCOPE)
    set(DEPENDENCIES_SEASOCKS_LIB_EXTERNAL seasocks_external PARENT_SCOPE)
    set(DEPENDENCIES_SEASOCKS_DEFINITIONS USE_SEASOCKS PARENT_SCOPE)
    message(STATUS "[Dependency] will use 'Seasocks'")
else()
    # empty dummy target
    add_custom_target(seasocks_external)
endif ()


# -- JSON -------------------------------------------------
message(STATUS "[Dependency] will use 'nlohmann/json'")
if (NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/json/json.hpp")
    file(DOWNLOAD "https://github.com/nlohmann/json/releases/download/v3.1.2/json.hpp" "${CMAKE_CURRENT_SOURCE_DIR}/json/json.hpp")
endif()
set(DEPENDENCIES_JSON_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/json PARENT_SCOPE)


# -- EXPRTK -----------------------------------------------
message(STATUS "[Dependency] will use 'exprtk'")
if (NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/exprtk/exprtk.hpp")
    file(DOWNLOAD "https://raw.githubusercontent.com/ArashPartow/exprtk/master/exprtk.hpp" "${CMAKE_CURRENT_SOURCE_DIR}/exprtk/exprtk.hpp")
endif()
set(DEPENDENCIES_EXPRTK_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/exprtk PARENT_SCOPE)


#ExternalProject_Add(exprtk_external
#        GIT_REPOSITORY https://github.com/ArashPartow/exprtk.git

#        PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/exprtk"
#        BUILD_IN_SOURCE 1
#        )




# -- websocket: uWebSockets -----------------------------------------------
if (OPTION_BUILD_DEPS)
    message(STATUS "[Dependency] will use 'uWebSockets'")
    IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
        set(LIB_SUFIX dylib)
    elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
        set(LIB_SUFIX so)
    endif()
    ExternalProject_Add(uWebSockets_external
            GIT_REPOSITORY https://github.com/uNetworking/uWebSockets.git
            GIT_TAG v0.14.8
            PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/uWebSockets"

            BUILD_IN_SOURCE 1
            CONFIGURE_COMMAND ""
            BUILD_COMMAND ${MAKE}
            INSTALL_COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/uWebSockets/src/uWebSockets_external/libuWS.${LIB_SUFIX} ${OUTPUT_LIB}
            )
else()
    # empty dummy target
    add_custom_target(uWebSockets_external)
endif ()

set(DEPENDENCIES_UWEBSOCKETS_LIB
        uWS pthread ssl crypto z uv
        PARENT_SCOPE)
set(DEPENDENCIES_UWEBSOCKETS_DIR
        ${CMAKE_CURRENT_SOURCE_DIR}/uWebSockets/src/uWebSockets_external
        /usr/local/opt/openssl/lib
        /usr/local/opt/libuv/lib
        PARENT_SCOPE)
set(DEPENDENCIES_UWEBSOCKETS_INCLUDE
        ${CMAKE_CURRENT_SOURCE_DIR}/uWebSockets/src/uWebSockets_external/src
        /usr/local/opt/openssl/include
        /usr/local/opt/libuv/include
        PARENT_SCOPE)