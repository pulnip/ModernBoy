file(GLOB ALL_SOURCES "*.cpp")
list(REMOVE_ITEM ALL_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/as_compiler.cpp)
add_library(ScriptModule STATIC ${ALL_SOURCES})

target_compile_options(ScriptModule PRIVATE
    -Wall -Wextra
    -fsanitize=address
)
target_compile_definitions(ScriptModule PRIVATE
    $<IF:$<CONFIG:DEBUG>, _DEBUG, NDEBUG>
)

target_include_directories(ScriptModule PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    # angelscript
    ${CMAKE_SOURCE_DIR}/external/angelscript/sdk/angelscript/include
)
target_link_libraries(ScriptModule PRIVATE
    # angelscript
    angelscript
    # SDL3
    SDL3::SDL3
)

add_executable(ASCompiler
    as_compiler.cpp
    as_helper.cpp
    as_stream.cpp
    as_typehelper.cpp
)
set(AS_MODULE_NAME ${PROJECT_NAME})
set(AS_SRC_DIR "${CMAKE_SOURCE_DIR}/src/script")
file(GLOB AS_SOURCES "${AS_SRC_DIR}/*.as")
set(AS_TGT_DIR "${CMAKE_SOURCE_DIR}/asset/script")
set(AS_TARGET "${AS_TGT_DIR}/${AS_MODULE_NAME}.asb")

target_compile_options(ASCompiler PRIVATE
    -Wall -Wextra
)
target_compile_definitions(ASCompiler PRIVATE
    $<IF:$<CONFIG:DEBUG>, _DEBUG, NDEBUG>
)

target_include_directories(ASCompiler PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    # angelscript
    ${CMAKE_SOURCE_DIR}/external/angelscript/sdk/angelscript/include
)
target_link_libraries(ASCompiler PRIVATE
    # angelscript
    angelscript
)

add_custom_command(
    OUTPUT ${AS_TARGET}
    COMMAND ASCompiler ${AS_MODULE_NAME} ${AS_SOURCES}
    COMMAND mv "${AS_SRC_DIR}/${AS_MODULE_NAME}.asb" ${AS_TGT_DIR}
    DEPENDS ASCompiler ${AS_SOURCES}
    WORKING_DIRECTORY ${AS_SRC_DIR}
    COMMENT "Building all AngelScript scripts into bytecode"
)
add_custom_target(
    build_scripts ALL
    DEPENDS ${AS_TARGET}
)
