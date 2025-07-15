// EEC_BSRV.cpp : Defines the entry point for the application.
//

#include "EEC_BSRV.h"

using namespace std;

int main()
{
	int retServerThread, retClientThread = 0;
	char buf[DEFAULT_BUFLEN];
	logger logger;
	
	thread server_thread(serverSok, ref(retServerThread));
	//The thread Wait for One Second
	this_thread::sleep_for(1000ms);
	thread client_thread(InitStartUpClientConnection, ref(retClientThread));

	server_thread.join();
	client_thread.join();

	if (retServerThread != 0)
	{
		memset(buf, 0, sizeof(buf));
		sprintf(buf, "Server Socket Error: %d", retServerThread);
		logger.writeToLog(ERRORS, buf);
	}

	if (retClientThread != 0)
	{
		memset(buf, 0, sizeof(buf));
		sprintf(buf, "Client Socket Error: %d", retClientThread);
		logger.writeToLog(ERRORS, buf);
	}

	return 0;
}
