add_library(Util STATIC
    as_helper.cpp
    as_stream.cpp
    as_typehelper.cpp
)

add_executable(ASCompiler as_compiler.cpp)
set(AS_MODULE_NAME ${PROJECT_NAME})
set(AS_SRC_DIR "${CMAKE_SOURCE_DIR}/src/script")
file(GLOB AS_SOURCES "${AS_SRC_DIR}/*.as")
set(AS_TGT_DIR "${CMAKE_SOURCE_DIR}/asset/script")
set(AS_TARGET "${AS_TGT_DIR}/${AS_MODULE_NAME}.asb")

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