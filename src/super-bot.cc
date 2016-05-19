#include "super-bot.h"
#include <algorithm>

Super::Super(int id, int port): Bot(id, port) {
	pthread_t thread;
	conn_info *c;
	stringstream sstm;

	listen_();
	parent_ = connect_ (MPORT);

	c = (conn_info *)malloc(sizeof(conn_info));
	c->bot = this;
	c->sfd = parent_;
	pthread_create(&thread, NULL, &utility::waitCmd, (void *)c);	

	sstm << "report " << id_ << endl;
	sendmsg(parent_, sstm.str().c_str());

	connect_neighbor_();
}

void Super::command (int argc, char **argv, int src_fd){	
	if (argc == 1){
		if(strcmp(argv[0],"addneighbor") == 0){
			neighbor_.push_back(src_fd);	
			return;
		}else if (strcmp(argv[0],"kill") == 0){
			broadcast("kill");
			alive_ = false;
			return;
		}else if(strcmp(argv[0],"show") == 0){
			stringstream sstm;
			sstm << "res-show " << id_;
			for(map<int,int>::iterator it = child_.begin(); it != child_.end(); it++){
				sstm << " " << it->first;
			}
			sendmsg(src_fd, sstm.str().c_str());
			return;
		}
	}

	if (argc == 2){
		if(strcmp(argv[0], "report") == 0){
			child_[atoi(argv[1])] = src_fd;
			if(child_.size() == CHILD_PER_SUPER){
				stringstream sstm;
				sstm << "report " << id_;
				for(map<int,int>::iterator it = child_.begin(); it != child_.end(); it++){
					sstm << " " << it->first;
				}
				sstm << endl;
				sendmsg(parent_, sstm.str().c_str());	
			}
			return;
		}
	}

	if (argc > 1){
		if (strcmp(argv[0],"search") == 0){
			int nchild = argc-1;

			for(int i=0; i<nchild; ++i){
				int cid = atoi(argv[1+i]);
				map<int,int>::iterator it = child_.find(cid);
				if(it != child_.end()){
					stringstream sstm;
			
					sstm << "res-search " << id_ << " " << cid << " alive" << endl;
					sendmsg(parent_,sstm.str().c_str());					
				}else if(find(dead_child_.begin(), dead_child_.end(), cid) != dead_child_.end()){
					stringstream sstm;

					sstm << "res-search " << id_ << " " << cid << " dead" << endl;
					sendmsg(parent_, sstm.str().c_str());		
				}
			}
			return;
		}else if(strcmp(argv[0],"kill") == 0){
			int nchild = argc-1;

			for(int i=0; i<nchild; ++i){
				int cid = atoi(argv[1+i]);
				map<int,int>::iterator it = child_.find(cid);
				if(it != child_.end()){
					sendmsg(it->second,"kill\n");
				}
			}
			return;
		}
	}

	if (argc > 2){
		if (strncmp(argv[0], "res-", 4) == 0){
			stringstream sstm;
			for(int i=0; i<argc-1; ++i){
				sstm << argv[i] << " ";
			}
			sstm << argv[argc-1] << endl;
			sendmsg(parent_, sstm.str().c_str());
			return;
		}else if(strcmp(argv[0], "read") == 0){
			if(strcmp(argv[1], "-date") == 0){
				int nchild = argc - 2;

				for(int i=0; i<nchild; ++i){
					int cid = atoi(argv[2+i]);
					map<int,int>::iterator it = child_.find(cid);
					if (it != child_.end())
						sendmsg(it->second,"read-date\n");
					else if (find(dead_child_.begin(), dead_child_.end(), cid) != dead_child_.end()){
						stringstream sstm;
						
						sstm << "res-rh " << cid << " dead" << endl;
						sendmsg(parent_, sstm.str().c_str());
					}
				}
				return;
			}else if(strcmp(argv[1], "-host") == 0){
				int nchild = argc -2;				

				for(int i=0; i<nchild; ++i){
					int cid = atoi(argv[2+i]);
					map<int,int>::iterator it = child_.find(cid);
					if (it != child_.end())
						sendmsg(it->second, "read-host\n");
					else if (find(dead_child_.begin(), dead_child_.end(), cid) != dead_child_.end()){
						stringstream sstm;

						sstm << "res-rh " << cid << " dead" << endl;
						sendmsg(parent_, sstm.str().c_str());
					}
				}
				return;
			}
		}else if(strcmp(argv[0], "create") == 0){
			int nchild = argc-2;
			
			for(int i=0; i<nchild; ++i){
				int cid = atoi(argv[2+i]);
				map<int,int>::iterator it = child_.find(cid);
				if (it != child_.end()){
					stringstream sstm;

					sstm << argv[0] << " " << argv[1] << endl;
					sendmsg(it->second, sstm.str().c_str());
				}else if(find(dead_child_.begin(), dead_child_.end(), cid) != dead_child_.end()){
					stringstream sstm;

					sstm << "res-create " << cid << " . dead" << endl;
					sendmsg(parent_, sstm.str().c_str());
				}
			}
			return;
		}
	}

	if (argc > 3){
		if(strcmp(argv[0],"send") == 0){
			int nchild = argc-3;

			for(int i=0; i<nchild; ++i){
				int cid = atoi(argv[3+i]);
				map<int,int>::iterator it = child_.find(cid);
				if (it != child_.end()){
					stringstream sstm;

					sstm << argv[0] << " " << argv[1] << " " << argv[2] << endl;
					sendmsg(it->second, sstm.str().c_str());
				}else if(find(dead_child_.begin(), dead_child_.end(), cid) != dead_child_.end()){
					stringstream sstm;

					sstm << "res-send " << cid << " dead" << endl;
					sendmsg(parent_, sstm.str().c_str());
				}
			}
		}
	}
	
	return;
}

void Super::connect_neighbor_() {
	int nport;	// neighbor port.
	int nfd;		// neighbor socket fd
	pthread_t thread;
	conn_info *c;

	if (id_ < 2) return;
	nport	= SUPER_PORTS[id_-2];
	nfd = connect_ (nport);

	// register neighbor
	neighbor_.push_back(nfd);	

	// wait command for neighbor	
	c = (conn_info *)malloc(sizeof(conn_info));
	c->bot = this;
	c->sfd = nfd;
	pthread_create(&thread, NULL, &utility::waitCmd, (void *)c);
	// request neighbor to register this bot as neighbor
	sendmsg(nfd, "addneighbor\n");
}

int main(int argc, char *argv[]) {
	Super *super = new Super(atoi(argv[1]),atoi(argv[2]));
	
	while(super->isAlive()){ }	
	return 0;
}
