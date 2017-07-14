include(ExternalProject)

# compile dependencies


# -- TENSORFLOW -------------------------------------------------
set(DEPENDENCIES_TENSORFLOW_DIR     /home/joshua/Downloads/tensorflow-cpp/lib/      PARENT_SCOPE)
set(DEPENDENCIES_TENSORFLOW_INCLUDE /home/joshua/Downloads/tensorflow-cpp/include/  PARENT_SCOPE)
set(DEPENDENCIES_TENSORFLOW_LIB     tensorflow                                      PARENT_SCOPE)




# -- SEASOCKS -------------------------------------------------
ExternalProject_Add(seasocks
        GIT_REPOSITORY https://github.com/mattgodbolt/seasocks.git
        PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/seasocks"
        BUILD_IN_SOURCE 1
        INSTALL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/seasocksInstall

        CMAKE_ARGS
            "-DCMAKE_INSTALL_PREFIX=${OUTPUT}"
            "-DUNITTESTS=OFF"
        )



# -- TENSORFLOW -------------------------------------------------
find_package(SDL2 QUIET)

if (SDL2_FOUND)
    message(STATUS "[DEPENDENCY] SDL2 => Found includeDir: ${SDL2_INCLUDE_DIRS}, libDir: ${SDL2_LIBDIR}")

    set(DEPENDENCIES_SDL2_INCLUDE ${SDL2_INCLUDE_DIRS} PARENT_SCOPE)
    set(DEPENDENCIES_SDL2_DIR ${SDL2_LIBDIR} PARENT_SCOPE)
    set(DEPENDENCIES_SDL2_LIB SDL2 PARENT_SCOPE)
else ()
    message(STATUS "[DEPENDENCY] SDL2 => NOT found => will compile SDL")

endif ()


# -- GLM -------------------------------------------------
set(DEPENDENCIES_GLM_INCLUDE ${CMAKE_SOURCE_DIR}/dependencies/glm/glm PARENT_SCOPE)