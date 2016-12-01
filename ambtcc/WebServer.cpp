
#include <memory>
#include "Common.h"
#include "WebServer.h"
#include "quickfix/Settings.h"
#include "quickfix/Utility.h"
#include "quickfix/pugixml.h"
#include "quickfix/SessionID.h"

WebServer::WebServer()
    : service_(), thread_(), port_(), stop_() {
}

void WebServer::start(unsigned short port) {

    port_ = port;
    stop_ = false;
    if (!FIX::thread_spawn(&start_thread, this, thread_))
        throw FIX::RuntimeError("Unable to spawn webserver thread");
}

THREAD_PROC WebServer::start_thread(void* p) {
        
    WebServer* server = static_cast<WebServer*>(p);
    server->run();
    return 0;
}

void WebServer::stop() {

    if (stop_) return;
    stop_ = true;

    if (!thread_) return;

    service_.stop();
    FIX::thread_join(thread_);
    thread_ = 0;
}

void WebServer::run() {

    auto persistent = std::make_shared<restbed::Resource>();
    persistent->set_path("/");
    persistent->set_method_handler("GET", get_method_handler);
    persistent->set_method_handler("POST", post_method_handler);

    auto settings = std::make_shared<restbed::Settings>();
    settings->set_port(port_);

    service_.publish(persistent);
    service_.start(settings);
}

void WebServer::get_method_handler(const std::shared_ptr<restbed::Session> session) {

    std::string response = "<form method=\"POST\"><input type=\"text\" name=\"name\" /><input type=\"submit\" /></form>";
    char response_length[10] = {};
    _itoa_s(response.size(), response_length, 10);
    session->yield(restbed::OK,
        response, {
            { "Content-Length", response_length },
            { "Content-Type", "text/html"},
            { "Connection", "keep-alive" }
        }
    );
}

void WebServer::post_method_handler(const std::shared_ptr<restbed::Session> session) {

    const auto request = session->get_request();

    auto headers = request->get_headers();
    auto header = headers.find("Content-Length");
    int content_length = header != headers.end() ? atoi(header->second.c_str()) : 0;
    session->fetch(content_length, [request](const std::shared_ptr<restbed::Session> session, const restbed::Bytes & body) {

        fprintf(stdout, "%.*s\n", (int)body.size(), body.data());

        pugi::xml_document request_document;
        request_document.load_buffer((void*)body.data(), body.size());

        pugi::xml_node request_node = request_document.document_element();
        std::string request_command_string(request_node.attribute("Command").as_string());

        pugi::xml_document response_document;
        pugi::xml_node response_node = response_document.append_child("AMMBT");

        if (!request_command_string.compare("UTCTIME")) {

            response_node.append_attribute("UTCTime").set_value(DateTime());
        }
        else if (!request_command_string.compare("HEALTH")) {

            response_node.append_attribute("Health").set_value(Health());
        }
        else if (!request_command_string.compare("CONNECTED")) {

            response_node.append_attribute("Connected").set_value(Connected());
        }
        else {

            //throw FIX::InvalidMessage(std::string("Invalid HTTP POST request (no Command): ").append(request_string));
        }

        std::stringstream response_stream;
        response_document.save(response_stream);

        char response_length[10] = {};
        _itoa_s(response_stream.str().size(), response_length, 10);

        session->yield(restbed::OK,
            response_stream.str(), {
                { "Content-Length", response_length },
                { "Content-Type", "text/xml" },
                { "Connection", "keep-alive" }
            }
        );
    });
}

FIX::Session WebServer::get_session(const char& symbol) {

    std::set<FIX::SessionID> sessions = FIX::Session::getSessions();
    std::set<FIX::SessionID>::iterator i;
    for (i = sessions.begin(); i != sessions.end(); ++i) {
        FIX::Session* session = FIX::Session::lookupSession(*i);
        if (!session) continue;
        
    }
}