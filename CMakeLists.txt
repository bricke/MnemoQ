cmake_minimum_required(VERSION 3.14)

project(mnemoq LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Mqtt REQUIRED)

add_executable(mnemoq
  main.cpp
  mnemoq.h mnemoq.cpp

)
target_link_libraries(mnemoq PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Mqtt)
