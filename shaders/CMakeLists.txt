set(SHADER_DIR ${CMAKE_SOURCE_DIR}/shaders)
set(EXE_PATH ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
get_filename_component(EXE_DIR ${EXE_PATH} DIRECTORY)

if(RENDER_BACKEND STREQUAL "DirectX")
    set(VERTEX_SHADERS VS.hlsl)
    set(PIXEL_SHADERS PS.hlsl)

    foreach(SHADER_FILE IN LISTS VERTEX_SHADERS)
        get_filename_component(SHADER ${SHADER_FILE} NAME_WE)
        set(SHADER_SRC ${SHADER_DIR}/${SHADER_FILE})
        add_custom_command(
            OUTPUT ${EXE_PATH}/$<CONFIG>/${SHADER}.cso
            COMMAND ${SHADER_COMPILER} /T vs_5_0 /E main /Fo ${EXE_PATH}/$<CONFIG>/${SHADER}.cso ${SHADER_SRC}
            DEPENDS ${SHADER_SRC}
            COMMENT "Compiling Vertex Shader: ${SHADER_FILE}"
        )
        list(APPEND SHADER_OUTPUTS ${EXE_PATH}/$<CONFIG>/${SHADER}.cso)
    endforeach()
    foreach(SHADER_FILE IN LISTS PIXEL_SHADERS)
        get_filename_component(SHADER ${SHADER_FILE} NAME_WE)
        set(SHADER_SRC ${SHADER_DIR}/${SHADER_FILE})
        add_custom_command(
            OUTPUT ${EXE_PATH}/$<CONFIG>/${SHADER}.cso
            COMMAND ${SHADER_COMPILER} /T ps_5_0 /E main /Fo ${EXE_PATH}/$<CONFIG>/${SHADER}.cso ${SHADER_SRC}
            DEPENDS ${SHADER_SRC}
            COMMENT "Compiling Pixel Shader: ${SHADER_FILE}"
        )
        list(APPEND SHADER_OUTPUTS ${EXE_PATH}/$<CONFIG>/${SHADER}.cso)
    endforeach()

    add_custom_target(Shaders ALL DEPENDS ${SHADER_OUTPUTS})
    unset(SHADER_COMPILER)
    unset(VERTEX_SHADERS)
    unset(PIXEL_SHADERS)
elseif(RENDER_BACKEND STREQUAL "Metal")
    set(METAL_SHADERS VS.metal)

    foreach(SHADER_FILE IN LISTS METAL_SHADERS)
        get_filename_component(SHADER ${SHADER_FILE} NAME_WE)
        set(SHADER_SRC ${SHADER_DIR}/${SHADER_FILE})
        add_custom_command(
            OUTPUT ${EXE_PATH}/$<CONFIG>/${SHADER}.metallib
            COMMAND xcrun -sdk macosx metal -c ${SHADER_SRC} -o $<TARGET_FILE_DIR:${PROJECT_NAME}>/${SHADER}.air
            COMMAND xcrun -sdk macosx metallib $<TARGET_FILE_DIR:${PROJECT_NAME}>/${SHADER}.air -o ${EXE_PATH}/$<CONFIG>/${SHADER}.metallib
            COMMAND rm $<TARGET_FILE_DIR:${PROJECT_NAME}>/${SHADER}.air
            DEPENDS ${SHADER_SRC}
            COMMENT "Compiling Metal shader: ${SHADER_FILE}"
        )
        list(APPEND SHADER_OUTPUTS ${EXE_PATH}/$<CONFIG>/${SHADER}.metallib)
    endforeach()

    add_custom_target(Shaders ALL DEPENDS ${SHADER_OUTPUTS})
    unset(AIR_OUT)
    unset(METAL_SHADERS)
elseif(RENDER_BACKEND STREQUAL "OpenGL")
    set(VERTEX_SHADERS VS.vert)
    set(FRAGMENT_SHADERS FS.frag)

    foreach(SHADER_FILE IN LISTS VERTEX_SHADERS)
        get_filename_component(SHADER ${SHADER_FILE} NAME_WE)
        set(SHADER_SRC ${SHADER_DIR}/${SHADER_FILE})
        add_custom_command(
            OUTPUT ${EXE_PATH}/$<CONFIG>/${SHADER}.spv
            COMMAND ${SHADER_COMPILER} -V ${SHADER_SRC} -o ${EXE_PATH}/$<CONFIG>/${SHADER}.spv
            DEPENDS ${SHADER_SRC}
            COMMENT "Compiling Vertex Shader: ${SHADER_FILE}"
        )
        list(APPEND SHADER_OUTPUTS ${EXE_PATH}/$<CONFIG>/${SHADER}.spv)
    endforeach()
    foreach(SHADER_FILE IN LISTS FRAGMENT_SHADERS)
        get_filename_component(SHADER ${SHADER_FILE} NAME_WE)
        set(SHADER_SRC ${SHADER_DIR}/${SHADER_FILE})
        add_custom_command(
            OUTPUT ${EXE_PATH}/$<CONFIG>/${SHADER}.spv
            COMMAND ${SHADER_COMPILER} -V ${SHADER_SRC} -o ${EXE_PATH}/$<CONFIG>/${SHADER}.spv
            DEPENDS ${SHADER_SRC}
            COMMENT "Compiling Fragment Shader: ${SHADER_FILE}"
        )
        list(APPEND SHADER_OUTPUTS ${EXE_PATH}/$<CONFIG>/${SHADER}.spv)
    endforeach()

    add_custom_target(Shaders ALL DEPENDS ${SHADER_OUTPUTS})
    unset(SHADER_COMPILER)
    unset(VERTEX_SHADERS)
    unset(PIXEL_SHADERS)
endif()

unset(SHADER_OUTPUTS)
unset(SHADER_SRC)
unset(EXE_PATH)
unset(SHADER_DIR)