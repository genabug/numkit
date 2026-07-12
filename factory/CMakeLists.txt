add_library(factory INTERFACE factory/ObjectsFactory.h)
target_compile_features(factory INTERFACE cxx_std_20)
target_include_directories(factory INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")
