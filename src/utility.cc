#include "utility.h"
#include "bot.h"
#include <string.h>
#include <sstream>
#include <pthread.h>

namespace utility
{
	void *startTimer(void *bot){
		Bot *b = (Bot *)bot;
		sleep(2);
		b->expire();	
	}
	
	char *assembleCmd (int argc, char **argv){
		stringstream sstm;
		char *ret;
		int len;	

		for(int i=0; i<argc; ++i){
			if(i < argc-1)
				sstm << argv[i] << " ";	
			else
				sstm << argv[i];
		}

		len = sstm.str().length();
		ret = (char *)malloc(len+1);
		memcpy(ret, sstm.str().c_str(), len);
		ret[len] = '\0';
		return ret;
	}

	void parseCmd (char *cmd, int &argc, char** &argv){
		char *token;
		int n = 0;
		char **argv_;		

		token = strtok(cmd, " ");
		while (token){
			token = strtok(NULL, " ");
			n++;
		}

		argv_ = (char **)malloc(n*sizeof(char *));
		token = cmd;
		for(int i=0; i<n; ++i){
			argv_[i] = token;
			token += strlen(token) + 1;
		}

		argc = n;
		argv = argv_;
	}

	void *waitCmd (void *c_info){
		// When command arrives invoke bot's command function.
		int sockfd = ((conn_info *)c_info)->sfd;
		Bot *bot = ((conn_info *)c_info)->bot;
		int n;
		int l;
		char buffer[1024];
		int argc;
		char **argv;
		char *token;
		char *cmd;
		char *new_buffer;

		free(c_info);
		// wait for command.
		while (1){
			l = 0;
			n = read(sockfd, buffer, 1024);
			if (n<0)
				cerr << "Error reading from socket" << endl;
			else if (n==0){
				break;
			}

			buffer[n] = '\0';
			token = strtok(buffer, "\n");
			while(token){
				token = strtok(NULL, "\n");
				l++;
			}
			cmd = buffer;	
			for(int i=0; i<l; ++i){
				n = strlen(cmd);
				new_buffer = (char *)malloc(n+1);
				strcpy(new_buffer,cmd);
				parseCmd (new_buffer, argc, argv);
				pthread_mutex_lock(&bot->cmd_lock);
				bot->command(argc, argv, sockfd);
				pthread_mutex_unlock(&bot->cmd_lock);
				free(new_buffer);
				free(argv);
				cmd += n+1;
				argv = NULL;
			}
		}
		bot->rmChild(sockfd);
		close(sockfd);
	}

	void *waitInConn (void *c){
		int newsockfd, cli_addr_size;
		struct sockaddr_in cli_addr;
		int sockfd = ((conn_info *)c)->sfd;
		Bot *bot = ((conn_info *)c)->bot;

		free (c);
		cli_addr_size = sizeof(cli_addr);
		while (1){
			pthread_t thread;
			
			newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, (socklen_t *)&cli_addr_size);
			if (newsockfd < 0){
				cerr << "Couldn't accept incoming connection." << endl;
				exit (-1);
			}
	
			c = (conn_info *)malloc(sizeof(conn_info));
			((conn_info *)c)->bot = bot;
			((conn_info *)c)->sfd = newsockfd;
			pthread_create (&thread, NULL, &waitCmd, (void *)c);
			c = NULL;
		}
	}
}
