cmake_minimum_required(VERSION 3.15)
project(RealCarMonitor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Для многопоточности на некоторых системах
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

include(FetchContent)
# Подключение ONNX
FetchContent_Declare(
    onnxruntime
    URL "https://github.com/microsoft/onnxruntime/releases/download/v1.14.1/onnxruntime-win-x64-1.14.1.zip"
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(onnxruntime)

# Подключение GoogleTest
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
FetchContent_MakeAvailable(googletest)

enable_testing()

# Исходники
set(SOURCES
    src/main.cpp
    src/obd_parser.cpp
    src/onnx_classifier.cpp
    src/dashboard.cpp
    src/dms_monitor.cpp
    src/dms_hud.cpp
)

add_executable(RealCarMonitor ${SOURCES})
target_include_directories(RealCarMonitor PRIVATE ${onnxruntime_SOURCE_DIR}/include)
target_link_libraries(RealCarMonitor PRIVATE ${OpenCV_LIBS} ${onnxruntime_SOURCE_DIR}/lib/onnxruntime.lib Threads::Threads)

# Сборка тестов OBD
add_executable(test_obd_parser tests/test_obd_parser.cpp src/obd_parser.cpp)
target_link_libraries(test_obd_parser gtest_main)
add_test(NAME OBDParserTests COMMAND test_obd_parser)

# Сборка тестов DMS
add_executable(test_dms_monitor tests/test_dms_monitor.cpp src/dms_monitor.cpp)
target_link_libraries(test_dms_monitor ${OpenCV_LIBS} gtest_main)
add_test(NAME DMSMonitorTests COMMAND test_dms_monitor)