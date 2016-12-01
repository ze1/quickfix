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

#ifdef _MSC_VER
#pragma warning( disable : 4503 4355 4786 )
#include "stdafx.h"
#else
#include "config.h"
#endif

const long FIX_VERSION = 44;
const char BEGIN_STRING[] = "FIX.4.4";
const char SERVER_HOST[] = "pro-fix.btcc.com";
const long SERVER_PORT = 9880;
const char SERVER_ID[] = "BTCC-PRO-EXCHANGE-SERVER";
const char SYMBOL[] = "XBTCNY";
const char ACCESS_KEY[] = "b972bc3f-d420-4c09-ac28-0870c8177cda"; // DEVELOPMENT KEY
const char SECRET_KEY[] = "977476f9-e174-46fd-94e2-3e9757baf921"; // DEVELOPMENT KEY
//const char ACCESS_KEY[] = "f9ea5a8f-93ed-4d19-9a43-5eec9d08a553"; // DEMO KEY
//const char SECRET_KEY[] = "546b2f4b-e7d0-4e55-a8f4-18b5ba4772d3"; // DEMO KEY

#include "windows.h"
#include "sha1.h"
#include "Application.h"
#include "quickfix/Session.h"
#include <iostream>
#include <string>
#include <cstring>

FIX44::MarketDataRequest Application::queryMarketDataRequest44() {

	FIX::MDReqID mdReqID("MARKETDATAID");
	FIX::SubscriptionRequestType subType(FIX::SubscriptionRequestType_SNAPSHOT_PLUS_UPDATES);
	FIX::MarketDepth marketDepth(0);

	FIX44::MarketDataRequest::NoRelatedSym symbolGroup;
	symbolGroup.set(FIX::Symbol(SYMBOL));

	FIX44::MarketDataRequest message(mdReqID, subType, marketDepth);
	message.addGroup(symbolGroup);

	FIX44::MarketDataRequest::NoMDEntryTypes group;
	FIX::MDEntryType group_bid(FIX::MDEntryType_BID); group.set(group_bid); message.addGroup(group);
	FIX::MDEntryType group_offer(FIX::MDEntryType_OFFER); group.set(group_offer); message.addGroup(group);
	FIX::MDEntryType group_trade(FIX::MDEntryType_TRADE); group.set(group_trade); message.addGroup(group);
	//FIX::MDEntryType group_close(FIX::MDEntryType_CLOSING_PRICE); group.set(group_close); message.addGroup(group);
	//FIX::MDEntryType group_high(FIX::MDEntryType_TRADING_SESSION_HIGH_PRICE); group.set(group_high); message.addGroup(group);
	//FIX::MDEntryType group_low(FIX::MDEntryType_TRADING_SESSION_LOW_PRICE); group.set(group_low); message.addGroup(group);
	//FIX::MDEntryType group_vwap(FIX::MDEntryType_TRADING_SESSION_VWAP_PRICE); group.set(group_vwap); message.addGroup(group);
	FIX::MDEntryType group_volume(FIX::MDEntryType_TRADE_VOLUME); group.set(group_volume); message.addGroup(group);

	queryHeader(message.getHeader());

	std::cout << message.toXML() << std::endl;
	std::cout << message.toString() << std::endl;

	return message;
}

