//----------------------------------------------------------------------------
// year   : 2018
// author : John Paul
// email  : johnpaultaken@gmail.com
// source : https://github.com/johnpaultaken
// description :
//      Some convenient functions and macros for unit test in C++11.
//----------------------------------------------------------------------------

#pragma once

#include <cstring>
#include <string>
#include <iostream>
#include <sstream>

namespace utils
{
    using std::cout;

    void assert_m(
        bool cond,
        const std::string & what,
        const std::string & func,
        int line
    )
    {
        auto filepath = __FILE__;
        auto filename = std::max<const char *>(
            filepath,
            std::max(strrchr(filepath, '\\'), strrchr(filepath, '/')) + 1
            );

        std::ostringstream msg;
        msg << (cond ? "\n OK : " : "\n FAIL : ")
            << what
            << " - " << func << "()"
            << " at " << filename << ":" << line << " ";

        cout << msg.str();
    }

#define ASSERT_M(cond, what) assert_m(cond, what, __func__, __LINE__ );
#define FAIL_M(what) assert_m(false, what, __func__, __LINE__ );
#define PASS_M(what) assert_m(true, what, __func__, __LINE__ );

#define ASSERT(cond) assert_m(cond, "", __func__, __LINE__ );
#define FAIL() assert_m(false, "", __func__, __LINE__ );
#define PASS() assert_m(true, "", __func__, __LINE__ );
}
