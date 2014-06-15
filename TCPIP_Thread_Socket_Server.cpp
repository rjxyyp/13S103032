
#include "DataStructAndConstant.h"
#include "stdhead.h"
#include "ProcessCommand.h"
#include "ToolFunction.h"
#include <winsock.h>

void doTCPFTP(SOCKET), UDPweb(SOCKET);

struct service svent[] =
{ 
	{ "ftp", TCP_SERV, INVALID_SOCKET, doTCPFTP },
	{ "web", UDP_SERV, INVALID_SOCKET, UDPweb }, 	//web ����
	{ 0, 0, 0, 0 },
};
/*

{ "get", TCP_SERV, INVALID_SOCKET, TCPdaytimed },
{ "put", TCP_SERV, INVALID_SOCKET, TCPtimed },
*/

int main()
{
	// ����socket��̬���ӿ�
	WORD wVersionRequested = MAKEWORD(2, 2); // ����2.2�汾��WinSock��
	WSADATA wsaData;	// ����Windows Socket�Ľṹ��Ϣ
	DWORD err = WSAStartup(wVersionRequested, &wsaData);

	if (0 != err){	// ����׽��ֿ��Ƿ�����ɹ�
		cerr << "Request Windows Socket Library Error!\n";
		return -1;
	}
	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2){// ����Ƿ�����������汾���׽��ֿ�
		WSACleanup();
		cerr << "Request Windows Socket Version 2.2 Error!\n";
		return -1;
	}

	// ������ʽ�׽���
	SOCKET ftplisten, weblisten;
	if ((ftplisten = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		errexit("server: FTP listen socket error: %d\n", errno);
		exit(-1);
	}

	if ((weblisten = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		printf("server: WEB listen socket error: %d\n", errno);
	}

	// ��SOCKET������
	SOCKADDR_IN ftpAddr, webAddr;
	ftpAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	ftpAddr.sin_family = AF_INET;
	ftpAddr.sin_port = htons(FTP_PORT);
	if (SOCKET_ERROR == bind(ftplisten, (SOCKADDR*)&ftpAddr, sizeof(SOCKADDR))){
		cerr << "Bind failed. Error:" << GetLastError() << endl;
		system("pause");
		return -1;
	}

	webAddr.sin_family = AF_INET;
	webAddr.sin_port = htons(WEB_PORT);
	webAddr.sin_addr.S_un.S_addr = htons(INADDR_ANY);
	if (bind(weblisten, (struct sockaddr *) &webAddr, sizeof(webAddr)) < 0) {
		printf("server: Web server Bind error: %d\n", errno);
		exit(0);
	}

	// ��SOCKET����Ϊ����ģʽ
	if (SOCKET_ERROR == listen(ftplisten, 10)){
		cerr << "server: Ftp Listen failed. Error: " << GetLastError() << endl;
		system("pause");
		return -1;
	}

	if (listen(weblisten, 3) < 0) {
		printf("server: Web listen error: %d\n", errno);
		exit(0);
	}

	// ��ʼ����IO����
	cout << "����������׼�����������ڵȴ��ͻ��˵Ľ���...\n";

	int nfds = 0, fd;
	fd_set afds, rfds;
	FD_ZERO(&afds);
	struct service	*psv,		/* service table pointer	*/
					*fd2sv[20];		/* map fd to service pointer	*/
	for (psv = &svent[0]; psv->sv_name; ++psv)
	{
		if (psv->sv_name == "ftp")
		{
			psv->sv_sock = ftplisten;
		}
		else if (psv->sv_name == "web")
		{
			psv->sv_sock = weblisten;
		}
		fd2sv[psv->sv_sock] = psv;
		nfds = MAX(psv->sv_sock + 1, nfds);
		FD_SET(psv->sv_sock, &afds);
	}
	while (true){
		memcpy(&rfds, &afds, sizeof(rfds));
		if (select(nfds, &rfds, (fd_set *)0, (fd_set *)0,
			(struct timeval *)0) < 0) {
			if (errno == EINTR)
				continue;
			errexit("select error: %d\n", GetLastError());
		}
		SOCKET ftpConnSocket, webConnSocket;
		SOCKADDR_IN saRemote;
		int alen = sizeof(saRemote);
		for (fd = 0; fd < nfds; ++fd)
		{
			if (FD_ISSET(fd, &rfds))
			{
				psv = fd2sv[fd];
				if (psv->sv_name == "ftp")
				{
					if ((ftpConnSocket = accept(psv->sv_sock, (struct sockaddr *)&saRemote, &alen)) == INVALID_SOCKET)
					{
						errexit("accept: %d\n", GetLastError());
						return -1;
					}
					// Create thread
					// not use superd server
					if (_beginthread((void(*)(void *))psv->sv_func, 0, (void *)ftpConnSocket) == (unsigned long)-1)
					{
						errexit("_beginthread: %s\n", GetLastError());// strerror_s(errno));
						return -1;
					}
				}
				else if (psv->sv_name == "web")
				{
					// add web service;
					if ((webConnSocket = accept(weblisten, (struct  sockaddr *)&saRemote, &alen)) < 0)
					{
						printf("server: web accept error: %d\n", errno);
						return -1;
					}
					UDPweb(webConnSocket);
				}
				
			}
		}
	}
}