FIX::Account Application::account_signature() {

    //return (long)(DateTime.UtcNow - new DateTime(1970, 1, 1, 0, 0, 0, DateTimeKind.Utc)).TotalMilliseconds;

    char tonce[32] = { 0 }, param[256] = { 0 };
    itoa(DateTime() * 1000, tonce, 32);
    sprintf_s(param, "tonce=%s&accesskey=%s&requestmethod=post&id=1&method=getForwardsAccountInfo&params=", tonce, SECRET_KEY);

    const size_t len = 64;
    unsigned char key[len] = { 0 };
    size_t keylen = strlen(SECRET_KEY);
    if (keylen <= len) memcpy(key, SECRET_KEY, keylen);
    else memcpy(key, sha1(SECRET_KEY).data(), len);
    for (size_t i = 0; i < len; ++i) key[i] ^= 0x36;
    SHA1 shadata, sharesult;
    shadata.update(std::string((char*)&key[0], len));
    shadata.update(std::string(param));
    std::string shadatastr(shadata.final());
    for (size_t i = 0; i < len; ++i) key[i] ^= 0x5C ^ 0x36;
    sharesult.update(std::string((char*)&key[0], len));
    sharesult.update(shadatastr);
    std::string sharesultstr(sharesult.final());
    std::string signature(ACCESS_KEY);
    signature.append(":");
    for (size_t i = 0; i < len; ++i) {
        char xx[3] = { 0 };
        sprintf_s(xx, "%02X", (unsigned char)sharesultstr[i]);
        signature.append(xx);
    }
    char sign[1024] = { 0 }, result[1024] = { 0 };
    strcpy_s(sign, signature.c_str());
    b64_encode(sign, result);

    return FIX::Account(std::string(tonce).append(":").append(result));
/*
    HMACSHA1 hmacsha1 = new HMACSHA1(Encoding.ASCII.GetBytes(key));
    byte[] signatureByte = hmacsha1.ComputeHash(Encoding.ASCII.GetBytes(data));
    return ByteToString(signatureByte);

    string hash = GetSignature(param, secretKey);
    string basicAuth = $"Basic {Convert.ToBase64String(Encoding.Default.GetBytes(accessKey + ":" + hash))}";

    return $"{tonce}:{basicAuth}";
*/
}

FIX44::MarketDataRequest Application::queryAccountInfoRequestBTCC() {

    FIXBTCC::AccReqID acc_req_id("ACCTINFOID");
    FIX::SubscriptionRequestType subscribe(FIX::SubscriptionRequestType_SNAPSHOT_PLUS_UPDATES);

    FIXBTCC::AccountInfoRequest message(account_signature(), acc_req_id, subscribe);
    message.addGroup(symbolGroup);

    FIX44::MarketDataRequest::NoMDEntryTypes group;
    FIX::MDEntryType group_bid(FIX::MDEntryType_BID); group.set(group_bid); message.addGroup(group);
    FIX::MDEntryType group_offer(FIX::MDEntryType_OFFER); group.set(group_offer); message.addGroup(group);
    FIX::MDEntryType group_trade(FIX::MDEntryType_TRADE); group.set(group_trade); message.addGroup(group);
    //FIX::MDEntryType group_close(FIX::MDEntryType_CLOSING_PRICE); group.set(group_close); message.addGroup(group);
    //FIX::MDEntryType group_high(FIX::MDEntryType_TRADING_SESSION_HIGH_PRICE); group.set(group_high); message.addGroup(group);
    //FIX::MDEntryType group_low(FIX::MDEntryType_TRADING_SESSION_LOW_PRICE); group.set(group_low); message.addGroup(group);
    //FIX::MDEntryType group_vwap(FIX::MDEntryType_TRADING_SESSION_VWAP_PRICE); group.set(group_vwap); message.addGroup(group);
    FIX::MDEntryType group_volume(FIX::MDEntryType_TRADE_VOLUME); group.set(group_volume); message.addGroup(group);

    queryHeader(message.getHeader());

    std::cout << message.toXML() << std::endl;
    std::cout << message.toString() << std::endl;

    return message;
}

void Application::queryHeader(FIX::Header& header) {

	header.setField(FIX::TargetCompID(SERVER_ID));
	header.setField(FIX::SenderCompID(ACCESS_KEY));
}

void Application::onCreate(const FIX::SessionID& sessionID) {

  bool active = false;
  Connected(&active);

  data_active_ = false;
  std::cout << std::endl << "=== Create - " << sessionID << std::endl;
}

void Application::onLogon(const FIX::SessionID& sessionID) {

    bool active = true;
    Connected(&active);
    std::cout << std::endl << "<<< Logon - " << sessionID << std::endl;
}

void Application::onLogout(const FIX::SessionID& sessionID) {

    bool active = false;
    Connected(&active);
    
    data_active_ = false;
    ticks_file_.close();
    std::cout << std::endl << "<<< Logout - " << sessionID << std::endl;
}

void Application::toAdmin(FIX::Message& message, const FIX::SessionID& sessionID) {

  std::cout << std::endl << ">>> Admin - " << sessionID << ": " << message.toString() << std::endl;
}

void Application::fromAdmin(const FIX::Message& message, const FIX::SessionID& sessionID) throw( FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::RejectLogon ) {

  std::cout << std::endl << "<<< Admin - " << sessionID << ": " << message.toString() << std::endl;
}

