add_library(Core SHARED)

file(GLOB_RECURSE CORE_PUBLIC_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
)
target_sources(Core
PRIVATE
    src/math/type.cpp
    src/memory/bit.cpp
    src/memory/dynamic_vector.cpp
    src/string/helper.cpp
PUBLIC
    FILE_SET HEADERS
    BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include
    FILES ${CORE_PUBLIC_HEADERS}
)

target_compile_features(Core
PUBLIC
    cxx_std_26
)
target_compile_options(Core
PRIVATE
    -Wall -Wextra
)
target_compile_definitions(Core
PUBLIC
    $<IF:$<CONFIG:DEBUG>, _DEBUG, NDEBUG>
)

target_include_directories(Core
PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

install(TARGETS Core
    EXPORT ModernBoyTargets
    FILE_SET HEADERS DESTINATION include
)
install(EXPORT ModernBoyTargets
    NAMESPACE ModernBoy::
    DESTINATION lib/cmake/ModernBoy
)
