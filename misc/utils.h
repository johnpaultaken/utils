//----------------------------------------------------------------------------
// year   : 2018
// author : John Paul
// email  : johnpaultaken@gmail.com
// source : https://github.com/johnpaultaken
// description :
//      shared utilities of smaller size.
//----------------------------------------------------------------------------

#pragma once

//#define TRACE

#include <iostream>
#include <typeinfo>

#ifdef __GNUC__    // if gcc or clang
#include <cxxabi.h>
#endif

namespace utils
{

//
// Allows compile time enabled tracing of code path and data.
// No run time cost when turned off.
//
class tstream
{
public:
    template<typename T>
    tstream & operator << (T && obj)
    {
#ifdef TRACE
        std::cout << std::forward<T>(obj);
#else
        (void)(obj);
#endif
        return *this;
    }

    //
    // This overload is needed for 'ctrace << std::flush' to work.
    // For some reason, the template function cannot deduce the type of
    // std::flush.
    //
    using stream_type = decltype(std::cout);
    tstream & operator << (stream_type& (* pf)(stream_type&))
    {
#ifdef TRACE
        std::cout << pf;
#else
        (void)(pf);
#endif
        return *this;
    }
};

//
// Use ctrace instead of cout for tracing.
//
tstream ctrace;

//
// for malloc-ed char array that needs to be freed automatically when it goes
// out of scope.
//
class scoped_pchar
{
public:
    scoped_pchar(char * p) : _str{p} {}
    ~scoped_pchar() {if (_str) free(_str);}

    operator char *() { return _str; };
private:
    char * _str;
};

//
// Get the demangled typename.
//
#ifdef __GNUC__    // if gcc or clang
#define TYPENAME(T) \
    scoped_pchar{abi::__cxa_demangle(typeid(T).name(), 0, 0, 0)}
#else    // Visual studio already returns demangled typename
#define TYPENAME(T) typeid(T).name()
#endif

}
