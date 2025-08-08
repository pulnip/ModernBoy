add_subdirectory(core)
add_subdirectory(game)
add_subdirectory(engine)
add_subdirectory(shader)

add_executable(${PROJECT_NAME}
    main.cpp
)
target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall -Wextra
    -fsanitize=address,leak
)
target_link_options(${PROJECT_NAME} PRIVATE
    -fsanitize=address,leak
)
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${ImGui_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Core
    Engine
    Game
    SDL3::SDL3
)

if(APPLE AND (CMAKE_GENERATOR MATCHES "Ninja"))
    find_program(DSYMUTIL_EXECUTABLE dsymutil)
    if(DSYMUTIL_EXECUTABLE)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${DSYMUTIL_EXECUTABLE} $<TARGET_FILE:${PROJECT_NAME}>
            COMMENT "Generating dSYM for $<TARGET_FILE_NAME:${PROJECT_NAME}>"
            VERBATIM)
    endif()
endif()