cmake_minimum_required(VERSION 2.8.0)
set(PROJECT_NAME server)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(${CMAKE_CURRENT_SOURCE_DIR}/../discover.cmake)
discover_headers(include header_list)
discover_cpp(src src_list)
find_package(Boost REQUIRED system thread)
set(IMPORTED_LIBS "Boost::thread;game")

set(IMPORTED_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../game/include;)
include_directories(${IMPORTED_DIRS})

add_executable(${PROJECT_NAME}exe ${src_list} ${header_list})
target_link_libraries(${PROJECT_NAME}exe ${IMPORTED_LIBS})






