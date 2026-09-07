target_sources(
    removeSharedClusters
    PRIVATE
    Postprocessor.cpp
    SimPostprocessor.cpp
)

target_sources(
    testClusterSharing
    PRIVATE
    Postprocessor.cpp
    SimPostprocessor.cpp
)

target_sources(
    sampleMagnets
    PRIVATE
    Postprocessor.cpp
    SimPostprocessor.cpp
)
