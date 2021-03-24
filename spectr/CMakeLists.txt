set (This spectr)

# OpenCV for windows
set(OpenCV_DIR ../install/opencv)
find_package(OpenCV REQUIRED core imgcodecs highgui)
message(STATUS "OpenCV library status:")
message(STATUS "    config: ${OpenCV_DIR}")
message(STATUS "    version: ${OpenCV_VERSION}")
message(STATUS "    libraries: ${OpenCV_LIBS}")
message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")

set (Sources
    src/main.cpp
    src/window.cpp
    src/model.cpp
)
set (Headers
)

add_executable(${This} ${Sources} ${Headers} )

include_directories(${OpenCV_INCLUDE_DIR})
target_link_libraries(${This} ${OpenCV_LIBS}
)


set_target_properties(${This} ${ThisLib} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)
append_warn_flags(${This})

install(TARGETS ${This} 
RUNTIME
DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/../../install/spectr)

