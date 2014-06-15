#include "stdhead.h"
#include "DataStructAndConstant.h"
#include "ToolFunction.h"
#include "ProcessCommand.h"

DWORD ProcessListCommand(LPPER_HANDLE_DATA pPerHandleData, LPPER_IO_OPERATION_DATA pPerIOData)
{
	DWORD dwNumberOfDataSended = 0;
	if (pPerHandleData->socket != NULL)
	{
		TCHAR *strSendDataBuffer = pPerIOData->buffer;
		//FILE_INFO Files[MAX_FILE];
		dwNumberOfDataSended = StorFileList(strSendDataBuffer, SEND_BUFFER_SIZE, pPerHandleData->strCurrentWorkPath);
		pPerIOData->BufferLen = dwNumberOfDataSended;
	}
	else
	{
		wprintf(pPerIOData->buffer, "425 Can't open data connection.\r\n");
	}	
	return dwNumberOfDataSended;
}

int ProcessCdCommand(LPPER_HANDLE_DATA PerHandleData, LPPER_IO_OPERATION_DATA PerIoData)
{
	// 在结构体中显示此函数是否返回成功
	char *beginFile = PerIoData->cmdBuffer + 2;
	while (*beginFile == '\t' || *beginFile == ' ') *beginFile++;
	char *tmpsubdir = PerIoData->cmdBuffer + (beginFile-PerIoData->cmdBuffer);
	if (strcmp(tmpsubdir, ".") == 0)
	{
		return 1;
	}
	if (strcmp(tmpsubdir, "..") == 0)
	{
		//wcout << PerHandleData->strCurrentWorkPath;
		TCHAR *curPath = PerHandleData->strCurrentWorkPath;
		int len = _tcslen(curPath);
		TCHAR* pEnd = curPath + len - 1;
		if (*pEnd == '\\') { *pEnd--; }
		while (*pEnd != '\\')
		{
			*pEnd--;
		}
		curPath[pEnd - curPath] = '\0';
		_tcscpy_s(PerIoData->buffer, PerHandleData->strCurrentWorkPath);
		PerIoData->BufferLen = _tcslen(PerIoData->buffer);
		return 1;
	}
	int issubdir = issubFileOrDir(PerHandleData->strCurrentWorkPath, ANSIToUnicode(tmpsubdir));

	// if issubdir == 1, this subdir is a dir.
	if (issubdir == 1)
	{
		TCHAR tmpCurDir[240];
		memset((void *)tmpCurDir, 0, sizeof(tmpCurDir));
		_tcscpy_s(tmpCurDir, PerHandleData->strCurrentWorkPath);
		if (tmpCurDir[_tcslen(tmpCurDir) - 1] != '\\')	_tcscat_s(tmpCurDir, L"\\");
		_tcscat_s(tmpCurDir, ANSIToUnicode(tmpsubdir));
		//wcout << tmpCurDir << endl;
		_tcscpy_s(PerHandleData->strCurrentWorkPath, tmpCurDir);
		_tcscpy_s(PerIoData->buffer, PerHandleData->strCurrentWorkPath);
		PerIoData->BufferLen = _tcslen(PerIoData->buffer);
		return 1;
	}
	else if (issubdir == 2)
	{
		char tmp[1024];
		sprintf_s(tmp, "%s is a file can't use cd command.\n", tmpsubdir);
		memset(PerIoData->buffer, 0, sizeof(PerIoData->buffer));
		_tcscpy_s(PerIoData->buffer, ANSIToUnicode(tmp));
		return 2;
	}
	return -1;
}

int ProcessGetCommand(LPPER_HANDLE_DATA PerHandleData, LPPER_IO_OPERATION_DATA PerIoData)
{
	// get command: get [filename]
	char *beginFile = PerIoData->cmdBuffer + 3;
	while (*beginFile == '\t' || *beginFile == ' ') *beginFile++;
	char *filename = PerIoData->cmdBuffer + (beginFile-PerIoData->cmdBuffer);
	int isfile = issubFileOrDir(PerHandleData->strCurrentWorkPath, ANSIToUnicode(filename));
	if (isfile == 1)
	{
		return 1;
	}
	else if (isfile == 2)
	{
		char tmpFullFilename[100] = { 0 };
		sprintf_s(tmpFullFilename, "%s\\%s", UnicodeToANSI(PerHandleData->strCurrentWorkPath), filename);
		int sendSuccess = FileSend(PerHandleData->socket, tmpFullFilename);
		if (sendSuccess != TRAN_SUCCESS)
		{
			cout << "Send file " << filename << " failed.\n";
			return -1;
		}
		return 2;
	}
	else
	{
		cout << "file " << filename << " is not existed.\n";
		return -1;
	}
	return -1;
}

