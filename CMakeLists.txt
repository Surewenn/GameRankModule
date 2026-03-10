cmake_minimum_required(VERSION 3.1)

project(GAME_RANK_MODULE VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src SKIP_LIST_SRC)

add_executable(skip_list ${SKIP_LIST_SRC})

target_include_directories(skip_list PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)