#ifndef	__STD_HEAD_H__
#define __STD_HEAD_H__
#include <iostream>
#include <tchar.h>
#include <stdio.h>
#include <string.h>
#include <cstdlib>
#include <clocale>
#include <ctime>
#include <vector>
#include <algorithm>
#include <winsock2.h>
#include <mswsock.h>
#include <direct.h>
#include <iphlpapi.h>
#include <Windows.h>
#include <stdarg.h>
#include <MSWSock.h>
#include <process.h>
#include <WS2tcpip.h>	// web service
#include <WinNls32.h> //WideCharToMultiByte 字节转换头文件
// TODO: 在此处引用程序要求的附加头文件
using namespace std;
#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"mswsock.lib")
#pragma comment(lib,"Iphlpapi.lib")

#pragma comment(lib, "Ws2_32.lib")		// Socket编程需用的动态链接库
#pragma comment(lib, "Kernel32.lib")	// IOCP需要用到的动态链接库

#endif