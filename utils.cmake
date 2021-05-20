function(import_dirs IMPORTED_DIRS)
    foreach(DIR ${IMPORTED_DIRS})
      add_subdirectory(${DIR})
    endforeach()

#    foreach(DIR ${IMPORTED_DIRS})
#      include_directories(${DIR}/include)
#    endforeach()
endfunction()

