// SPDX-License-Identifier: MPL-2.0
// SPDX-FileCopyrightText: Copyright 2024 Falk Werner

#include <oizys/oizys.hpp>

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
    std::cout << R"(notification-server, (C) 2024 Falk Werner
Sample notification server

Usage:
    notification-server -e <endpoint>

Options:
    -e, --endpoint <endpoint> - Endpoint to listen (default: /tmp/oizys_example_notify.sock)
    -h, --help                - Prints this message

Example:
    notification-server -e /tmp/notify.sock
)";
}


struct application
{
    application(int argc, char* argv[])
    : endpoint("/tmp/oizys_example_notify.sock")
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

class notification_server
{
public:
    notification_server(boost::asio::io_context& context, std::string const & endpoint)
    : acceptor(context, boost::asio::local::stream_protocol::endpoint(endpoint))
    {
        do_accept();
    }

    void close()
    {
        for (auto const & item: sessions)
        {
            item.second->close();
        }
    }

    uint32_t add(std::shared_ptr<oizys::session_i> session)
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

    std::shared_ptr<oizys::session_i> get(uint32_t id)
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
    void do_accept() {
        acceptor.async_accept([this](auto err, auto sock) {
            if (!err)
            { 
                std::cout << "info: new connection" << std::endl;

                auto session = oizys::create_session(std::move(sock));
                auto const id = add(session);

                session->set_onclose([this, id](){
                    std::cout << "info: connection closed" << std::endl;
                    remove(id);
                });

                // set on message handler to start reading
                // this allows to detect a closed connection
                session->set_onmessage([](auto){});
            }

            do_accept();
        });        
    }

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

    boost::asio::local::stream_protocol::acceptor acceptor;
    std::unordered_map<uint32_t, std::shared_ptr<oizys::session_i>> sessions;
    uint32_t id = 0;
};

void notify(boost::asio::deadline_timer &timer, notification_server& server)
{
    timer.expires_from_now(boost::posix_time::seconds(10));
    timer.async_wait([&timer, &server](auto){
        server.send_all("notify");
        notify(timer, server);
    });    
}

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
            notification_server server(context, app.endpoint);

            boost::asio::deadline_timer timer(context);
            notify(timer, server);

            std::cout << "info: listening on " << app.endpoint << std::endl;
            
            while (!shutdown_requested)
            {
                context.run();
            }
            
            server.close();
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