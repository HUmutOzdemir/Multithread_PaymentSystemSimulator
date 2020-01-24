# Multithread_PaymentSystemSimulator

 It is a multihreaded bank payment simulator.

## `Project2.cpp`

It is the main simulator. You can use this simulator in UNIX based systems. Beacuse Pthreads are used to implement it.

### Compile and Run

> make && simulation input.txt

## `RandomInput.cpp`

It takes number of customers and a file name as an argument and creates a random input with that file name.

### Compile and Run

> g++ RandomInput.cpp -o RandomInput

>./RandomInput 300 input.txt

## `RandomSequenceChecker.cpp`

It takes number of customers and a file name as an argument and creates a random input and the output of that input file.

### Compile and Run

> g++ RandomSequenceChecker.cpp -o RandomInput

>./RandomInput 300 input.txt
