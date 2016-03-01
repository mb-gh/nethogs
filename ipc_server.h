#ifndef _NETHOGS_IPC_SERVER_H_
#define _NETHOGS_IPC_SERVER_H_

#include "line.h"

class IPCServer
{
	IPCServer();
	IPCServer(IPCServer const&);
	
public:
	static void start();
	static void stop();
	void nh_dbus_send_process_info(int count, const Line * const lines []);

private:
	static void* connectionThreadProc(void* p);
	static void* threadProc(void* p);
};

#endif // _NETHOGS_IPC_SERVER_H_
