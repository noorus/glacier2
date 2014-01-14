#pragma once

// Glacier² Game Engine © 2014 noorus
// All rights reserved.

// _PROFILE
// Identify the build profile.
#ifdef _DEBUG
# define _PROFILE L"Debug"
#else
# define _PROFILE L"Release"
#endif

// _COMPILER
// Identify the compiler.
#ifdef _MSC_VER
# if _MSC_VER >= 1700
#  define _COMPILER L"Visual C++ 2012"
# elif _MSC_VER >= 1600
#  define _COMPILER L"Visual C++ 2010"
# elif _MSC_VER >= 1500
#  define _COMPILER L"Visual C++ 2008"
# elif _MSC_VER >= 1400
#  define _COMPILER L"Visual C++ 2005"
# elif _MSC_VER >= 1310
#  define _COMPILER L"Visual C++ 2003"
# elif _MSC_VER >= 1300
#  define _COMPILER L"Visual C++ 2002"
# elif _MSC_VER >= 1200
#  define _COMPILER L"Visual C++ 6"
# elif _MSC_VER >= 1100
#  define _COMPILER L"Visual C++ 5"
# elif _MSC_VER >= 1000
#  define _COMPILER L"Visual C++ 4"
# elif _MSC_VER >= 900
#  define _COMPILER L"Visual C++ 2"
# elif _MSC_VER >= 800
#  define _COMPILER L"Visual C++ 1.1"
# else
#  define _COMPILER L"Visual C++"
# endif
#elif __GNUC__
# if __GNUC__ == 3
#  define _COMPILER L"GCC (G++) 3.x.x"
# elif __GNUC__ == 2
#  define _COMPILER L"GCC (G++) 2.x"
# elif __GNUC__ == 1
#  define _COMPILER L"GCC (G++) 1.x"
# else
#  define _COMPILER L"GCC (G++)"
# endif
#elif __BORLANDC__
# if __BORLANDC__ == 0x0520
#  define _COMPILER L"C++ Builder 1"
# elif __BORLANDC__ == 0x0530
#  define _COMPILER L"C++ Builder 3"
# elif __BORLANDC__ == 0x0540
#  define _COMPILER L"C++ Builder 4"
# elif __BORLANDC__ == 0x0550
#  define _COMPILER L"C++ Builder 5"
# elif __BORLANDC__ == 0x0560
#  define _COMPILER L"C++ Builder 6"
# else
#  define _COMPILER L"C++ Builder"
# endif
#else
# define _COMPILER L"Unknown"
#endif