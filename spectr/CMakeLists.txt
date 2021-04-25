cmake_minimum_required(VERSION 3.15.5 FATAL_ERROR)

set (This spectr)

set (Sources
    src/capture.cpp
    src/controller.cpp
    src/main.cpp
    src/model.cpp
    src/view.cpp
    src/window.cpp
)

set (Headers
    inc/capture.h
    inc/controller.h
    inc/format.h
    inc/fps.h
    inc/model.h
    inc/ocv.h
    inc/optlog.h
    inc/save_dialog.h
    inc/version.h.in
    inc/view.h
    inc/window.h
)

set(WIN_RESOURCE_FILE res/spectr.rc)

add_executable(${This} ${Sources} ${Headers} ${WIN_RESOURCE_FILE})

configure_file(inc/version.h.in version.h)
target_include_directories(${This} PRIVATE 
    ${CMAKE_CURRENT_BINARY_DIR}
)

target_include_directories(${This} PRIVATE
    inc 
    ${OpenCV_INCLUDE_DIR}
    ${CvPlot_INCLIDE_DIR}
)


target_link_libraries(${This} PRIVATE 
    ${OpenCV_LIBS}
    CvPlot::CvPlot
    fmt::fmt-header-only
)

set_target_properties(${This}  PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_definitions(/DVERBOSE_LEVEL=3)
endif()

append_warn_flags(${This})

add_custom_command(TARGET ${This} POST_BUILD
                   COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/res/spectr.yml" "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/"
)

install(TARGETS ${This} 
RUNTIME
DESTINATION programs
COMPONENT applications
)

set(dsfx $<$<CONFIG:Debug>:d>)

set(libqt_files 
    ${CMAKE_SOURCE_DIR}/qt5lib/Qt5Core${dsfx}.dll
    ${CMAKE_SOURCE_DIR}/qt5lib/Qt5Gui${dsfx}.dll
    ${CMAKE_SOURCE_DIR}/qt5lib/Qt5OpenGL${dsfx}.dll
    ${CMAKE_SOURCE_DIR}/qt5lib/Qt5Test${dsfx}.dll
    ${CMAKE_SOURCE_DIR}/qt5lib/Qt5Test${dsfx}.dll
    ${CMAKE_SOURCE_DIR}/qt5lib/Qt5Widgets${dsfx}.dll
)

set(libqt_platform_files 
    ${CMAKE_SOURCE_DIR}/qt5lib/platforms/qdirect2d${dsfx}.dll
    ${CMAKE_SOURCE_DIR}/qt5lib/platforms/qminimal${dsfx}.dll
    ${CMAKE_SOURCE_DIR}/qt5lib/platforms/qoffscreen${dsfx}.dll
    ${CMAKE_SOURCE_DIR}/qt5lib/platforms/qwindows${dsfx}.dll
)



install(FILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/res/spectr.yml
    "${_OpenCV_LIB_PATH}/opencv_world451${dsfx}.dll"
    ${libqt_files}
DESTINATION programs
COMPONENT applications
)

install(FILES 
    ${libqt_platform_files}
DESTINATION programs/platforms
COMPONENT applications
)

# include(InstallRequiredSystemLibraries)
# message(${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS})
# INSTALL(FILES ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS} DESTINATION bin COMPONENT Libraries)



