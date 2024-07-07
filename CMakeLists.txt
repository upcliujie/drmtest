cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

project(demo)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Gui Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Gui Widgets REQUIRED)

find_package(PkgConfig REQUIRED)

pkg_check_modules(drm REQUIRED IMPORTED_TARGET libdrm)
if (drm_FOUND)
    include_directories(${drm_INCLUDE_DIRS})
endif()

add_executable(demo
    main.cpp
    drm_connector.cpp
    drm_buffer.cpp
    drm_gpu.cpp
)

target_link_libraries(demo
    PkgConfig::drm
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
)