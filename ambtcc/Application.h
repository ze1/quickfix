/* -*- C++ -*- */

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

#ifndef TRADECLIENT_APPLICATION_H
#define TRADECLIENT_APPLICATION_H

#include "quickfix/Application.h"
#include "quickfix/MessageCracker.h"
#include "quickfix/Values.h"
#include "quickfix/Mutex.h"

#include "quickfix/fix44/NewOrderSingle.h"
#include "quickfix/fix44/ExecutionReport.h"
#include "quickfix/fix44/OrderCancelRequest.h"
#include "quickfix/fix44/OrderCancelReject.h"
#include "quickfix/fix44/OrderCancelReplaceRequest.h"
#include "quickfix/fix44/BusinessMessageReject.h"
#include "quickfix/fix44/MarketDataRequest.h"
#include "quickfix/fix44/MarketDataRequestReject.h"
#include "quickfix/fix44/MarketDataSnapshotFullRefresh.h"
#include "quickfix/fix44/MarketDataIncrementalRefresh.h"

#include <queue>
#include <atomic>
#include <fstream>

#include "Common.h"
#include "WebServer.h"
#include "AccountInfoRequest.h"

class Application :
	public FIX::Application,
	public FIX::MessageCracker {

public:
    Application(const FIX::SessionSettings& settings)
        : data_active_(), ticks_file_(), webserver_(), settings_(settings) {}
	void run();

private:
	void onCreate(const FIX::SessionID&);
	void onLogon(const FIX::SessionID& sessionID);
	void onLogout(const FIX::SessionID& sessionID);
	void toAdmin(FIX::Message&, const FIX::SessionID&);
	void toApp(FIX::Message&, const FIX::SessionID&) throw(FIX::DoNotSend);
	void fromAdmin(const FIX::Message&, const FIX::SessionID&) throw(FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::RejectLogon);
	void fromApp(const FIX::Message& message, const FIX::SessionID& sessionID) throw(FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::UnsupportedMessageType);

	void onMessage(const FIX44::ExecutionReport&, const FIX::SessionID&);
	void onMessage(const FIX44::OrderCancelReject&, const FIX::SessionID&);
	void onMessage(const FIX44::BusinessMessageReject&, const FIX::SessionID&);
	void onMessage(const FIX44::MarketDataSnapshotFullRefresh&, const FIX::SessionID&);
	void onMessage(const FIX44::MarketDataIncrementalRefresh&, const FIX::SessionID&);
	void onMessage(const FIX44::MarketDataRequestReject&, const FIX::SessionID&);

	//void queryEnterOrder();
	//void queryCancelOrder();
	//void queryReplaceOrder();
	void queryMarketDataRequest();

	//FIX44::NewOrderSingle queryNewOrderSingle44();
	//FIX44::OrderCancelRequest queryOrderCancelRequest44();
	//FIX44::OrderCancelReplaceRequest queryCancelReplaceRequest44();
	FIX44::MarketDataRequest queryMarketDataRequest44();
    FIX44::MarketDataRequest Application::queryAccountInfoRequestBTCC();

	void queryHeader(FIX::Header& header);

	std::atomic_bool data_active_;

	class TickLine {
	public:
		TickLine(std::ofstream& file) : file_(file), bid_(0.00), ask_(0.00), date_(), time_() {}
		virtual ~TickLine() { write(); }
		void setTick(const FIX::MDEntryDate& tick_date, const FIX::MDEntryTime& tick_time,
			const FIX::MDEntryType& tick_type, const FIX::MDEntryPx& tick_price);
	private:

        void write();
        FIX::Account account_signature();
		std::ofstream& file_;
		FIX::MDEntryPx bid_;
		FIX::MDEntryPx ask_;
		FIX::MDEntryDate date_;
		FIX::MDEntryTime time_;
	};

	std::ofstream ticks_file_;
    WebServer webserver_;

    FIX::SessionSettings settings_;
};

#endif
