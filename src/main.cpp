#include <boost/asio.hpp>
#include <string>

namespace
{

void con_write(boost::asio::local::stream_protocol::socket & sock, std::string const & value)
{
    auto length = value.size();
    if (length < 1024)
    {
        unsigned char header[4];
        header[0] = 0;
        for(int i = 3; i > 0; i--)
        {
            header[i] = (unsigned char) (length & 0xff);
            length >>= 8;
        }

        boost::asio::write(sock, boost::asio::buffer(header, 4));
        boost::asio::write(sock, boost::asio::buffer(value, value.size()));
    }

}

}

int main(int argc, char* argv[])
{

    boost::asio::io_context context;

    boost::asio::local::stream_protocol::socket sock(context);
    boost::asio::local::stream_protocol::endpoint ep("/tmp/chat.sock");
    sock.connect(ep);

    con_write(sock, "Hi,");
    con_write(sock, " there!");

    return 0;
}