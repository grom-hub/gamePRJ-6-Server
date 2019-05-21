cmake_minimum_required(VERSION 2.8)

project(PRJ-6-Server)
add_executable(${PROJECT_NAME} "main.cpp" "server.cpp" "game.cpp")

target_link_libraries(${PROJECT_NAME} pthread)
