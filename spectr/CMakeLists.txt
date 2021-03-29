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
    src/controller.cpp
    src/capture.cpp
)

set (Headers
    inc/controller.h
    inc/model.h
    inc/view.h
    inc/window.h
    inc/ocv.h
    inc/optlog.h
    inc/capture.h
)

add_executable(${This} ${Sources} ${Headers} )

target_include_directories(${This} PRIVATE
    inc 
    ${OpenCV_INCLUDE_DIR}
)

target_link_libraries(${This} 
    ${OpenCV_LIBS}
)

set_target_properties(${This} ${ThisLib} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

append_warn_flags(${This})

install(TARGETS ${This} 
RUNTIME
DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/../../install/spectr)

