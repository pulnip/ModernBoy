add_library(Util STATIC
    as_helper.cpp
    as_stream.cpp
    as_typehelper.cpp
)

add_executable(ASCompiler as_compiler.cpp)
set(AS_MODULE_NAME ${PROJECT_NAME})
set(AS_SCRIPT_DIR "${CMAKE_SOURCE_DIR}/assets/script")
file(GLOB AS_SCRIPT_FILES "${AS_SCRIPT_DIR}/*.as")
message(STATUS "files: ${AS_SCRIPT_FILES}")

add_custom_command(
    OUTPUT "${AS_SCRIPT_DIR}/${AS_MODULE_NAME}.asb"
    COMMAND ASCompiler ${AS_MODULE_NAME} ${AS_SCRIPT_FILES}
    DEPENDS ASCompiler ${AS_SCRIPT_FILES}
    WORKING_DIRECTORY ${AS_SCRIPT_DIR}
    COMMENT "Building all AngelScript scripts into bytecode"
)
add_custom_target(
    build_scripts ALL
    DEPENDS "${AS_SCRIPT_DIR}/${AS_MODULE_NAME}.asb"
)

target_compile_options(Util PRIVATE
    -Wall -Wextra
)
target_compile_definitions(Util PRIVATE
    $<IF:$<CONFIG:DEBUG>, _DEBUG, NDEBUG>
)
target_compile_options(ASCompiler PRIVATE
    -Wall -Wextra
)
target_compile_definitions(ASCompiler PRIVATE
    $<IF:$<CONFIG:DEBUG>, _DEBUG, NDEBUG>
)

target_include_directories(Util PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    # angelscript
    ${CMAKE_SOURCE_DIR}/external/angelscript/sdk/angelscript/include
)
target_include_directories(ASCompiler PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    # angelscript
    ${CMAKE_SOURCE_DIR}/external/angelscript/sdk/angelscript/include
)
target_link_libraries(Util PRIVATE
    # angelscript
    angelscript
)
target_link_libraries(ASCompiler PRIVATE
    Util
    # angelscript
    angelscript
)