// IOCP_TCPIP_Socket_Client.cpp

#include <iostream>
#include <cstdio>
#include <string>
#include <cstring>
#include <winsock2.h>
#include <Windows.h>

using namespace std;

#pragma comment(lib, "Ws2_32.lib")		// Socket������õĶ�̬���ӿ�

// ���ڷ��͡������ļ�ʱ������
typedef struct _FILE_RECV
{
	char filename[88];
	int fileBlock;				/// �ļ��ֿ��С
	int BlockNum;				/// �ļ�������
	unsigned long filesize;		/// �ļ��ܴ�С
}FILE_RECV, *LPFILE_RECV;

SOCKET sockClient;		// ���ӳɹ�����׽���
HANDLE bufferMutex;		// �����ܻ���ɹ�����ͨ�ŵ��ź������
#define FTP_PORT  6000
#define FILE_ERROR -1
#define SEND_BUFFER_SIZE 1024*8
#define RECV_BUFFER_SIZE 1024*8
#define DST_FTP_DIR "D:\\CtestCode\\testDstFTP"

int recvData(SOCKET s, char* BaseFilename);

int main()
{
	// ����socket��̬���ӿ�(dll)
	WORD wVersionRequested;
	WSADATA wsaData;	// ��ṹ�����ڽ���Wjndows Socket�Ľṹ��Ϣ��
	wVersionRequested = MAKEWORD(2, 2);	// ����2.2�汾��WinSock��
	int err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {	// ����ֵΪ���ʱ���Ǳ�ʾ�ɹ�����WSAStartup
		return -1;
	}
	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) { // ���汾���Ƿ���ȷ
		WSACleanup();
		return -1;
	}

	// ����socket������������ʽ�׽��֣������׽��ֺ�sockClient
	sockClient = socket(AF_INET, SOCK_STREAM, 0);
	if (sockClient == INVALID_SOCKET) {
		printf("Error at socket():%ld\n", WSAGetLastError());
		WSACleanup();
		return -1;
	}

	// ���׽���sockClient��Զ����������
	// int connect( SOCKET s,  const struct sockaddr* name,  int namelen);
	// ��һ����������Ҫ�������Ӳ������׽���
	// �ڶ����������趨����Ҫ���ӵĵ�ַ��Ϣ
	// ��������������ַ�ĳ���
	SOCKADDR_IN addrSrv;
	addrSrv.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");		// ���ػ�·��ַ��127.0.0.1; 
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(FTP_PORT);
	while (SOCKET_ERROR == connect(sockClient, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR))){
		// �����û�����Ϸ�������Ҫ������
		cout << "����������ʧ�ܣ��Ƿ��������ӣ���Y/N):";
		char choice;
		while (cin >> choice && (!((choice != 'Y' && choice == 'N') || (choice == 'Y' && choice != 'N')))){
			cout << "�����������������:";
			cin.sync();
			cin.clear();
		}
		if (choice == 'Y' || choice == 'y'){
			continue;
		}
		else{
			cout << "�˳�ϵͳ��...";
			system("pause");
			return 0;
		}
	}
	cin.sync();
	cout << "���ͻ�����׼���������û���ֱ�����������������������Ϣ��\n";
	char welcomeMsg[] = "Welcome to my ftp world.";
	send(sockClient, welcomeMsg, strlen(welcomeMsg), 0); // : A Client has enter...

	char sendBuf[240];
	char recvBuf[10 * 1024];
	while (1){
		cout << "\nftp> ";
		gets_s(sendBuf);
		int sendLength = strlen(sendBuf);
		char *pEnd = sendBuf + sendLength - 1;
		// ȥ���ַ�����Ŀո����tab�����ȴ������ո����tab�ַ���Ȼ����ǰ��
		while (*pEnd == '\t' || *pEnd == ' ')
			*pEnd--;
		sendBuf[pEnd - sendBuf + 1] = '\0';
		//ȥ���ַ�ǰ��Ŀո����tab��
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
			// �ȷ�����Ҫ���ص��ļ�����
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
			if (send(sockClient, sendBuf, strlen(sendBuf), 0) < 0)	// ������Ϣ
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
			if (send(sockClient, sendBuf, strlen(sendBuf), 0) < 0)	// ������Ϣ
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
			printf("%s", recvBuf);		// ������Ϣ
			memset(recvBuf, 0, sizeof(recvBuf));
		}
	}
	closesocket(sockClient);
	WSACleanup();	// ��ֹ���׽��ֿ��ʹ��

	printf("End linking...\n");
	system("pause");
	return 0;
}

int recvData(SOCKET s, char* BaseFilename)
{
	// �Ƚ����ļ��ṹ��Ϣ
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
	// ���ܷ������η��͵��������� �����ﻹδŪ�����ʲô���͵ģ�
	recv(s, (char *)recvData, RECV_BUFFER_SIZE, 0);
	//delete[] recvData;
	//recvData = NULL;
	fclose(stream);
	return 1;
}

/*
// ��������ַ���
char* FormatString(char* strStrToFormat)
{
	char *p = strStrToFormat;
	// if *p �ǿո񣬻���tab
	while (*p == '\t' || *p == ' ') *p++;
	char *pnext = p + 1;
	//pָ���һ���ַ���pnextָ��p֮����ַ�
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