#ifndef _IPC_STRUCTS_H
#define _IPC_STRUCTS_H

#pragma pack(push, 1)

#define NETHOGS_IPC_STRUCT_ID_PROCESS_COUNT		1
#define NETHOGS_IPC_STRUCT_ID_PROCESS_DATA		2

#include <sys/types.h>

typedef struct
{
	int id;
	int count;
} nethogs_ipc_process_count;


typedef struct
{
	int32_t  id;
	int32_t  m_pid;
	int32_t  m_uid;	
	double   sent_value;
	double   recv_value;
	char 	 m_processname[150];
	char 	 m_devicename [50];
} nethogs_ipc_process_data;

#pragma pack(pop)

#endif
