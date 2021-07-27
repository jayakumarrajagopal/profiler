 C++ profiler 
 prog: Jayakumar Rajagopal  jaykumar.r@gmail.com
 

 Other profilers: 
 In profilers, in general the program you profile(benchmark) is executed by the profiler (calculating time taken in each function). It is time consuming, performance downgrading, and spans entire exe's run time. (although non-invasive)

 *My profiler is straight opposite:* 
 The C++ program being profiled (source code) calls my profiler to start and stop profiling  when just needed (calculating time taken in each place this profiler is refered). We have have to add a function call at the place(block/function). But it is very light, only specific portions and times we want profile will have overhead -- although invasive since we are adding few lines of function calls. 
 
syntax: 
 1. bench_mark_fun; // this lines causes the current function to be profiled.
 2. bench_mark_block; // this lines causes the current loop/conditional block to be profiled.

Advantage: Your program can run the profiler for narrow portion(s) of the code. i.e 

     1. for few blocks alone, 
     
     2. for a few iterations alone 
     
     3. a specific values alone (such as one account number alone(among 1000s) that causes delay etc), by embedding the profiler call with if-block.
        eg:  if ( acctId == 89941362 )
               bench_mark_fun;  // The profiling will occur only for this account id.
     
     4. start and stop profiler between specific duration, such as if 100 loops / calls made etc while the program will continue its normal run.
        eg:  if ( ctr++ < 10 )
               bench_mark_fun;  // The profiling will occur only for first 10 calls, program will run normally for all other times.  
               
             if ( i % 10 == 0 )   // once every 10 loops
                 print_perf_report(); // print current profiler statistics

This has a distinct advantage : You need not run profiler over a library or a trivial code that cannot be tuned anyway. There is a disadvantage too: You must add the profiler call in each place that you need to profile.

     Usage : g++ fib.cpp et_prof_code.cpp
     
     The fib.cpp is sample program, that uses this profiler. 
