#include "auth_server.hpp"
#include <boost/program_options.hpp>
#include <iostream>
#include <fstream>


int main( int argc, char **argv )
{
    namespace po = boost::program_options;

    std::string bind_address;
    uint16_t    bind_port;
    std::string zone_filename;
    std::string apex;

    po::options_description desc( "unbound" );
    desc.add_options()( "help,h", "print this message" )

        ( "bind,b", po::value<std::string>( &bind_address )->default_value( "0.0.0.0" ), "bind address" )
        ( "port,p", po::value<uint16_t>( &bind_port )->default_value( 53 ), "bind port" )
	( "file,f", po::value<std::string>( &zone_filename ),           "zone filename" )
	( "zone,z", po::value<std::string>( &apex),                     "zone apex" );
    
    po::variables_map vm;
    po::store( po::parse_command_line( argc, argv, desc ), vm );
    po::notify( vm );

    if ( vm.count( "help" ) ) {
        std::cerr << desc << "\n";
        return 1;
    }

    try {
	dns::DNSServerParameters params;
	params.mBindAddress = bind_address;
	params.mBindPort    = bind_port;
	dns::AuthServer server( params );
	server.load( apex, zone_filename );
	server.start();
    }
    catch ( std::runtime_error &e ) {
	std::cerr << e.what() << std::endl;
    }
    catch ( std::logic_error &e ) {
	std::cerr << e.what() << std::endl;
    }
    return 0;
}
