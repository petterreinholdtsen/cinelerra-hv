#ifndef BCSIGNALS_H
#define BCSIGNALS_H

#include "arraylist.h"
#include "bcsignals.inc"
#include <pthread.h>
#include <signal.h>

#define TRON(x) BC_Signals::new_function(x);
#define TROFF(x) BC_Signals::delete_function(x);

// BC_Signals must be initialized at the start of every program using
// debugging.
#define ENABLE_TRACE
//#define TRACE_LOCKS
#ifdef TRACE_LOCKS
#undef TRACE_LOCKS
#endif
//#define TRACE_MEMORY


// Need to use structs to avoid the memory manager.
// One of these tables is created every time someone locks a lock.
// After successfully locking, the table is flagged as being the owner of the lock.
// In the unlock function, the table flagged as the owner of the lock is deleted.
typedef struct 
{
	void *ptr;
	char *title;
	char *location;
	int is_owner;
	int id;
} bc_locktrace_t;

class BC_Signals
{
public:
	BC_Signals();
	void initialize();
	void initialize2();


	virtual void signal_handler(int signum);

#ifdef ENABLE_TRACE
// Add a trace
#define TRACE(text) BC_Signals::new_trace(text);

// Delete all traces
#define UNTRACE BC_Signals::delete_traces();

#else

#define TRACE(text) ;
#define UNTRACE ;

#endif


#ifdef TRACE_LOCKS

// Before user acquires
#define SET_LOCK(ptr, title, location) int table_id = BC_Signals::set_lock(ptr, title, location);
// After successful acquisition
#define SET_LOCK2 BC_Signals::set_lock2(table_id);
// Release current lock table after failing to acquire
#define UNSET_LOCK2 BC_Signals::unset_lock2(table_id);

// Release current owner of lock
#define UNSET_LOCK(ptr) BC_Signals::unset_lock(ptr);

// Delete a lock
#define UNSET_ALL_LOCKS(ptr) BC_Signals::unset_all_locks(ptr);

#else

#define SET_LOCK(ptr, title, location) ;
#define SET_LOCK2 ;
#define UNSET_LOCK(ptr) ;
#define UNSET_LOCK2 ;
#define UNSET_ALL_LOCKS(ptr) ;

#endif


#ifdef TRACE_MEMORY

#define ENABLE_BUFFER BC_Signals::enable_memory();
#define DISABLE_BUFFER BC_Signals::disable_memory();
#define BUFFER(size, ptr, location) BC_Signals::set_buffer(size, ptr, location);
#define UNBUFFER(ptr) BC_Signals::unset_buffer(ptr);

#else

#define ENABLE_BUFFER ;
#define DISABLE_BUFFER ;
#define BUFFER(size, ptr, location);
#define UNBUFFER(ptr);

#endif

// Handling of temporary files in crash
#define SET_TEMP BC_Signals::set_temp
#define UNSET_TEMP BC_Signals::unset_temp

// Temporary files
	static void delete_temps();
	static void set_temp(char *string);
	static void unset_temp(char *string);




	static int set_lock(void *ptr, char *title, char *location);
	static void set_lock2(int table_id);
	static void unset_lock2(int table_id);
	static void unset_lock(void *ptr);
// Used in lock destructors so takes away all references
	static void unset_all_locks(void *ptr);

	static void new_trace(char *text);
	static void delete_traces();

	static void enable_memory();
	static void disable_memory();
	static void set_buffer(int size, void *ptr, char* location);
// This one returns 1 if the buffer wasn't found.
	static int unset_buffer(void *ptr);

	static void dump_traces();
	static void dump_locks();
	static void dump_buffers();

// Convert signum to text
	static char* sig_to_str(int number);

	static BC_Signals *global_signals;
};


#endif
