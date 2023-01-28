



TO_CHECK_DIR_LIB_GERONIMO=../thirdparties/dependencies/geronimo

if [ -d "$TO_CHECK_DIR_LIB_GERONIMO" ]
then

  export DIR_LIB_GERONIMO=$TO_CHECK_DIR_LIB_GERONIMO

else
  echo "  => the dependency \"geronimo\" is not installed, stopping now"
  exit 1
fi



# reset
# rm -rf obj/src/network/
# make -j6

# # ./bin/exec --gtest_filter=basic_double_linked_list__swap_two_links_from_same_list*
# # ./bin/exec --gtest_filter=physic_wrapper*
# ./bin/exec --gtest_filter=network_wrapper*

# exit 0



reset
# rm -rf obj/src/system/
rm -rf obj
make -j6

./bin/exec
# valgrind --leak-check=full ./bin/exec

exit 0



# reset

# # make build_mode="debug" build_platform="native" framework fclean
# make build_mode="debug" build_platform="native" framework -j6

# make build_mode="debug" build_platform="native" fclean_tests
# make build_mode="debug" build_platform="native" build-tests -j6

# ./bin/exec
# valgrind --leak-check=full ./bin/exec

# error
# valgrind --leak-check=full ./bin/exec  --gtest_filter=network_wrapper.test_tcp_socket_with_selector

# error
# valgrind --leak-check=full ./bin/exec  --gtest_filter=network_wrapper.test_tcp_socket_with_poller

# error
# valgrind --leak-check=full ./bin/exec  --gtest_filter=network_wrapper.test_udp_socket_with_poller

# clean run
# valgrind --leak-check=full ./bin/exec  --gtest_filter=network_wrapper.test_udp_socket_with_selector
