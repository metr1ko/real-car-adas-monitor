cmake_minimum_required(VERSION 3.15)
project(RealCarMonitor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Подключаем OpenCV (путь мы указываем в терминале через -DOpenCV_DIR)
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# 2. Скачиваем ONNX Runtime для Windows x64
include(FetchContent)
FetchContent_Declare(
    onnxruntime
    URL "https://github.com/microsoft/onnxruntime/releases/download/v1.14.1/onnxruntime-win-x64-1.14.1.zip"
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE # Убирает желтое предупреждение из логов
)
FetchContent_MakeAvailable(onnxruntime)

# 3. Добавляем все наши исходные файлы (включая новый dashboard.cpp)
add_executable(RealCarMonitor
    src/main.cpp
    src/obd_parser.cpp
    src/onnx_classifier.cpp
    src/dashboard.cpp
)

# 4. Указываем путь к заголовочным файлам ONNX Runtime
target_include_directories(RealCarMonitor PRIVATE ${onnxruntime_SOURCE_DIR}/include)

# 5. Линкуем библиотеки к нашему приложению
# Для компилятора Visual Studio (MSVC) нужно линковать файл .lib
target_link_libraries(RealCarMonitor PRIVATE 
    ${OpenCV_LIBS} 
    ${onnxruntime_SOURCE_DIR}/lib/onnxruntime.lib
)