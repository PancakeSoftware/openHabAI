add_custom_target (
    frontend
    COMMAND cd ${CMAKE_CURRENT_SOURCE_DIR} && npm install && npm run build
)

add_custom_target (
        frontendRun
        COMMAND cd ${CMAKE_CURRENT_SOURCE_DIR} && npm run server
)

add_dependencies(frontendRun frontend)