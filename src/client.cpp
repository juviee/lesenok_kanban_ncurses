#include <iostream>
#include "asio.hpp"
#include "client_connection.hpp"

void chit_chat(client_connection& cl){
    while(true){
        std::string msg;
        std::cout << "You: " << std::endl;
        std::getline(std::cin, msg);
        cl.send_message(msg);
        std::string reply = cl.read_reply();
        std::cout << "Reply: " << std::endl;
        std::cout << reply << std::endl;
    }
}



int main(int argc, char* argv[]){
    if(argc != 3){
        return 1;
    }
    asio::io_service ios;
    client_connection cl(ios, argv[1], argv[2]);
    chit_chat(cl);
    return 0;
}
