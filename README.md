 C++ profiler 
 prog: Jayakumar Rajagopal  jaykumar.r@gmail.com
 

 Why this profiler: 
 In profilers, the program you profile/benchmark are being run by the profiler. It is time consuming, performance downgrading though non-invasive.
 
 My profiler is straight opposite. 
 The program calls my profiler , though invasive. 

Advantage: Your program can run the profiler for narrow portion(s) of the code. i.e 

     1. for few blocks alone, 
     
     2. few iterations alone 
     
     3. a specific account number alone(among 1000s) that causes delay etc, by embedding the profiler call with if-block.
     
     4. start and stop profiler between specific duration, such as if 100 loops / calls made etc while the program will continue its normal run.
     

This has a distinct advantage : You need not run profiler over a library or a trivial code that cannot be tuned anyway.

     Usage : g++ fib.cpp et_prof_code.cpp
     
     The fib.cpp is sample program, that uses this profiler. 
