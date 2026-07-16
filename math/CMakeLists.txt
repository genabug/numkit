add_library(math INTERFACE
  math/Type.h
  math/Tensor.h
  math/Vector.h
  math/details.h
)

target_link_libraries(math INTERFACE common)
target_compile_features(math INTERFACE cxx_std_23)
target_include_directories(math INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")
