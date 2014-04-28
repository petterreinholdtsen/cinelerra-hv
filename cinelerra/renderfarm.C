#include "assets.h"
#include "clip.h"
#include "defaults.h"
#include "edl.h"
#include "filesystem.h"
#include "filexml.h"
#include "mutex.h"
#include "mwindow.h"
#include "preferences.h"
#include "render.h"
#include "renderfarm.h"
#include "timer.h"
#include "transportque.h"


#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>


// Request codes
enum
{
	RENDERFARM_PREFERENCES,  // Get preferences on startup
	RENDERFARM_ASSET,        // Get output format on startup
	RENDERFARM_EDL,          // Get EDL on startup
	RENDERFARM_PACKAGE,      // Get one package after another to render
	RENDERFARM_PROGRESS,     // Update completion total
	RENDERFARM_SET_RESULT,   // Update error status
	RENDERFARM_GET_RESULT,   // Retrieve error status
	RENDERFARM_DONE          // Quit
};


RenderFarmServer::RenderFarmServer(MWindow *mwindow, Render *render)
{
	this->mwindow = mwindow;
	this->render = render;
	this->preferences = render->preferences;
	client_lock = new Mutex;
}

RenderFarmServer::~RenderFarmServer()
{
	clients.remove_all_objects();
	delete client_lock;
}

// Open connections to clients.
int RenderFarmServer::start_clients()
{
	int result = 0;
	
	for(int i = 0; i < render->preferences->get_enabled_nodes() && !result; i++)
	{
		client_lock->lock();
		RenderFarmServerThread *client = new RenderFarmServerThread(mwindow, this, i);
		clients.append(client);

		result = client->start_loop();
		client_lock->unlock();
//usleep(100000);
// Fails to connect all without a delay
	}
	
	return result;
}

// The render farm must wait for all the clients to finish.
int RenderFarmServer::wait_clients()
{
	clients.remove_all_objects();
	return 0;
}

double RenderFarmServer::calculate_avg_fps()
{
	client_lock->lock();
	double total_fps = 0;
	double total_clients = 0;
//printf("RenderFarmServer::calculate_avg_fps 1\n");
	for(int i = 0; i < clients.total; i++)
	{
		RenderFarmServerThread *thread = clients.values[i];
		if(!EQUIV(thread->frames_per_second, 0))
		{
			total_fps += thread->frames_per_second;
//printf("RenderFarmServer::calculate_avg_fps 2 %f %f\n", total_fps, thread->frames_per_second);
			total_clients++;
		}
	}

	if(!EQUIV(render->frames_per_second, 0))
	{
		total_fps += render->frames_per_second;
		total_clients++;
	}
	client_lock->unlock();

//printf("RenderFarmServer::calculate_avg_fps 3 %f %f\n", total_fps, total_clients);
	if(total_clients)
		return total_fps / total_clients;
	else
		return 0;
}












// Waits for requests from every client.
// Joins when the client is finished.
RenderFarmServerThread::RenderFarmServerThread(MWindow *mwindow, 
	RenderFarmServer *server, 
	int number)
 : Thread()
{
	this->mwindow = mwindow;
	this->server = server;
	this->number = number;
	socket_fd = -1;
	frames_per_second = 0;
	Thread::set_synchronous(1);
}



RenderFarmServerThread::~RenderFarmServerThread()
{
	Thread::join();
	if(socket_fd >= 0) close(socket_fd);
}


