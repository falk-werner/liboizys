#include <com/com.hpp>
#include <com/unstable/context.hpp>

#include <getopt.h>
#include <unistd.h>

#include <string>
#include <iostream>
#include <unordered_map>
#include <stdexcept>

namespace
{

void print_usage()
{
    std::cout << R"(chat-server, (C) 2024 Falk Werner
Sample chat server

Usage:
    chat-server -e <endpoint>

Options:
    -e, --endpoint <endpoint> - Endpoint to listen (default: /tmp/com_example_chat.sock)
    -h, --help                - Prints this message

Example:
    chat-server -e /tmp/chat.sock
)";
}


struct application
{
    // NOLINTNEXTLINE
    application(int argc, char* argv[])
    : endpoint("/tmp/com_example_chat.sock")
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

class session_manager
{
public:
    void close()
    {
        for (auto const & item: sessions)
        {
            item.second->close();
        }
    }

    uint32_t add(std::shared_ptr<com::session_i> session)
    {
        auto const id = create_id();
        sessions.emplace(id, session);
        return id;
    }

    void remove(uint32_t id)
    {
        auto it = sessions.find(id);
        if (it == sessions.end())
        {
            it->second->close();
            sessions.erase(it);
        }
    }

    std::shared_ptr<com::session_i> get(uint32_t id)
    {
        auto it = sessions.find(id);
        if (it == sessions.end())
        {
            throw std::runtime_error("session gone");
        }        

        return it->second;
    }

    void send_all(std::string const & message)
    {
        for(auto const&  item: sessions)
        {
            auto session = item.second;
            session->send(message);
        }
    }

private:
    uint32_t create_id()
    {
        while ((id == 0) || (has_id(id)))
        {
            id++;
        }

        return id;
    }    

    bool has_id(uint32_t id) const
    {
        auto it = sessions.find(id);
        return (it != sessions.end());
    }

    std::unordered_map<uint32_t, std::shared_ptr<com::session_i>> sessions;
    uint32_t id = 0;
};

}

int main(int argc, char* argv[])
{
    application app(argc, argv);
    
    if (!app.show_help)
    {
        try 
        {
            auto manager = std::make_shared<session_manager>();
            
            bool shutdown_requested = false;
            boost::asio::io_context context;
            boost::asio::signal_set signals(context, SIGINT, SIGTERM);
            signals.async_wait([&](auto &, auto){
                shutdown_requested = true;
                context.stop();
            });

            unlink(app.endpoint.c_str());
            auto com_context = com::context_from_asio(context);
            auto listener = com_context->new_listener(app.endpoint, [manager](auto session){
                std::cout << "info: new connection" << std::endl;

                auto const id = manager->add(session);

                session->set_on_close([manager, id](){
                    std::cout << "info: connection closed" << std::endl;
                    manager->remove(id);
                });

                session->set_on_message([manager, id](auto const & message){
                    std::cout << message << std::endl;
                    manager->send_all(message);
                });
            });

            listener->start();
            std::cout << "info: listening on " << app.endpoint << std::endl;

            
            while (!shutdown_requested)
            {
                context.run();
            }
            
            manager->close();
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