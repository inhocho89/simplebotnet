all: master-bot super-bot child-bot run
	
master-bot: bot.o master-bot.o utility.o
	g++ bot.o master-bot.o utility.o -o master-bot -lpthread

super-bot: bot.o super-bot.o utility.o
	g++ bot.o super-bot.o utility.o -o super-bot -lpthread

child-bot: bot.o child-bot.o utility.o
	g++ bot.o child-bot.o utility.o -o child-bot -lpthread

run: run.cc
	g++ run.cc -o run

bot.o: bot.cc bot.h
	g++ -c bot.cc -lpthread

master-bot.o: master-bot.cc master-bot.h
	g++ -c master-bot.cc -lpthread

super-bot.o: super-bot.cc super-bot.h
	g++ -c super-bot.cc -lpthread

child-bot.o: child-bot.cc child-bot.h
	g++ -c child-bot.cc -lpthread

utility.o: utility.cc utility.h
	g++ -c utility.cc

clean:
	rm *.o super-bot master-bot child-bot run
