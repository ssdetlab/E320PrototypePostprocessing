cmake_minimum_required(VERSION 3.14)

project(DataPostprocessing LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-Wl,--no-as-needed")

find_package(
    ROOT 
    COMPONENTS 
    Core
    Hist
    HistPainter
    RIO 
    Tree
    Physics
    REQUIRED
)

add_executable(
    dataPostprocessing 
    dataPostprocessor.cpp
)

target_include_directories(
   dataPostprocessing
   PUBLIC
   $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

add_subdirectory(src/Analysis)

target_link_libraries(
    dataPostprocessing
    PUBLIC
    ROOT::Core
    ROOT::Hist
    ROOT::HistPainter
    ROOT::RIO
    ROOT::Tree
    ROOT::Physics
    /home/romanurmanov/work/TrackingPipeline/ActsTrackingPipeline_build/lib/libSimEventDict.so
)
