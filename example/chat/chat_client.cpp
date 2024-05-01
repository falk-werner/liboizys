#include <oizys/oizys.hpp>
#include <oizys/unstable/context.hpp>

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
    // NOLINTNEXTLINE
    application(int argc, char* argv[])
    : endpoint("/tmp/com_example_chat.sock")
    , user("Bob")
    {
        option long_opts[] = {   // NOLINT
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
    : input(context, STDERR_FILENO)
    {
    }

    void start(std::shared_ptr<oizys::session_i> session_)
    {
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
                            session->send(message);
                        }
                        do_read();
                    }
            }
        });
    }

private:
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
                context.stop();
            });

            console_reader console(context);

            auto com_context = oizys::context_from_asio(context);
            com_context->connect_to(app.endpoint, [&](auto session){
                if (!session)
                {
                    std::cerr << "failed to connect" << std::endl;
                    app.exit_code = EXIT_FAILURE;
                    shutdown_requested = true;
                    return;
                }
                
                console.start(session);

                session->set_on_close([&shutdown_requested](){
                    shutdown_requested = true;
                });

                session->set_on_message([](auto const & message){
                    std::cout << message << std::endl;
                });
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