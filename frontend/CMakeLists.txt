add_custom_target (
    frontend
    COMMAND cd ${CMAKE_CURRENT_SOURCE_DIR} && npm install && gulp
)

add_custom_target (
        frontendRun
        COMMAND cd ${CMAKE_CURRENT_SOURCE_DIR} && gulp webserver
)

add_dependencies(frontendRun frontend)