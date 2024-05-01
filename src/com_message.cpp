#include "com_message.hpp"
#include <stdexcept>

namespace oizys
{

constexpr size_t const com_max_message_size = 0x00ffffff;

com_message::com_message()
: header{0,0,0,0}
{

}

com_message::com_message(std::string const & content)
: payload(content)
{
    if ((content.empty()) || (content.size() > com_max_message_size))
    {
        throw std::runtime_error("invalid message size");
    }

    // NOLINTBEGIN
    header[0] = 0; // reserved;
    header[1] = static_cast<unsigned char>((content.size() >> 16) & 0xff);
    header[2] = static_cast<unsigned char>((content.size() >>  8) & 0xff);
    header[3] = static_cast<unsigned char>( content.size()        & 0xff);
    // NOLINTEND
}

bool com_message::parse_header(size_t & length)
{
    if (header[0] != 0) { return false; }

    length = 0;
    for(int i = 1; i < 4; i++)
    {
        // NOLINTBEGIN
        length <<= 8;
        length |= static_cast<size_t>(header[i]) & 0xff;
        // NOLINTEND
    }

    return (length > 0) && (length <= com_max_message_size);
}


}