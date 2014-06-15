#ifndef __DATASTRUCT_AND_CONSTANT_H__
#define __DATASTRUCT_AND_CONSTANT_H__
#include "stdhead.h"

#define RECV_BUFFER_SIZE 1024*4
#define SEND_BUFFER_SIZE 1024*4
#define _DEBUG 0
#define MAX_FILE 1024		     ///定义最多文件数
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

typedef struct _FILE_INFO                           ///定义一个文件结构，用于提取文件属性
{
	TCHAR cFileName[MAX_PATH];                      ///文件名称
	DWORD dwFileAttributes;                         ///文件属性（是否目录）
	FILETIME ftCreationTime;                        ///文件创建时间
	FILETIME ftLastAccessTime;                      ///文件的最后访问时间
	FILETIME ftLastWriteTime;                       ///文件的最后修改时间
	DWORD nFileSizeHigh;                            ///文件大小的高32位
	DWORD nFileSizeLow;                             ///文件大小的低32位
} FILE_INFO, *LPFILE_INFO;

// 用于发送、接受文件时的属性
typedef struct _FILE_SEND
{
	char filename[88];
	int fileBlock;				/// 文件分块大小
	int BlockNum;				/// 文件块数量
	unsigned long filesize;		/// 文件总大小
}FILE_SEND, *LPFILE_SEND;

/**
* 结构体名称：PER_IO_DATA
* 结构体功能：重叠I/O需要用到的结构体，临时记录IO数据
**/
typedef struct
{
	char cmdBuffer[20];					 // 存放客户端发送过来的命令
	TCHAR buffer[SEND_BUFFER_SIZE];      // 存放返回给客户端对应命令的数据
	DWORD BufferLen;        
}PER_IO_OPERATEION_DATA, *LPPER_IO_OPERATION_DATA, *LPPER_IO_DATA, PER_IO_DATA;

typedef struct
{
	SOCKET socket;
	SOCKADDR_STORAGE ClientAddr;
	TCHAR strCurrentWorkPath[1024]; //该控制连接目前所在的工作路径
}PER_HANDLE_DATA, *LPPER_HANDLE_DATA;

/**
* 结构体名称：PER_HANDLE_DATA
* 结构体存储：记录单个套接字的数据，包括了套接字的变量及套接字对应的客户端地址。
* 结构体作用：当服务器连接上客户端时，信息存储到该结构体中，知道客户端的地址以便于回访。
**/

typedef struct
{
	PER_HANDLE_DATA* handle_data;
	PER_IO_OPERATEION_DATA* io_operation_data;
}HANDLE_IOPERATION_DATA, *LPHANDLE_IOPERATION_DATA;

// 客户端请求的服务结构体
struct service {
	char	*sv_name;
	char	sv_useTCP;
	SOCKET	sv_sock;
	void(*sv_func)(SOCKET);
};
#define TCP_SERV 1

// 定义全局变量
#define DefaultPort 6000
//vector < PER_HANDLE_DATA* > clientGroup;		// 记录客户端的向量组

#ifndef	MAX
#define	MAX(x, y)	((x) > (y) ? (x) : (y))
#endif	/* MAX */

#endif