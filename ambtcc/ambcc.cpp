/****************************************************************************
** Copyright (c) 2001-2014
**
** This file is part of the QuickFIX FIX Engine
**
** This file may be distributed under the terms of the quickfixengine.org
** license as defined by quickfixengine.org and appearing in the file
** LICENSE included in the packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.quickfixengine.org/LICENSE for licensing information.
**
** Contact ask@quickfixengine.org if any conditions of this licensing are
** not clear to you.
**
****************************************************************************/

#ifdef _MSC_VER
#pragma warning( disable : 4503 4355 4786 4146 )
//#include "stdafx.h"
#else
#include "config.h"
#endif

#include "quickfix/FileStore.h"
#include "quickfix/ThreadedSSLSocketInitiator.h"
#include "quickfix/SessionSettings.h"
#include "quickfix/Log.h"
#include "Application.h"
#include <string>
#include <iostream>
#include <fstream>

#include "../src/getopt-repl.h"

int main(int argc, char** argv) {

    std::string file;
    if (argc == 2) {

        file.assign(argv[1]);
    }
    else {

        file.assign(argv[0]);
        file.resize(file.find_last_of('.'));
        file.append(".ini");
        std::cout <<
            "usage: " << argv[0] << " [settings_filename]" << std::endl <<
            "\tloading default: " << file << " ..." << std::endl;
    }
    try {

        FIX::SessionSettings settings(file);

        Application application(settings);
        FIX::FileStoreFactory stores(settings);
        FIX::ScreenLogFactory logs(settings);

        FIX::ThreadedSSLSocketInitiator socket(application, stores, settings, logs);

        socket.start();
        application.run();
        socket.stop();

        return 0;
    }
    catch (std::exception & e) {

        std::cout << e.what();
        return 1;
    }
}
