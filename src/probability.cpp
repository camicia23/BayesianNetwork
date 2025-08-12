#include "probability.hpp"
#include "structures.hpp"



// Algorithms for Variable elimination

Factor::Factor(const Variable& var, const BayesianNetwork& net){
    var_ids.clear();
    var_values.clear();

    var_ids.push_back(var.id);
    var_values.push_back(var.values);
    var_to_pos[var.id] = 0;

    for(const string& parent_name : var.parents){
        int64_t parent_id = net.name_to_id.at(parent_name);
        var_ids.push_back(parent_id);
        var_values.push_back(net.variables[parent_id].values);
        var_to_pos[parent_id] = var_ids.size() - 1;
    }

    // copy flattened cpt
    int64_t total_size = 1;
    for(const auto& values: var_values)
        total_size *= values.size();
    factor_values.resize(total_size);

    int64_t i = 0;
    for (const auto& row : var.cpt) {
        for (double cpt_prob : row)
            factor_values[i++] = cpt_prob;
    }

}


void Factor::restrict_factor(int64_t var_id, int value_id){

    if (!var_to_pos.count(var_id)) return;

    size_t var_pos = var_to_pos[var_id];
    size_t old_var_number = var_ids.size();

    // start building new restricted Factor
    vector<int64_t> new_var_ids;
    vector<vector<string>> new_var_values;
    unordered_map<int64_t, int64_t> new_var_to_pos;

    int64_t new_factor_values_size = 1;

    for(size_t i=0; i < old_var_number; i++){
        if(i == var_pos) continue;
        new_var_ids.push_back(var_ids[i]);
        new_var_values.push_back(var_values[i]);
        new_var_to_pos[var_ids[i]] = new_var_ids.size() - 1;
        new_factor_values_size *= var_values[i].size();
    }


    // build restricted factor values
    vector<double> new_factor_values(new_factor_values_size, 0.0);

    for(size_t i=0; i < factor_values.size(); i++){
        int64_t old_temp = i;

        bool flag = false;

        vector<int64_t> combination(var_ids.size());

        for(size_t j=0; j < var_ids.size(); j++){
            size_t var_values_size = var_values[j].size();

            combination[j] = old_temp % var_values_size;
            old_temp /= var_values_size;

            if(var_pos == j && combination[var_pos] != value_id){
                flag = true;
                break;
            }

        }


        if(flag) continue;

        vector<int> new_assignment;
        for (size_t j = 0; j < old_var_number; j++)
            if (j != var_pos)
                new_assignment.push_back(combination[j]);


        // insert value in new factor_values
        int64_t new_config_pos = 0;
        int64_t temp = 1;
        for (size_t j = 0; j < new_assignment.size(); j++) {  // Little-endian
            new_config_pos += new_assignment[j] * temp;
            temp *= new_var_values[j].size();
        }

        new_factor_values[new_config_pos] = factor_values[i];
    }


    // assign everything to this Factor
    var_ids = new_var_ids;
    var_values = new_var_values;
    var_to_pos = new_var_to_pos;
    factor_values = new_factor_values;   
}


int64_t Factor::get_index(const vector<int>& combination) const {
    int64_t ans = 0;
    int64_t temp = 1;
    for(size_t i=0; i<combination.size(); i++){
        ans += combination[i] * temp;
        temp *= var_values[i].size();
    }
    return ans;
}


