###
# Loadable algorithms and processes

kwiver_add_python_module( ${CMAKE_CURRENT_SOURCE_DIR}/__init__.py
  sprokit/processes/pytorch
  __init__ )

kwiver_add_python_module( ${CMAKE_CURRENT_SOURCE_DIR}/srnn_tracker.py
  sprokit/processes/pytorch
  srnn_tracker )

kwiver_add_python_module( ${CMAKE_CURRENT_SOURCE_DIR}/pytorch_descriptors.py
  sprokit/processes/pytorch
  pytorch_descriptors )

kwiver_add_python_module( ${CMAKE_CURRENT_SOURCE_DIR}/resnet_augmentation.py
  sprokit/processes/pytorch
  resnet_augmentation )

if( KWIVER_ENABLE_PYTORCH_PYSOT )
  kwiver_add_python_module( ${CMAKE_CURRENT_SOURCE_DIR}/pysot_tracker.py
    sprokit/processes/pytorch
    pysot_tracker )
endif()

add_subdirectory( utils )
