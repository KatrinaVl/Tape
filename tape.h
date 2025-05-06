#pragma once
#include <string>
#include <fstream>
#include <stdio.h>
#include <thread>
#include <iostream>
#include <queue>
#include "config.h"

class Tape {
public:
    Tape(std::string tape) : tape_file_(tape) {}
    Tape() = default;

    std::string GetFile()
    /* return name of file which imitate the tape */
    {
        return tape_file_;
    }

private:
    std::string tape_file_;
};

class TapeWorker {
public:
    TapeWorker() = default;
    TapeWorker(const Tape tape, Config config) : tape_(tape), config_(config){}
    ~TapeWorker() { close(); }

    void close()
    /* closing stream for file */

    {
        if (file_ifstream_.is_open()) {
            file_ifstream_.close();
        }
        if (file_ofstream_.is_open()) {
            file_ofstream_.close();
        }
    }

    void open(bool for_read)
    /* open file for reading if for_read == true, and in opposite open file for writing */
    {
        if (for_read) {
            file_ifstream_.open(tape_.GetFile(), std::ios::in);
        } else {
            file_ofstream_.open(tape_.GetFile(), std::ios::out);
        }
    }

    int read()
    /* read current number from tape and then shift tape */
    {
        int number = -1;
        if (!file_ifstream_.is_open()) {
            throw std::runtime_error("Не удалось найти ленту");
        }

        if (file_ifstream_.eof()) {
            std::cerr << "Лента закончилась" << std::endl;
        }

        file_ifstream_ >> number;
        std::this_thread::sleep_for(std::chrono::milliseconds(config_.read_delay));
        std::this_thread::sleep_for(std::chrono::milliseconds(config_.shift_delay));
        return number;
    }

    void write(int number)
    /* write current number to tape and then shift tape */
    {

        if (quantity_of_numbers == 0) {
            file_ofstream_ << number;
        } else {
            file_ofstream_ << "\n" << number;
        }
        ++quantity_of_numbers;
        std::this_thread::sleep_for(std::chrono::milliseconds(config_.write_delay));
        std::this_thread::sleep_for(std::chrono::milliseconds(config_.shift_delay));
    }

    void SeekValue()
    /* rewinding the tape to the desired value */
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(config_.seek_delay));
    }

    bool NotEndOfTape()
    /* checking of the file to ending */
    {
        return !file_ifstream_.eof();
    }



private:
    Tape tape_;
    mutable std::ifstream file_ifstream_;
    mutable std::ofstream file_ofstream_;
    Config config_;
    int quantity_of_numbers = 0;
};

class SortTape {
public:
    SortTape(Tape tape_in, Tape tape_out, Config config) :
    output_tape(tape_out), tape_size_(config.N), memory_size_(config.M),
    worker_(TapeWorker(tape_in, config)), tmp_(config.tmp), config_(config) {}


    void sort()
    /* main sort function
     * Idea: distribute input tape on small tapes (large equal to (M / 4))
     * and then merge all of tmp_tapes.
     *
     * This is the most optimal way, because the difficulty approximately N * (log N / M)
     * */
    {
        std::vector<std::string> tapes = DistributeToTapes();

        while (tapes.size() != 1) {
            tapes = Merge(tapes);
            ++round_;
        }

        std::filesystem::rename(tapes[0], output_tape.GetFile());

    }

private:
    int64_t tape_size_;
    int64_t memory_size_;
    Tape output_tape;
    TapeWorker worker_;
    std::string tmp_;
    int64_t round_ = 0;
    Config config_;

    std::vector<std::string> DistributeToTapes()
    /* split input file to tapes which had size (M / 4, because only such size can load) */
    {
        std::vector<std::string> tmp_tapes;
        std::vector<int> buffer;
        int64_t current_tape_size = 0;
        worker_.open(true);
        while (current_tape_size < tape_size_) {

            int64_t current_memory_size = 0;
            while (current_memory_size < (memory_size_ / sizeof(int)) && worker_.NotEndOfTape()) {
                buffer.push_back(worker_.read());
                ++current_memory_size;
                ++current_tape_size;
            }

            std::sort(buffer.begin(), buffer.end());

            std::string new_file = tmp_ + std::to_string(tmp_tapes.size()) + ".txt";

            Tape new_tape (new_file);
            TapeWorker curr_worker(new_tape, config_);

            curr_worker.open(false);

            for (auto v : buffer){
                curr_worker.write(v);
            }
            curr_worker.close();

            tmp_tapes.push_back(new_file);
            buffer.clear();
        }
        return tmp_tapes;
    }

    /* Merging all tmp_tapes:
     * take every M/4 tapes and merge them using function MergeMTapes*/
    std::vector<std::string> Merge(std::vector<std::string>& tmp_tapes) {
        std::vector<std::string> merged_tapes;

        int64_t M = memory_size_ / (sizeof(int));

        for (size_t i = 0; i < tmp_tapes.size(); i += M) {
            std::vector<Tape> M_tapes;
            Tape m_tape;
            std::string m_file;
            m_file = tmp_ + "M_" + std::to_string(round_) + "_" + std::to_string(i);
            m_tape = Tape(m_file);

            for (int j = i; j < std::min(size_t(i + M), tmp_tapes.size()); ++j) {
                Tape curr_tape (tmp_tapes[j]);
                M_tapes.push_back(curr_tape);
            }

            MergeMTapes(M_tapes, m_tape);
            merged_tapes.push_back(m_file);

        }

        return merged_tapes;
    }

    struct Element {
        int index;
        int value;
    };

    struct Comparator {
        bool operator()(const Element& a, const Element& b) {
            return a.value > b.value;
        }
    };

    void MergeMTapes(std::vector<Tape>& tapes, Tape& output)
    /* Merging M/4 tmp_tapes using priority_queue*/
    {
        TapeWorker curr_worker(output, config_);

        std::priority_queue<Element, std::vector<Element>, Comparator> que;
        std::vector<std::shared_ptr<TapeWorker>> tapes_workers;

        for (int i = 0; i < tapes.size(); ++i) {
            std::shared_ptr<TapeWorker> curr_worker_tape = std::make_shared<TapeWorker>(tapes[i].GetFile(), config_);
            curr_worker_tape->open(true);
            curr_worker.SeekValue();
            que.push({i, curr_worker_tape->read()});
            tapes_workers.push_back(curr_worker_tape);
        }

        curr_worker.open(false);
        while (!que.empty()) {
            Element e = que.top();
            que.pop();
            curr_worker.write(e.value);
            std::shared_ptr<TapeWorker> t = tapes_workers[e.index];
            if (t->NotEndOfTape()) {
                int number = t->read();
                que.push({e.index, number});
            }
        }
        curr_worker.close();
    }
};

