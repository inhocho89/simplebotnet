#ifndef __CHILD_BOT_H__
#define __CHILD_BOT_H__

#define SPORT(id_) (50000+(id_/10)%10)

#include "bot.h"

class Child : public Bot {
public:
	virtual void command(int argc, char **argv, int src_fd);
	Child(int id, int port);
	~Child();
protected:
	int sendpacket(char *ip, int port);
};

#endif
