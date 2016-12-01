#ifndef AMBTCC_WEBSERVER_H
#define AMBTCC_WEBSERVER_H

#include <restbed>
#include "quickfix/Mutex.h"
#include "quickfix/Settings.h"
#include "quickfix/Session.h"

class WebServer {
public:

    WebServer();

    void start(unsigned short port);
    void stop();

private:

    static THREAD_PROC start_thread(void* p);

    static void get_method_handler(const std::shared_ptr<restbed::Session> session);
    static void post_method_handler(const std::shared_ptr<restbed::Session> session);

    void run();

    FIX::Session get_session(const char& symbol);

    restbed::Service service_;
    FIX::thread_id thread_;
    unsigned short port_;
    bool stop_;
};

#endif
