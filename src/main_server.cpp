#include <iostream>
#include "base_structures.h"
#include "asio.hpp"
#include "config.h"
#include "server.hpp"
int main(int argc, char* argv[]){
    try{
        if(argc != 2){
            std::cout << "Usage: " << argv[0] << " <port>" << std::endl;
            return 1;
        }
        asio::io_service ios;
        server s(ios, std::atoi(argv[1]));
        ios.run();
    }
    catch( std::exception& e){
        std::cerr << e.what() << std::endl;
    }
    logic::get_instance().write_desks();
    return 0;
}
