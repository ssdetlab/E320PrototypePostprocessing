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
    removeSharedClustersData 
    removeSharedClustersData.cpp
)

add_executable(
    removeSharedClustersSim 
    removeSharedClustersSim.cpp
)

add_executable(
    testClusterSharing 
    testClusterSharing.cpp
)

add_executable(
    sampleMagnets 
    sampleMagnets.cpp
)

target_include_directories(
   removeSharedClustersData
   PUBLIC
   $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_include_directories(
   removeSharedClustersSim
   PUBLIC
   $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_include_directories(
   testClusterSharing
   PUBLIC
   $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_include_directories(
   sampleMagnets
   PUBLIC
   $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

add_subdirectory(src/Analysis)

target_link_libraries(
    removeSharedClustersData
    PUBLIC
    ROOT::Core
    ROOT::Hist
    ROOT::HistPainter
    ROOT::RIO
    ROOT::Tree
    ROOT::Physics
    /home/romanurmanov/work/TrackingPipeline/ActsTrackingPipeline_build/lib/libSimEventDict.so
)

target_link_libraries(
    removeSharedClustersSim
    PUBLIC
    ROOT::Core
    ROOT::Hist
    ROOT::HistPainter
    ROOT::RIO
    ROOT::Tree
    ROOT::Physics
    /home/romanurmanov/work/TrackingPipeline/ActsTrackingPipeline_build/lib/libSimEventDict.so
)

target_link_libraries(
    testClusterSharing
    PUBLIC
    ROOT::Core
    ROOT::Hist
    ROOT::HistPainter
    ROOT::RIO
    ROOT::Tree
    ROOT::Physics
    /home/romanurmanov/work/TrackingPipeline/ActsTrackingPipeline_build/lib/libSimEventDict.so
)

target_link_libraries(
    sampleMagnets
    PUBLIC
    ROOT::Core
    ROOT::Hist
    ROOT::HistPainter
    ROOT::RIO
    ROOT::Tree
    ROOT::Physics
    /home/romanurmanov/work/TrackingPipeline/ActsTrackingPipeline_build/lib/libSimEventDict.so
)
