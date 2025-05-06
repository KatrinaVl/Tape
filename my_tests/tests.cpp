#define CATCH_CONFIG_MAIN
#include "../external/catch2/catch_amalgamated.hpp"
#include "../tape.h"
#include <fstream>

TEST_CASE("CheckConfig") {
    /* This test checks config*/
    Config config = loadConfig("my_tests/test_files/test_config_1.txt");

    CHECK(config.input == "my_tests/test_files/test_in_1.txt");
    CHECK(config.output == "my_tests/test_files/test_out_1.txt");
    CHECK(config.tmp == "tmp/");
    CHECK(config.N == 12);
    CHECK(config.M == 20);
    CHECK(config.read_delay == 5);
    CHECK(config.write_delay == 10);
    CHECK(config.seek_delay == 30);
    CHECK(config.shift_delay == 3);
}

TEST_CASE("CheckWorking") {
    /* This test checks regular working*/

    Config config = loadConfig("my_tests/test_files/test_config_1.txt");
    Tape in("my_tests/test_files/test_in_1.txt");
    Tape out("my_tests/test_files/test_out_1.txt");
    SortTape sort_tape(in, out, config);
    sort_tape.sort();


    std::vector<int> result;
    TapeWorker worker(out, config);
    worker.open(true);
    while (worker.NotEndOfTape()) {
        result.push_back(worker.read());
    }
    worker.close();

    CHECK(result == std::vector<int>({0, 1, 2, 3, 4, 4, 5, 7, 7, 8, 8, 9}));
}

TEST_CASE("CheckWorking2") {
    /* This test checks situation where number of tmp tapes > M and (M / 4) < N*/

    Config config = loadConfig("my_tests/test_files/config.txt");
    Tape in("my_tests/test_files/input.txt");
    Tape out("my_tests/test_files/output.txt");
    SortTape sort_tape(in, out, config);
    sort_tape.sort();


    std::vector<int> result;
    TapeWorker worker(out, config);
    worker.open(true);
    while (worker.NotEndOfTape()) {
        result.push_back(worker.read());
    }
    worker.close();

    std::vector<int> sorted;
    TapeWorker worker_in(in, config);
    worker_in.open(true);
    while (worker_in.NotEndOfTape()) {
        sorted.push_back(worker_in.read());
    }
    worker_in.close();

    std::sort(sorted.begin(), sorted.end());

    CHECK(result == sorted);
}

TEST_CASE("CheckWorking3") {
    /* This test checks situation where number of tmp tapes == 1 and (M / 4) > N */

    Config config = loadConfig("my_tests/test_files/test_config_2.txt");
    Tape in("my_tests/test_files/test_in_2.txt");
    Tape out("my_tests/test_files/test_out_2.txt");
    SortTape sort_tape(in, out, config);
    sort_tape.sort();


    std::vector<int> result;
    TapeWorker worker(out, config);
    worker.open(true);
    while (worker.NotEndOfTape()) {
        result.push_back(worker.read());
    }
    worker.close();

    std::vector<int> sorted;
    TapeWorker worker_in(in, config);
    worker_in.open(true);
    while (worker_in.NotEndOfTape()) {
        sorted.push_back(worker_in.read());
    }
    worker_in.close();

    std::sort(sorted.begin(), sorted.end());

    CHECK(result == sorted);
}

TEST_CASE("CheckWorking4") {
    /* This test checks situation where number of tmp tapes < M and > 1 and also that (M / 4) < N */

    Config config = loadConfig("my_tests/test_files/test_config_3.txt");
    Tape in("my_tests/test_files/test_in_3.txt");
    Tape out("my_tests/test_files/test_out_3.txt");
    SortTape sort_tape(in, out, config);
    sort_tape.sort();


    std::vector<int> result;
    TapeWorker worker(out, config);
    worker.open(true);
    while (worker.NotEndOfTape()) {
        result.push_back(worker.read());
    }
    worker.close();

    std::vector<int> sorted;
    TapeWorker worker_in(in, config);
    worker_in.open(true);
    while (worker_in.NotEndOfTape()) {
        sorted.push_back(worker_in.read());
    }
    worker_in.close();

    std::sort(sorted.begin(), sorted.end());

    CHECK(result == sorted);
}