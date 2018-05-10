#ifndef RESPONSETEST_H
#define RESPONSETEST_H
#include "Stimulation.h"
#include <windows.h>
#include <atltime.h>
#include <atlstr.h>  
#include <tchar.h>
#include <fstream>      // std::ofstream
#include <stdio.h>
#include <random>  
#include <istream>
#include <iostream>  
#include <fstream>
#include <iomanip>
#include <errno.h>
#include <string>
#include <time.h>       /* time_t, struct tm, difftime, time, mktime */

#define SYSERROR()  errno
//https://msdn.microsoft.com/en-us/library/windows/desktop/ms724390(v=vs.85).aspx
//https://msdn.microsoft.com/en-us/library/windows/desktop/ms724950(v=vs.85).aspx
// https://msdn.microsoft.com/en-us/library/windows/desktop/ms725496(v=vs.85).aspx

int response_verification(void);
#endif
