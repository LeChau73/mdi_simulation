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

# Định nghĩa thư mục output (Bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Bin)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Quick)

# Tìm các gói Qt cần thiết
find_package(Qt6 COMPONENTS REQUIRED Core Gui Quick)

# Cấu hình setup Qt
qt_standard_project_setup()

# Định nghĩa danh sách source files
set(DIR_SOURCES core/src/)

set(SOURCES
    core/src/main.cpp
    core/src/FileDataLoader.cpp
    core/src/MainWindow.cpp
    resources/qml/qml.qrc
    resources/qml/ParameterTable.qml
    resources/qml/main.qml
)

# Định nghĩa danh sách header files
set(HEADERS
    core/include/FileDataLoader.h
    core/include/MainWindow.h
)

qt_add_resources(MY_RESOURCES qml.qrc)

qt_add_executable(MDISimulator
    ${SOURCES}
    ${HEADERS}
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
)

message("CMAKE_SOURCE_DIR = ${CMAKE_SOURCE_DIR}")

# Thêm include directories
target_include_directories(MDISimulator PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)
