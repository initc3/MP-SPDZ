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
void _save(const string &key, const vector<uint8_t> &payload);
vector<uint8_t> _load(const string &key);
// exposed dummy functions
void set_variable(const string &task_id,
                  const string &key,
                  const vector<uint8_t> &payload,
                  const string &sender,
                  const vector<string> &receivers);
vector<uint8_t> get_variable(const string &task_id,
                             const string &key,
                             const string &sender,
                             const string &receiver);

#endif // COLINK_VT_DUMMY_H