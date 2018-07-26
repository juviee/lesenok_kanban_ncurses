#include "session.hpp"
#include "parser.hpp"

using asio::ip::tcp;

void session::rec_length(std::function<void(asio::error_code, size_t)> handler)
{
    socket_.async_read_some(
        asio::buffer(&length_input, sizeof(length_input)),
        handler
        );
}
void session::send_length(std::function<void(asio::error_code, size_t)> handler){
    length_output = data_out.length();
#ifndef NDEBUG
    std::cout << "send_length(" << length_output << ")" << std::endl;
#endif
    socket_.async_write_some(
            asio::buffer(&length_output, sizeof(length_output)),
            handler);
}

void session::rec_data(std::function<void(asio::error_code, size_t)> handler){
        data_in = std::string(length_input, '\0');
        asio::async_read(socket_, 
            asio::buffer(data_in, length_input),
            handler
            );
}

void session::login(){
    auto self(shared_from_this());
    rec_length(boost::bind(&session::login_rec, self,
                boost::placeholders::_1, boost::placeholders::_2));

}
void session::login_rec(asio::error_code ec, size_t){
    if(!ec){
        auto self(shared_from_this());
#ifndef NDEBUG
        std::cout << "login_rec \n" << "size : " << length_input 
            << std::endl;
#endif
        if(!length_input){
            failed_login();
            return;
        }
        auto handler = boost::bind(&session::check_login, self, 
            boost::placeholders::_1, boost::placeholders::_2);
        rec_data(handler);
    }
}

void session::check_login(asio::error_code ec, size_t){
    if(!ec){
        std::stringstream a(data_in);
        std::string login;
        std::string password;
        a >> login >> password;
        session_user = User(login, password);
        int ec = session_user.from_json();
        if(ec != return_val::right){
#ifndef NDEBUG/////////////////////////
            std::cout << "failed_login" << std::endl;
#endif/////////////////////////////////
            auto self(shared_from_this());
            auto fail_ = boost::bind(&session::failed_login, self);
            fail_();
            return;
        }
#ifndef NDEBUG/////////////////////////
        std::cout << "User_data" << std::endl;
        for(auto it : session_user.perm_desks){
            std::cout << it.first << ":[" << it.second << "]" 
                <<std::endl;
        }
#endif/////////////////////////////////
        user_parser = parser(session_user);
        logic::get_instance().load_desks(session_user);
        success_login();
    }
}

void session::failed_login(){
#ifndef NDEBUG
    std::cerr << "failed_login:";
#endif
    keep_session = false;
    auto self(shared_from_this());
    send_message("failed to login");
}

void session::success_login(){
#ifndef NDEBUG
    std::cerr << "success_login:";
#endif
    keep_session = true;
    auto self(shared_from_this());
    auto send_message_ = boost::bind(&session::send_message, self,
            boost::placeholders::_1);
    send_message_("success");
}

void session::listen_commands(asio::error_code ec, size_t){
    auto self = shared_from_this();
    auto handler = boost::bind(&session::listen_commands_rec_data,
            self, boost::placeholders::_1,
            boost::placeholders::_2);
    rec_length(handler);
}


void session::listen_commands_rec_data(asio::error_code ec, size_t){
    if(!ec){
        auto self = shared_from_this();
        auto handler = boost::bind(&session::command_handler,
                self, boost::placeholders::_1,
                boost::placeholders::_2);
        rec_data(handler);
    }
}

void session::command_handler(asio::error_code ec, size_t){
    if(!ec){
        std::string cmd(data_in);
        std::string reply = user_parser.parse(cmd);
        send_message(reply);
    }
}

void session::send_message(std::string msg){
#ifndef NDEBUG
    std::cout << "send_message(" << msg << ")" << std::endl;
#endif
    data_out = msg;
    length_output = msg.length();
    auto self(shared_from_this());
    send_length(boost::bind(&session::send_message_handler,
        self, boost::placeholders::_1, boost::placeholders::_2));
}

void session::send_message_handler(asio::error_code ec, size_t){
#ifndef NDEBUG
    std::cout<<"send_message_handler("<< data_out << ")" <<std::endl;
#endif
    if(!ec){
        auto self(shared_from_this());
        std::function<void(asio::error_code, size_t)> handler;
        if(keep_session){
            handler = boost::bind(&session::listen_commands, self, 
                boost::placeholders::_1, boost::placeholders::_2);
        }
        else{
            handler = [this, self](asio::error_code, size_t){};
        }
        asio::async_write(socket_, 
            asio::buffer(data_out, length_output), handler);
    }
}
