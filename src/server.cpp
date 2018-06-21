#include "server.hpp"

using asio::ip::tcp;

server::server(asio::io_service& ios, short port):
    acc(ios, tcp::endpoint(tcp::v4(), port))
{
    do_accept();
}

void server::do_accept(){
    acc.async_accept (
        [this](asio::error_code ec, tcp::socket sock)->void
        {
            if (!ec) {
#ifndef NDEBUG
                std::cout << "accepted" << std::endl;
#endif
                std::make_shared<session>(std::move(sock))->start_session();
            }
#ifndef NDEBUG
            else{
                std::cout << ec.message() << std::endl;
            }
#endif
            do_accept();
        });
}
