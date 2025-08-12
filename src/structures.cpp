#include "structures.hpp"
#include "probability.hpp"


// Variable

void Variable::make_values_id(){
    for(int i=0; i < int(values.size()); i++){
        values_id[values[i]] = i;
    }
}


void Variable::add_parents(vector<string>& _parents){
    parents = _parents;
}


void Variable::add_cpt(vector<vector<double>>& cpt_input){
    cpt = cpt_input;
    return;
}






// BayesianNetwork

void BayesianNetwork::clear_network(){
    network_name.clear();
    num_variables = 0;
    variables.clear();
    variables.shrink_to_fit();
    name_to_id.clear();
    graph.clear();
    graph.shrink_to_fit();
    toposort_vector.clear();
    toposort_vector.shrink_to_fit();
    memo.clear();
    memo.shrink_to_fit();
    return;
}



void BayesianNetwork::add_variable(const Variable& var){
    variables.push_back(var);
    num_variables++;
    name_to_id[var.name] = var.id;
}



void BayesianNetwork::make_graph(){

    // Build vector<vector<int64_t>> graph;
    graph.resize(num_variables);

    for(Variable var : variables){
        for(string par : var.parents){
            int64_t id_parent = name_to_id[par];
            graph[id_parent].push_back(var.id);
        }
    }

    // Initialize memo
    memo.resize(num_variables);
    for(int i=0;i<num_variables;i++){
        Variable var = variables[i];
        memo[i].resize(var.values.size(), -1.0);
    }
}



void dfs_toposort(int64_t var_id, vector<bool>& visited, BayesianNetwork& network){
    if(visited[var_id])
        return;
    if(network.graph[var_id].empty()){
        network.toposort_vector.push_back(var_id);
        visited[var_id] = 1;
        return;
    }
    for(int64_t child_id : network.graph[var_id]){
        if(!visited[child_id])
            dfs_toposort(child_id, visited, network);
    }
    
    network.toposort_vector.push_back(var_id);
    visited[var_id] = 1;
}



void BayesianNetwork::topological_sort(){
    vector<bool> visited(num_variables, 0);
    for(int i=0; i<num_variables; i++){
        if(!visited[i])
            dfs_toposort(i, visited, *this);
    }
    
    reverse(toposort_vector.begin(), toposort_vector.end());
}



void BayesianNetwork::print_nodes(){
    for(int64_t x : toposort_vector){
        cout << variables[x].name << endl;
    }
}



void BayesianNetwork::print_var(string name_var){
    if(check_variable_existence(name_var))
        return;
    Variable var = variables[name_to_id[name_var]];
    cout << "Variable:  " << var.name << endl;
    cout << "Values:    ";
    for(string x : var.values)
        cout << x << " ";
    cout << endl;
    if(var.parents.empty()){
        cout << "No parents" << endl;
        return;
    }
    cout << "Parents:   ";
    for(string x : var.parents)
        cout << x << " ";
    cout << endl;
    return;
}



bool BayesianNetwork::check_variable_existence(string var_name, string value_name){
    //  1 = doesn't exist
    //  0 = does exist

    // variable doesn't exist
    if(name_to_id.find(var_name) == name_to_id.end() ){
        cout << "Variable " << var_name << " doesn't exist..." << endl;
        return 1;
    }

    // doesn't ask for value
    if(value_name.empty())
        return 0;

    // value doesn't exist
    Variable variable = variables[name_to_id[var_name]];
    if (variable.values_id.find(value_name) == variable.values_id.end()){
        cout << var_name << "'s value " << value_name << " doesn't exist..." << endl;
        return 1;
    }

    return 0;
}