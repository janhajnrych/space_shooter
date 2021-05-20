cmake_minimum_required(VERSION 2.8.0)
set(PROJECT_NAME engine)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(${CMAKE_CURRENT_SOURCE_DIR}/../discover.cmake)
discover_headers(include header_list)
discover_cpp(src src_list)

set(IMPORTED_LIBS "soil;glad;glfw;glm;")
set(IMPORTED_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/../soil/include;
                   ${CMAKE_CURRENT_SOURCE_DIR}/../glfw/include;
                   ${CMAKE_CURRENT_SOURCE_DIR}/../glad/include;
                   ${CMAKE_CURRENT_SOURCE_DIR}/../glm/include")
include_directories(${IMPORTED_DIRS})
add_library(${PROJECT_NAME} ${src_list} ${header_list})
target_link_libraries(${PROJECT_NAME} ${IMPORTED_LIBS})






