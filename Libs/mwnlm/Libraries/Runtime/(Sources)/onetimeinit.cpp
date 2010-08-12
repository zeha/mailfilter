#define NDEBUG

#include <stdlib.h>
#include <string.h>
#include <CPlusLib.h>

/*
3.3.2 One-time Construction API 

As described in Section 2.8, function-scope static objects have
associated guard variables used to support the requirement that they
be initialized exactly once, the first time the scope declaring them
is entered. An implementation that does not anticipate supporting
multi-threading may simply check the low-order byte of that guard
variable, initializing if and only if its value is zero, and then
setting it to a non-zero value.

However, an implementation intending to support automatically
thread-safe, one-time initialization (as opposed to requiring explicit
user control for thread safety) requires more control, and should use
the following API to achieve interoperability with other such
implementations.

    if ( obj_guard.first_byte == false ) {
      if ( __cxa_guard_acquire (obj_guard) ) {
        ... initialize the object ...;
        ... queue object destructor with __cxa_atexit() ...;
        __cxa_guard_release (obj_guard);
      }
    }

<Functions described below>

An implementation need not include the simple inline test of the
initialization flag in the guard variable around the above
sequence. If it does so, the cost of this scheme, when run
single-threaded with minimal versions of the above functions, will be
two extra function calls, each of them accessing the guard variable,
the first time the scope is entered.

An implementation supporting thread-safety on multiprocessor systems
must also guarantee that references to the initialized object do not
occur before the load of the initialization flag. On Itanium, this can
be done by using a ld1.acq operation to load the flag.

The intent of specifying an 8-byte structure for the guard variable,
but only describing one byte of its contents, is to allow flexibility
in the implementation of the API above. On systems with good small
lock support, the second word might be used for a mutex lock. On
others, it might identify (as a pointer or index) a more complex lock
structure to use.
*/

typedef enum 
{
	NOT_INITIALIZED,
	INITIALIZED,
	BEING_INITIALIZED
}	RTState;

typedef struct RTMutex
{
	char					flag;		// 0 or 1
	char					pad[3];
	volatile unsigned long	state;
}	RTMutex;

/*
extern "C" int __cxa_guard_acquire ( __int64_t *guard_object );

The guard_object argument is the guard variable (see Section 2.8)
associated with the object to be initialized. If it indicates that the
object has been initialized already, return false (0)
immediately. Otherwise, acquire a lock associated with the guard
variable (possibly part of it, but unspecified by this ABI). If the
object has now been initialized, release the lock and return false
(0). If not, return true (1) without releasing the lock.
*/

inline unsigned long atomic_cmpxchg(volatile unsigned long *addr, unsigned long cmp, unsigned long src)
{
	register unsigned long ret;
	asm
	{
		optimize
		mov ecx, addr
		mov eax, cmp
		mov edx, src
		lock cmpxchg dword ptr [ecx], edx
		mov ret, eax
	}
	return ret;
}

int __cdecl __cxa_guard_acquire ( long long *guard_object )
{
	RTMutex *m = (RTMutex *)guard_object;
	
	// the frontend already checked, but be sure
	if (m->flag || m->state == INITIALIZED) return 0;
	
	// acquire a spinlock
	if (NOT_INITIALIZED == atomic_cmpxchg(&m->state, NOT_INITIALIZED, BEING_INITIALIZED))
	{
		// ready to initialize
		return 1;
	}
	else
	{
		// else, wait for other thread to complete
		while (m->state == BEING_INITIALIZED) /**/;

		// we know someone else initialized it or aborted
		return 0;
	}
}

/*
extern "C" void __cxa_guard_release ( __int64_t *guard_object );

The guard_object argument is the guard variable (see Section 2.8)
associated with the object being initialized. Set it to indicate that
initialization is complete, release the associated lock (in that
order), and return.
*/

void __cdecl __cxa_guard_release ( long long *guard_object )
{
	RTMutex *m = (RTMutex *)guard_object;

	// initialization complete
	m->flag = 1;
	m->state = INITIALIZED;
}

/*
extern "C" void __cxa_guard_abort ( __int64_t *guard_object );

The guard_object argument is the guard variable (see Section 2.8)
associated with an object being initialized. Set it to indicate that
initialization is not complete, release the associated lock (in that
order), and return. This routine is intended for use when the
initialization raises an exception, which requires that initialization
be attempted again by the next pass through the object's scope.
*/

void __cdecl __cxa_guard_abort ( long long *guard_object )
{
	RTMutex *m = (RTMutex *)guard_object;

	// initialization not complete
	m->flag = 0;
	m->state = NOT_INITIALIZED;
}

/* Change History
 * ejs 020819	Added one-time construction API
 */
 

