#include <fstream>
#include <pthread.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <string>
#include <atomic>

#define NUMBER_OF_ATM 10
#define AMOUNT 0
#define TYPE 1
#define CUSTOMER 2

using namespace std;

struct customer_args{
    int customer_index;
    int ATM_index;
    int payment_amount;
    int payment_type;
    int sleep_time;
};

//Global Array to maintain the communication between ATM and customer threads
//0 => Amount of payment  1 => Bank Account Type 2 => Customer Index 
int ATM_payment_info[NUMBER_OF_ATM][3];
//Mutex of ATMs
pthread_mutex_t ATM_mutex[NUMBER_OF_ATM];
//With using these locks customer thread will end after ATM thread makes the payment
atomic<bool> synch_bool[NUMBER_OF_ATM];
//Stores the total amount of payment to each banck account
int bank_accounts[5];
//Mutex of the bank account variables
pthread_mutex_t bank_account_mutex[5];
//Output File
ofstream outfile;
//Mutex of the output file
pthread_mutex_t output_mutex;

string name_of_logfile(string s){
    string temp = "";
    for(int i=0;i<s.size()-3;i++){
        if(".txt"!=s.substr(i,i+3))
            temp+=s[i];
    }
    if(temp=="")
        temp = s;
    return temp + "_log.txt";
}

void *ATM_func(void * arg){
    int ATM_index = *((int *)arg);
    free(arg);
    while(true){
        //Waits for any customer to start
        while(!synch_bool[ATM_index].load())
            pthread_testcancel();
        //Waits for mutex of the bank account type then makes the payment    
        pthread_mutex_lock(&bank_account_mutex[ATM_payment_info[ATM_index][TYPE]]);
        bank_accounts[ATM_payment_info[ATM_index][TYPE]]+=ATM_payment_info[ATM_index][AMOUNT];
        pthread_mutex_unlock(&bank_account_mutex[ATM_payment_info[ATM_index][TYPE]]);
        //Prepares the output string
        string output_string = "Customer" + to_string(ATM_payment_info[ATM_index][CUSTOMER]) + "," + to_string(ATM_payment_info[ATM_index][AMOUNT]) + "TL,";
        if(ATM_payment_info[ATM_index][TYPE]==0)
            output_string += "cableTV";
        else if(ATM_payment_info[ATM_index][TYPE]==1)
            output_string += "electricity";
        else if(ATM_payment_info[ATM_index][TYPE]==2)
            output_string += "gas";
        else if(ATM_payment_info[ATM_index][TYPE]==3)
            output_string += "telecommunication";
        else
            output_string += "water";
        //Waits for mutex of the output file 
        pthread_mutex_lock(&output_mutex);
            outfile << output_string << endl;
        pthread_mutex_unlock(&output_mutex);
        //Clears the payment information
        for(int i=0;i<2;i++)
            ATM_payment_info[ATM_index][i] = -1;
        //Sends a finish signal to customer thread
        synch_bool[ATM_index].store(false);
    }
}
 
void *customer_func(void *arguments){
    struct customer_args *args = (struct customer_args *)arguments;
    int customer_index = args->customer_index;
    int ATM_index = args->ATM_index;
    int payment_amount = args->payment_amount;
    int payment_type = args->payment_type;
    int sleep_time = args->sleep_time;
    free(arguments);
    usleep(sleep_time*1000);
    //Waits for the ATM and locks it
    pthread_mutex_lock(&ATM_mutex[ATM_index]);
    //Give the payment information to ATM
    ATM_payment_info[ATM_index][AMOUNT] = payment_amount;
    ATM_payment_info[ATM_index][TYPE] = payment_type;
    ATM_payment_info[ATM_index][CUSTOMER] = customer_index;
    //Starts the ATM
    synch_bool[ATM_index].store(true);
    //Busy wait until a customer arrives
    while(synch_bool[ATM_index].load());
    //Unlocks the ATM
    pthread_mutex_unlock(&ATM_mutex[ATM_index]);
}

int main(int argc, char** argv){
    //Fills the array with default values
    memset(&ATM_payment_info,-1,sizeof(int)*NUMBER_OF_ATM*3);
    memset(&bank_accounts,0,sizeof(int)*5);
    //Initialize the mutexes
    for(int i=0;i<NUMBER_OF_ATM;i++){
        pthread_mutex_init(&ATM_mutex[i], NULL);
        synch_bool[i] = false;
    }
    for(int i=0;i<5;i++){
        pthread_mutex_init(&bank_account_mutex[i], NULL);
    }
    pthread_mutex_init(&output_mutex,NULL);
    pthread_t ATM_tids[10];
    outfile.open(name_of_logfile(argv[1]));
    //Creates the ATM threads
    //Each ATM thread takes its index as parameter
    for(int i=0;i<NUMBER_OF_ATM;i++){
        int *arg =(int *)malloc(sizeof(*arg));
        *arg = i;
        pthread_create(&ATM_tids[i],NULL,ATM_func,arg);
    }
    ifstream inputfile;
    inputfile.open(argv[1]);
    int number_of_customers;
    inputfile >> number_of_customers;
    string delimiter = ",";
    pthread_t customer_tids[number_of_customers];
    // Read the input file and create the customer threads
    for(int i=0;i<number_of_customers;i++){
        string line;
        inputfile >> line;
        struct customer_args *args = (struct customer_args *)malloc(sizeof(*args));
        args->customer_index = i+1;
        int index = 0;
        string arr[4];
        size_t start;
        size_t end = 0;
        const char delim = ',';
        while((start = line.find_first_not_of(delim, end)) != string::npos){
            end = line.find(delim, start);
		    arr[index] = line.substr(start, end - start);
            index++;
        }
        args->sleep_time = stoi(arr[0]);
        args->ATM_index = stoi(arr[1])-1;
        if(arr[2]=="cableTV")
            args->payment_type=0;
        else if(arr[2]=="electricity")
            args->payment_type=1;
        else if(arr[2]=="gas")
            args->payment_type=2;
        else if(arr[2]=="telecommunication")
            args->payment_type=3;
        else
            args->payment_type=4;
        args->payment_amount = stoi(arr[3]);
        pthread_create(&customer_tids[i],NULL,customer_func,(void*)args);
    }
    //Wait until all customer threads are finished
    for(int i=0;i<number_of_customers;i++)
        pthread_join(customer_tids[i],NULL);
    //Terminate all ATM threads
    for(int i=0;i<NUMBER_OF_ATM;i++)
        pthread_cancel(ATM_tids[i]);
    //Complete the output
    outfile << "All payments are completed." << endl;
    outfile << "CableTV: " << bank_accounts[0] << "TL" << endl;
    outfile << "Electricity: " << bank_accounts[1] << "TL" << endl;
    outfile << "Gas: " << bank_accounts[2] << "TL" << endl;
    outfile << "Telecommunication: " << bank_accounts[3] << "TL" << endl;
    outfile << "Water: " << bank_accounts[4] << "TL" << endl;
}