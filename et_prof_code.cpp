#include "etprofcode.h"
#include <sys/time.h>
#include <inttypes.h>
#include <iomanip>
#define DEBUG
#define FUN_COL_SIZE 36

bool code_instance::_stop_metrics = false;
bool code_instance::_wait_for_print = false;
uint32_t code_instance::_print_interval = 0;

uint64_t code_chunk::_perf_global_time=0, diff(timeval &, timeval &);
int code_chunk::_code_id=0;
code_map_type code_map;
void create_final_report( int64_t, map<int,code_chunk> &ref = ::code_map );
stack <int> call_stack;

class __report_when_dead {
public:
__report_when_dead(){};
~__report_when_dead() {
   if( code_instance::_stop_metrics == false  && code_instance::_wait_for_print == true ) //&& _wait_for_print == true )
	create_final_report(code_chunk:: _perf_global_time );
}
};	
__report_when_dead __report_when_dead_obj; // to auotmatically invoke print function...

void code_chunk::register_time(uint64_t runtime)
{
if ( code_instance::_stop_metrics )
	return;
int64_t excl_runt = runtime - child_runt;
#ifdef DEBUG
cout << "Run time : " << runtime << "Child time : " << child_runt <<"  tot_sub_time : " << tot_sub_time <<"\n";
#endif
if ( excl_runt < 0 )
	cerr << " ERROR : some nasty bug.. child runtime was more than parent's!! \n";
if ( min == -1 || min > excl_runt )
	min = excl_runt;
if ( max < excl_runt )
	max = excl_runt;
child_runt = 0; // this code_chunk finished.. so no children running.. 
#ifdef DEBUG
cout << "register_time() Code chunk time  noted \n tot_time " << tot_time << "run time : "<< runtime << endl;
#endif
tot_time += runtime;
return;
}

int code_chunk::gen_new_id(string s,string s1,string s2,int n)
{
++_code_id;
char temp[200];
sprintf(temp,"%s:%d:%s%s",s.c_str(),n,s1.c_str(),s2.c_str());
code_map.insert(pair<int,string>(_code_id,temp));
return _code_id;
}

void code_chunk::register_child_time ( uint64_t child_time )
{
tot_sub_time += child_time; // sum of all sub routines in all instances of code_chunk..
child_runt += child_time; // sum of all sub routines time in this instance execution..
}

code_instance::code_instance(const int &id)
{
   if ( _stop_metrics ) {
	_participating = false; 
	return;
	}

   this->_id = id;
   _register();
}

int code_instance::gen_new_id(const char* file,const char* code, const char * brac, int line )
{
	return code_chunk::gen_new_id(file,code,brac,line);
}

void code_instance::_register()
{
   #ifdef DEBUG
   cout << " Code instance of ("<< _id << "[" << code_map[_id].name << "] started \n";
   #endif

   _participating = true;

   gettimeofday(&_start,NULL);

   code_map_type::iterator it ;
   
   it = code_map.find ( this->_id );
   it->second.ci_ref_ptr=this;
   it->second.ctr++;

   call_stack.push(this->_id);

   #ifdef DEBUG
   cout << " Code instance of ("<< _id << "[" << code_map[_id].name << "] registered \n";
   #endif
}

void code_instance::_deregister( uint64_t time_taken)
{
    call_stack.pop();
    if ( call_stack.size() > 0 )
    {

    	int parent_code = call_stack.top();
	code_map_type::iterator it ;
	it = code_map.find ( parent_code );
	if (it == code_map.end() )
	{
	cerr << " ERROR : unregistered code in _deregister : " << parent_code << endl;
	}
	else {
		#ifdef DEBUG
		cout << " Parent Time ["<< parent_code << "] SubTime :" << it->second.tot_sub_time <<" subCode : " << _id << " Time[" << time_taken << "]\n";
		#endif
		if ( ! _stop_metrics )
			it->second.register_child_time(time_taken);
	}
    }
    else {
	//main_time=time_taken;
        //cout << " main time " << main_time << "time take " << time_taken << endl;
	if ( ! _wait_for_print )
		create_final_report(time_taken);
	else
		if ( ! _stop_metrics )
			code_chunk::_perf_global_time += time_taken;
    }
}

void code_instance::print_interval(uint32_t n)
{
_print_interval = n;
}

void code_instance::wait_for_print(bool flag) 
{ 
_wait_for_print = flag; 
//if (flag) 
//	gettimeofday(&_start,NULL); // if true, start recording time from NOW..
} 

