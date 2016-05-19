#include "bot.h"

void Bot::listen_ (){
	int sockfd, newsockfd, cli_addr_size;
	struct sockaddr_in serv_addr;
	pthread_t thread;
	conn_info *c;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0){
		cerr << "Error: Couldn't open socket." << endl;
		exit (-1);
	}

	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port_);
	serv_addr.sin_addr.s_addr = INADDR_ANY;

	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
		cerr << "[" << botName() << "]Error: Couldn't bind to socket." << endl;
		exit (-1);
	}

	listen(sockfd, 5);
	
	c = (conn_info *)malloc(sizeof(conn_info));
	c->bot = this;
	c->sfd = sockfd;

	pthread_create (&thread, NULL, &utility::waitInConn, (void *)c);	
}

string Bot::botName(){
	stringstream sstm;
	if (id_ == 0)
		sstm << "[master]";
	else
		sstm << "[bot " << id_ << "]";

	return sstm.str();
}

int Bot::connect_ (int port){
	int sockfd, n;
	struct sockaddr_in serv_addr;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0){
		cerr << "Couldn't open the socket." << endl;
		exit (-1);
	}	

	memset((char *)&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	serv_addr.sin_port = htons(port);

	if (connect(sockfd, (sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
		cerr << "Couldn't connecto to the server. (port =" << port << ")" << endl;
		exit (-1);
	}

	return sockfd;
}

void Bot::sendmsg (int fd, const char *msg){
	write(fd, msg, strlen(msg));
}

void Bot::broadcast (const char *msg){
	for(map<int,int>::iterator it = child_.begin(); it != child_.end(); it++){
		sendmsg(it->second, msg);
	}
}

void Bot::rmChild (int sfd){
	for(map<int,int>::iterator it = child_.begin(); it != child_.end(); it++){
		if(it->second == sfd){
			dead_child_.push_back(it->first);
			child_.erase(it);
			return;
		}
	}
}
