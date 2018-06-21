#ifndef SERVER_HPP
#define SERVER_HPP

#include "asio.hpp"
#include <memory>
#include <iostream>
#include "session.hpp"

using asio::ip::tcp;
class server{
    public:
        server(asio::io_service& ios, short port);
    private:
        tcp::acceptor acc;
        void do_accept();
};
#endif
