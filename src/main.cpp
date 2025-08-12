#include "user_interface.hpp"
using namespace std;


int main(){
    BayesianNetwork network;
    
    welcome();

    while(true){
        if(read_input(network))
            break;
    }

    return 0;
}