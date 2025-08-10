add_library(sanitizers INTERFACE)
target_compile_options(sanitizers INTERFACE
    -fsanitize=address,leak -fno-omit-frame-pointer
)
target_link_options(sanitizers INTERFACE
    -fsanitize=address
)

add_subdirectory(core)
add_subdirectory(game)
add_subdirectory(engine)
add_subdirectory(shader)

add_executable(${PROJECT_NAME} main.cpp)
target_compile_options(${PROJECT_NAME} PRIVATE
PRIVATE
    $<IF:$<CONFIG:DEBUG>, -Wall -Wextra,>
)
target_include_directories(${PROJECT_NAME} PRIVATE
    ${ImGui_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Core
    Engine
    Game
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