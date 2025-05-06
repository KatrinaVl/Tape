#pragma once
#include <fstream>
#include <string>
#include <sstream>
#include <stdexcept>

struct Config {
    std::string input;
    std::string output;
    std::string tmp;
    size_t N;
    size_t M;
    int read_delay;
    int write_delay;
    int seek_delay;
    int shift_delay;
};

Config loadConfig(const std::string& file) {
    std::ifstream c(file);
    if (!c.is_open()) {
        throw std::runtime_error("Не удалось открыть конфигурационный файл");
    }

    std::unordered_map<std::string, std::string> values;
    std::string line;
    while (std::getline(c, line)) {
        auto delimiterPos = line.find('=');
        std::string key = line.substr(0, delimiterPos);
        std::string value = line.substr(delimiterPos + 1);

        values[key] = value;
    }

    Config config;

    config.input = values.at("input");
    config.output = values.at("output");
    config.tmp = values.at("tmp");
    config.N = std::stoul(values.at("N"));
    config.M = std::stoul(values.at("M"));
    config.read_delay = std::stoi(values.at("read_delay"));
    config.write_delay = std::stoi(values.at("write_delay"));
    config.seek_delay = std::stoi(values.at("seek_delay"));
    config.shift_delay = std::stoi(values.at("shift_delay"));

    return config;
}

