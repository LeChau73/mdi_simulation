# Thiết lập phiên bản tối thiểu của CMake
cmake_minimum_required(VERSION 3.16)

# Định nghĩa tên dự án và ngôn ngữ
project(mdi_simulation LANGUAGES CXX)

# Thiết lập chuẩn C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)

set(CMAKE_AUTORCC ON)

set(CMAKE_AUTOUIC ON)

message("CMAKE_BINARY_DIR = ${CMAKE_BINARY_DIR}")

# set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_COLOR_MAKEFILE ON CACHE BOOL "Enable color output in Makefile")

# Định nghĩa thư mục output (Bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Bin)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Quick)

# Tìm các gói Qt cần thiết
find_package(Qt6 COMPONENTS REQUIRED Core Gui Quick Widgets Network)

# Cấu hình setup Qt
qt_standard_project_setup()

# Định nghĩa danh sách source files
set(SOURCES
    core/src/*.cpp
    resources/qml/*.qrc
    resources/qml/*.qml
)

# Định nghĩa danh sách header files
set(HEADERS
    core/include/*.h
    core/common/*.h
)

file(GLOB HEADER_FILES "core/include/*.h" "core/common/*.h")
file(GLOB SOURCES_FILE "core/src/*.cpp")
file(GLOB QML_FILE "resources/qml/*.qml")
list(REMOVE_ITEM HEADER_FILES "core/include/exclude_this.h")

qt_add_resources(MY_RESOURCES qml.qrc)

qt_add_executable(MDISimulator
    ${HEADER_FILES}
    ${SOURCES_FILE}
    ${QML_FILE}
    resources/qml/qml.qrc
)

# Thêm module QML
# qt_add_qml_module(MDISimulator
# URI MDISimulator
# VERSION 1.0
# QML_FILES
# resources/qml/main.qml
# )

# Link thư viện Qt
target_link_libraries(MDISimulator PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Quick
    Qt6::Widgets
    Qt6::Network
)

message("CMAKE_SOURCE_DIR = ${CMAKE_SOURCE_DIR}")

# Thêm include directories
target_include_directories(MDISimulator PRIVATE
    ${CMAKE_SOURCE_DIR}/core/include
)