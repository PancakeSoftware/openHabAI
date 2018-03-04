include(ExternalProject)
include("../cmake/HunterGate.cmake")

# compile dependencies

# -- MXNET -------------------------------------------------
if (OPTION_BUILD_DEPS)
    ExternalProject_Add(mxnet_external
            GIT_REPOSITORY https://github.com/dmlc/mxnet
            GIT_TAG 1.0.0.rc1

            PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/mxnet"
            BUILD_IN_SOURCE 1
            INSTALL_DIR ${OUTPUT}

            CMAKE_ARGS
            -DCMAKE_INSTALL_PREFIX=${OUTPUT}
            -DTESTS=OFF
            -DUSE_CPP_PACKAGE=ON
            -DUSE_BLAS=openblas
            -DUSE_CUDA=0
            -DUSE_OPENCV=0
            )

    add_library(mxnet SHARED IMPORTED)
endif ()

set(DEPENDENCIES_MXNET_LIB mxnet PARENT_SCOPE)
set(DEPENDENCIES_MXNET_DIR ${CMAKE_CURRENT_SOURCE_DIR}/mxnet/src/mxnet_external/ PARENT_SCOPE)
set(DEPENDENCIES_MXNET_INCLUDE
        ${CMAKE_CURRENT_SOURCE_DIR}/mxnet/src/mxnet_external/cpp-package/include
        ${CMAKE_CURRENT_SOURCE_DIR}/mxnet/src/mxnet_external/nnvm/include
        ${CMAKE_CURRENT_SOURCE_DIR}/mxnet/src/mxnet_external/include PARENT_SCOPE)


# -- SEASOCKS -------------------------------------------------
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

set(DEPENDENCIES_SEASOCKS_LIB seasocks PARENT_SCOPE)


# -- JSON -------------------------------------------------
set(DEPENDENCIES_JSON_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/json PARENT_SCOPE)


# -- EXPRTK -----------------------------------------------
if (NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/exprtk/exprtk.hpp")
    file(DOWNLOAD "https://raw.githubusercontent.com/ArashPartow/exprtk/master/exprtk.hpp" "${CMAKE_CURRENT_SOURCE_DIR}/exprtk/exprtk.hpp")
endif()
set(DEPENDENCIES_EXPRTK_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/exprtk PARENT_SCOPE)

#ExternalProject_Add(exprtk_external
#        GIT_REPOSITORY https://github.com/ArashPartow/exprtk.git

#        PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/exprtk"
#        BUILD_IN_SOURCE 1
#        )