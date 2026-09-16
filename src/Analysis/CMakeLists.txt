target_sources(
    removeSharedClustersData
    PRIVATE
    Postprocessor.cpp
    SimPostprocessor.cpp
    RandomDeduplicationAlgorithm.cpp
    FullBestChi2DeduplicationAlgorithm.cpp
)

target_sources(
    removeSharedClustersSim
    PRIVATE
    Postprocessor.cpp
    SimPostprocessor.cpp
    RandomDeduplicationAlgorithm.cpp
    FullBestChi2DeduplicationAlgorithm.cpp
)

target_sources(
    testClusterSharing
    PRIVATE
    Postprocessor.cpp
    SimPostprocessor.cpp
    RandomDeduplicationAlgorithm.cpp
    FullBestChi2DeduplicationAlgorithm.cpp
)

target_sources(
    sampleMagnets
    PRIVATE
    Postprocessor.cpp
    SimPostprocessor.cpp
    RandomDeduplicationAlgorithm.cpp
    FullBestChi2DeduplicationAlgorithm.cpp
)
