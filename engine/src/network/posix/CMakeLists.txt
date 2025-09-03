add_executable(SimpleServer server.cpp socket.cpp)
add_executable(SimpleClient client.cpp socket.cpp)

target_include_directories(SimpleServer PRIVATE
    ${CMAKE_SOURCE_DIR}/engine/include
)
target_include_directories(SimpleClient PRIVATE
    ${CMAKE_SOURCE_DIR}/engine/include
)
