#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main (void){
	pid_t pid;

	pid = fork();
	if (pid > 0){ // parent process (foreground)
		char *argv[] = {"master-bot","50000",NULL};
		execv("./master-bot", argv);	
	}else{ // child process (foreground)
		pid_t pid2;
		char **argv;
		int i;

		argv = (char **)malloc(4*sizeof(char *));
		for(i=0; i<3; ++i){
			argv[i] = (char *)calloc(1,10);
		}
		argv[3] = (char *)NULL;
	
		i = 1;
		while (i<=48){
			usleep(100000);
			pid2 = fork();

			if(pid2 == 0){
				snprintf(argv[1],10,"%d",i);
				snprintf(argv[2],10,"%d",50000+i);
				if(i <= 4){
					strcpy(argv[0],"super-bot");
					execv("./super-bot", argv);
				}
				else{
					strcpy(argv[0],"child-bot");
					execv("./child-bot", argv);
				}
			}

			if(i == 4)
				i = 11;
			else if((i > 10) && (i%10) ==8)
				i += 3;
			else
				i++;	
		}	
	}

	return 0;
}
