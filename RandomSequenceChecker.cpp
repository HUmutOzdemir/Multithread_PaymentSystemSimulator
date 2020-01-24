#include <fstream>
#include <cstring>
#include <iostream>
using namespace std;

string name_of_logfile(string s){
    string temp = "";
    for(int i=0;i<s.size()-3;i++){
        if(".txt"!=s.substr(i,i+3))
            temp+=s[i];
    }
    if(temp=="")
        temp = s;
    return "true_" + temp + "_log.txt";
}


int main(int argc, char ** argv){
    string payment_types[5] = {"cableTV","electricity","gas","telecommunication","water"};
    int total_payments[5] = {0,0,0,0,0};
    int number_of_customers = stoi(argv[1]);
    ofstream inputfile(argv[2]);
    ofstream logfile(name_of_logfile(argv[2]));
    inputfile << number_of_customers << endl;
    int sequence[number_of_customers];
    int paymnet_arr[number_of_customers];
    int sequence_types[number_of_customers];
    memset(&sequence,-1,sizeof(int)*number_of_customers);
    srand(time(NULL));
    for(int i=1; i<=number_of_customers;i++){
        int sleeptime = rand()%300;
        while(sequence[sleeptime]!=-1){
            sleeptime = rand()%300;
        }
        sequence[sleeptime] = i;
        inputfile << sleeptime*50 << "," << rand()%10+1 << ",";
        int index = rand()%5;
        sequence_types[sleeptime] = index;
        inputfile << payment_types[index] << ",";
        int payment = rand() % 1000 + 1;
        paymnet_arr[sleeptime] = payment;
        total_payments[index]+=payment;
        inputfile << payment << endl;
    }
    for(int i=0;i<number_of_customers;i++){
        logfile << "Customer" << sequence[i] << "," << paymnet_arr[i] << "TL";
        logfile << "," << payment_types[sequence_types[i]] << endl;
    }
    logfile << "All payments are completed." << endl;
    logfile << "CableTV: " << total_payments[0] << "TL" << endl;
    logfile << "Electricity: " << total_payments[1] << "TL" << endl;
    logfile << "Gas: " << total_payments[2] << "TL" << endl;
    logfile << "Telecommunication: " << total_payments[3] << "TL" << endl;
    logfile << "Water: " << total_payments[4] << "TL" << endl;


}