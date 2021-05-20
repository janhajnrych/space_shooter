cmake_minimum_required(VERSION 2.8.0)
set(PROJECT_NAME cgame)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(${CMAKE_CURRENT_SOURCE_DIR}/utils.cmake)

include_directories( ${CMAKE_BINARY_DIR})

import_dirs("soil;glad;glfw;glm;")
import_dirs("window;engine;server")



