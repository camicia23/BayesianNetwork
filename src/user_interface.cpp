#include "user_interface.hpp"
#include "parser.hpp"
#include "structures.hpp"
#include "probability.hpp"


void Factor::print(BayesianNetwork& net, vector<pair<string, string>>& evidences, int64_t var_id, string var, int value_id, string value) const {

    // print all the values
    if(value_id == -1){
        for(size_t i=0; i < var_values[0].size(); i++){

            cout << "P(" << var << " = " << net.variables[var_id].values[i];
            
            if(!evidences.empty()) cout << " |";
            for(size_t i=0; i < evidences.size(); i++){
                cout << " " << evidences[i].first << " = " << evidences[i].second;
                if(i != evidences.size()-1)
                    cout << ",";
            }
            cout << ") = " << factor_values[i] << endl;
        }

    // print one single value
    } else {
        cout << "P(" << var << " = " << value;
        if(!evidences.empty()) cout << " |";
        for(size_t i=0; i < evidences.size(); i++){
            cout << " " << evidences[i].first << " = " << evidences[i].second;
            if(i != evidences.size()-1)
                cout << ",";
        }
        cout << ") = " << factor_values[value_id] << endl;
    }
}



void print_files(){
    fs::path dir = "data/";

    try {
        for (const auto& entry : fs::directory_iterator(dir)) {
            if (fs::is_regular_file(entry.status()) && entry.path().extension() == ".bif") {
                std::cout << entry.path().filename().string() << std::endl;
            }
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return;
}



bool read_input(BayesianNetwork& network){
    string input, junk1 = "", junk2 = "", command, body;
    cout << "\n> ";
    getline(cin, input);
    istringstream iss(input);
    iss >> command;

    transform(command.begin(), command.end(), command.begin(), [](unsigned char c) {
        return tolower(c);
    });


    // HELP
    if (command == "help"){
        print_help();
    
    // IMPORT FILE
    } else if (command == "import"){
        iss >> input;
        parser(input, network);
    
    // FILES
    } else if (command == "files") {
        print_files();

    // LIST
    } else if (command == "list"){
        if(iss >> body){                            // list VAR
            network.print_var(body);
        } else {                                    // list
            network.print_nodes();            
        }
    
    // PROBABILITY
    } else if (command == "prob"){
        string var, value = "", evidence_var, evidence_value;
        iss >> var >> junk1;

        // SINGLE PROBABILITY?
        if(junk1 == "="){
            iss >> value >> junk2;
        }

        if(network.check_variable_existence(var, value))
            return 0;


        vector<pair<string, string>> evidences;
        unordered_map<int64_t, int> evidences_map;

        int64_t var_id = network.name_to_id[var];
        int value_id;
        if(junk1 == "=")
            value_id = network.variables[var_id].values_id[value];


        // read evidences
        while(iss >> evidence_var >> junk2 >> evidence_value){
            if(evidence_value.back() == ',')
                evidence_value = evidence_value.substr(0, evidence_value.size()-1);
            
            if(evidence_var == var){
                cout << "Error! Variable " << var << " present both as query and evidence" << endl;
                return 0;
            }

            if(network.check_variable_existence(evidence_var, evidence_value))
                return 0;
            
            evidences.push_back({evidence_var, evidence_value});
            
            int64_t evidence_id = network.name_to_id[evidence_var];
            int evidence_value_id = network.variables[evidence_id].values_id[evidence_value];
            evidences_map[evidence_id] = evidence_value_id;
        }
        
        
        Factor query_factor = variable_elimination(network, var_id, evidences_map);

        // SINGLE CONDITIONAL PROBABILITY
        if(junk1 == "=")
            query_factor.print(network, evidences, var_id, var, value_id, value);
        // ALL CONDITIONAL PROBABILITIES
        else
            query_factor.print(network, evidences, var_id, var);

    // QUIT
    } else if (command == "quit"){
        cout << "Goodbye!" << endl;
        return 1;
    
    // INVALID COMMAND
    } else {
        cout << "Invalid input..." << endl;
    }
    
    return 0;
}




void print_help(){
    cout << endl;
    cout << "-------------------------------------------------------------------------   MANUAL   --------------------------------------------------------------------------" << endl << endl;
    cout << "  COMMAND                                                             MEANING" << endl << endl;
    cout << "   help                                                                open the user manual" << endl << endl;
    cout << "   import FILENAME                                                     import BIF file FILENAME.bif from data/" << endl << endl;
    cout << "   files                                                               list available BIF files in data/" << endl << endl;
    cout << "   list                                                                list variables in topological order" << endl << endl;
    cout << "   list VAR                                                            list variable VAR with its values and parents" << endl << endl;
    cout << "   prob VAR [ = VALUE ]                                                compute the marginal probability P(VAR [ = VALUE ])" << endl << endl;
    cout << "   prob VAR [ = VALUE ] | VAR1 = VALUE1, VAR2 = VALUE2, ...            compute the conditional probability P(VAR [= VALUE] | VAR1 = VALUE1, VAR2 = VALUE2, ...)" << endl << endl;
    cout << "   quit                                                                exit program" << endl;
    cout << endl;
    cout << "----------------------------------------------------------------------------------------------------------------------------------------------------------------" << endl;
    cout << endl;
}




void welcome(){
    cout << "\nWelcome!" << endl;
    cout << "Insert 'help' for the user manual." << endl;
}