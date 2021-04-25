cmake_minimum_required(VERSION 3.15.5 FATAL_ERROR)

# Add path to look for includes
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
include(utils)


project("spectrograph"
    VERSION 0.0.1
    DESCRIPTION "OTUS c++ final project: spectrograph"
    LANGUAGES CXX
)


# OpenCV for windows
set(OpenCV_DIR install/opencv)
find_package(OpenCV REQUIRED core imgcodecs highgui)
message(STATUS "OpenCV library status:")
message(STATUS "    config: ${OpenCV_DIR}")
message(STATUS "    version: ${OpenCV_VERSION}")
message(STATUS "    libraries: ${OpenCV_LIBS}")
message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")
message(STATUS "    lib path: ${_OpenCV_LIB_PATH}")
# gtest
#set(BUILD_GMOCK ON CACHE BOOL "" FORCE)
#add_subdirectory("googletest" "googletest")
#if (MSVC)
#    set(gtest_force_shared_crt ON cache BOOL "" FORCE)
#endif()
#include(CTest)
#enable_testing()


set(CVPLOT_WITH_TESTS  OFF CACHE BOOL "cv-plot test" FORCE)
set(CVPLOT_WITH_EXAMPLES OFF CACHE BOOL "cv-plot examples" FORCE)
set(CVPLOT_USE_CONAN  OFF CACHE BOOL "cv-plot use conan" FORCE) 
add_subdirectory(cv-plot)
# set(CVPLOT_HEADER_ONLY  1) 
set(CvPlot_INCLIDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/cv-plot/CvPlot/inc")

add_subdirectory(fmt)

add_subdirectory(spectr)



# Package setup

set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP TRUE)
include(InstallRequiredSystemLibraries)
install(PROGRAMS ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS}
        DESTINATION programs
        COMPONENT applications)

set(CPACK_PACKAGE_NAME "Spectr Installer")
set(CPACK_DISPLAY_NAME "Spectr")
set(CPACK_PACKAGE_VENDOR "Sergey Simonov")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Spectr application")
set(CPACK_PACKAGE_VERSION_MAJOR "0")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "1")
set(CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "Spectr")
set(CPACK_PACKAGE_CONTACT ssbdex@yandex.ru)

set(CPACK_COMPONENTS_ALL applications)
set(CPACK_COMPONENT_APPLICATIONS_DISPLAY_NAME "Spectr")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/spectr/res/license.txt")
#set(CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}/res/spectr.ico")
#set(CPACK_NSIS_MUI_ICON "${CMAKE_SOURCE_DIR}/spectr/res/spectr.ico")
#set(CPACK_NSIS_MUI_UNIICON "${CMAKE_SOURCE_DIR}/spectr/res/spectr.ico")
set(CPACK_NSIS_MENU_LINKS "programs\\spectr.exe" "Spectr")

include(CPack)






