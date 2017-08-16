include(ExternalProject)

# compile dependencies

# -- MXNET -------------------------------------------------
ExternalProject_Add(mxnet_external
        GIT_REPOSITORY https://github.com/dmlc/mxnet
        GIT_TAG 0.11.0.rc1

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
set(DEPENDENCIES_MXNET_DIR     ${CMAKE_CURRENT_SOURCE_DIR}/mxnet/src/mxnet_external/                        PARENT_SCOPE)
set(DEPENDENCIES_MXNET_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/mxnet/src/mxnet_external/cpp-package/include
                               ${CMAKE_CURRENT_SOURCE_DIR}/mxnet/src/mxnet_external/nnvm/include
                               ${CMAKE_CURRENT_SOURCE_DIR}/mxnet/src/mxnet_external/include                 PARENT_SCOPE)
set(DEPENDENCIES_MXNET_LIB     mxnet                                                                       PARENT_SCOPE)



# -- SEASOCKS -------------------------------------------------
ExternalProject_Add(seasocks_external
        GIT_REPOSITORY https://github.com/mattgodbolt/seasocks.git
        PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/seasocks"
        #BUILD_IN_SOURCE 1

        CMAKE_ARGS
            "-DCMAKE_INSTALL_PREFIX=${OUTPUT}"
            "-DUNITTESTS=OFF"
        )
add_library(seasocks SHARED IMPORTED)
set(DEPENDENCIES_SEASOCKS_LIB     seasocks           PARENT_SCOPE)



# -- JSON -------------------------------------------------
set(DEPENDENCIES_JSON_INCLUDE     ${CMAKE_CURRENT_SOURCE_DIR}/json           PARENT_SCOPE)