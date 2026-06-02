cmake_minimum_required(VERSION 3.20)
project(RealCarMonitor VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Явно указываем CMake, где лежит файл OpenCVConfig.cmake
set(OpenCV_DIR "C:/opencv/x64/mingw/lib")

# 1. Поиск OpenCV
find_package(OpenCV REQUIRED)

# 2. Настройка путей для ONNX Runtime
set(ONNXRUNTIME_DIR "C:/onnxruntime")
include_directories(${ONNXRUNTIME_DIR}/include)
link_directories(${ONNXRUNTIME_DIR}/lib)

# 3. Объявляем исходные файлы
set(SOURCES
    src/main.cpp
)

# 4. Объявляем исполняемый файл приложения
add_executable(RealCarMonitor ${SOURCES})

# Привязываем библиотеки к нашему приложению
target_link_libraries(RealCarMonitor ${OpenCV_LIBS} onnxruntime)

# 5. Подключение Google Test
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)