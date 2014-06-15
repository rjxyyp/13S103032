// IOCP_TCPIP_Socket_Client.cpp

#include <iostream>
#include <cstdio>
#include <string>
#include <cstring>
#include <winsock2.h>
#include <Windows.h>

using namespace std;

#pragma comment(lib, "Ws2_32.lib")		// Socket编程需用的动态链接库

// 用于发送、接受文件时的属性
typedef struct _FILE_RECV
{
	char filename[88];
	int fileBlock;				/// 文件分块大小
	int BlockNum;				/// 文件块数量
	unsigned long filesize;		/// 文件总大小
}FILE_RECV, *LPFILE_RECV;

SOCKET sockClient;		// 连接成功后的套接字
HANDLE bufferMutex;		// 令其能互斥成功正常通信的信号量句柄
#define FTP_PORT  6000
#define FILE_ERROR -1
#define SEND_BUFFER_SIZE 1024*8
#define RECV_BUFFER_SIZE 1024*8
#define DST_FTP_DIR "D:\\CtestCode\\testDstFTP"

int recvData(SOCKET s, char* BaseFilename);

int main()
{
	// 加载socket动态链接库(dll)
	WORD wVersionRequested;
	WSADATA wsaData;	// 这结构是用于接收Wjndows Socket的结构信息的
	wVersionRequested = MAKEWORD(2, 2);	// 请求2.2版本的WinSock库
	int err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {	// 返回值为零的时候是表示成功申请WSAStartup
		return -1;
	}
	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) { // 检查版本号是否正确
		WSACleanup();
		return -1;
	}

	// 创建socket操作，建立流式套接字，返回套接字号sockClient
	sockClient = socket(AF_INET, SOCK_STREAM, 0);
	if (sockClient == INVALID_SOCKET) {
		printf("Error at socket():%ld\n", WSAGetLastError());
		WSACleanup();
		return -1;
	}

	// 将套接字sockClient与远程主机相连
	// int connect( SOCKET s,  const struct sockaddr* name,  int namelen);
	// 第一个参数：需要进行连接操作的套接字
	// 第二个参数：设定所需要连接的地址信息
	// 第三个参数：地址的长度
	SOCKADDR_IN addrSrv;
	addrSrv.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");		// 本地回路地址是127.0.0.1; 
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(FTP_PORT);
	while (SOCKET_ERROR == connect(sockClient, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR))){
		// 如果还没连接上服务器则要求重连
		cout << "服务器连接失败，是否重新连接？（Y/N):";
		char choice;
		while (cin >> choice && (!((choice != 'Y' && choice == 'N') || (choice == 'Y' && choice != 'N')))){
			cout << "输入错误，请重新输入:";
			cin.sync();
			cin.clear();
		}
		if (choice == 'Y' || choice == 'y'){
			continue;
		}
		else{
			cout << "退出系统中...";
			system("pause");
			return 0;
		}
	}
	cin.sync();
	cout << "本客户端已准备就绪，用户可直接输入文字向服务器反馈信息。\n";
	char welcomeMsg[] = "Welcome to my ftp world.";
	send(sockClient, welcomeMsg, strlen(welcomeMsg), 0); // : A Client has enter...

	char sendBuf[240];
	char recvBuf[10 * 1024];
	while (1){
		cout << "\nftp> ";
		gets_s(sendBuf);
		int sendLength = strlen(sendBuf);
		char *pEnd = sendBuf + sendLength - 1;
		// 去掉字符后面的空格或者tab键；先处理后面空格或者tab字符，然后处理前面
		while (*pEnd == '\t' || *pEnd == ' ')
			*pEnd--;
		sendBuf[pEnd - sendBuf + 1] = '\0';
		//去掉字符前面的空格或者tab键
		char *phead = sendBuf;
		while (*phead == ' ' || *phead == '\t')
			*phead++;
		strcpy_s(sendBuf, phead);
		
		if ("quit" == sendBuf || sockClient <= 0){
			send(sockClient, sendBuf, 40, 0);
			cout << "Quit System ...";
			WSACleanup();
			break;
		}

		memset((void*)recvBuf, 0, sizeof(recvBuf));
		
		if (strncmp(sendBuf, "get ", 4) == 0)
		{
		//	char *fName = sendBuf + 4;
			// 先发送需要下载的文件名字
			if (send(sockClient, sendBuf, strlen(sendBuf), 0) < 0)
			{
				WSACleanup();
				cout << "send failed\n";
				system("pause");
				return -1;
			}
			if (recvData(sockClient, sendBuf + 4) != 1)
			{
				cout << "recvData error.\n";
				system("pause");
				break;
			}
		}
		else if ((strncmp(sendBuf, "cd ", 3)) == 0)
		{
			if (send(sockClient, sendBuf, strlen(sendBuf), 0) < 0)	// 发送信息
			{
				char tmp[50];
				sprintf_s(tmp, "Client. cd send error. %s\n", errno);
				perror(tmp);
				system("pause");
				break;
			}
			memset(recvBuf, 0, sizeof(recvBuf));
			int recvLen = -1;
			if ((recvLen = recv(sockClient, recvBuf, RECV_BUFFER_SIZE, 0)) < 0)
			{
				char tmp[50];
				sprintf_s(tmp, "Client. recv in cd error. %s\n", errno);
				perror(tmp);
				system("pause");
			}
			char tmp[88];
			if (strstr(recvBuf, "can't use cd command") != NULL)
				strncpy_s(tmp, recvBuf, strlen(recvBuf) + 1);
			else if (strstr(recvBuf, "is not existed") != NULL)
				strncpy_s(tmp, recvBuf, strlen(recvBuf) + 1);
			else
				sprintf_s(tmp, "Change to dir %s\n", recvBuf);
			cout << tmp;
		}
		else
		{
			if (send(sockClient, sendBuf, strlen(sendBuf), 0) < 0)	// 发送信息
			{
				perror("Client. Other command send error.\n");
				break;
			}
			//memset(recvBuf, 0, sizeof(recvBuf));
			if (recv(sockClient, recvBuf, RECV_BUFFER_SIZE, 0) < 0)
			{
				perror("Client. other command receive error.\n");
				break;
			}
			printf("%s", recvBuf);		// 接收信息
			memset(recvBuf, 0, sizeof(recvBuf));
		}
	}
	closesocket(sockClient);
	WSACleanup();	// 终止对套接字库的使用

	printf("End linking...\n");
	system("pause");
	return 0;
}

