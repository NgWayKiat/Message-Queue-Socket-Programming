#include "ClientSocket.h"

using namespace std;			

//SOCKET sock;

struct ClientServer {
    string name;
    string ip;
    int port;
    int status;  // 0:DISCONNECTED 1:CONNECTED
    SOCKET sok;  // Keep socket value
};

map<string, ClientServer> ClientServerMapList;

//vector<unique_ptr<SOCKET>> sockets;

void InitClientSetting()
{
    logger logger;
    char buf[DEFAULT_BUFLEN];

    memset(buf, 0, sizeof(buf));
    sprintf(buf, "Start the Initiate Client Connection Setting");
    logger.writeToLog(INFO, buf);

    ClientServerMapList["TSRV1"] = { "TSRV1", "127.0.0.1", 6000, 0, INVALID_SOCKET };
    ClientServerMapList["TSRV2"] = { "TSRV2", "127.0.0.1", 6500, 0, INVALID_SOCKET };

    memset(buf, 0, sizeof(buf));
    sprintf(buf, "The Client Connection Mapping List:");
    logger.writeToLog(INFO, buf);

    for (const auto& key : ClientServerMapList) {
        memset(buf, 0, sizeof(buf));
        sprintf(buf, "Map Key [%s], Value Name [%s], Ip [%s], Port [%d]", (key.first).c_str(), (key.second.name).c_str(), (key.second.ip).c_str(), key.second.port);
        logger.writeToLog(INFO, buf);
    }
}

void InitStartUpClientConnection(int& retVal)
{
    sockaddr_in client_addr;
    SOCKET client_sock;
    WSADATA data;
    logger logger;

    int client_port;
    int ret;
    char* client_ip =  new char[20];
    char buf[DEFAULT_BUFLEN];
    char status[20];

    InitClientSetting();

    for (const auto& key : ClientServerMapList) {

        memset(client_ip, 0, sizeof(client_ip));
        sprintf(client_ip, (key.second.ip).c_str());
        client_port = key.second.port;

        client_addr.sin_family = AF_INET;
        client_addr.sin_port = htons(client_port);
        client_addr.sin_addr.s_addr = inet_addr(client_ip);

        ret = WSAStartup(0x101, &data);

        if (ret != 0)
        {
            memset(buf, 0, sizeof(buf));
            sprintf(buf, "WSAStartup failed with error: %d", ret);
            logger.writeToLog(ERRORS, buf);
            retVal = 1;
            return;
        }

        client_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

        if (client_sock == INVALID_SOCKET)
        {
            memset(buf, 0, sizeof(buf));
            sprintf(buf, "invalid socket with error: %d", WSAGetLastError());
            logger.writeToLog(ERRORS, buf);
            retVal = 1;
            return;
        }
        else if (client_sock == SOCKET_ERROR)
        {
            memset(buf, 0, sizeof(buf));
            sprintf(buf, "socket error with error: %d", WSAGetLastError());
            logger.writeToLog(ERRORS, buf);
            retVal = 1;
            return;
        }

        ret = connect(client_sock, (struct sockaddr*)&client_addr, sizeof(client_addr));

        if (ret != 0)
        {
            memset(buf, 0, sizeof(buf));
            sprintf(buf, "socket connect with error: %d", WSAGetLastError());
            logger.writeToLog(ERRORS, buf);
            retVal = 1;
            continue;
        }

        ClientServerMapList[key.first].status = 1;
        ClientServerMapList[key.first].sok = client_sock;
        //sockets.push_back(make_unique<SOCKET>(client_sock));
    }

    memset(buf, 0, sizeof(buf));
    sprintf(buf, "List of Client Server Connection:");
    logger.writeToLog(INFO, buf);

    for (const auto& key : ClientServerMapList) {

        memset(client_ip, 0, sizeof(client_ip));
        sprintf(client_ip, (key.second.ip).c_str());
        client_port = key.second.port;

        if (key.second.status == 0)
        {
            memset(status, 0, sizeof(status));
            sprintf(status, "DISCONNECTED");
        }
        else
        {
            memset(status, 0, sizeof(status));
            sprintf(status, "CONNECTED");
        }

        memset(buf, 0, sizeof(buf));
        sprintf(buf, "Name: [%s] IP: [%s] Port: [%d] Status: [%s]", (key.first).c_str(), client_ip, client_port, status);
        logger.writeToLog(INFO, buf);
    }
}

int SendClientMessage(string serverKey, string message)
{
    SOCKET socketValue;
    logger logger;
    bool isConnected = false;
    int client_port_map;
    int ret;
    char* client_ip_map = new char[20];
    char sendData[DEFAULT_BUFLEN];
    char buf[DEFAULT_BUFLEN];
    char recData[DEFAULT_BUFLEN];

    auto findKey = ClientServerMapList.find(serverKey);

    if (findKey != ClientServerMapList.end())
    {
        if (findKey->second.status == 0)
        {
            memset(buf, 0, sizeof(buf));
            sprintf(buf, "Failed to Send Client Message. Client Socket Disconnected [1].");
            logger.writeToLog(INFO, buf);
            return 1;
        }

        memset(client_ip_map, 0, sizeof(client_ip_map));
        sprintf(client_ip_map, (findKey->second.ip).c_str());
        client_port_map = findKey->second.port;

        socketValue = findKey->second.sok;

        if (IsClientServerAlive(serverKey))
        {
            memset(sendData, 0, sizeof(sendData));
            sprintf(sendData, message.c_str());
            ret = send(socketValue, sendData, sizeof(sendData), 0);

            if (ret == 0)
            {
                memset(buf, 0, sizeof(buf));
                sprintf(buf, "Send to client Failed: %s", sendData);
                logger.writeToLog(INFO, buf);

                return 1;
            }

            memset(buf, 0, sizeof(buf));
            sprintf(buf, "Send to client: %s", sendData);
            logger.writeToLog(INFO, buf);

            while (true)
            {
                memset(recData, 0, sizeof(recData));
                ret = recv(socketValue, recData, sizeof(recData), 0);

                if (ret > 0)
                {
                    memset(buf, 0, sizeof(buf));
                    sprintf(buf, "Receive from client: %s", recData);
                    logger.writeToLog(INFO, buf);
                    break;
                }
            }
        }
        else
        {
            memset(buf, 0, sizeof(buf));
            sprintf(buf, "Failed to Send Client Message. Client Socket Disconnected [2].");
            logger.writeToLog(INFO, buf);
            return 1;
        }
    }
    else
    {
        memset(buf, 0, sizeof(buf));
        sprintf(buf, "Failed to Send Client Message. Client Socket Could Not Found in Socket Map.");
        logger.writeToLog(INFO, buf);
        return 1;
    }

    return 0;
}

bool IsClientServerAlive(string serverKey)
{
    int ret;
    char buf[DEFAULT_BUFLEN];
    SOCKET sok = ClientServerMapList[serverKey].sok;

    logger logger;

    if (sok == INVALID_SOCKET)
    {
        return false;
    }

    ret = send(sok, nullptr, 0, 0);

    if (ret == 0 || (ret == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))) {
        return true;
    }
    else if (ret == -1 && errno == EPIPE) {
        ClientServerMapList[serverKey].sok = INVALID_SOCKET;
        ClientServerMapList[serverKey].status = 0;
        closesocket(sok);
        return false;
    }
    else
    {
        ClientServerMapList[serverKey].sok = INVALID_SOCKET;
        ClientServerMapList[serverKey].status = 0;
        closesocket(sok);
        return false;
    }

    return false;
}