// SPDX-License-Identifier: MPL-2.0
// SPDX-FileCopyrightText: Copyright 2024 Falk Werner

#include <oizys/oizys.hpp>

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
    std::cout << R"(notification-client, (C) 2024 Falk Werner
Sample notification client

Usage:
    notification-client -e <endpoint> -u <user>

Options:
    -e, --endpoint <endpoint> - Endpoint to listen (default: /tmp/oizys_example_notify.sock)
    -h, --help                - Prints this message

Example:
    notification-client -e /tmp/notify.sock
)";
}


struct application
{
    application(int argc, char* argv[])
    : endpoint("/tmp/oizys_example_notify.sock")
    , user("Bob")
    {
        option long_opts[] = {
            {"help", no_argument, nullptr, 'h'},
            {"endpoint", required_argument, nullptr, 'e'},
            {nullptr, 0, nullptr, 0}
        };

        opterr = 0;
        optind = 0;

        bool done = false;
        while (!done)
        {
            int option_index = 0;
            int const code = getopt_long(argc, argv, "e:h", long_opts, &option_index);
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
                context.stop();
            });

            boost::asio::local::stream_protocol::socket sock(context);
            sock.connect(app.endpoint);
            auto session = oizys::create_session(std::move(sock));
            session->set_on_close([&shutdown_requested](){
                shutdown_requested = true;
            });
            session->set_on_message([](auto const & message){
                std::cout << message << std::endl;
            });
                        
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

    return app.exit_code;
}