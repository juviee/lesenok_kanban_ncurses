#include <iostream>
#include <string>
#include "client_connection.hpp"
#include "asio.hpp"
using asio::ip::tcp;

client_connection::client_connection (asio::io_service& ios, std::string addr, std::string port):
    sock (ios)
{
    tcp::resolver rsv(ios);
    asio::connect (sock, rsv.resolve(addr, port));
}
void client_connection::send_message (std::string msg){
    try{
        size_t len = msg.size();
        asio::write (sock, asio::buffer (&len, sizeof(len)));
        asio::write (sock, asio::buffer (msg));
    }
    catch(std::exception& e){
        std::cout << "Exception: " << e.what() << std::endl; 
    }
}
std::string client_connection::read_reply (){
    try{
        size_t len = 0;
        asio::error_code ec;
        asio::read (sock, asio::buffer(&len, sizeof(len)), ec);
        std::string reply (len + 1, 0);
        asio::read (sock, asio::buffer(reply, len), ec);
        return reply;
    }
    catch(std::exception& e){
        std::cout << "Exception: " << e.what() << std::endl; 
        return e.what();
    }
    return "UNHANDLED SMTH";
}

