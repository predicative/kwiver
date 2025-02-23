project(vital_python_algo)

include_directories(${pybind11_INCLUDE_DIR})

kwiver_add_python_library(algorithm
  vital/algo
  algorithm.h
  detected_object_set_input.h
  detected_object_set_output.h
  image_filter.h
  image_object_detector.h
  refine_detections.h
  train_detector.h

  algorithm.cxx
  detected_object_set_input.cxx
  detected_object_set_output.cxx
  image_filter.cxx
  image_object_detector.cxx
  refine_detections.cxx
  train_detector.cxx
  algorithm_module.cxx

  trampoline/algorithm_trampoline.txx
  trampoline/detected_object_set_input_trampoline.txx
  trampoline/detected_object_set_output_trampoline.txx
  trampoline/image_filter_trampoline.txx
  trampoline/image_object_detector_trampoline.txx
  trampoline/refine_detections_trampoline.txx
  trampoline/train_detector_trampoline.txx
)


target_link_libraries(python-vital.algo-algorithm
  PUBLIC ${PYTHON_LIBRARIES}
          vital
          vital_config
          vital_algo
          vital_python_util
)

kwiver_add_python_library(algorithm_factory
  vital/algo
  algorithm_factory.cxx
)

target_link_libraries(python-vital.algo-algorithm_factory
  PUBLIC ${PYTHON_LIBRARIES}
          vital
          vital_algo
)

kwiver_create_python_init(vital/algo
  algorithm
  algorithm_factory
)
