set(SRC_DIR ${CMAKE_SOURCE_DIR}/src/shader)
set(TGT_DIR ${CMAKE_SOURCE_DIR}/asset/shader)

if(RENDER_BACKEND STREQUAL "DirectX")
    set(VERTEX_SHADERS VS.hlsl)
    set(PIXEL_SHADERS PS.hlsl)

    foreach(SHADER_FILE IN LISTS VERTEX_SHADERS)
        get_filename_component(SHADER ${SHADER_FILE} NAME_WE)
        set(SHADER_SRC ${SRC_DIR}/${SHADER_FILE})
        add_custom_command(
            OUTPUT ${TGT_DIR}/$<CONFIG>/${SHADER}.cso
            COMMAND ${SHADER_COMPILER} /T vs_5_0 /E main /Fo ${TGT_DIR}/$<CONFIG>/${SHADER}.cso ${SHADER_SRC}
            DEPENDS ${SHADER_SRC}
            COMMENT "Compiling Vertex Shader: ${SHADER_FILE}"
        )
        list(APPEND SHADER_OUTPUTS ${TGT_DIR}/$<CONFIG>/${SHADER}.cso)
    endforeach()
    foreach(SHADER_FILE IN LISTS PIXEL_SHADERS)
        get_filename_component(SHADER ${SHADER_FILE} NAME_WE)
        set(SHADER_SRC ${SRC_DIR}/${SHADER_FILE})
        add_custom_command(
            OUTPUT ${TGT_DIR}/$<CONFIG>/${SHADER}.cso
            COMMAND ${SHADER_COMPILER} /T ps_5_0 /E main /Fo ${TGT_DIR}/$<CONFIG>/${SHADER}.cso ${SHADER_SRC}
            DEPENDS ${SHADER_SRC}
            COMMENT "Compiling Pixel Shader: ${SHADER_FILE}"
        )
        list(APPEND SHADER_OUTPUTS ${TGT_DIR}/$<CONFIG>/${SHADER}.cso)
    endforeach()

    add_custom_target(Shaders ALL DEPENDS ${SHADER_OUTPUTS})
    unset(SHADER_COMPILER)
    unset(VERTEX_SHADERS)
    unset(PIXEL_SHADERS)
elseif(RENDER_BACKEND STREQUAL "Metal")
    set(AIR_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/../shaders)
    set(METAL_SHADERS vs.metal fs.metal)

    foreach(SHADER_FILE IN LISTS METAL_SHADERS)
        get_filename_component(SHADER ${SHADER_FILE} NAME_WE)
        set(SHADER_SRC ${SRC_DIR}/${SHADER_FILE})
        set(AIR_FILE ${AIR_DIR}/${SHADER}.air)
        add_custom_command(
            OUTPUT ${AIR_FILE}
            COMMAND xcrun -sdk macosx metal -MD -MF ${AIR_FILE}.d -c ${SHADER_SRC} -o ${AIR_FILE}
            DEPFILE ${AIR_FILE}.d
            DEPENDS ${SHADER_SRC}
            COMMENT "Compiling Metal shader to AIR: ${SHADER_FILE}"
        )
        list(APPEND AIR_FILES ${AIR_FILE})
    endforeach()
    add_custom_command(
        OUTPUT ${TGT_DIR}/${PROJECT_NAME}.metallib
        COMMAND xcrun -sdk macosx metallib ${AIR_FILES} -o ${TGT_DIR}/${PROJECT_NAME}.metallib
        DEPENDS ${AIR_FILES}
        COMMENT "Linking all AIRs into METALLIB"
    )
    add_custom_target(Shaders ALL DEPENDS ${TGT_DIR}/${PROJECT_NAME}.metallib)
    unset(AIR_FILE)
    unset(AIR_FILES)
    unset(METAL_SHADERS)
elseif(RENDER_BACKEND STREQUAL "OpenGL")
    set(VERTEX_SHADERS VS.vert)
    set(FRAGMENT_SHADERS FS.frag)

    foreach(SHADER_FILE IN LISTS VERTEX_SHADERS)
        get_filename_component(SHADER ${SHADER_FILE} NAME_WE)
        set(SHADER_SRC ${SRC_DIR}/${SHADER_FILE})
        add_custom_command(
            OUTPUT ${TGT_DIR}/$<CONFIG>/${SHADER}.spv
            COMMAND ${SHADER_COMPILER} -V ${SHADER_SRC} -o ${TGT_DIR}/$<CONFIG>/${SHADER}.spv
            DEPENDS ${SHADER_SRC}
            COMMENT "Compiling Vertex Shader: ${SHADER_FILE}"
        )
        list(APPEND SHADER_OUTPUTS ${TGT_DIR}/$<CONFIG>/${SHADER}.spv)
    endforeach()
    foreach(SHADER_FILE IN LISTS FRAGMENT_SHADERS)
        get_filename_component(SHADER ${SHADER_FILE} NAME_WE)
        set(SHADER_SRC ${SRC_DIR}/${SHADER_FILE})
        add_custom_command(
            OUTPUT ${TGT_DIR}/$<CONFIG>/${SHADER}.spv
            COMMAND ${SHADER_COMPILER} -V ${SHADER_SRC} -o ${TGT_DIR}/$<CONFIG>/${SHADER}.spv
            DEPENDS ${SHADER_SRC}
            COMMENT "Compiling Fragment Shader: ${SHADER_FILE}"
        )
        list(APPEND SHADER_OUTPUTS ${TGT_DIR}/$<CONFIG>/${SHADER}.spv)
    endforeach()

    add_custom_target(Shaders ALL DEPENDS ${SHADER_OUTPUTS})
    unset(SHADER_COMPILER)
    unset(VERTEX_SHADERS)
    unset(PIXEL_SHADERS)
endif()

unset(SHADER_OUTPUTS)
unset(SHADER_SRC)
unset(TGT_DIR)
unset(SRC_DIR)