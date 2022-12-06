#ifndef COLINK_VT_DUMMY_H
#define COLINK_VT_DUMMY_H

#include <random>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include "picosha2.h"

using namespace std;

// dummy helper functions
void _save(const string &key, const vector<uint8_t> &payload)
{
    // preprocess the key to be fixed length
    std::string hash = "";
    picosha2::hash256_hex_string(key.begin(), key.end(), hash); // hash now has length=64
    // next, store it in a temporary file
    std::ofstream ofs;
    ofs.open(hash + ".comm.bin", std::ofstream::out | std::ofstream::trunc);
    ofs.write((char *)payload.data(), payload.size());
    ofs.close();
}
vector<uint8_t> _load(const string &key)
{
    // preprocess the key to be fixed length
    std::string hash = "";
    picosha2::hash256_hex_string(key.begin(), key.end(), hash); // hash now has length=64
    // next, check if the file exists and wait until it exists
    while (true)
    {
        std::ifstream ifs;
        ifs.open(hash + ".comm.bin", std::ifstream::in);
        if (ifs.good())
        {
            ifs.seekg(0, std::ios::end);
            size_t size = ifs.tellg();
            vector<uint8_t> payload(size);
            ifs.seekg(0, std::ios::beg);
            ifs.read((char *)payload.data(), size);
            ifs.close();
            return payload;
        }
        ifs.close();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
// exposed dummy functions
void set_variable(const string &task_id,
                  const string &key,
                  const vector<uint8_t> &payload,
                  const string &sender,
                  const vector<string> &receivers)
{
    for (auto &receiver : receivers)
    {
        _save(task_id + "." + sender + "." + receiver + "." + key, payload);
    }
}
vector<uint8_t> get_variable(const string &task_id,
                             const string &key,
                             const string &sender,
                             const string &receiver)
{
    return _load(task_id + "." + sender + "." + receiver + "." + key);
}

#endif // COLINK_VT_DUMMY_H