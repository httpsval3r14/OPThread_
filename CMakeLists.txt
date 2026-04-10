cmake_minimum_required(VERSION 3.15)
project(OPThread VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SOURCES "src/*.cpp")
add_executable(OPThread ${SOURCES})

target_include_directories(OPThread PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_include_directories(OPThread PRIVATE "C:/SFML/include")
target_link_directories(OPThread PRIVATE "C:/SFML/lib")
target_link_libraries(OPThread PRIVATE sfml-graphics sfml-window sfml-system)

file(GLOB SFML_DLLS "C:/SFML/bin/*.dll")
add_custom_command(TARGET OPThread POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SFML_DLLS} $<TARGET_FILE_DIR:OPThread>
    COMMENT "copying dlls"
)

add_custom_command(TARGET OPThread POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/assets"
    "$<TARGET_FILE_DIR:OPThread>/assets"
    COMMENT "coppying assets"
)