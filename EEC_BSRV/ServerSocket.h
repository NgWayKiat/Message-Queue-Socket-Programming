#ifndef SRVSOK_HDR_FILE
#define SRVSOK_HDR_FILE

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <winsock.h>
#include <string>
#include "Logger.h"
#include <thread>
#include "ClientSocket.h"

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT 5000

DWORD WINAPI receive_cmds(LPVOID lpParam);
void serverSok(int& retVal);

#endif