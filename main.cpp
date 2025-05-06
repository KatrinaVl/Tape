#include "tape.h"

int main(int argc, char* argv[]) {

    Config config = loadConfig("config.txt");
    Tape tape_in(argv[1]);
    Tape tape_out(argv[2]);
    SortTape sort_tape(tape_in, tape_out, config);
    sort_tape.sort();
    return 0;
}
