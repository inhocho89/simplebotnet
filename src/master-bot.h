#ifndef __SUPER_BOT_H__
#define __SUPER_BOT_H__

#include "bot.h"

#define N_SUPER 4

class Master : public Bot {
public:
	virtual void command (int argc, char **argv, int src_fd);
	void getcommand ();
	Master (int port): Bot (0, port), res_wait_(N_SUPER) { listen_(); }
protected:
	bool isMyChild (int id);
	void filterMyChild (char **argv, int n, vector<int> &vlist);
	map<int,int> reg_child_; // rigistered child
	int res_wait_;
};

#endif
