project(Latexey)
cmake_minimum_required(VERSION 2.8)
include_directories(include)
file(GLOB SOURCES "src/*.cpp")
add_executable(test ${SOURCES})

find_package(wxWidgets REQUIRED COMPONENTS net core base)
include(${wxWidgets_USE_FILE})
target_link_libraries(test ${wxWidgets_LIBRARIES})

find_package(PythonLibs REQUIRED)
include_directories(${PYTHON_INCLUDE_DIRS})
target_link_libraries(test ${PYTHON_LIBRARIES})

find_package(X11 REQUIRED)
include_directories(${X11_INCLUDE_DIR})
link_directories(${X11_LIBRARIES})
target_link_libraries(test ${X11_LIBRARIES})

target_link_libraries(test xdo)