#ifndef OIZYS_COM_MESSAGE_HPP
#define OIZYS_COM_MESSAGE_HPP

#include <string>

namespace oizys
{

struct com_message
{
    com_message();
    explicit com_message(std::string const & content);
    bool parse_header(size_t & length);

    unsigned char header[4];
    std::string payload;
};

}

#endif
