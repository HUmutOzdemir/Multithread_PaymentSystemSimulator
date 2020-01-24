#include <fstream>
#include <cstring>
#include <iostream>
using namespace std;



int main(int argc, char ** argv){
    string payment_types[5] = {"cableTV","electricity","gas","telecommunication","water"};
    int total_payments[5] = {0,0,0,0,0};
    int number_of_customers = stoi(argv[1]);
    ofstream outfile;
    outfile.open(argv[2]);
    outfile << number_of_customers << endl;
    srand(time(NULL));
    for(int i=0; i<number_of_customers;i++){
        outfile << rand() % 800 << "," << rand()%10+1 << ",";
        int index = rand()%5;
        outfile << payment_types[index] << ",";
        int payment = rand() % 1000 + 1;
        total_payments[index]+=payment;
        outfile << payment << endl;
    }

    for(int i=0;i<5;i++){
        cout << payment_types[i] << " " << total_payments[i] << endl;
    }


}