#ifndef __DATASTRUCT_AND_CONSTANT_H__
#define __DATASTRUCT_AND_CONSTANT_H__
#include "stdhead.h"

#define RECV_BUFFER_SIZE 1024*4
#define SEND_BUFFER_SIZE 1024*4
#define _DEBUG 0
#define MAX_FILE 1024		     ///��������ļ���
#define DataBuffer_Size 10*1024
#define DEFAULT_WORKPATH "D:\\CtestCode\\testSrcFTP"
#define TRAN_SUCCESS 1

#define NTDDI_VERSION >= NTDDI_LONGHORN	//web service

#define FTP_PORT 6000
#define WEB_PORT 8080
#define	TCP_SERV	 1
#define	UDP_SERV	 0
#define WEBMAXLINE 1024
const size_t BUFFER_SIZE = 65535;

typedef struct _FILE_INFO                           ///����һ���ļ��ṹ��������ȡ�ļ�����
{
	TCHAR cFileName[MAX_PATH];                      ///�ļ�����
	DWORD dwFileAttributes;                         ///�ļ����ԣ��Ƿ�Ŀ¼��
	FILETIME ftCreationTime;                        ///�ļ�����ʱ��
	FILETIME ftLastAccessTime;                      ///�ļ���������ʱ��
	FILETIME ftLastWriteTime;                       ///�ļ�������޸�ʱ��
	DWORD nFileSizeHigh;                            ///�ļ���С�ĸ�32λ
	DWORD nFileSizeLow;                             ///�ļ���С�ĵ�32λ
} FILE_INFO, *LPFILE_INFO;

// ���ڷ��͡������ļ�ʱ������
typedef struct _FILE_SEND
{
	char filename[88];
	int fileBlock;				/// �ļ��ֿ��С
	int BlockNum;				/// �ļ�������
	unsigned long filesize;		/// �ļ��ܴ�С
}FILE_SEND, *LPFILE_SEND;

/**
* �ṹ�����ƣ�PER_IO_DATA
* �ṹ�幦�ܣ��ص�I/O��Ҫ�õ��Ľṹ�壬��ʱ��¼IO����
**/
typedef struct
{
	char cmdBuffer[20];					 // ��ſͻ��˷��͹���������
	TCHAR buffer[SEND_BUFFER_SIZE];      // ��ŷ��ظ��ͻ��˶�Ӧ���������
	DWORD BufferLen;        
}PER_IO_OPERATEION_DATA, *LPPER_IO_OPERATION_DATA, *LPPER_IO_DATA, PER_IO_DATA;

typedef struct
{
	SOCKET socket;
	SOCKADDR_STORAGE ClientAddr;
	TCHAR strCurrentWorkPath[1024]; //�ÿ�������Ŀǰ���ڵĹ���·��
}PER_HANDLE_DATA, *LPPER_HANDLE_DATA;

/**
* �ṹ�����ƣ�PER_HANDLE_DATA
* �ṹ��洢����¼�����׽��ֵ����ݣ��������׽��ֵı������׽��ֶ�Ӧ�Ŀͻ��˵�ַ��
* �ṹ�����ã��������������Ͽͻ���ʱ����Ϣ�洢���ýṹ���У�֪���ͻ��˵ĵ�ַ�Ա��ڻطá�
**/

typedef struct
{
	PER_HANDLE_DATA* handle_data;
	PER_IO_OPERATEION_DATA* io_operation_data;
}HANDLE_IOPERATION_DATA, *LPHANDLE_IOPERATION_DATA;

// �ͻ�������ķ���ṹ��
struct service {
	char	*sv_name;
	char	sv_useTCP;
	SOCKET	sv_sock;
	void(*sv_func)(SOCKET);
};
#define TCP_SERV 1

// ����ȫ�ֱ���
#define DefaultPort 6000
//vector < PER_HANDLE_DATA* > clientGroup;		// ��¼�ͻ��˵�������

#ifndef	MAX
#define	MAX(x, y)	((x) > (y) ? (x) : (y))
#endif	/* MAX */

#endif