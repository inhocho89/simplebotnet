####################################
#          Simple Botnet           #
#        20154553 Inho Cho         #
#      <inho00@kaist.ac.kr>        #
#     IS539 Network Security       #
#    Programming Assignment #3     #
####################################

<Simple Botnet>
Simple Botnet is simple botnet whose bots are running in one machine. Their structure is hybrid style and there exist three types of bots - master, super, and child. There are 1 master bot, 4 super bots, and 32 child bots. Their archritcture is as below

                          master (0)
                              |
        -------------------------------------------------------
        |                  |                 |                |
        |                  |                 |                |
        |                  |                 |                |
     super (1)----------super (2)---------super (3)--------super (4)
      | | | |            | | | |           | | | |          | | | |
   child (11-18)      child (21-28)      child(31-38)     child(41-48)

The number in the parenthesis indicates the unique ID of each bots. Bots communicate via TCP channel. Master bot and Super bot has its listening port (50000 + id). For example, master has port number 50000, super(1) has port number 50001. Child bots also have its own port number, but they do not listen for other icnoming connections. 

<How to install>
1. Make shure you have installed latest g++ before you install Simple Botent. If you don't have g++, install with following command.

	# sudo apt-get install g++

2. Inside the src directory, compile the source code with make command.

	# cd src
	# make

3. Or simply you can run ./install.sh

	# sudo ./install.sh

Note that ./install requires sudo permission.

<How to run>
If you installed the Simple Botnet successfully, executables are created in the src directory.
To launch the botnet, just execute running script.

	# sudo ./run.sh

Note that it requires sudo permission for running script for full functionality of Simple Botnet.
If you do not execute Simple Botnet with sudo permission, some functions can be limited.

<Commands>
Belows are currently supported commands.

show
  This command shows current botnet architecture.

search [child_bot_ID1] [child_bot_ID2] ...
  This command reports the state of the specified bots with its super bot ID.

read [-date | -host] [child_bot_ID1] [child_bot_ID2] ...
  This command reads either system date or host name of the child bots.

create [filename] [child_bot_ID1] [child_bot_ID2] ...
  This command make child bots to create a file named filename with postfix of its bot ID.

send [#_packets] [host(:port)] [child_bot_ID1] [child_bot_ID2] ...
  This command make child bots to send specified number of packets to the host with its port number. If port number is not specified, random port number between 49152 and 65535 (not-registerable ports by IANA) will be designated.

kill [child_bot_ID1] [child_bot_ID2] ...
  This command kill the child bots with specified bot ID.

kill
  This command kill all bots including super bots and master bot itself.
 
