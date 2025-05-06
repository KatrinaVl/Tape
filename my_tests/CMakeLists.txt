add_executable(my_tests tests.cpp ../tape.h ../external/catch2/catch_amalgamated.cpp)

target_include_directories(my_tests PRIVATE ../external/catch2)
target_include_directories(my_tests PRIVATE ../)

add_test(NAME my_tests COMMAND my_tests)