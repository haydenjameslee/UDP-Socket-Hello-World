#include "Socket.h"

Socket::Socket()
{

}

Socket::~Socket()
{

}

bool Socket::Open( unsigned short port )
{
    this->handle = socket( AF_INET,
                     SOCK_DGRAM,
                     IPPROTO_UDP );

    if ( this->handle <= 0 )
    {
        printf( "failed to create socket\n" );
        return false;
    }

    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( (unsigned short) port );

    if ( bind( this->handle,
               (const sockaddr*) &address,
               sizeof(sockaddr_in) ) < 0 )
    {
        printf( "failed to bind socket\n" );
        return false;
    }
    printf( "Listening on port %d\n", port );


    #if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX

    int nonBlocking = 1;
    if ( fcntl( this->handle,
                F_SETFL,
                O_NONBLOCK,
                nonBlocking ) == -1 )
    {
        printf( "failed to set non-blocking\n" );
        return false;
    }

    #elif PLATFORM == PLATFORM_WINDOWS

    DWORD nonBlocking = 1;
    if ( ioctlsocket( handle,
                      FIONBIO,
                      &nonBlocking ) != 0 )
    {
        printf( "failed to set non-blocking\n" );
        return false;
    }

    #endif

    return true;
}

void Socket::Close()
{
    #if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
    close( this->handle );
    #elif PLATFORM == PLATFORM_WINDOWS
    closesocket( this->handle );
    #endif
}

bool Socket::IsOpen() const
{
    return this->handle > 0;
}

bool Socket::Send( const Address & destination,
               const void * packet_data,
               int packet_size )
{
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl( destination.GetAddress() );
    addr.sin_port = htons( destination.GetPort() );

    int sent_bytes =
        sendto( this->handle,
                (const char*)packet_data,
                packet_size,
                0,
                (sockaddr*)&addr,
                sizeof(sockaddr_in) );

    if ( sent_bytes != packet_size )
    {
        printf( "failed to send packet\n" );
        return false;
    }

    return true;
}


int Socket::Receive( Address & sender,
                     void * packet_data,
                     int max_packet_size )
{
    #if PLATFORM == PLATFORM_WINDOWS
    typedef int socklen_t;
    #endif

    sockaddr_in from;
    socklen_t fromLength = sizeof( from );

    int bytes = recvfrom( this->handle,
                          (char*)packet_data,
                          max_packet_size,
                          0,
                          (sockaddr*)&from,
                          &fromLength );

    unsigned int from_address =
        ntohl( from.sin_addr.s_addr );

    unsigned int from_port =
        ntohs( from.sin_port );

    sender = Address(from_address, from_port);

    return bytes;
}