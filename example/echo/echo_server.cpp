#include <oizys/oizys.hpp>
#include <oizys/unstable/context.hpp>

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
    // NOLINTNEXTLINE
    application(int argc, char* argv[])
    : endpoint("/tmp/com_example_echo.sock")
    {
        option long_opts[] = {   // NOLINT
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
            auto com_context = oizys::context_from_asio(context);
            auto listener = com_context->new_listener(app.endpoint, [](auto session){
                std::cout << "info: new connection" << std::endl;

                session->set_on_close([](){
                    std::cout << "info: connection closed" << std::endl;
                });

                session->set_on_message([session](auto const & message){
                    std::cout << message << std::endl;
                    session->send(message);
                });
            });

            listener->start();
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
