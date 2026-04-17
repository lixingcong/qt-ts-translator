cmake_minimum_required(VERSION 3.16)

set(project_name demo)
project(${project_name} VERSION 1.0)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

add_executable(${project_name}
	res.qrc # 资源文件
	main.cpp
	TranslateLoader.h TranslateLoader.cpp
	MainWindow.h MainWindow.cpp
)

if(MSVC)
    # MSVC编译器设置为utf-8文件编码
    string(APPEND CMAKE_C_FLAGS " /utf-8")
    string(APPEND CMAKE_CXX_FLAGS " /utf-8")
endif()

target_link_libraries(${project_name} PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
