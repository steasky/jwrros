execute_process(COMMAND "/home/book/ts1_ws/build/geometry-melodic-devel/tf/catkin_generated/python_distutils_install.sh" RESULT_VARIABLE res)

if(NOT res EQUAL 0)
  message(FATAL_ERROR "execute_process(/home/book/ts1_ws/build/geometry-melodic-devel/tf/catkin_generated/python_distutils_install.sh) returned error code ")
endif()