#pragma once
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
using namespace std;



// Utility functions

bool read_junk(int quanti);

void read_parameters(int numPar, vector<string>& v);

bool check_var_existence(string var_name, BayesianNetwork& network);



// Parsing: network

void read_network(BayesianNetwork& network);



// Parsing: variable

void read_values(vector<double>& values);

void read_cpt(int64_t numParents, int number_values, Variable& var);

void read_variable(BayesianNetwork& network);



// Parsing: probability

bool read_probability(BayesianNetwork& network);



// File parser

void parser(string file_name, BayesianNetwork& network);