code_instance::~code_instance()
{
if ( ! _participating )
	return;
#ifdef DEBUG
cout << " Code instance of ("<< _id << " destructed.. \n";
#endif
gettimeofday(&_end,NULL);

uint64_t time_taken = diff(_end,_start);

code_map_type::iterator it ;
it = code_map.find ( this->_id );
if (it == code_map.end() )
	{
	cerr << " ERROR : unregistered code in destructor : " << this->_id << endl;
	}
else {
	if (! code_instance::_stop_metrics )
		it->second.register_time(time_taken);

	_deregister(time_taken);
     }

#ifdef DEBUG
cout << " Code instance of ("<< _id << " ended \n";
#endif
}

uint64_t diff(timeval & end, timeval & start)
{
return (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
}

void print_perf_report( bool final) 
{ 
//timeval end; 
//gettimeofday(&end,NULL); 

if ( call_stack.size() != 0) {
//        uint64_t aggr_time_taken_now = _perf_global_time;
        code_map_type map_copy;
        code_map_type ::iterator it ;
	cout << "\nWARNING : bench marked functions  STILL RUNNING...: " << endl;
	cout << "Running functions will be timed as if ended now* : " << endl;
	timeval now;
        gettimeofday(&now,NULL);
	stack<int> temp_stack = call_stack;
        code_map_type & map_ref = final ? code_map : (map_copy = code_map);
	uint64_t running_func_microsec;
        while ( ! temp_stack.empty() ) { 
        	int & code_id = temp_stack.top();
        	temp_stack.pop(); 
		it = map_ref.find(code_id);
		if ( it == map_ref.end() ) {
			cerr<< "ERROR :Some nasty bug in" << __FILE__ <<":"<< __LINE__<<".. code map copy does not have stack funcion..";
			exit(-1);
		}
        	cout << it->second.name <<'(' <<code_id << ") is running now"<< endl; 
		if ( ! code_instance::_stop_metrics )
		{
		    code_chunk & cc = it->second;
		    running_func_microsec = diff(now,cc.ci_ref_ptr->_start);
		    cc.tot_time += running_func_microsec ;
		    
		    if ( cc.min == -1 || cc.min > running_func_microsec - cc.child_runt )
			cc.min = running_func_microsec - cc.child_runt ;
		    if ( cc.max < running_func_microsec - cc.child_runt )
			cc.max = running_func_microsec - cc.child_runt ;
		}
		it->second.name += "*";
	} // end while..
//	aggr_time_taken_now += running_func_microsec;
	create_final_report(code_chunk::_perf_global_time + running_func_microsec,map_ref); // add base stack elements time ..
}// end if
else
	create_final_report(code_chunk::_perf_global_time);

if ( final )
	{
	code_instance::_stop_metrics=true;
/*
	for (it = code_map.begin() ; it != code_map.end() ; it++ )
	    {
		it->second.tot_time = map_ref[it->first].tot_time;	
		it->second.max = map_ref[it->first].max;	
	    }
*/
	//code_map.clear();
	}
}

void create_final_report(int64_t whole_time, code_map_type & code_map)
{
code_map_type::iterator it ;

char temp[300];
cerr << "\n===============================================\n";
if ( code_instance::_stop_metrics )
	cerr << "\n WARNING : metrics/statistics were PREVIOUSLY calculated.. NOT recently\n";
int len = sprintf(temp,"!%36s |%12s |%8s |%13s |%12s |%5s |%12s |%6s |%8s |%s ","code name", "Exclu.Time","No calls" , "Total Time", 
	"Avg. Time","%Time","Excl. Avg", "min t","max t","Subroutines" );
char line[300];
memset(line+1,'=',len-1);
line[len+1] = '\0';
line[len] = line[0]='|';

cerr<< line << endl << temp <<'|'<< endl << line <<endl;
//cerr <<"-----------------------------------------------------------------------------------------------------------------------------------------------\n";
for (it = code_map.begin() ; it != code_map.end() ; it++ )
	{
	code_chunk & c = it->second;
        uint64_t tot,sub;
        tot = c.tot_time;
        sub = c.tot_sub_time;
        if ( sub > tot )
		{
		cout << "WARNING : "<< c.name << " STILL RUNNING[or bug :-( in code.o] .. not considered in computation " << endl;
		continue;
                }
        float pct = float(tot-sub)/(float)whole_time * 100 ;
        double f = 1000.0;
	char cname[FUN_COL_SIZE]; 
	strncpy(cname,c.name.c_str(),FUN_COL_SIZE-1);
	cname[FUN_COL_SIZE-1]='\0';
	sprintf(temp,"!%36s %13.3lf %9llu %15.3lf %13.6lf %6.2f %13.6lf %7llu %9llu %11.5f |" , cname, 
	           (double(tot-sub))/f, c.ctr, tot/f, (double)tot/c.ctr/f, pct, (double(tot-sub))/c.ctr/f,
			c.ctr > 1 ? c.min : c.min, c.max, sub/f );
	cerr << temp<<  endl ; 
	}	
cerr << line << endl;
}
