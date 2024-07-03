file(GLOB IMGUI_IMPL_SRC "*.cpp")

add_library(imgui_impl
    ${IMGUI_IMPL_SRC}
)

target_include_directories(imgui_impl PRIVATE
    "${CMAKE_SOURCE_DIR}/include"
)

target_compile_definitions(imgui_impl PRIVATE
    $<IF:$<CONFIG:DEBUG>,_DEBUG D3DCOMPILE_DEBUG D3DCOMPILE_SKIP_OPTIMIZATION D3DCOMPILE_ENABLE_STRICTNESS,NDEBUG>
)

target_link_libraries(imgui_impl PRIVATE
    d3d11 D3DCompiler dwmapi
)

find_package(imgui CONFIG REQUIRED)
if(imgui_FOUND)
    message(STATUS "Using ImGUI package")
    target_link_libraries(imgui_impl PRIVATE
        imgui::imgui
    )
endif()
