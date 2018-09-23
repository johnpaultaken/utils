//----------------------------------------------------------------------------
// year   : 2018
// author : John Paul
// email  : johnpaultaken@gmail.com
// source : https://github.com/johnpaultaken
// description :
//      shared utilities of smaller size.
//----------------------------------------------------------------------------

#pragma once

#include <iostream>

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

}
