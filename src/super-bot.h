#ifndef __SUPER_BOT_H__
#define __SUPER_BOT_H__

#define MPORT 50000	// master-bot's port
#define CHILD_PER_SUPER 8

#include "bot.h"

static int SUPER_PORTS[] = {50001, 50002, 50003, 50004};

class Super : public Bot {
public:
	virtual void command(int argc, char **argv, int src_fd);
	Super(int id, int port);
protected:
	vector<int> neighbor_;
	void connect_neighbor_();	// connect to the neighbor.
};

#endif
