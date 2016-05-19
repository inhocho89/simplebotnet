namespace utility
{
	void *startTimer (void *bot);
	char *assemCmd (int argc, char **argv);
	void parseCmd (char *cmd, int &argc, char** &argv);
	void *waitCmd (void *c_info);
	void *waitInConn (void *c);	
}