int RenderFarmServerThread::start_loop()
{
	int result = 0;

	if((socket_fd = socket(PF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("RenderFarmServerThread::start_loop: socket");
		result = 1;
	}
	else
	{
		struct sockaddr_in addr;
		struct hostent *hostinfo;
   		addr.sin_family = AF_INET;
		addr.sin_port = htons(server->preferences->get_node_port(number));
		hostinfo = gethostbyname(server->preferences->get_node_hostname(number));
		if(hostinfo == NULL) 
    	{
    		fprintf(stderr, "RenderFarmServerThread::start_loop: unknown host %s.\n", 
				server->preferences->get_node_hostname(number));
    		result = 1;
    	}
		else
		{
			addr.sin_addr = *(struct in_addr *) hostinfo->h_addr;	

			if(connect(socket_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
			{
				fprintf(stderr, "RenderFarmServerThread::start_loop: %s: %s", 
					server->preferences->get_node_hostname(number), 
					strerror(errno));
				result = 1;
			}
		}
	}

	if(!result) Thread::start();

	return result;
}


int RenderFarmServerThread::read_socket(int socket_fd, char *data, int len)
{
	int bytes_read = 0;
	int offset = 0;
	while(len > 0 && bytes_read >= 0)
	{
		bytes_read = read(socket_fd, data + offset, len);
		if(bytes_read >= 0)
		{
			len -= bytes_read;
			offset += bytes_read;
		}
	}
	return offset;
}

void RenderFarmServerThread::run()
{
// Wait for requests
	unsigned char header[5];
	unsigned char *buffer = 0;
	long buffer_allocated = 0;
	int done = 0;

	while(!done)
	{
		read_socket(socket_fd, (char*)header, 5);
//printf("RenderFarmServerThread::run %02x%02x%02x%02x%02x\n",
//	header[0], header[1], header[2], header[3], header[4]);
		
		int request_id = header[0];
		long request_size = (((u_int32_t)header[1]) << 24) |
							(((u_int32_t)header[2]) << 16) |
							(((u_int32_t)header[3]) << 8)  |
							(u_int32_t)header[4];

		if(buffer && buffer_allocated < request_size)
		{
			delete [] buffer;
			buffer = 0;
		}

		if(!buffer && request_size)
		{
			buffer = new unsigned char[request_size];
			buffer_allocated = request_size;
		}

		read_socket(socket_fd, (char*)buffer, request_size);

		switch(request_id)
		{
			case RENDERFARM_PREFERENCES:
				send_preferences();
				break;
			
			case RENDERFARM_ASSET:
				send_asset();
				break;
			
			case RENDERFARM_EDL:
				send_edl();
				break;
			
			case RENDERFARM_PACKAGE:
				send_package(buffer);
				break;
			
			case RENDERFARM_PROGRESS:
				set_progress(buffer);
				break;
			
			case RENDERFARM_SET_RESULT:
				set_result(buffer);
				break;
			
			case RENDERFARM_GET_RESULT:
				get_result();
				break;
			
			case RENDERFARM_DONE:
				done = 1;
				break;
			
			default:
				printf("RenderFarmServerThread::run: unknown request %02x\n", request_id);
				break;
		}
	}
	
	if(buffer) delete [] buffer;
}

#define STORE_INT32(value) \
	datagram[i++] = (((u_int32_t)(value)) >> 24) & 0xff; \
	datagram[i++] = (((u_int32_t)(value)) >> 16) & 0xff; \
	datagram[i++] = (((u_int32_t)(value)) >> 8) & 0xff; \
	datagram[i++] = ((u_int32_t)(value)) & 0xff;

int RenderFarmServerThread::write_string(int socket_fd, char *string)
{
	unsigned char *datagram;
	int i, len;
	i = 0;

	len = strlen(string) + 1;
	datagram = new unsigned char[len + 4];
	STORE_INT32(len);
	memcpy(datagram + i, string, len);
	write(socket_fd, datagram, len + 4);
//printf("RenderFarmServerThread::write_string %02x%02x%02x%02x\n",
//	datagram[0], datagram[1], datagram[2], datagram[3]);

	delete [] datagram;
}

void RenderFarmServerThread::send_preferences()
{
	Defaults defaults;
	char *string;

//printf("RenderFarmServerThread::send_preferences 1\n");
	server->preferences->save_defaults(&defaults);
	defaults.save_string(string);
//printf("RenderFarmServerThread::send_preferences 2 \n%s\n", string);
	write_string(socket_fd, string);

	delete [] string;
}

void RenderFarmServerThread::send_asset()
{
	FileXML file;
//printf("RenderFarmServerThread::send_asset 1\n");
	
	server->render->default_asset->write(mwindow->plugindb, 
		&file, 
		0, 
		0);
	file.terminate_string();

//printf("RenderFarmServerThread::send_asset 2\n");
	write_string(socket_fd, file.string);
//printf("RenderFarmServerThread::send_asset 3\n");
}


void RenderFarmServerThread::send_edl()
{
	FileXML file;

//printf("RenderFarmServerThread::send_edl 1 %p %p %p %p\n",
//	server,	server->render,	server->render->command, server->render->command->get_edl());
	server->render->command->get_edl()->save_xml(mwindow->plugindb,
		&file, 
		0);
	file.terminate_string();
//printf("RenderFarmServerThread::send_edl\n%s\n\n", file.string);

	write_string(socket_fd, file.string);
//printf("RenderFarmServerThread::send_edl 2\n");
}


void RenderFarmServerThread::send_package(unsigned char *buffer)
{
	this->frames_per_second = (double)((((u_int32_t)buffer[0]) << 24) |
		(((u_int32_t)buffer[1]) << 16) |
		(((u_int32_t)buffer[2]) << 8)  |
		((u_int32_t)buffer[3])) / 
		65536.0;

//printf("RenderFarmServerThread::send_package 1 %f\n", frames_per_second);
	RenderPackage *package = 
		server->render->packages->get_package(frames_per_second, 
			server->calculate_avg_fps());
//printf("RenderFarmServerThread::send_package 2\n");

	char datagram[BCTEXTLEN];
// No more packages
	if(!package)
	{
		datagram[0] = datagram[1] = datagram[2] = datagram[3] = 0;
		write(socket_fd, datagram, 4);
	}
	else
// Encode package
	{
//printf("RenderFarmServerThread::send_package 3\n");
		int i = 4;
		strcpy(&datagram[i], package->path);
		i += strlen(package->path);
		datagram[i++] = 0;

		STORE_INT32(package->audio_start);
		STORE_INT32(package->audio_end);
		STORE_INT32(package->video_start);
		STORE_INT32(package->video_end);
		
		int len = i;
		i = 0;
		STORE_INT32(len - 4);
		
		write(socket_fd, datagram, len);
//printf("RenderFarmServerThread::send_package 4\n");
	}
//printf("RenderFarmServerThread::send_package 5\n");
}


void RenderFarmServerThread::set_progress(unsigned char *buffer)
{
	server->render->counter_lock->lock();
	server->render->total_rendered += (long)(((u_int32_t)buffer[0]) << 24) |
											(((u_int32_t)buffer[1]) << 16) |
											(((u_int32_t)buffer[2]) << 8)  |
											((u_int32_t)buffer[3]);
//printf("RenderFarmServerThread::set_progress %ld\n", server->render->total_rendered);
	server->render->counter_lock->unlock();

//	server->render->progress->update(server->render->total_rendered);
}


void RenderFarmServerThread::set_result(unsigned char *buffer)
{
//printf("RenderFarmServerThread::set_result %p\n", buffer);
	if(!server->render->result)
		server->render->result = buffer[0];
}


void RenderFarmServerThread::get_result()
{
	unsigned char data[1];
	data[0] = server->render->result;
	write(socket_fd, data, 1);
}
















// The render client waits for connections from the server.
// Then it starts a thread for each connection.
RenderFarmClient::RenderFarmClient(int port)
{
	this->port = port;
	thread = new RenderFarmClientThread(this);
	
	MWindow::init_defaults(boot_defaults);
	boot_preferences = new Preferences;
	boot_preferences->load_defaults(boot_defaults);
	MWindow::init_plugins(boot_preferences, plugindb);
}

	
	
	
RenderFarmClient::~RenderFarmClient()
{
	delete thread;
	delete boot_defaults;
	delete boot_preferences;
	plugindb->remove_all_objects();
	delete plugindb;
}

	
void RenderFarmClient::main_loop()
{
// Open listening port
	int socket_fd;
	struct sockaddr_in addr;

	if((socket_fd = socket(PF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("RenderFarmClient::main_loop: socket");
		return;
	};

	addr.sin_family = AF_INET;
	addr.sin_port = htons((unsigned short)port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if(bind(socket_fd, 
		(struct sockaddr*)&addr, 
		sizeof(addr)) < 0)
	{
		perror("RenderFarmClient::main_loop: bind");
		return;
	}
	
	while(1)
	{
		if(listen(socket_fd, 256) < 0)
    	{
    		perror("RenderFarmClient::main_loop: listen");
    		return;
    	}
		
		int new_socket_fd;
		struct sockaddr_in clientname;
		socklen_t size = sizeof(clientname);
    	if((new_socket_fd = accept(socket_fd,
                	(struct sockaddr*)&clientname, 
					&size)) < 0)
    	{
        	perror("RenderFarmClient::main_loop: accept");
        	return;
    	}
		else
		{
printf("RenderFarmClient::main_loop: Session started from %s\n", inet_ntoa(clientname.sin_addr));
			thread->main_loop(new_socket_fd);
		}
	}
}











FarmPackageRenderer::FarmPackageRenderer(RenderFarmClientThread *thread,
		int socket_fd)
 : PackageRenderer()
{
	this->thread = thread;
	this->socket_fd = socket_fd;
}



FarmPackageRenderer::~FarmPackageRenderer()
{
}


int FarmPackageRenderer::get_result()
{
	thread->send_request_header(socket_fd, 
		RENDERFARM_GET_RESULT, 
		0);
	unsigned char data[1];
	RenderFarmServerThread::read_socket(socket_fd, (char*)data, 1);
	return data[0];
}

void FarmPackageRenderer::set_result(int value)
{
	thread->send_request_header(socket_fd, 
		RENDERFARM_SET_RESULT, 
		1);
	unsigned char data[1];
	data[0] = value;
	write(socket_fd, data, 1);
}

void FarmPackageRenderer::set_progress(long value)
{
	thread->send_request_header(socket_fd, 
		RENDERFARM_PROGRESS, 
		4);
	unsigned char datagram[4];
	int i = 0;
	STORE_INT32(value);
	write(socket_fd, datagram, 4);
}






// The thread requests jobs from the server until the job table is empty
// or the server reports an error.  This thread must poll the server
// after every frame for the error status.
// Detaches when finished.
RenderFarmClientThread::RenderFarmClientThread(RenderFarmClient *client)
 : Thread()
{
	this->client = client;
	frames_per_second = 0;
	Thread::set_synchronous(0);
}

RenderFarmClientThread::~RenderFarmClientThread()
{
}

void RenderFarmClientThread::send_request_header(int socket_fd, 
	int request, 
	int len)
{
	unsigned char datagram[5];
	datagram[0] = request;

	int i = 1;
	STORE_INT32(len);
//printf("RenderFarmClientThread::send_request_header %02x%02x%02x%02x%02x\n",
//	datagram[0], datagram[1], datagram[2], datagram[3], datagram[4]);

	write(socket_fd, datagram, 5);
}

void RenderFarmClientThread::read_string(int socket_fd, char* &string)
{
	unsigned char header[4];
	RenderFarmServerThread::read_socket(socket_fd, (char*)header, 4);

	long len = (((u_int32_t)header[0]) << 24) | 
				(((u_int32_t)header[1]) << 16) | 
				(((u_int32_t)header[2]) << 8) | 
				((u_int32_t)header[3]);
//printf("RenderFarmClientThread::read_string %d %02x%02x%02x%02x\n",
//	len, header[0], header[1], header[2], header[3]);

	if(len)
	{
		string = new char[len];
		RenderFarmServerThread::read_socket(socket_fd, string, len);
	}
	else
		string = 0;

//printf("RenderFarmClientThread::read_string \n%s\n", string);
}


void RenderFarmClientThread::read_preferences(int socket_fd, 
	Preferences *preferences)
{
//printf("RenderFarmClientThread::read_preferences 1\n");
	send_request_header(socket_fd, 
		RENDERFARM_PREFERENCES, 
		0);

//printf("RenderFarmClientThread::read_preferences 2\n");
	char *string;
	read_string(socket_fd, string);
//printf("RenderFarmClientThread::read_preferences 3\n%s\n", string);

	Defaults defaults;
	defaults.load_string((char*)string);
	preferences->load_defaults(&defaults);
//printf("RenderFarmClientThread::read_preferences 4\n");

	delete [] string;
}



void RenderFarmClientThread::read_asset(int socket_fd, Asset *asset)
{
	send_request_header(socket_fd, 
		RENDERFARM_ASSET, 
		0);

	char *string;
	read_string(socket_fd, string);

	FileXML file;
	file.read_from_string((char*)string);
	asset->read(client->plugindb, &file);

	delete [] string;
}

void RenderFarmClientThread::read_edl(int socket_fd, 
	EDL *edl, 
	Preferences *preferences)
{
	send_request_header(socket_fd, 
		RENDERFARM_EDL, 
		0);

	char *string;
	read_string(socket_fd, string);

//printf("RenderFarmClientThread::read_edl \n%s\n", string);

	FileXML file;
	file.read_from_string((char*)string);
	delete [] string;








	edl->load_xml(client->plugindb,
		&file, 
		LOAD_ALL);



// Expand directories in EDL
	
	FileSystem fs;
	for(Asset *asset = edl->assets->first;
		asset;
		asset = asset->next)
	{
		char string2[BCTEXTLEN];
		strcpy(string2, asset->path);
		fs.join_names(asset->path, preferences->renderfarm_mountpoint, string2);
		
	}


//edl->dump();
}

int RenderFarmClientThread::read_package(int socket_fd, RenderPackage *package)
{
//printf("RenderFarmClientThread::read_package 1\n");
	send_request_header(socket_fd, 
		RENDERFARM_PACKAGE, 
		4);
//printf("RenderFarmClientThread::read_package 1\n");

	unsigned char datagram[5];
	int i = 0;
//printf("RenderFarmClientThread::read_package 1 %f\n", frames_per_second);


// Fails if -ieee isn't set.
	long fixed = !EQUIV(frames_per_second, 0.0) ? 
		(long)(frames_per_second * 65536.0) : 0;
//printf("RenderFarmClientThread::read_package 1\n");
	STORE_INT32(fixed);
//printf("RenderFarmClientThread::read_package 1\n");
	write(socket_fd, datagram, 4);

//printf("RenderFarmClientThread::read_package 1\n");

	char *data;
	unsigned char *data_ptr;
	read_string(socket_fd, data);
//printf("RenderFarmClientThread::read_package 2\n");

	if(!data) return 1;


//printf("RenderFarmClientThread::read_package 3\n");

	data_ptr = (unsigned char*)data;
	strcpy(package->path, data);
	data_ptr += strlen(package->path);
	data_ptr++;
	package->audio_start = (((uint32_t)data_ptr[0]) << 24) | 
							(((uint32_t)data_ptr[1]) << 16) | 
							(((uint32_t)data_ptr[2]) << 8) | 
							((uint32_t)data_ptr[3]);
	data_ptr += 4;
	package->audio_end = (((uint32_t)data_ptr[0]) << 24) | 
							(((uint32_t)data_ptr[1]) << 16) | 
							(((uint32_t)data_ptr[2]) << 8) | 
							((uint32_t)data_ptr[3]);
	data_ptr += 4;
	package->video_start = (((uint32_t)data_ptr[0]) << 24) | 
							(((uint32_t)data_ptr[1]) << 16) | 
							(((uint32_t)data_ptr[2]) << 8) | 
							((uint32_t)data_ptr[3]);
	data_ptr += 4;
	package->video_end = (((uint32_t)data_ptr[0]) << 24) | 
							(((uint32_t)data_ptr[1]) << 16) | 
							(((uint32_t)data_ptr[2]) << 8) | 
							((uint32_t)data_ptr[3]);
	data_ptr += 4;
//printf("RenderFarmClientThread::read_package 4 %p\n", data);

	delete [] data;

//printf("RenderFarmClientThread::read_package 5\n");
	return 0;
}

void RenderFarmClientThread::send_completion(int socket_fd)
{
	send_request_header(socket_fd, 
		RENDERFARM_DONE, 
		0);
}




void RenderFarmClientThread::main_loop(int socket_fd)
{
	this->socket_fd = socket_fd;
	Thread::start();
}

void RenderFarmClientThread::run()
{
	int socket_fd = this->socket_fd;
//printf("RenderFarmClientThread::run: 1\n");
	EDL *edl;
	RenderPackage *package;
	Asset *default_asset;
	Preferences *preferences;
	FarmPackageRenderer package_renderer(this, socket_fd);

//printf("RenderFarmClientThread::run 2\n");
// Read settings
	preferences = new Preferences;
	default_asset = new Asset;
	package = new RenderPackage;
	edl = new EDL;
	edl->create_objects();
//printf("RenderFarmClientThread::run 3\n");







	read_preferences(socket_fd, preferences);
//printf("RenderFarmClientThread::run 3\n");
	read_asset(socket_fd, default_asset);
//printf("RenderFarmClientThread::run 3\n");
	read_edl(socket_fd, edl, preferences);
//edl->dump();







//printf("RenderFarmClientThread::run 4\n");

	package_renderer.initialize(0,
			edl, 
			preferences, 
			default_asset,
			client->plugindb);
//printf("RenderFarmClientThread::run 5\n");

// Read packages
	while(1)
	{
		int result = read_package(socket_fd, package);
//printf("RenderFarmClientThread::run 6\n");


// Finished list
		if(result)
		{

//printf("RenderFarmClientThread::run 6.1\n");
			send_completion(socket_fd);
			break;
		}

		Timer timer;
		timer.update();

// Error
		if(package_renderer.render_package(package))
		{
			send_completion(socket_fd);
			break;
		}

		frames_per_second = (double)(package->video_end - package->video_start) / 
			((double)timer.get_difference() / 1000);

//printf("RenderFarmClientThread::run 8\n");



	}


//printf("RenderFarmClientThread::run 9\n");
	delete default_asset;
	delete edl;
	delete preferences;
//printf("RenderFarmClientThread::run: Session finished.\n");


}





