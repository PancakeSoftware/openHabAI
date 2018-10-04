
# Set output directories
function(setOutDir ALL BIN LIB INCLUDE ARCH PACK)
    set(OUTPUT                          ${CMAKE_SOURCE_DIR}/${ALL} PARENT_SCOPE)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_SOURCE_DIR}/${BIN} PARENT_SCOPE)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY  ${CMAKE_SOURCE_DIR}/${LIB} PARENT_SCOPE)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY  ${CMAKE_SOURCE_DIR}/${ARCH} PARENT_SCOPE)

    set(OUTPUT_BIN                      ${CMAKE_SOURCE_DIR}/${BIN} PARENT_SCOPE)
    set(OUTPUT_LIB                      ${CMAKE_SOURCE_DIR}/${LIB} PARENT_SCOPE)
    set(OUTPUT_INCLUDE                  ${CMAKE_SOURCE_DIR}/${INCLUDE} PARENT_SCOPE)
    set(OUTPUT_ARCHIVE                  ${CMAKE_SOURCE_DIR}/${ARCH} PARENT_SCOPE)
    set(OUTPUT_PACKAGE                  ${CMAKE_SOURCE_DIR}/${PACK} PARENT_SCOPE)

    # create dirs
    file(MAKE_DIRECTORY ${ALL})
    file(MAKE_DIRECTORY ${BIN})
    file(MAKE_DIRECTORY ${LIB})
    file(MAKE_DIRECTORY ${INCLUDE})
endfunction(setOutDir)


# Meta information about the project
function(setMetaProject
        NAME NAME_HEADER DISCRIPITOIN
        AUTHOR)
    set(META_PROJECT_NAME ${NAME} PARENT_SCOPE)
    set(META_PROJECT_DESCRIPTION ${DISCRIPITOIN} PARENT_SCOPE)
    set(META_PROJECT_AUTHOR ${AUTHOR} PARENT_SCOPE)

    # default version
    set(VERSION_MAJOR 0)
    set(VERSION_MINOR 0)
    set(VERSION_PATCH 0)

    # get verion from git
    find_package(Git)
    if (GIT_FOUND)
        execute_process(
                COMMAND ${GIT_EXECUTABLE} describe --match "v[0-9]*" --tags
                OUTPUT_VARIABLE VERSION_GIT
                OUTPUT_STRIP_TRAILING_WHITESPACE)

        #parse the version information into pieces.
        if (("${VERSION_GIT}" STREQUAL "")  OR  ("${VERSION_GIT}" MATCHES "fatal"))
            message(STATUS "!! no version from git found, set to 0.0.0")
        else()
            string(REGEX REPLACE "^v([0-9]+)\\..*" "\\1" VERSION_MAJOR "${VERSION_GIT}")
            string(REGEX REPLACE "^v[0-9]+\\.([0-9]+).*" "\\1" VERSION_MINOR "${VERSION_GIT}")
            string(REGEX REPLACE "^v[0-9]+\\.[0-9]+\\.([0-9]+).*" "\\1" VERSION_PATCH "${VERSION_GIT}")
        endif()
    endif ()


    set(META_VERSION_MAJOR ${VERSION_MAJOR} PARENT_SCOPE)
    set(META_VERSION_MINOR ${VERSION_MINOR} PARENT_SCOPE)
    set(META_VERSION_PATCH ${VERSION_PATCH} PARENT_SCOPE)
    set(VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
    set(META_VERSION ${VERSION} PARENT_SCOPE)
    set(META_NAME_VERSION "${NAME}-${VERSION}" PARENT_SCOPE)
    set(META_NAME_HEADER ${NAME_HEADER} PARENT_SCOPE)

    message(STATUS "${NAME}: v${VERSION}")
endfunction()