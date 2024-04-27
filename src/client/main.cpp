#include "proto/messages.pb.h"
#include <boost/asio.hpp>
#include <string>
#include <iostream>

class chat_client
{
public:
    chat_client(boost::asio::io_context & context )
    : input(context, STDERR_FILENO)
    {

    }

    void do_read() {
        boost::asio::async_read_until(input, inStream, '\n',
            [this](auto & error, auto){
                if (!error)
                {
                    std::istream is(&inStream);
                    std::string s;
                    is >> s;
                    inStream.consume(s.size());      
                    std::cout << s << std::endl;

                    do_read();
                }
        });

    }

private:
    boost::asio::streambuf inStream;
    boost::asio::posix::stream_descriptor input;
    //boost::asio::local::stream_protocol::socket sock;

};

int main(int argc, char* argv[])
{
    (void) argc;
    (void) argv;

    bool shutdown_requested = false;
    boost::asio::io_context context;
    boost::asio::signal_set signals(context, SIGINT, SIGTERM);
    signals.async_wait([&context,&shutdown_requested](auto, auto){
        shutdown_requested = true;
        context.stop();
    });

    chat_client client(context);
    client.do_read();

    while (!shutdown_requested)
    {
        context.run();
    }


    return 0;
}