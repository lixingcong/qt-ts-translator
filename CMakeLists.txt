cmake_minimum_required(VERSION 3.11)

project(qt-ts-translator)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Xml)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Xml)

set(BIN_NAME app)
set(LIB_NAME linguist)

# linguist
aux_source_directory(qt-linguist-shared LINGUIST_SRC)
add_library(${LIB_NAME} OBJECT ${LINGUIST_SRC})
target_link_libraries(${LIB_NAME} PUBLIC Qt${QT_VERSION_MAJOR}::Xml)

# app
add_executable(${BIN_NAME} main.cpp)
target_link_libraries(${BIN_NAME} PUBLIC ${LIB_NAME})

# demo
option(BuildDemo "Bulid demo for switch between languages" OFF)
if(BuildDemo)
	add_subdirectory(demo)
endif()