void Application::fromApp(const FIX::Message& message, const FIX::SessionID& sessionID) throw(FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::UnsupportedMessageType) {

	crack(message, sessionID);
	//std::cout << std::endl << "<<< IN: " << message << std::endl;
	std::cout << std::endl << "<<< App - " << sessionID << ": " << message.toString() << std::endl;
}

void Application::toApp(FIX::Message& message, const FIX::SessionID& sessionID) throw(FIX::DoNotSend) {

	try {

		FIX::PossDupFlag possDupFlag;
		message.getHeader().getField(possDupFlag);
		if (possDupFlag) throw FIX::DoNotSend();
	}
	catch (FIX::FieldNotFound&) {
	
		std::cout << std::endl << ">>> App - Field not found" << std::endl;
	}
	std::cout << std::endl << ">>> App - " << sessionID << ": " << message.toString() << std::endl;
}

void Application::onMessage( const FIX44::MarketDataSnapshotFullRefresh& message, const FIX::SessionID& sessionID) {

	if (!ticks_file_.is_open())
        ticks_file_.open("ticks.BTCCNY.csv", std::ofstream::out | std::ofstream::app);

	FIX::NoMDEntries mde_count;
	message.get(mde_count);
	if (mde_count) {

		TickLine tick(ticks_file_);

		for (int i = 1; i <= mde_count; ++i) {

			FIX44::MarketDataSnapshotFullRefresh::NoMDEntries group;
			message.getGroup(i, group);

			FIX::MDEntryType mde_type;
			group.get(mde_type);

			FIX::MDEntryDate mde_date;
			FIX::MDEntryTime mde_time;
			FIX::MDEntryPx  mde_price;
			bool has_date = group.getIfSet(mde_date);
			bool has_time = group.getIfSet(mde_time);
			bool has_price = group.getIfSet(mde_price);

			tick.setTick(mde_date, mde_time, mde_type, mde_price);
		}
	}
	std::cout << std::endl << "<<< Message [MarketDataSnapshotFullRefresh] - " << sessionID << ": " << message.toString() << std::endl;
}

void Application::onMessage( const FIX44::MarketDataIncrementalRefresh& message, const FIX::SessionID& sessionID) {

	if (!ticks_file_.is_open())
        ticks_file_.open("ticks.BTCCNY.csv", std::ofstream::out | std::ofstream::app);

	FIX::NoMDEntries mde_count;
	message.get(mde_count);
	if (mde_count) {

		TickLine tick(ticks_file_);

		for (int i = 1; i <= mde_count; ++i) {

			FIX44::MarketDataIncrementalRefresh::NoMDEntries group;
			message.getGroup(i, group);

			FIX::MDEntryType mde_type;
			group.get(mde_type);

			FIX::MDEntryDate mde_date;
			FIX::MDEntryTime mde_time;
			FIX::MDEntryPx  mde_price;
			bool has_date = group.getIfSet(mde_date);
			bool has_time = group.getIfSet(mde_time);
			bool has_price = group.getIfSet(mde_price);

			tick.setTick(mde_date, mde_time, mde_type, mde_price);
		}
	}
	std::cout << std::endl << "<<< Message [MarketDataIncrementalRefresh] - " << sessionID << ": " << message.toString() << std::endl;
}

void Application::TickLine::write() {

	if (bid_ || ask_) {

		file_ << date_ << "," << time_ << ",,,,,,";

		if (bid_) { file_ << bid_; bid_ = 0; }
		file_ << ",,,,,,,";

		if (ask_) { file_ << ask_; ask_ = 0; }
		file_ << std::endl;
	}
}


void Application::TickLine::setTick(const FIX::MDEntryDate& tick_date, const FIX::MDEntryTime& tick_time,
	const FIX::MDEntryType& tick_type, const FIX::MDEntryPx& tick_price) {

	if (tick_date != date_ || tick_time != time_) {

		write();
		date_ = tick_date;
		time_ = tick_time;
	}
	switch (tick_type) {
		case '0': if (bid_.getLength()) write(); bid_ = tick_price; break; // bid
		case '1': if (ask_.getLength()) write(); ask_ = tick_price; break; // offer
		default: break;
	}
}

