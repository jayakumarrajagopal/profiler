#ifndef __ETPROFCODE__
#define __ETPROFCODE__

#include <sys/time.h>
#include <string>
#include <iostream>
#include <map>
#include <stack>

#include "etprof.h"

uint64_t diff ( timeval &, timeval &);
using namespace std;
class code_chunk
{
public:
string name;
uint64_t tot_time, tot_sub_time, ctr, min, max, child_runt;
static uint64_t _perf_global_time;
code_instance *ci_ref_ptr;
code_chunk() {};
code_chunk(string token)
    {
     tot_time =tot_sub_time= ctr= max = child_runt = (uint64_t)0;
     min=-1;
     name=token;
    }
void register_time(uint64_t );
void register_child_time(uint64_t );
static int _code_id;
static int gen_new_id(string,string a="",string b="",int c=0);
};

typedef map<int,code_chunk> code_map_type;

#endif 
