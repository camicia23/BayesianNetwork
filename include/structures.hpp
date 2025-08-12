#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>
using namespace std;




struct Variable{
    string name;
    int64_t id;
    vector<string> values;
    vector<string> parents;
    unordered_map<string, int> values_id;
    vector<vector<double>> cpt;
    double memoComputed;

    Variable(string nV, int64_t _id, vector<string>&lP) : name(nV), id(_id), values(lP), memoComputed(false) {
        make_values_id();
    }
    void make_values_id();
    void add_parents(vector<string>& _parents);
    void add_cpt(vector<vector<double>>& cpt_input);
};




struct BayesianNetwork{
    string network_name;
    int64_t num_variables;
    vector<Variable> variables;
    unordered_map<string, int64_t> name_to_id;
    vector<vector<int64_t>> graph;                  // adjacent list
    vector<int64_t> toposort_vector;
    vector<vector<double>> memo;

    BayesianNetwork() : num_variables(0){}

    void clear_network();
    void add_variable(const Variable& var);
    void make_graph();
    void topological_sort();
    void print_nodes();
    void print_var(string name_var);
    bool check_variable_existence(string var_name, string value_name = "");
};


void dfs_toposort(int64_t var_id, vector<bool>& visited, BayesianNetwork& network);



struct Factor {
    vector<int64_t> var_ids; // variables id
    vector<vector<string>> var_values;
    unordered_map<int64_t, int64_t> var_to_pos; // map: var_id → position in var_ids
    vector<double> factor_values; // flattened table
    
    Factor() {}
    Factor(const Variable& var, const BayesianNetwork& net);
    
    void restrict_factor(int64_t var_id, int value_index);
    Factor multiply(const Factor& other) const;
    Factor sum_out(int64_t var_id) const;

    void normalize();
    int64_t get_index(const vector<int>& assignment) const;
    void print(BayesianNetwork& net, vector<pair<string, string>>& evidences, int64_t var_id, string var, int value_id = -1, string value = "") const;
};