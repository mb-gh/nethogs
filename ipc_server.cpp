#include "ipc_server.h"
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <libgen.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <iostream>
#include <map>
#include <mutex>
#include <atomic>

static pthread_t server_thread;
static std::atomic_bool stop_server_flag(false);
static const char* server_socket_path = "/tmp/nethogs/1000.socket";
static int server_socket_fd = 0;


typedef struct connection_thread_data
{
	int id;
	int socket_fd;
	pthread_t thread;
} connection_thread_data_t;

class ClientConnectionMap
{
public:
	typedef std::map<int, connection_thread_data_t> TMap;

	void erase(int id)
	{
		std::lock_guard<std::mutex> lock(_mutex);
		_map.erase(id);		
	}
	connection_thread_data_t& insert(int id)
	{
		std::lock_guard<std::mutex> lock(_mutex);
		return _map.insert(std::make_pair(id, connection_thread_data())).first->second;
	}
	
	inline TMap copy()
	{
		std::lock_guard<std::mutex> lock(_mutex);
		return _map;
	}
private:
	TMap _map;
	std::mutex _mutex;
};

static ClientConnectionMap client_connections;

void* IPCServer::connectionThreadProc(void* p)
{
	std::cout << "Starting a new client connection thread" << std::endl;
	
	const connection_thread_data *client_thread_data = (connection_thread_data*)p;
	
	int rc;
    char buf[100];

	while((rc = read(client_thread_data->socket_fd, buf, sizeof(buf))) > 0)
	{
		printf("IPCServer: read %u bytes: %.*s\n", rc, rc, buf);
	}

	if( stop_server_flag )
	{
		printf("IPCServer: closing client connection socked\n");
	}
	else if(rc == -1)
	{
		perror("IPCServer: read error");
	}
	else if (rc == 0) 
	{
		printf("IPCServer: EOF\n");
	}

	close(client_thread_data->socket_fd);

	{
		client_connections.erase(client_thread_data->id);
	}

	std::cout << "Stopping a client connection thread" << std::endl;

	return NULL;
}

void* IPCServer::threadProc(void* p)
{
	std::cout << "Starting IPC Server thread" << std::endl;	
	
	(void)p;
    struct sockaddr_un addr;
    int rc;

    if((server_socket_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
	{
	    perror("IPCServer: socket error");
	    return NULL;
	}

	char* socket_path_cpy = (char*)malloc(strlen(server_socket_path) + 1);
	strcpy(socket_path_cpy, server_socket_path);
	mkdir(dirname(socket_path_cpy), ACCESSPERMS);
	free(socket_path_cpy);

	//prepare socket path
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, server_socket_path, sizeof(addr.sun_path) - 1);
    unlink(server_socket_path);
	
    if(bind(server_socket_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1)
	{
	    perror("IPCServer: bind error");
	    return NULL;
	}

    if(listen(server_socket_fd, 5) == -1)
	{
	    perror("IPCServer: listen error");
	    return NULL;
	}

    while( !stop_server_flag )
	{
		int conn_socket_fd; 
	    if((conn_socket_fd = accept(server_socket_fd, NULL, NULL)) == -1)
		{
			if( !stop_server_flag )
			{
				perror("IPCServer: accept error");
			}
			std::cout << "Stopping IPC Server thread" << std::endl;
			return NULL;
		}
		
		static int connection_id = 0;
		++connection_id;

		connection_thread_data* client = NULL;
		{
			client = &client_connections.insert(connection_id);	
		}
		
		client->id = connection_id;
		client->socket_fd = conn_socket_fd;

		pthread_t conn_thread = 0;
		rc = pthread_create(&conn_thread, NULL, &connectionThreadProc, client);
		if( rc )
		{
			printf("IPCServer: Failed to create server thread: %s\n", strerror(rc));
			client_connections.erase(connection_id);
			return NULL;
		}
		client->thread = conn_thread;
	}
	

	ClientConnectionMap::TMap connections = client_connections.copy();
	for(ClientConnectionMap::TMap::const_iterator it=connections.begin();
		it != connections.end(); ++it)
	{
		shutdown(it->second.socket_fd, SHUT_RDWR);
		pthread_join(it->second.thread, 0);
	}
	
	close(server_socket_fd);
	
	std::cout << "Stopping IPC Server thread" << std::endl;		
	return NULL;
}

void IPCServer::start()
{	
	std::cout << "Starting IPC Server" << std::endl;	
	stop_server_flag = false;
	int rc = pthread_create(&server_thread, NULL, &threadProc, 0);
	if( rc )
	{
		printf("IPCServer: Failed to create server thread: %s\n", strerror(rc));
	}
}

void IPCServer::stop()
{
	std::cout << "Stopping IPC Server" << std::endl;	
	stop_server_flag = true;
	shutdown(server_socket_fd, SHUT_RDWR);
	pthread_join(server_thread, 0);
}

void IPCServer::nh_dbus_send_process_info(int count, const Line * const lines [])
{
	//client_connections.
}
