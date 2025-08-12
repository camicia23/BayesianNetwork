#pragma once
#include "structures.hpp"
#include <string>
#include <unordered_set>


// variable elimination

Factor variable_elimination(BayesianNetwork& net, int64_t query_id, unordered_map<int64_t, int>& evidences);
