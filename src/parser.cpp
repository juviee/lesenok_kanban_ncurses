#include "parser.hpp"
#include <string>
#include <sstream>
#include "base_structures.h"
#include <algorithm>
std::string parser::parse(std::string com_line){
    std::stringstream ss(com_line);
    std::string command;
    ss >> command;
    if(command == "create_simple_task"){
        int id;
        ss >> id;
        logic::get_instance().create_simple_task(id);
        session_user.set_perm(id, 1);
        return "succ";
    }
    else if(command == "create_complex_task"){
        int id;
        ss >> id;
        logic::get_instance().create_complex_task(id);
        session_user.set_perm(id, 1);
        return "succ";
    }
    else if(command == "create_desk"){
        int id;
        ss >> id;
        logic::get_instance().create_complex_task(id);
        return "succ";
    }
    else if(command == "remove_task"){
        int id;
        ss >> id;
        logic::get_instance().remove_task(id);
        //doesn't work as intended to
        //insufficient fuctionality of base logic
        return "succ";
    }
    else if(command == "get_task"){
        int id;
        ss >> id;
        auto tsk = logic::get_instance().get_task(id);
        if(!tsk){
            return "fail";
        }
        boost::property_tree::ptree pt;
        tsk->to_json(pt);
        std::stringstream a;
        boost::property_tree::write_json(a, pt, false);
        std::string response;
        while(a >> response);
        return response;
    }
    else if(command == "write_desks"){
        logic::get_instance().write_desks();
        return "succ";
    }
    else if(command == "set_description"){
        int id ;
        ss >> id;
        auto tsk = logic::get_instance().get_task(id);
        std::string description;
        while(!(ss >> description));
        tsk->set_description(description);
        return "succ";
    }
    else if(command == "set_name"){
        int id;
        ss >> id;
        auto tsk = logic::get_instance().get_task(id);
        std::string name;
        while(!(ss>>name));
        tsk->set_name(name);
        return "succ";
    }
    else if(command == "set_done"){
        int id;
        ss >> id;
        logic::get_instance().get_task(id)->set_done();
        return "succ";
    }
    else if(command == "set_undone"){
        int id;
        ss >> id;
        auto tsk = logic::get_instance().get_task(id);
        if(!tsk){
            return "fail";
        }
        tsk->set_undone();
        return "succ";
    }
    else if(command == "set_id"){
        int id;
        ss >> id;
        int new_id;
        ss >> new_id;
        auto tsk = logic::get_instance().get_task(id);
        if(!tsk){
            return "fail";
        }
        tsk->set_id(id);
        return "succ";
    }
    else if(command == "set_deadline"){
        int id;
        ss >> id;
        time_t time;
        ss >> time;
        auto tsk = logic::get_instance().get_task(id);
        if(!tsk){
            return "fail";
        }
        tsk->set_deadline(time);
        return "succ";
    }
    else if(command == "set_start_time"){
        int id;
        ss >> id;
        time_t time;
        ss >> time;
        auto tsk = logic::get_instance().get_task(id);
        if(!tsk){
            return "fail";
        }
        tsk->set_start_time(time);
        return "succ";
    }
    else if(command == "add_sub_task"){
        int id;
        ss >> id;
        int sub_id;
        ss >> sub_id;
        auto tsk = logic::get_instance().get_task(id);
        if(!tsk){
            return "fail";
        }
        tsk->add_sub_task(sub_id);
        return "succ";
    }
    else if(command == "pop_sub_task"){
        int id;
        ss >> id;
        int sub_id;
        ss >> sub_id;
        auto tsk = logic::get_instance().get_task(id);
        if(!tsk){
            return "fail";
        }
        int i = tsk->pop_sub_task(sub_id);
        if(i != return_val::right){
            return "fail";
        }
        return "succ";
    }
    else if(command == "remove_sub_tasks"){
        int id;
        ss >> id;
        auto tsk = logic::get_instance().get_task(id);
        if(!tsk){
            return "fail";
        }
        auto bk = tsk->remove_sub_tasks();
        std::stringstream reply_stream;
        std::for_each(bk.begin(), bk.end(),\
                [&](int i)->void{
                    reply_stream << std::to_string(i) << ", ";
                }
                );
        std::string reply;
        while(!(reply_stream >> reply));
        return reply;
    }
    
    
    return "no_cmd";
}