Factor Factor::multiply(const Factor& other_factor) const{
    Factor result;


    // create var_ids, var_values and var_to_pos of result
    unordered_set<int64_t> inserted_ids;
    int64_t new_cpt_size = 1;
    for(size_t i=0; i<var_ids.size(); i++){
        inserted_ids.insert(var_ids[i]);
        result.var_ids.push_back(var_ids[i]);
        result.var_values.push_back(var_values[i]);
        new_cpt_size *= var_values[i].size();
    }
    for(size_t i=0; i<other_factor.var_ids.size(); i++){
        if(inserted_ids.count(other_factor.var_ids[i])) continue;
        inserted_ids.insert(other_factor.var_ids[i]);
        result.var_ids.push_back(other_factor.var_ids[i]);
        result.var_values.push_back(other_factor.var_values[i]);
        new_cpt_size *= other_factor.var_values[i].size();
    }

    result.factor_values.resize(new_cpt_size, 0.0);

    for(size_t i=0; i<result.var_ids.size(); i++)
        result.var_to_pos[result.var_ids[i]] = i;

    vector<int> combination(result.var_ids.size(), 0);
    for(int64_t i=0; i < new_cpt_size; i++){
        int64_t temp = i;
        for(size_t j=0; j < result.var_ids.size(); j++){
            combination[j] = temp % result.var_values[j].size();
            temp /= result.var_values[j].size();
        }

        // find the corresponding combination of values of the two original Factors
        vector<int> combination_1(var_ids.size()), combination_2(other_factor.var_ids.size());
        for(size_t j=0; j<var_ids.size(); j++)
            combination_1[j] = combination[result.var_to_pos[var_ids[j]]];
        for(size_t j=0; j<other_factor.var_ids.size(); j++)
            combination_2[j] = combination[result.var_to_pos[other_factor.var_ids[j]]];
        

        // multiply the 2 factor_values of the 2 original factors
        double v1 = factor_values[get_index(combination_1)];
        double v2 = other_factor.factor_values[other_factor.get_index(combination_2)];

        result.factor_values[i] = v1 * v2;
    }

    return result;
}


Factor Factor::sum_out(int64_t var_id) const {
    if(!var_to_pos.count(var_id)) return *this;

    Factor result;
    
    size_t pos = var_to_pos.at(var_id);

    int64_t result_cpt_size = 1;
    for(size_t i=0; i<var_ids.size(); i++){
        if(i == pos)    continue;
        
        result.var_ids.push_back(var_ids[i]);
        result.var_values.push_back(var_values[i]);
        result.var_to_pos[var_ids[i]] = result.var_ids.size() - 1;
        result_cpt_size *= var_values[i].size();
    }

    result.factor_values.resize(result_cpt_size, 0.0);

    for(size_t i=0; i<factor_values.size(); i++){
        vector<int> reduced_combination;
        int temp_combination;



        int64_t temp = i;
        for(size_t j=0; j<var_ids.size(); j++){
            temp_combination = temp % var_values[j].size();
            temp /= var_values[j].size();

            if(j == pos) continue;
            reduced_combination.push_back(temp_combination);
        }


        int64_t result_idx = result.get_index(reduced_combination);

        result.factor_values[result_idx] += factor_values[i];
    }

    return result;
}


void Factor::normalize() {
    double sum = 0.0;
    for(double x : factor_values) sum += x;
    if(sum == 0) return;
    for(double& x : factor_values) x /= sum;
}




// Variable elimination

Factor variable_elimination(BayesianNetwork& net, int64_t query_id, unordered_map<int64_t, int>& evidences){

    // 1: Apply evidences by restricting factor by 1 evidence variable
    vector<Factor> factors;
    for(Variable& var : net.variables){
        // create Factor of var
        Factor f(var, net);
        for(auto& [ev_id, ev_value_id] : evidences)
            if(f.var_to_pos.count(ev_id))
                f.restrict_factor(ev_id, ev_value_id);
            

        factors.push_back(f);        
    }


    // 2: List hidden variables
    vector<int64_t> hidden_variables;

    for(int64_t i=0; i<net.num_variables; i++)
        if(i != query_id && !evidences.count(i))
            hidden_variables.push_back(i);


    // 3: Remove hidden variables by multiplying and summing out
    for(int64_t elem : hidden_variables){

        vector<Factor> present;
        vector<Factor> not_present;

        for(Factor& f : factors){
            if(f.var_to_pos.count(elem))
                present.push_back(f);
            else
                not_present.push_back(f);
        }


        if(present.empty()) continue;
        Factor multiplied = present[0];
        for(size_t i = 1; i < present.size(); i++){
            multiplied = multiplied.multiply(present[i]);
        }
        
        
        Factor summed = multiplied.sum_out(elem);
        not_present.push_back(summed);
        factors = not_present;
    }



    // 4: Multiply remaining factors
    Factor result = factors[0];

    for(size_t i = 1; i < factors.size(); i++)
        result = result.multiply(factors[i]);

    
    // 5: Normalize the factor
    result.normalize();
    return result;
}


