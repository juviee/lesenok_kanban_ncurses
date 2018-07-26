#ifndef SESSION_HPP
#define SESSION_HPP

#include "base_structures.h"
#include "asio.hpp"
#include <boost/bind.hpp>
#include <memory>
#include <functional>
#include <iostream>
#include "parser.hpp"


using asio::ip::tcp;
class session:
    public std::enable_shared_from_this<session>
{
    private:
        tcp::socket socket_;
        std::string data_in;
        std::string data_out;
        size_t length_input;
        size_t length_output;
        User session_user;
        parser user_parser;
        bool keep_session = true;//if login failed, then session stops
        enum{max_length = 8192};
        //interfaces to simplify usage asio
        void rec_length(std::function<void(asio::error_code, size_t)>);
        void rec_data(std::function<void(asio::error_code, size_t)>);
        void send_length(std::function<void(asio::error_code, size_t)>);
        void send_message(std::string);
        void send_message_handler(asio::error_code, size_t);
        //login functions
        void login();
        void login_rec(asio::error_code, size_t);
        void check_login(asio::error_code, size_t);
        void failed_login();
        void success_login();
        //commands from user handlers
        void listen_commands(asio::error_code, size_t);
        void listen_commands_rec_data(asio::error_code ec, size_t);
        void command_handler(asio::error_code ec, size_t);

    public:
        session(tcp::socket socket):
            socket_(std::move(socket)),
            session_user(std::string(), std::string()),
            user_parser(session_user)
        {
        }
        void start_session(){
            login();
        }
        ~session(){
            logic::get_instance().write_desks();
        }

};
#endif
