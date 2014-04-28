#ifndef RENDERFARM_H
#define RENDERFARM_H


#include "arraylist.h"
#include "defaults.inc"
#include "mutex.inc"
#include "mwindow.inc"
#include "packagerenderer.h"
#include "preferences.inc"
#include "render.inc"
#include "thread.h"


// Request format
// 1 byte -> request code
// 4 bytes -> size of packet exclusive
// size of packet -> data



// General reply format
// 4 bytes -> size of packet exclusive
// size of packet -> data


class RenderFarmServerThread;

class RenderFarmServer
{
public:
	RenderFarmServer(MWindow *mwindow, Render *render);
	~RenderFarmServer();

// Open connections to clients.
	int start_clients();
// The render farm must wait for all the clients to finish.
	int wait_clients();

// Likewise the render farm must check the internal render loop before 
// dispatching the next job and whenever a client queries for errors.


// Get average frames per second for all nodes which have completed a package
	double calculate_avg_fps();

	ArrayList<RenderFarmServerThread*> clients;
	MWindow *mwindow;
	Render *render;
	Preferences *preferences;
	Mutex *client_lock;
};


// Waits for requests from every client.
// Joins when the client is finished.
class RenderFarmServerThread : public Thread
{
public:
	RenderFarmServerThread(MWindow *mwindow, 
		RenderFarmServer *server, 
		int number);
	~RenderFarmServerThread();
	
	static int read_socket(int socket_fd, char *data, int len);
// Inserts header and writes string to socket
	static int write_string(int socket_fd, char *string);
	int start_loop();
	void send_preferences();
	void send_asset();
	void send_edl();
	void send_package(unsigned char *buffer);
	void set_progress(unsigned char *buffer);
	void set_result(unsigned char *buffer);
	void get_result();

	
	void run();
	
	MWindow *mwindow;
	RenderFarmServer *server;
	int socket_fd;
	int number;
// Rate of last job or 0
	double frames_per_second;
};







class RenderFarmClientThread;



// The render client waits for connections from the server.
// Then it starts a thread for each connection.
class RenderFarmClient
{
public:
	RenderFarmClient(int port);
	~RenderFarmClient();
	
	void main_loop();
	
	
	
	RenderFarmClientThread *thread;
	
	int port;
// The plugin paths must be known before any threads are started
	Defaults *boot_defaults;
	Preferences *boot_preferences;
	ArrayList<PluginServer*> *plugindb;
};




class FarmPackageRenderer : public PackageRenderer
{
public:
	FarmPackageRenderer(RenderFarmClientThread *thread,
		int socket_fd);
	~FarmPackageRenderer();
	
	
	int get_result();
	void set_result(int value);
	void set_progress(long value);

	
	int socket_fd;
	RenderFarmClientThread *thread;
};

// The thread requests jobs from the server until the job table is empty
// or the server reports an error.  This thread must poll the server
// after every frame for the error status.
// Detaches when finished.
class RenderFarmClientThread : public Thread
{
public:
	RenderFarmClientThread(RenderFarmClient *client);
	~RenderFarmClientThread();

	void send_request_header(int socket_fd, 
		int request, 
		int len);
	void read_string(int socket_fd, char* &string);
	void RenderFarmClientThread::read_preferences(int socket_fd, 
		Preferences *preferences);
	void read_asset(int socket_fd, Asset *asset);
	void read_edl(int socket_fd, 
		EDL *edl, 
		Preferences *preferences);
	int read_package(int socket_fd, RenderPackage *package);
	void send_completion(int socket_fd);

	void main_loop(int socket_fd);
	void run();

// Everything must be contained in run()
	int socket_fd;
	RenderFarmClient *client;
	double frames_per_second;
};


#endif
