# CMake generated Testfile for 
# Source directory: /home/book/ts1_ws/src/geometry-melodic-devel/tf_conversions
# Build directory: /home/book/ts1_ws/build/geometry-melodic-devel/tf_conversions
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
ADD_TEST(_ctest_tf_conversions_gtest_test_eigen_tf "/home/book/ts1_ws/build/catkin_generated/env_cached.sh" "/usr/bin/python" "/opt/ros/indigo/share/catkin/cmake/test/run_tests.py" "/home/book/ts1_ws/build/test_results/tf_conversions/gtest-test_eigen_tf.xml" "--return-code" "/home/book/ts1_ws/devel/lib/tf_conversions/test_eigen_tf --gtest_output=xml:/home/book/ts1_ws/build/test_results/tf_conversions/gtest-test_eigen_tf.xml")
ADD_TEST(_ctest_tf_conversions_gtest_test_kdl_tf "/home/book/ts1_ws/build/catkin_generated/env_cached.sh" "/usr/bin/python" "/opt/ros/indigo/share/catkin/cmake/test/run_tests.py" "/home/book/ts1_ws/build/test_results/tf_conversions/gtest-test_kdl_tf.xml" "--return-code" "/home/book/ts1_ws/devel/lib/tf_conversions/test_kdl_tf --gtest_output=xml:/home/book/ts1_ws/build/test_results/tf_conversions/gtest-test_kdl_tf.xml")
ADD_TEST(_ctest_tf_conversions_nosetests_test.posemath.py "/home/book/ts1_ws/build/catkin_generated/env_cached.sh" "/usr/bin/python" "/opt/ros/indigo/share/catkin/cmake/test/run_tests.py" "/home/book/ts1_ws/build/test_results/tf_conversions/nosetests-test.posemath.py.xml" "--return-code" "/usr/bin/cmake -E make_directory /home/book/ts1_ws/build/test_results/tf_conversions" "/usr/bin/nosetests-2.7 -P --process-timeout=60 /home/book/ts1_ws/src/geometry-melodic-devel/tf_conversions/test/posemath.py --with-xunit --xunit-file=/home/book/ts1_ws/build/test_results/tf_conversions/nosetests-test.posemath.py.xml")
