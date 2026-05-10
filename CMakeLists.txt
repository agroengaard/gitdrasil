cmake_minimum_required(VERSION 3.10)

project(Gitdrasil)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)

list(APPEND CMAKE_PREFIX_PATH "C:\\Libraries\\eigen-5.0.0")
include(FetchContent)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(OpenCASCADE REQUIRED)
#find_package(Eigen3 REQUIRED)
include_directories("C:\\Libraries\\eigen-5.0.0")

FetchContent_Declare(
    sciplot_content
    GIT_REPOSITORY https://github.com/sciplot/sciplot.git
    GIT_TAG 75d813a362e5ef4a43e671ca84517748060f3b25
)

set(SCIPLOT_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SCIPLOT_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(SCIPLOT_BUILD_DOCS OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(sciplot_content)

find_program(GNUPLOT_EXECUTABLE gnuplot)
if(NOT GNUPLOT_EXECUTABLE)
    message(WARNING "gnuplot was not found in PATH. Gitdrasil will compile with sciplot, but plotting at runtime will fail until gnuplot is installed.")
endif()

add_executable(Gitdrasil
    main.cpp
    MainWindow.cpp
    GitService.cpp
    OcctViewerWidget.cpp
    foils.cpp
)

target_include_directories(Gitdrasil PRIVATE ${OpenCASCADE_INCLUDE_DIR})
target_link_libraries(Gitdrasil
    Qt6::Widgets
    sciplot
    TKernel TKMath
    TKBRep TKGeomBase TKGeomAlgo TKG3d TKTopAlgo
    TKService TKV3d TKOpenGl
    TKPrim
)
