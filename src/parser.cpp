#include "structures.hpp"
#include "parser.hpp"
using namespace std;

ifstream in;



// Utility functions

bool read_junk(int quanti=INT_MAX){
    string sporcizia = "";
    for(int i=0; i<quanti; i++){
        in >> sporcizia;
        if(sporcizia=="}")
            return 1;
    }
    return 0;
}


void read_parameters(int numPar, vector<string>& v){
    string parola;
    for(int i=0;i<numPar-1;i++){
        in >> parola;
        parola = parola.substr(0, parola.length()-1);
        v.push_back(parola);
    }
    in >> parola;
    v.push_back(parola);
    return;
}


bool check_var_existence(string var_name, BayesianNetwork& network){
    if(network.name_to_id.find(var_name) == network.name_to_id.end()){
        cout << "ERROR! Variable \"" + var_name + "\" not defined in the file!" << endl;
        return 1;
    }
    return 0;
}




// Parsing: network

void read_network(BayesianNetwork& network){
    in >> network.network_name;
    read_junk();
    return;
}




// Parsing: variable

void read_variable(BayesianNetwork& network){
    string nameVar;
    int numPar;
    in >> nameVar;
    read_junk(4);
    in >> numPar;
    read_junk(2);

    vector<string> listPar;
    read_parameters(numPar, listPar);

    int64_t id = network.num_variables;
    
    Variable new_variable(nameVar, id, listPar);
    
    network.add_variable(new_variable);

    read_junk();
    return;
}




// Parsing: probability

void read_values(vector<double>& values){
    string temp;
    for(double& v : values){
        in >> temp;
        temp = temp.substr(0, temp.size()-1);
        v = stod(temp);
    }
    return;
}


void read_cpt(int64_t numParents, int number_values, Variable& var){
    vector<vector<double>> cpt_input;
    double prob;
    string probs;

    while(!read_junk(max(int64_t(1), numParents))){
        vector<double> temp;

        for(int i=0; i<number_values; i++){
            in >> probs;
            probs = probs.substr(0, probs.size()-1);
            prob = stod(probs);
            temp.push_back(prob);
        }
        cpt_input.push_back(temp);
    }

    var.add_cpt(cpt_input);
    
    return;
}


bool read_probability(BayesianNetwork& network){
    read_junk(1);
    string var_name, par_name;
    in >> var_name;

    if(check_var_existence(var_name, network))
        return 1;
    int64_t id_var = network.name_to_id[var_name];
    int number_values_var = network.variables[id_var].values.size();
    Variable& var = network.variables[id_var];


    string temp;
    in >> temp;
    vector<string> parents;
    
    // read (if any) parents
    if(temp == "|"){

        while(in >> par_name){
            if(par_name == ")") break;
            if(par_name[par_name.size()-1] == ',')
                par_name = par_name.substr(0, par_name.length()-1);
            if(check_var_existence(par_name, network))
                return 1;
            parents.push_back(par_name);   
        }
    }
    
    var.add_parents(parents);


    // read CPT
    read_junk(1);
    read_cpt(parents.size(), number_values_var, var);

    return 0;
}




// File parser

void parser(string file_name, BayesianNetwork& network){

    string directory = "data/";
    string file = directory + file_name + ".bif";

    network.clear_network();

    cout << "Parsing " << file_name << ".bif..." << endl;
    in.open(file);
    if (!in.is_open()) {
        cerr << "Error: couldn't open file " << file_name << endl;
        return;
    }


    string input;

    while(in >> input){
        if(input == "network")
            read_network(network);
        else if(input == "variable")
            read_variable(network);
        else if(input == "probability"){
            if(read_probability(network)){
                network.clear_network();
                return;
            }
        }
    }

    in.close();

    network.make_graph();
    network.topological_sort();

    cout << "Done!" << endl;
    return;
}

