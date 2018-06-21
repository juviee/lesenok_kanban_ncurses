#ifndef CLIENT_CONNECTION_HPP
#define CLIENT_CONNECTION_HPP
#include <string>
#include "asio.hpp"
using asio::ip::tcp;
class client_connection{
    tcp::socket sock;
    public:
        client_connection (asio::io_service& ios, std::string addr, 
                std::string port);
        void send_message (std::string msg);
        std::string read_reply ();
};
#endif
