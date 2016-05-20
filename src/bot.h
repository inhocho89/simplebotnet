#ifndef __BOT_H__
#define __BOT_H__

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <strings.h>
#include <string.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include "utility.h"

using namespace std;

class Bot;
typedef struct {
	Bot *bot;	// bot
	int sfd;		// socket fd
} conn_info;

class Bot {
public:
	pthread_mutex_t cmd_lock;
	virtual void command (int, char **, int) = 0;
	string botName();
	int id () { return id_; }
	bool isAlive() { return alive_; }
	virtual void expire() { }
	void rmChild (int sfd);
	Bot (int id, int port) : id_(id), port_(port), parent_(0), alive_(true) 
									{ pthread_mutex_init(&cmd_lock,NULL); }
protected:
	bool alive_;		// whether this bot is alive or not.
	int id_;				// bot ID
	int port_;			// port number: this number is used for inter-bot connection	
	int parent_;		// parent socket file descriptor
	
	map<int, int> child_;
	vector<int> dead_child_;

	void listen_ ();	// bind the listening socket.
	int connect_ (int port);	// make TCP channel to the port
	void sendmsg (int fd, const char *msg);
	void broadcast (const char *msg);
};

#endif
