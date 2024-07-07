cmake_minimum_required(VERSION 3.15)

project(demo)

find_package(PkgConfig REQUIRED)

pkg_check_modules(drm REQUIRED IMPORTED_TARGET libdrm)
if (drm_FOUND)
    include_directories(${drm_INCLUDE_DIRS})
endif()

add_executable(demo main.cpp)

target_link_libraries(demo
    PkgConfig::drm
)