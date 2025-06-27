file(GLOB ALL_SOURCES "*.cpp")
set(AS_ADD_ON_ROOT ${CMAKE_SOURCE_DIR}/external/angelscript/sdk/add_on)
add_library(ScriptModule STATIC
    ${ALL_SOURCES}
    ${AS_ADD_ON_ROOT}/scriptstdstring/scriptstdstring.cpp
    ${AS_ADD_ON_ROOT}/scriptstdstring/scriptstdstring_utils.cpp
    ${AS_ADD_ON_ROOT}/scriptbuilder/scriptbuilder.cpp
)

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
    ${AS_ADD_ON_ROOT}
)
target_link_libraries(ScriptModule PRIVATE
    # angelscript
    angelscript
    # SDL3
    SDL3::SDL3
)

# add_executable(ASCompiler
#     as_compiler.cpp
#     as_helper.cpp
#     as_stream.cpp
#     as_typehelper.cpp
# )
# set(AS_SRC_DIR "${CMAKE_SOURCE_DIR}/src/script")
# set(AS_TGT_DIR "${CMAKE_SOURCE_DIR}/asset/script")
# file(GLOB SCRIPT_FILES "${CMAKE_SOURCE_DIR}/src/script/*.as")
# foreach(AS_SCRIPT IN LISTS SCRIPT_FILES)
#     get_filename_component(AS_MODULE ${AS_SCRIPT} NAME_WE)
#     set(AS_TARGET "${AS_TGT_DIR}/${AS_MODULE}.asb")
#     add_custom_command(
#         OUTPUT ${AS_TARGET}
#         COMMAND ASCompiler ${AS_MODULE} ${AS_SCRIPT}
#         COMMAND mv ${AS_SRC_DIR}/${AS_MODULE}.asb ${AS_TGT_DIR}
#         DEPENDS ASCompiler ${AS_SCRIPT}
#         WORKING_DIRECTORY ${AS_SRC_DIR}
#         COMMENT "Building all AngelScript scripts into bytecode"
#     )
#     list(APPEND AS_TARGETS ${AS_TARGET})
# endforeach()

# add_custom_target(
#     AS_Scripts ALL
#     DEPENDS ${AS_TARGETS}
# )

# target_compile_options(ASCompiler PRIVATE
#     -Wall -Wextra
# )
# target_compile_definitions(ASCompiler PRIVATE
#     $<IF:$<CONFIG:DEBUG>, _DEBUG, NDEBUG>
# )

# target_include_directories(ASCompiler PRIVATE
#     ${CMAKE_SOURCE_DIR}/include
#     # angelscript
#     ${CMAKE_SOURCE_DIR}/external/angelscript/sdk/angelscript/include
# )
# target_link_libraries(ASCompiler PRIVATE
#     # angelscript
#     angelscript
# )
