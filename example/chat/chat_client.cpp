// SPDX-License-Identifier: MPL-2.0
// SPDX-FileCopyrightText: Copyright 2024 Falk Werner

#include <oizys/oizys.hpp>
#include "messages.pb.h"

#include <getopt.h>
#include <unistd.h>

#include <string>
#include <iostream>
#include <unordered_map>
#include <stdexcept>
#include <sstream>

namespace
{

void print_usage()
{
    std::cout << R"(chat-client, (C) 2024 Falk Werner
Sample chat client

Usage:
    chat-client -e <endpoint> -u <user>

Options:
    -e, --endpoint <endpoint> - Endpoint to listen (default: /tmp/com_example_chat.sock)
    -n, --user <user>         - Set user name (default: Bob)
    -h, --help                - Prints this message

Example:
    chat-client -e /tmp/chat.sock -u John
)";
}


struct application
{
    application(int argc, char* argv[])
    : endpoint("/tmp/com_example_chat.sock")
    , user("Bob")
    {
        option long_opts[] = {
            {"help", no_argument, nullptr, 'h'},
            {"endpoint", required_argument, nullptr, 'e'},
            {"user", required_argument, nullptr, 'u'},
            {nullptr, 0, nullptr, 0}
        };

        opterr = 0;
        optind = 0;

        bool done = false;
        while (!done)
        {
            int option_index = 0;
            int const code = getopt_long(argc, argv, "e:u:h", long_opts, &option_index);
            switch (code)
            {
                case -1:
                    done = true;
                    break;
                case 'h':
                    show_help = true;
                    break;
                case 'e':
                    endpoint = optarg;
                    break;
                case 'u':
                    user = optarg;
                    break;
                default:
                    std::cerr << "error: invalid option" << std::endl;
                    show_help = true;
                    exit_code = EXIT_FAILURE;
                    done = true;
                    break;
            }
        }
    }

    int exit_code = EXIT_SUCCESS;
    bool show_help = false;
    std::string endpoint;
    std::string user;
};

class console_reader
{
public: 
    console_reader(boost::asio::io_context & context)
    : user("unknown")
    , input(context, STDERR_FILENO)
    {
    }

    void start(std::string const & username, std::shared_ptr<oizys::session_i> session_)
    {
        user = username;
        session = session_;
        do_read();
    }

private:
    void do_read() {
        boost::asio::async_read_until(input, buffer, '\n',
            [this](auto & error, auto length){
                if (!error)
                {
                    std::istream stream(&buffer);
                    std::string message;
                    if (std::getline(stream, message))
                    {
                        buffer.consume(length); 
                        if (message.size() > 0) {
                            chat::chat_message msg;
                            msg.set_user(user);
                            msg.set_content(message);
                            session->send(msg.SerializeAsString());
                        }
                        do_read();
                    }
            }
        });
    }

private:
    std::string user;
    boost::asio::streambuf buffer;
    boost::asio::posix::stream_descriptor input;
    std::shared_ptr<oizys::session_i> session;
};

}

int main(int argc, char* argv[])
{
    application app(argc, argv);
    
    if (!app.show_help)
    {
        try 
        {
            bool shutdown_requested = false;
            boost::asio::io_context context;
            boost::asio::signal_set signals(context, SIGINT, SIGTERM);
            signals.async_wait([&](auto &, auto){
                shutdown_requested = true;
            });

            console_reader console(context);

            boost::asio::local::stream_protocol::socket sock(context);
            sock.connect(app.endpoint);
            auto session = oizys::create_session(std::move(sock));
            session->set_onclose([&shutdown_requested](auto){
                shutdown_requested = true;
            });
            session->set_onmessage([&](auto const & message){
                chat::chat_message msg;
                if (msg.ParseFromString(message))
                {
                    std::cout << msg.user() << ": " << msg.content() << std::endl;
                }
                else
                {
                    shutdown_requested = true;
                }
            });

            console.start(app.user, session);
            
            while (!shutdown_requested)
            {
                context.run_one();
            }
            
        }
        catch (std::exception const & ex)
        {
            std::cerr << "error: " << ex.what() << std::endl;
        }
        catch (...)
        {
            std::cerr << "error: unknown error" << std::endl;
        }
    }
    else
    {
        print_usage();
    }

    google::protobuf::ShutdownProtobufLibrary();
    return app.exit_code;
}