// vi: set ts=4 sw=4 :
// vim: set tw=75 :

// osdep.h - operating system dependencies

/*
 * Copyright (c) 2001 Will Day <willday@hpgx.net>
 *
 *    This file is part of Metamod.
 *
 *    Metamod is free software; you can redistribute it and/or modify it
 *    under the terms of the GNU General Public License as published by the
 *    Free Software Foundation; either version 2 of the License, or (at
 *    your option) any later version.
 *
 *    Metamod is distributed in the hope that it will be useful, but
 *    WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with Metamod; if not, write to the Free Software Foundation,
 *    Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *    In addition, as a special exception, the author gives permission to
 *    link the code of this program with the Half-Life Game Engine ("HL
 *    Engine") and Modified Game Libraries ("MODs") developed by Valve,
 *    L.L.C ("Valve").  You must obey the GNU General Public License in all
 *    respects for all of the code used other than the HL Engine and MODs
 *    from Valve.  If you modify this file, you may extend this exception
 *    to your version of the file, but you are not obligated to do so.  If
 *    you do not wish to do so, delete this exception statement from your
 *    version.
 *
 */

#ifndef OSDEP_H
#define OSDEP_H

// Various differences between WIN32 and Linux.

#include "support_meta.h"	// mBOOL, etc
#include "mreg.h"			// REG_CMD_FN, etc
#include "log_meta.h"		// LOG_ERROR, etc


// String describing platform/DLL-type, for matching lines in metamod.ini.
#ifdef linux
	#define PLATFORM	"linux"
#elif defined(_WIN32)
	#define PLATFORM	"win32"
#else /* unknown */
	#error "OS unrecognized"
#endif /* linux */


// Macro for function-exporting from DLL..
// from SDK dlls/cbase.h:
//! C functions for external declarations that call the appropriate C++ methods

// Windows uses "__declspec(dllexport)" to mark functions in the DLL that
// should be visible/callable externally.
//
// It also apparently requires WINAPI for GiveFnptrsToDll().
//
// See doc/notes_windows_coding for more information..

// Attributes to specify an "exported" function, visible from outside the
// DLL.
#undef DLLEXPORT
#ifdef _WIN32
	#define DLLEXPORT	__declspec(dllexport)
	// WINAPI should be provided in the windows compiler headers.
	// It's usually defined to something like "__stdcall".
#elif defined(linux)
	#define DLLEXPORT	/* */
	#define WINAPI		/* */
#endif /* _WIN32 */

// Simplified macro for declaring/defining exported DLL functions.  They
// need to be 'extern "C"' so that the C++ compiler enforces parameter
// type-matching, rather than considering routines with mis-matched
// arguments/types to be overloaded functions...
//
// AFAIK, this is os-independent, but it's included here is osdep.h where
// DLLEXPORT is defined, for convenience.
#define C_DLLEXPORT		extern "C" DLLEXPORT


#ifdef _MSC_VER
	// Disable MSVC warning:
	//    4390 : empty controlled statement found; is this what was intended?
	// generated by the RETURN macros.
	#pragma warning(disable: 4390)
#endif /* _MSC_VER */


// Functions & types for DLL open/close/etc operations.
#ifdef linux
	#include <dlfcn.h>
	typedef void* DLHANDLE;
	typedef void* DLFUNC;
	inline DLHANDLE DLOPEN(const char *filename) {
		return(dlopen(filename, RTLD_NOW));
	}
	inline DLFUNC DLSYM(DLHANDLE handle, const char *string) {
		return(dlsym(handle, string));
	}
	inline int DLCLOSE(DLHANDLE handle) {
		return(dlclose(handle));
	}
	inline char* DLERROR(void) {
		return(dlerror());
	}
#elif defined(_WIN32)
	typedef HINSTANCE DLHANDLE;
	typedef FARPROC DLFUNC;
	inline DLHANDLE DLOPEN(const char *filename) {
		return(LoadLibrary(filename));
	}
	inline DLFUNC DLSYM(DLHANDLE handle, const char *string) {
		return(GetProcAddress(handle, string));
	}
	inline int DLCLOSE(DLHANDLE handle) {
		// NOTE: Windows FreeLibrary returns success=nonzero, fail=zero,
		// which is the opposite of the unix convention, thus the '!'.
		return(!FreeLibrary(handle));
	}
	// Windows doesn't provide a function corresponding to dlerror(), so
	// we make our own.
	char *str_GetLastError(void);
	inline char* DLERROR(void) {
		return(str_GetLastError());
	}
#endif /* linux */
const char *DLFNAME(void *memptr);
mBOOL IS_VALID_PTR(void *memptr);


// Attempt to call the given function pointer, without segfaulting.
mBOOL os_safe_call(REG_CMD_FN pfn);


// Windows doesn't have an strtok_r() routine, so we write our own.
#ifdef _WIN32
	#define strtok_r(s, delim, ptrptr)	my_strtok_r(s, delim, ptrptr)
	char *my_strtok_r(char *s, const char *delim, char **ptrptr);
#endif /* _WIN32 */


// Set filename and pathname maximum lengths.  Note some windows compilers
// provide a <limits.h> which is incomplete and/or causes problems; see
// doc/windows_notes.txt for more information.
#ifdef linux
	#include <limits.h>
#elif defined(_WIN32)
	#define NAME_MAX	255
	#define PATH_MAX	255
#endif /* linux */


// Various other windows routine differences.
#ifdef linux
	#include <unistd.h>	// sleep
#elif defined(_WIN32)
	#define snprintf	_snprintf
	#define vsnprintf	_vsnprintf
	#define sleep(x)	Sleep(x*1000)
	#define strcasecmp	stricmp
	#define strncasecmp	_strnicmp
#endif /* _WIN32 */

#ifndef S_ISREG
	// Linux gcc defines this; earlier mingw didn't, later mingw does;
	// MSVC doesn't seem to.
	#define S_ISREG(m)	((m) & S_IFREG)
#endif /* not S_ISREG */


#endif /* OSDEP_H */
