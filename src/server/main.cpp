#include "chat_server.hpp"
#include <boost/asio.hpp>
#include <unistd.h>

#include <iostream>


int main(int argc, char* argv[])
{
    try {
        bool shutdown_requested = false;
        boost::asio::io_context context;
        boost::asio::signal_set signals(context, SIGINT, SIGTERM);
        signals.async_wait([&context,&shutdown_requested](auto & error, auto id){
          shutdown_requested = true;
          context.stop();
        });


        unlink("/tmp/chat.sock");
        chat::chat_server server(context, "/tmp/chat.sock");
        while (!shutdown_requested)
        {
          context.run();
        }
    }
    catch (...) {
        std::cerr << "error" << std::endl;
    }

    return 0;
}