void Application::onMessage( const FIX44::MarketDataRequestReject& message, const FIX::SessionID& sessionID) {

  std::cout << std::endl << "<<< Message [MarketDataRequestReject] - " << sessionID << ": " << message.toString() << std::endl;
}

void Application::onMessage(const FIX44::BusinessMessageReject& message, const FIX::SessionID& sessionID) {

	FIX::BusinessRejectRefID reject_ref_id;
	FIX::BusinessRejectReason reject_reason;
	message.get(reject_ref_id);
	message.get(reject_reason);
	std::cout << std::endl << "<<< BusinessMessageReject - " << sessionID << " [" << reject_ref_id << "]: " << reject_reason << std::endl;
}

void Application::onMessage(const FIX44::ExecutionReport& message, const FIX::SessionID& sessionID) {

	std::cout << std::endl << "<<< ExecutionReport - " << sessionID << ": " << message.toString() << std::endl;
}

void Application::onMessage(const FIX44::OrderCancelReject& message, const FIX::SessionID& sessionID) {

	std::cout << std::endl << "<<< OrderCancelReject - " << sessionID << ": " << message.toString() << std::endl;
}

void Application::run() {

    unsigned short port = settings_.get().getInt("TradingServicePort");
    webserver_.start(port);
	for (;;) {

		try {
			if (!data_active_ && Connected()) {

			    data_active_ = true;

                FIX::Message md = queryMarketDataRequest44();
                FIX::Session::sendToTarget(md);
            }
		}
		catch (std::exception & e) {

			std::cout << "Message Not Sent: " << e.what();
		}
#ifdef _MSC_VER
		Sleep(50);
#else
		usleep(50000);
#endif
	}
    webserver_.stop();
}
/*
void Application::queryMarketDataRequest() {

	FIX::Message md = queryMarketDataRequest44();
	FIX::Session::sendToTarget(md);
}
*//*
void Application::queryEnterOrder() {

	FIX::Message order = queryNewOrderSingle44();
	FIX::Session::sendToTarget(order);
}

void Application::queryCancelOrder() {

	FIX::Message cancel = queryOrderCancelRequest44();
	FIX::Session::sendToTarget(cancel);
}

void Application::queryReplaceOrder() {

	FIX::Message replace = queryCancelReplaceRequest44();
	FIX::Session::sendToTarget(replace);
}

FIX44::NewOrderSingle Application::queryNewOrderSingle44() {

	FIX::OrdType ordType;

	FIX44::NewOrderSingle newOrderSingle(
		queryClOrdID(),
		querySide(),
		FIX::TransactTime(),
		ordType = queryOrdType()
	);
	newOrderSingle.set(FIX::HandlInst('1'));
	newOrderSingle.set(querySymbol());
	newOrderSingle.set(queryOrderQty());
	newOrderSingle.set(queryTimeInForce());

	if (ordType == FIX::OrdType_LIMIT || ordType == FIX::OrdType_STOP_LIMIT) newOrderSingle.set(queryPrice());
	if (ordType == FIX::OrdType_STOP || ordType == FIX::OrdType_STOP_LIMIT) newOrderSingle.set(queryStopPx());

	queryHeader(newOrderSingle.getHeader());
	return newOrderSingle;
}

FIX44::OrderCancelRequest Application::queryOrderCancelRequest44() {

	FIX44::OrderCancelRequest orderCancelRequest(queryOrigClOrdID(), queryClOrdID(), querySide(), FIX::TransactTime());
	orderCancelRequest.set(querySymbol());
	orderCancelRequest.set(queryOrderQty());

	queryHeader(orderCancelRequest.getHeader());
	return orderCancelRequest;
}

FIX44::OrderCancelReplaceRequest Application::queryCancelReplaceRequest44() {

	FIX44::OrderCancelReplaceRequest cancelReplaceRequest(
		queryOrigClOrdID(),
		queryClOrdID(),
		querySide(),
		FIX::TransactTime(),
		queryOrdType()
	);
	cancelReplaceRequest.set(FIX::HandlInst('1'));
	cancelReplaceRequest.set(querySymbol());

	//cancelReplaceRequest.set(queryPrice());
	//cancelReplaceRequest.set(queryOrderQty());

	queryHeader(cancelReplaceRequest.getHeader());
	return cancelReplaceRequest;
}
*/

