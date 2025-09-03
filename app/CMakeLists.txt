option(MODERNBOY_USE_SANITIZER "Use Sanitizer" ON)

add_executable(${PROJECT_NAME} main.cpp)
target_compile_options(${PROJECT_NAME}
PRIVATE
    $<IF:$<CONFIG:DEBUG>, -Wall -Wextra,>
)

target_link_libraries(${PROJECT_NAME}
PRIVATE
    ImGui
    Game
    SDL3::SDL3
)

if(MODERNBOY_APP_USE_SANITIZER)
    target_link_libraries(Core
    PRIVATE
        $<IF:$<CONFIG:DEBUG>, sanitizer,>
    )
    target_link_libraries(Engine
    INTERFACE
        $<IF:$<CONFIG:DEBUG>, sanitizer,>
    )
    target_link_libraries(Game
    PRIVATE
        $<IF:$<CONFIG:DEBUG>, sanitizer,>
    )
    target_link_libraries(${PROJECT_NAME}
    PRIVATE
        $<IF:$<CONFIG:DEBUG>, sanitizer,>
    )
endif()

if(APPLE AND (CMAKE_GENERATOR MATCHES "Ninja"))
    find_program(DSYMUTIL_EXECUTABLE dsymutil)
    if(DSYMUTIL_EXECUTABLE)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${DSYMUTIL_EXECUTABLE} $<TARGET_FILE:${PROJECT_NAME}>
            COMMENT "Generating dSYM for $<TARGET_FILE_NAME:${PROJECT_NAME}>"
            VERBATIM)
    endif()
endif()
