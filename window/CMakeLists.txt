cmake_minimum_required(VERSION 2.8.0)
set(PROJECT_NAME window)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(${CMAKE_CURRENT_SOURCE_DIR}/../discover.cmake)
discover_headers(include header_list)
discover_cpp(src src_list)

set(IMPORTED_LIBS "glad;engine;server")
set(IMPORTED_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../glad/include;
                  ${CMAKE_CURRENT_SOURCE_DIR}/../engine/include;
                  ${CMAKE_CURRENT_SOURCE_DIR}/../server/include;)
include_directories(${IMPORTED_DIRS})
add_executable(${PROJECT_NAME} ${src_list} ${header_list})
target_link_libraries(${PROJECT_NAME} ${IMPORTED_LIBS})






