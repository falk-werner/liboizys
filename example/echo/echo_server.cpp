// SPDX-License-Identifier: MPL-2.0
// SPDX-FileCopyrightText: Copyright 2024 Falk Werner

#include <oizys/oizys.hpp>

#include <getopt.h>
#include <unistd.h>

#include <string>
#include <iostream>

namespace
{

void print_usage()
{
    std::cout << R"(echo_server, (C) 2024 Falk Werner
Sample echo server

Usage:
    echo-server -e <endpoint>

Options:
    -e, --endpoint <endpoint> - Endpoint to listen (default: /tmp/com_example_echo.sock)
    -h, --help                - Prints this message

Example:
    echo-server -e /tmp/echo.sock
)";
}

struct application
{
    application(int argc, char* argv[])
    : endpoint("/tmp/com_example_echo.sock")
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
};

class echo_server
{
public:
    echo_server(boost::asio::io_context& context, std::string const & endpoint)
    : acceptor(context, boost::asio::local::stream_protocol::endpoint(endpoint))
    {
        do_accept();
    }

private:
    void do_accept() {
        acceptor.async_accept([this](auto err, auto sock) {
            if (!err)
            {
                std::cout << "info: new connection" << std::endl;

                auto session = oizys::create_session(std::move(sock));
                session->set_onclose([](){
                    std::cout << "info: connection closed" << std::endl;
                });

                session->set_onmessage([session](auto const & message){
                    std::cout << message << std::endl;
                    session->send(message);
                });
            }

            do_accept();
        });        
    }

    boost::asio::local::stream_protocol::acceptor acceptor;
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

            unlink(app.endpoint.c_str());
            echo_server server(context, app.endpoint);
            
            std::cout << "info: listening on " << app.endpoint << std::endl;
            while (!shutdown_requested)
            {
                context.run();
            }
            
            std::cout << "info: shutdown" << std::endl;
        }
        catch (std::exception const & ex)
        {
            std::cerr << "error: " << ex.what() << std::endl;
        }
        catch (...)
        {
            std::cerr << "error: unknown error" << std::endl;
        }

        unlink(app.endpoint.c_str());
    }
    else
    {
        print_usage();
    }

    return app.exit_code;
}
