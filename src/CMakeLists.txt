file(GLOB MAIN_SRC "*.cpp")

add_executable(${PROJECT_NAME}
    ${MAIN_SRC}
)

add_subdirectory(Algorithm)
add_subdirectory(imgui_impl)

target_compile_definitions(${PROJECT_NAME} PUBLIC
    $<IF:$<CONFIG:DEBUG>,_DEBUG D3DCOMPILE_DEBUG D3DCOMPILE_SKIP_OPTIMIZATION D3DCOMPILE_ENABLE_STRICTNESS,NDEBUG>
    GLM_ENABLE_EXPERIMENTAL
)

target_include_directories(${PROJECT_NAME} PRIVATE
    "${CMAKE_SOURCE_DIR}/include"
)

if(NOT ${DIRECTX_ARCH} MATCHES "^arm")
    if(CMAKE_SIZEOF_VOID_P EQUAL 4)
        set(ARCH_SSE2 $<$<CXX_COMPILER_ID:MSVC,Intel>:/arch:SSE2> $<$<NOT:$<CXX_COMPILER_ID:MSVC,Intel>>:-msse2>)
    else()
        set(ARCH_SSE2 $<$<NOT:$<CXX_COMPILER_ID:MSVC,Intel>>:-msse2>)
    endif()

    target_compile_options(${PROJECT_NAME} PRIVATE ${ARCH_SSE2})
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE
    d3d11 D3DCompiler
    imgui_impl
    algorithm_impl
)

# find_package(directxmath CONFIG REQUIRED)
# if(directxmath_FOUND)
#     message(STATUS "Using DirectXMath package")
#     target_link_libraries(${PROJECT_NAME} PRIVATE
#         Microsoft::DirectXMath
#     )
# endif()

find_package(gsl-lite CONFIG REQUIRED)
if(gsl-lite_FOUND)
    message(STATUS "Using gsl-lite package")
    target_link_libraries(${PROJECT_NAME} PRIVATE
        gsl::gsl-lite-v1
    )
endif()

find_package(RapidJSON CONFIG REQUIRED)
if(RapidJSON_FOUND)
    message(STATUS "Using RapidJSON package")
    target_link_libraries(${PROJECT_NAME} PRIVATE
        rapidjson
    )
endif()

find_package(imgui CONFIG REQUIRED)
if(imgui_FOUND)
    message(STATUS "Using ImGUI package")
    target_link_libraries(${PROJECT_NAME} PRIVATE
        imgui::imgui
    )
endif()

find_package(glm CONFIG REQUIRED)
if(glm_FOUND)
    message(STATUS "Using OpenGL Math package")
    target_link_libraries(${PROJECT_NAME} PRIVATE
        glm::glm
    )
endif()

find_package(Stb REQUIRED)
if(Stb_FOUND)
    message(STATUS "Using Stb package")
    target_include_directories(${PROJECT_NAME} PRIVATE
        ${Stb_INCLUDE_DIR}
    )
endif()

find_package(SDL2 CONFIG REQUIRED)
    message(STATUS "Using SDL2 package")
    target_link_libraries(${PROJECT_NAME} PRIVATE
        $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
        $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
    )

# Uncomment if using the DirectX Tool Kit
# find_package(directxtk CONFIG REQUIRED)
# target_link_libraries(${PROJECT_NAME} PRIVATE Microsoft::DirectXTK)

# Uncomment if using XAudio2Redist (required for MINGW)
# find_package(xaudio2redist CONFIG REQUIRED)
# target_link_libraries(${PROJECT_NAME} PRIVATE Microsoft::XAudio2Redist)

target_compile_options(${PROJECT_NAME}
PRIVATE
    -Wall -Wextra
    # -Wno-ignored-attributes
)

if(WIN32)
    target_compile_definitions(${PROJECT_NAME}
    PRIVATE
        _UNICODE
        UNICODE
    )

    if(${DIRECTX_ARCH} MATCHES "^arm")
        target_compile_definitions(${PROJECT_NAME} PRIVATE _WIN32_WINNT=0x0A00)
    else()
        target_compile_definitions(${PROJECT_NAME} PRIVATE _WIN32_WINNT=0x0601)
    endif()
endif()

if (${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.21")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E $<IF:$<BOOL:$<TARGET_RUNTIME_DLLS:${PROJECT_NAME}>>,copy,true>
        $<TARGET_RUNTIME_DLLS:${PROJECT_NAME}> $<TARGET_FILE_DIR:${PROJECT_NAME}>
        COMMAND_EXPAND_LISTS
    )
endif()
