#include <stdio.h>
#include <string>
#include "Socket.h"
#include "Address.h"

int port = 30000;
int sendToPort = 30001;
bool sending = false;
const char data[] = "hello world!";

Socket sock;


bool InitializeSockets()
{
    #if PLATFORM == PLATFORM_WINDOWS
    WSADATA WsaData;
    return WSAStartup( MAKEWORD(2,2),
                       &WsaData )
        == NO_ERROR;
    #else
    return true;
    #endif
}

void ShutdownSockets()
{
    #if PLATFORM == PLATFORM_WINDOWS
    WSACleanup();
    #endif
}


void CreateSocket()
{
    if ( !sock.Open( port ) )
    {
        printf( "failed to open socket!\n" );
    }
}

void SendPacket(const char data[], const int len)
{
    sock.Send( Address(127,0,0,1,sendToPort), data, len );
}

void ReceivePackets()
{
    while ( true )
    {
        Address sender;
        unsigned char buffer[256];
        int bytes_read =
            sock.Receive( sender,
                            buffer,
                            sizeof( buffer ) );
        if ( bytes_read <= 0)
            continue;

        // process packet
        const char* packet_data = (const char*)buffer;
        printf("%d: %s\n", bytes_read, packet_data);
    }
}

bool ParseCmdLine(int argc, char** argv)
{
    for (int i = 0; i < argc; i++)
    {
        std::string argStr = std::string(argv[i]);
        if (argStr == "--port" || argStr == "-p")
        {
            std::string portStr = std::string(argv[i + 1]);
            port = std::stoi(portStr);
        }
        else if (argStr == "--sendto" || argStr == "-st")
        {
            std::string portStr = std::string(argv[i + 1]);
            sendToPort = std::stoi(portStr);
        }
        else if (argStr == "--send" || argStr == "-s")
        {
            sending = true;
        }
    }
    return true;
}

int main(int argc, char** argv)
{
    if (!ParseCmdLine(argc, argv))
    {
        return 1;
    }

    InitializeSockets();
    CreateSocket();

    if (sending)
    {
        SendPacket(data, sizeof(data));
        printf( "Sent data: %s\n", data );
    }
    else
    {
        ReceivePackets();
    }

    ShutdownSockets();
    return 0;
}