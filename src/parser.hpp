#ifndef CMD_PARSER_HPP
#define CMD_PARSER_HPP
#include "base_structures.h"
class parser{
    private:
        User session_user;
    public:
        parser(User s_u):
            session_user(s_u)
        {
        }
        std::string parse(std::string);
};

#endif
