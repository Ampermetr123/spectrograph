cmake_minimum_required(VERSION 3.14.0 FATAL_ERROR)

# Add path to look for includes
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
include(utils)


project("spectrograph"
    VERSION 0.0.$ENV{TRAVIS_BUILD_NUMBER}
    DESCRIPTION "OTUS c++ final project: spectrograph"
    LANGUAGES CXX
)

# pthread required
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

# gtest
#set(BUILD_GMOCK ON CACHE BOOL "" FORCE)
#add_subdirectory("googletest" "googletest")
#if (MSVC)
#    set(gtest_force_shared_crt ON cache BOOL "" FORCE)
#endif()
#include(CTest)
#enable_testing()


#add_subdirectory(opencv open)
add_subdirectory(spectr)



# Package setup
#install(TARGETS join_server RUNTIME)
#set(CPACK_GENERATOR DEB)
#set(CPACK_PROJECT_NAME ${PROJECT_NAME})
#set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
#set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
#set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
#set(CPACK_PACKAGE_CONTACT ssbdex@yandex.ru)
#include(CPack)