/*------------------------------------------------------------------------
* doTCP - handle a TCP service connection request
*------------------------------------------------------------------------
*/
void doTCPFTP(SOCKET s)
{
	PER_HANDLE_DATA * PerHandleData = (LPPER_HANDLE_DATA)GlobalAlloc(GPTR, sizeof(PER_HANDLE_DATA));	// 在堆中为这个PerHandleData申请指定大小的内存
	ZeroMemory(PerHandleData, sizeof(PER_HANDLE_DATA));
	LPPER_IO_OPERATION_DATA PerIoData = (LPPER_IO_OPERATION_DATA)GlobalAlloc(GPTR, sizeof(PER_IO_OPERATEION_DATA));
	ZeroMemory(PerIoData, sizeof(PER_IO_OPERATEION_DATA));

	PerHandleData->socket = s;
	_tcscpy_s(PerHandleData->strCurrentWorkPath, _TEXT(DEFAULT_WORKPATH));

	int zero = 0;
	if (setsockopt(PerHandleData->socket, SOL_SOCKET, SO_SNDBUF, (char *)&zero, sizeof(zero)) == SOCKET_ERROR)
	{
		printf("Set socket option Error %d", WSAGetLastError());
	}
	zero = 0;
	if (setsockopt(PerHandleData->socket, SOL_SOCKET, SO_RCVBUF, (char *)&zero, sizeof(zero)) == SOCKET_ERROR)
	{
		printf("Set socket option Error %d", WSAGetLastError());
	}

	char recvBuf[RECV_BUFFER_SIZE];
	memset((void *)recvBuf, 0, sizeof(recvBuf));
	if (recv(s, recvBuf, RECV_BUFFER_SIZE, 0) == -1)
	{
		cout << "At first, recv error." << endl;
		exit(-1);
	}
	cout << "A Client enter in, says: " << recvBuf << endl;

	// Begin to deal with ftp command
	while (true)
	{
		memset((void *)recvBuf, 0, sizeof(recvBuf));
		int recvConn = -1;
		if ((recvConn = recv(s, recvBuf, RECV_BUFFER_SIZE, 0)) < 0)
			continue;
		if (strcmp(recvBuf, "quit") == 0)
			break;
		strncpy_s(PerIoData->cmdBuffer, recvBuf, strlen(recvBuf) + 1);
		cout << "A Client says: " << recvBuf << endl;
		if (strncmp(recvBuf, "get ", 4) == 0)
		{
			// At first, judge the file is exist or not. or this file is a dir ?
			char *p = recvBuf + 4;
			while (*p == '\t' || *p == ' ') *p++;
			TCHAR *subFilename = ANSIToUnicode(PerIoData->cmdBuffer) + (p - recvBuf);
			int dirOrFileExistedFlag = issubFileOrDir(PerHandleData->strCurrentWorkPath, subFilename);
			// this sub string is a dir. 1: a dir, -1: not exist, 2: a file.
			if (dirOrFileExistedFlag != 2)
			{
				if (send(PerHandleData->socket, (const char *)&dirOrFileExistedFlag, sizeof(int), 0) < 0)
				{
					cout << "File attribute send error.\n";
					closesocket(PerHandleData->socket);
					exit(-1);
				}
				continue;
			}
			// if this is a dir. send a flag.
			int flag = ProcessGetCommand(PerHandleData, PerIoData);
			if (flag == 1)
			{
				cout << "This file is a dir, Please choose a file." << endl;
			}
			else if (flag == -1)
			{
				cout << "File send error.\n";
				closesocket(s);
				exit(-1);
			}
			continue;
		}
		else if (strcmp(recvBuf, "ls") == 0)
		{
			memset(PerIoData->buffer, 0, sizeof(PerIoData->buffer));
			DWORD dwNumberOfDataSended = ProcessListCommand(PerHandleData, PerIoData);
		}
		else if (strncmp(recvBuf, "cd ", 3) == 0)
		{
			if (ProcessCdCommand(PerHandleData, PerIoData) == -1)
			{
				char tmp[88];
				sprintf_s(tmp, "Dir[File] %s is not existed.\n", recvBuf + 3);
				memset(PerIoData->buffer, 0, sizeof(PerIoData->buffer));
				_tcsncpy_s(PerIoData->buffer, ANSIToUnicode(tmp), strlen(tmp) + 1);
			}
		}
		else if (strncmp(recvBuf, "pwd", 3) == 0)
		{
			TCHAR talk[200];
			_tcscpy_s(talk, PerHandleData->strCurrentWorkPath);
			_tcscat_s(talk, L"\n");
			_tcscpy_s(PerIoData->buffer, talk);
		}
		else
		{
			char talk[200];
			sprintf_s(talk, "The client says: %s", recvBuf);
			_tcscpy_s(PerIoData->buffer, ANSIToUnicode(recvBuf));
		}

	//	cout << sizeof(PerIoData) << sizeof(PerIoData->buffer) << endl;
	//	wcout << _tcslen(PerIoData->buffer) << endl;
		if (send(PerHandleData->socket,
			UnicodeToANSI(PerIoData->buffer),
			2000,
			0) < 0)
		{
			perror("[server] send error.\n");
			closesocket(s);
			break;
		}
		// 判断此时客户端是否连接
	}
	closesocket(s);
}



/*------------------------------------------------------------------------
* UDPweb - do UDP web on the given socket
*------------------------------------------------------------------------
*/
void UDPweb(SOCKET webconn){
	char client_ip[32];
	struct sockaddr_in clientaddr;
	int	len = sizeof(clientaddr);
	inet_ntop(AF_INET, (struct in_addr *) &clientaddr.sin_addr, client_ip, sizeof(client_ip));
	printf("\nClient %s connected.\n", client_ip);

	char buffer[WEBMAXLINE], msg[1024];
	while (1) {
		memset(msg, 0, sizeof(msg));
		if (recvfrom(webconn, msg, sizeof(msg), 0, (struct sockaddr *) &clientaddr, &len) > 0) {
			printf("From %s port %d: %s\n", client_ip, ntohs(clientaddr.sin_port), msg);
		}
		else {
			printf("Client %s disconnected. \n", client_ip);
			break;
		}
		sprintf_s(buffer, "HTTP/1.0 200 OK\r\n");
		sprintf_s(buffer, "Content-Type:text/html;charset=UTF-8\r\nContent-Length:{0}\r\n");
		sprintf_s(buffer, "<html><head><title>A test for web service</title></head><body><h1> Hello Welcome to my world !</h1></body></html>");
		sendto(webconn, buffer, strlen(buffer), 0, (const struct sockaddr *) &clientaddr, len);
		closesocket(webconn);
	}
	closesocket(webconn);
}