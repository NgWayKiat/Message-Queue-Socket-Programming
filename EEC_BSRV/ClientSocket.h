#ifndef CSOK_HDR_FILE
#define CSOK_HDR_FILE

#include <chrono>
#include <iostream>
#include <cstdio>
#include <vector>
#include <windows.h>
#include <winsock.h>
#include <stdio.h>
#include <conio.h>
#include <signal.h>
#include <map>
#include <tuple>
#include "Logger.h"

#define DEFAULT_BUFLEN 512

void InitClientSetting();

void InitStartUpClientConnection(int& retVal);

int SendClientMessage(string serverKey, string message);

bool IsClientServerAlive(string serverKey);

#endif
