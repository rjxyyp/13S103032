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
#include <WinNls32.h> //WideCharToMultiByte �ֽ�ת��ͷ�ļ�
// TODO: �ڴ˴����ó���Ҫ��ĸ���ͷ�ļ�
using namespace std;
#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"mswsock.lib")
#pragma comment(lib,"Iphlpapi.lib")

#pragma comment(lib, "Ws2_32.lib")		// Socket������õĶ�̬���ӿ�
#pragma comment(lib, "Kernel32.lib")	// IOCP��Ҫ�õ��Ķ�̬���ӿ�

#endif