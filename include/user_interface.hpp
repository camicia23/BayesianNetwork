#pragma once
#include <iostream>
#include <sstream>
#include <filesystem>
#include "structures.hpp"
using namespace std;
namespace fs = std::filesystem;


void print_files();
bool read_input(BayesianNetwork& network);
void print_help();
void welcome();