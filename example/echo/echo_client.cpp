// SPDX-License-Identifier: MPL-2.0
// SPDX-FileCopyrightText: Copyright 2024 Falk Werner

#include <oizys/oizys.hpp>

#include <getopt.h>

#include <string>
#include <iostream>

namespace
{

void print_usage()
{
    std::cout << R"(echo-client, (C) 2024 Falk Werner
Sample echo client

Usage:
    echo-client -e <endpoint> -m <message>

Options:
    -e, --endpoint <endpoint> - Endpoint to connect to (default): /tmp/com_example.sock)
    -m, --message <message>   - message to send (default: Hi)
    -h, --help                - Prints this message

Example:
    echo-server -e /tmp/echo.sock -m 'Hello, World!'
)";
}

struct application
{
    // NOLINTNEXTLINE
    application(int argc, char* argv[])
    : endpoint("/tmp/com_example_echo.sock")
    , message("Hi")
    {
        option long_opts[] = {   // NOLINT
            {"help", no_argument, nullptr, 'h'},
            {"endpoint", required_argument, nullptr, 'e'},
            {"message", required_argument, nullptr, 'm'},
            {nullptr, 0, nullptr, 0}
        };

        opterr = 0;
        optind = 0;

        bool done = false;
        while (!done)
        {
            int option_index = 0;
            int const code = getopt_long(argc, argv, "e:m:h", long_opts, &option_index);
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
                case 'm':
                    message = optarg;
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
    std::string message;
};

}


int main(int argc, char* argv[])
{
    application app(argc, argv);

    if (!app.show_help)
    {
        try 
        {
            boost::asio::io_context context;
            bool done = false;

            boost::asio::local::stream_protocol::socket sock(context);
            sock.connect(app.endpoint);
            auto session = oizys::create_session(std::move(sock));
                
            session->set_onmessage([&done](auto const & message){
                std::cout << message << std::endl;
                done = true;
            });

            session->send(app.message);
            
            while (!done)
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
