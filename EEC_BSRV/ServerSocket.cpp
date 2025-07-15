#include "ServerSocket.h"

using namespace std;
//CRITICAL_SECTION cs;

DWORD WINAPI receive_cmds(LPVOID lpParam)
{
    int ret;
    char buf[DEFAULT_BUFLEN];
    char recData[DEFAULT_BUFLEN];
    char sendData[DEFAULT_BUFLEN];
    int client_port;
    char* client_ip = new char[20];

    logger logger;

    // set socket to the socket passed in as a parameter   
    SOCKET current_client = (SOCKET)lpParam;

    sockaddr_in SockAddr;
    int addrlen = sizeof(SockAddr);

    // infinite receive loop
    while (true)
    {
        
        memset(recData, 0, sizeof(recData));
        ret = recv(current_client, recData, sizeof(recData), 0); // recv cmds

        if (ret > 0) {
            memset(buf, 0, sizeof(buf));
            sprintf(buf, "Message Received: %s", recData);
            logger.writeToLog(INFO, buf);

            //EnterCriticalSection(&cs);
            memset(sendData, 0, sizeof(sendData));
            memcpy(sendData, recData, sizeof(recData));
            ret = send(current_client, sendData, ret, 0);
            //LeaveCriticalSection(&cs);

            memset(buf, 0, sizeof(buf));
            sprintf(buf, "Message Sent: %s", sendData);
            logger.writeToLog(INFO, buf);

            SendClientMessage("TSRV1", "send test message to client connection");
        }
        else
        {
            // get foreign protocol to indicate which network is disconnect
            getpeername(current_client, (LPSOCKADDR)&SockAddr, &addrlen);
            strcpy(client_ip, inet_ntoa(SockAddr.sin_addr));
            client_port = ntohs(SockAddr.sin_port);
            
            closesocket(current_client);
            memset(buf, 0, sizeof(buf));
            sprintf(buf, "Client Disconnected. IP [%s] PORT [%d]", client_ip, client_port);
            logger.writeToLog(INFO, buf);

            ExitThread(0);
        }
    }
}

void serverSok(int& retVal)
{
    int fromlen;
    int ret;
    DWORD thread;
    WSADATA wsaData;
    sockaddr_in server;
    sockaddr_in from;
    SOCKET client;
    SOCKET sock;
    logger logger;

    int client_port;
    char* client_ip = new char[20];
    char buf[DEFAULT_BUFLEN];

    memset(buf, 0, sizeof(buf));
    sprintf(buf, "Start server socket function...");
    logger.writeToLog(INFO, buf);

    // start winsock
    ret = WSAStartup(0x101, &wsaData); // use highest version of winsock avalible

    if (ret != 0)
    {
        memset(buf, 0, sizeof(buf));
        sprintf(buf, "WSAStartup failed with error: %d", ret);
        logger.writeToLog(ERRORS, buf);
        retVal = 1;
        return;
    }

    // fill in winsock struct ... 
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(DEFAULT_PORT); // listen on telnet port 5000

    // create server socket
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (sock == INVALID_SOCKET)
    {
        memset(buf, 0, sizeof(buf));
        sprintf(buf, "invalid socket with error: %d", WSAGetLastError());
        logger.writeToLog(ERRORS, buf);
        retVal = 1;
        return;
    }
    else if (sock == SOCKET_ERROR)
    {
        memset(buf, 0, sizeof(buf));
        sprintf(buf, "socket error with error: %d", WSAGetLastError());
        logger.writeToLog(ERRORS, buf);
        retVal = 1;
        return;
    }

    // bind our socket to a port(port 123) 
    if (bind(sock, (sockaddr*)&server, sizeof(server)) != 0)
    {
        memset(buf, 0, sizeof(buf));
        sprintf(buf, "bind failed with error: %d", WSAGetLastError());
        logger.writeToLog(ERRORS, buf);
        retVal = 1;
        return;
    }

    // listen for a connection with allow 10 connection
    // SOMAXCONN
    if (listen(sock, 10) != 0)
    {
        memset(buf, 0, sizeof(buf));
        sprintf(buf, "listen failed with error: %d", WSAGetLastError());
        logger.writeToLog(ERRORS, buf);
        retVal = 1;
        return;
    }

    // infinite loop waiting new client connection
    while (true)
    {
        // accept connections
        fromlen = sizeof(from);
        client = accept(sock, (struct sockaddr*)&from, &fromlen);

        if (client == INVALID_SOCKET) {
            memset(buf, 0, sizeof(buf));
            sprintf(buf, "accept failed with error: %d", WSAGetLastError());
            logger.writeToLog(ERRORS, buf);
            continue;
        }

        strcpy(client_ip, inet_ntoa(from.sin_addr));
        client_port = ntohs(from.sin_port);

        memset(buf, 0, sizeof(buf));
        sprintf(buf, "Client Connected. IP [%s] PORT [%d]", client_ip, client_port);
        logger.writeToLog(INFO, buf);

        // create our recv_cmds thread and parse client socket as a parameter
        CreateThread(NULL, 0, receive_cmds, (LPVOID)client, 0, &thread);
    }

    // shutdown winsock
    closesocket(sock);
    WSACleanup();
}