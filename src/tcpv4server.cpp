#include "tcpv4server.hpp"
#include <unistd.h>
#include <sys/socket.h>
#include <cstring>

namespace tcpv4
{

    Connection::~Connection()
    {
	close( tcp_socket );
    }

    void Connection::shutdownReceive()
    {
	shutdown( tcp_socket, SHUT_RD );
    }

    void Connection::shutdownSend()
    {
	shutdown( tcp_socket, SHUT_WR );
    }

    PacketData Connection::receive( int size )
    {
	PacketData recv_buffer;
	recv_buffer.resize( size );

    retry:
	int recv_size = read( tcp_socket, &recv_buffer[0], size );
	if ( recv_size < 0 ) {
	    if ( errno == EINTR || errno == EAGAIN )
		goto retry;
	    else {
		throw SocketError( get_error_message( "cannot read data from peer", errno ) );
	    }
	}

	return recv_buffer;
    }

    ssize_t Connection::send( const PacketData &data )
    {
	return send( &data[0], data.size() );
    }


    ssize_t Connection::send( const uint8_t *begin, const uint8_t *end )
    {
	return send( begin, end - begin );
    }

    ssize_t Connection::send( const uint8_t *data, int size )
    {
    retry:
	int sent_size = write( tcp_socket, data, size );
	if ( sent_size < 0 ) {
	    if ( errno == EINTR || errno == EAGAIN )
		goto retry;
	    else {
		throw SocketError( get_error_message( "cannot write data to peer", errno ) );
	    }
	}

	return sent_size;
    }


    Server::Server( const ServerParameters &parameters )
    {
	tcp_socket = socket( AF_INET, SOCK_STREAM, 0 );
	if ( tcp_socket < 0 ) {
	    SocketError( get_error_message( "cannot create socket", errno ) );
	}

	sockaddr_in socket_address;
        std::memset( &socket_address, 0, sizeof(socket_address) );
        socket_address.sin_family = AF_INET;
        socket_address.sin_addr   = convert_address_string_to_binary( parameters.bind_address );
        socket_address.sin_port   = htons( parameters.bind_port );
        if ( bind( tcp_socket, reinterpret_cast<const sockaddr *>( &socket_address ), sizeof(socket_address) ) < 0 ) {
            close( tcp_socket );
	    tcp_socket = -1;
            throw SocketError( get_error_message( "cannot bind to " + parameters.bind_address, errno ) );
        }

        if ( listen( tcp_socket, 10 ) < 0 ) {
            close( tcp_socket );
	    tcp_socket = -1;
            throw SocketError( get_error_message( "cannot listen", errno ) );
        }
    }

    Server::~Server()
    {
	close( tcp_socket );
    }

    ConnectionPtr Server::acceptConnection()
    {
	sockaddr_in socket_address;
	socklen_t   socket_address_size;

    retry:
	int new_connection = accept( tcp_socket, reinterpret_cast<sockaddr *>( &socket_address ), &socket_address_size );
	if ( new_connection < 0 ) {
	    if ( errno == EAGAIN || errno == EINTR )
		goto retry;
            throw SocketError( get_error_message( "cannot accept", errno ) );
	}
	return ConnectionPtr( new Connection( new_connection ) );
    }

}