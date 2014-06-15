#ifndef __PROCESS_COMMAND_H__
#define __PROCESS_COMMAND_H__
#include "DataStructAndConstant.h"

DWORD ProcessListCommand(LPPER_HANDLE_DATA PerHandleData, LPPER_IO_OPERATION_DATA PerIoData);
int ProcessCdCommand(LPPER_HANDLE_DATA PerHandleData, LPPER_IO_OPERATION_DATA PerIoData);
int ProcessGetCommand(LPPER_HANDLE_DATA PerHandleData, LPPER_IO_OPERATION_DATA PerIoData);

void doTCPFTP(SOCKET s);
void UDPweb(SOCKET webconn);
#endif