#ifndef __ETPROFH__
#define __ETPROFH__

#include <sys/time.h>
#include <string>

#define bench_mark_fun et_profile ( __FILE__,__FUNCTION__, "()", __LINE__ )
#define bench_mark_block_id(x) et_profile ( __FILE__,#x, "{ }" , __LINE__ )

#define et_profile(a,b,c,d) static const int cat(__et_profiler_id , __LINE__ )\
		 = code_instance::gen_new_id(a,b,c,d); \
		code_instance cat(ci__ , __LINE__) ( cat(__et_profiler_id , __LINE__ ))
#define cat(a,b) cat2(a,b)
#define cat2(a,b) a ## b

class code_instance
{
static uint32_t _print_interval;
static void print_interval(uint32_t );
public:
uint64_t child_runt;

static bool _stop_metrics,_wait_for_print;
bool _participating;
int _id;
timeval _start, _end;
code_instance(const int & );
~code_instance();
void _register();
void _deregister(uint64_t);
void register_child_time(uint64_t);
static void wait_for_print(bool flag=true);
static int gen_new_id(const char *,const char*,const char*,int);
};


void print_perf_report( bool final=false);

#endif 


