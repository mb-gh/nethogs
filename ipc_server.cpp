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
<<<<<<< HEAD
#include <atomic>
=======
>>>>>>> a1b5b38... Daemon mode - work in progress

static pthread_t server_thread;

typedef struct connection_thread_data
{
	int id;
	int socket_fd;
	pthread_t thread;
} connection_thread_data_t;

typedef std::map<int, connection_thread_data_t> ClientConnectionsMap;
static ClientConnectionsMap client_connections;
static std::mutex client_connections_mutex;

<<<<<<< HEAD
static std::atomic_bool stop_server_flag(false);

static const char* server_socket_path = "/tmp/nethogs/1000.socket";
static int server_socket_fd = 0;
=======
static bool stop_server_flag = false;

const char* socket_path = "/tmp/nethogs/1000.socket";
>>>>>>> a1b5b38... Daemon mode - work in progress

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

<<<<<<< HEAD
	if( stop_server_flag )
	{
		printf("IPCServer: closing client connection socked\n");
	}
	else if(rc == -1)
=======
	if(rc == -1)
>>>>>>> a1b5b38... Daemon mode - work in progress
	{
		perror("IPCServer: read error");
	}
	else if (rc == 0) 
	{
		printf("IPCServer: EOF\n");
<<<<<<< HEAD
	}

	close(client_thread_data->socket_fd);

=======
		close(client_thread_data->socket_fd);
	}

>>>>>>> a1b5b38... Daemon mode - work in progress
	{
		std::lock_guard<std::mutex> lock(client_connections_mutex);
		client_connections.erase(client_thread_data->id);
	}
<<<<<<< HEAD
		
=======
	
>>>>>>> a1b5b38... Daemon mode - work in progress
	std::cout << "Stopping a client connection thread" << std::endl;

	return NULL;
}

void* IPCServer::threadProc(void* p)
{
	std::cout << "Starting IPC Server thread" << std::endl;	
	
	(void)p;
    struct sockaddr_un addr;
    int rc;

<<<<<<< HEAD
    if((server_socket_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
=======
	int socket_fd;
    if((socket_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
>>>>>>> a1b5b38... Daemon mode - work in progress
	{
	    perror("IPCServer: socket error");
	    return NULL;
	}

	//create socket dir
<<<<<<< HEAD
	char* socket_path_cpy = (char*)malloc(strlen(server_socket_path) + 1);
	strcpy(socket_path_cpy, server_socket_path);
=======
	char* socket_path_cpy = (char*)malloc(strlen(socket_path) + 1);
	strcpy(socket_path_cpy, socket_path);
>>>>>>> a1b5b38... Daemon mode - work in progress
	mkdir(dirname(socket_path_cpy), ACCESSPERMS);
	free(socket_path_cpy);

	//prepare socket path
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
<<<<<<< HEAD
    strncpy(addr.sun_path, server_socket_path, sizeof(addr.sun_path) - 1);
    unlink(server_socket_path);
	
    if(bind(server_socket_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1)
=======
    strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path) - 1);
    unlink(socket_path);
	
    if(bind(socket_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1)
>>>>>>> a1b5b38... Daemon mode - work in progress
	{
	    perror("IPCServer: bind error");
	    return NULL;
	}

<<<<<<< HEAD
    if(listen(server_socket_fd, 5) == -1)
=======
    if(listen(socket_fd, 5) == -1)
>>>>>>> a1b5b38... Daemon mode - work in progress
	{
	    perror("IPCServer: listen error");
	    return NULL;
	}

    while( !stop_server_flag )
	{
		int conn_socket_fd; 
<<<<<<< HEAD
	    if((conn_socket_fd = accept(server_socket_fd, NULL, NULL)) == -1)
		{
			if( !stop_server_flag )
			{
				perror("IPCServer: accept error");
			}
			std::cout << "Stopping IPC Server thread" << std::endl;
=======
	    if((conn_socket_fd = accept(socket_fd, NULL, NULL)) == -1)
		{
		    perror("IPCServer: accept error");
>>>>>>> a1b5b38... Daemon mode - work in progress
			return NULL;
		}
		
		static int connection_id = 0;
		++connection_id;

		connection_thread_data* client = NULL;
		{
			std::lock_guard<std::mutex> lock(client_connections_mutex);	
			client = &client_connections.insert(
			std::make_pair(connection_id, connection_thread_data())).first->second;	
		}
		
		client->id = connection_id;
		client->socket_fd = conn_socket_fd;

		pthread_t conn_thread = 0;
		rc = pthread_create(&conn_thread, NULL, &connectionThreadProc, client);
		if( rc )
		{
			printf("IPCServer: Failed to create server thread: %s\n", strerror(rc));
			std::lock_guard<std::mutex> lock(client_connections_mutex);	
			client_connections.erase(connection_id);
			return NULL;
		}
		client->thread = conn_thread;
	}
	

	ClientConnectionsMap connections;
	{
		std::lock_guard<std::mutex> lock(client_connections_mutex);	
		connections = client_connections;
		for(ClientConnectionsMap::const_iterator it=connections.begin();
			it != connections.end(); ++it)
		{
<<<<<<< HEAD
			shutdown(it->second.socket_fd, SHUT_RDWR);
=======
			close(it->second.socket_fd);
>>>>>>> a1b5b38... Daemon mode - work in progress
			pthread_join(it->second.thread, 0);
		}
	}
	
<<<<<<< HEAD
	close(server_socket_fd);
	
	std::cout << "Stopping IPC Server thread" << std::endl;		
=======
	close(socket_fd);
	
	std::cout << "Stopping IPC Server thread" << std::endl;	
	
>>>>>>> a1b5b38... Daemon mode - work in progress
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
<<<<<<< HEAD
	shutdown(server_socket_fd, SHUT_RDWR);
=======
>>>>>>> a1b5b38... Daemon mode - work in progress
	pthread_join(server_thread, 0);
}
