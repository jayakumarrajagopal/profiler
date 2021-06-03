// purpose is to demonstrate profiler..
// usage : ./fib <n> <m>
#include <iostream>
#include <math.h>
#include "etprof.h"
long fibonocci(long);
using namespace std;
int main(int argc, char *argv[])
	{
	bench_mark_fun ; //*** This activates profiler for main function ****//
	int n = 20; // default
	if ( argc < 2 ) {
		cerr << " number parameter missing\n taking default";
	}
	else 
	{
	    n = atoi(argv[1]);
	}

	if ( n < 0 ) {
		cerr << " enter small +ve integer\n";
		return -2;
	}

	for ( int i=1; i <= n ; i++ )
	{
		bench_mark_block_id(for_loop); //*** This activates profiler for for() loop ****//
		cout <<  " fibonocci series for num("  << i << ") = " << fibonocci(i) << "##\n";
	}

	return 0;
}

long fibonocci(long  n)
{
	bench_mark_fun;    //*** This activates profiler for this function ****//
	cout<< "fib( "<<n<<" ) - ";
	if ( n == 0 || n == 1 )
		return 1;
	else 
		return fibonocci(n-1) + fibonocci(n-2) ;
}