int recvData(SOCKET s, char* BaseFilename)
{
	// 先接受文件结构信息
	FILE_RECV fileinfo;
	memset(&fileinfo, 0x00, sizeof(fileinfo));
	if (recv(s, (char *)&fileinfo, sizeof(fileinfo), 0) < 0)
	{
		return SOCKET_ERROR;
	}

	FILE* stream;
	char dstFilename[100];
	memset(dstFilename, 0, sizeof(dstFilename));
	sprintf_s(dstFilename, "%s\\%s", DST_FTP_DIR, BaseFilename);
	if ((fopen_s(&stream, dstFilename, "wb")) != 0)
	{
		return FILE_ERROR;
	}
	unsigned long dataLen = fileinfo.filesize;
	BYTE *recvData = new BYTE[RECV_BUFFER_SIZE];
	unsigned long cbLeftToReceive = dataLen;

	int block = 0;
	do
	{
		int iiRecd;
		int iiGet = (cbLeftToReceive < RECV_BUFFER_SIZE) ? cbLeftToReceive : RECV_BUFFER_SIZE;
		
		if ((iiRecd = recv(s, (char *)recvData, iiGet, 0)) < 0)
		{
			return -1;
		}
		recvData[iiGet] = '\0';
		block++;
		fwrite(recvData, iiRecd, 1, stream);
		cbLeftToReceive -= iiRecd;
		send(s, (char *)&block, sizeof(int), 0);
	} while (cbLeftToReceive > 0);
	cout << "Save file " << BaseFilename << " in " << DST_FTP_DIR << endl;
	// 接受服务器段发送的冗余数据 （这里还未弄清楚是什么发送的）
	recv(s, (char *)recvData, RECV_BUFFER_SIZE, 0);
	//delete[] recvData;
	//recvData = NULL;
	fclose(stream);
	return 1;
}

/*
// 规格化输入字符串
char* FormatString(char* strStrToFormat)
{
	char *p = strStrToFormat;
	// if *p 是空格，或者tab
	while (*p == '\t' || *p == ' ') *p++;
	char *pnext = p + 1;
	//p指向第一个字符，pnext指向p之后的字符
	while (*pnext != '\0')
	{
		if ((*p == ' ' || *p == '\t') && (*pnext == ' ' || *pnext == '\t'))
		{
			*pnext = 0;
			*p++;
			*pnext++;
		}
	}
	for (ULONG i = 0; i<strlen(strStrToFormat); i++)
	{
		if (strStrToFormat[i] == 13 || strStrToFormat[i] == 10)
		{
			strStrToFormat[i] = 0;
		}
	}
	}*/