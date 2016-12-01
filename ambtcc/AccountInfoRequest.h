#ifndef FIXBTCC_ACCOUNTINFOREQUEST_H__
#define FIXBTCC_ACCOUNTINFOREQUEST_H__

#include "quickfix/FieldTypes.h"
#include "quickfix/Message.h"

namespace FIX {

    namespace FIELD {

        const int AccReqID = 8000;
        const int SubAccountInfoRequestType = 9263;
    }

    DEFINE_STRING(AccReqID);
    DEFINE_CHAR(SubAccountInfoRequestType);

    const char MsgType_AccountInfoRequest[] = "U1000";

    class AccountInfoRequest : public FIX::Message {
    public:

        AccountInfoRequest() : FIX::Message(FIX::MsgType()) {}
        AccountInfoRequest(const FIX::Message& m) : Message(m) {}
//        AccountInfoRequest(const Message& m) : Message(m) {}
        AccountInfoRequest(const AccountInfoRequest& m) : Message(m) {}
        static FIX::MsgType MsgType() { return FIX::MsgType(MsgType_AccountInfoRequest); }

        AccountInfoRequest(
            const FIX::Account& account,
            const AccReqID& request_id,
            const SubAccountInfoRequestType& subscribe) : Message(MsgType()) {
            set(account);
            set(request_id);
            set(subscribe);
        }

        FIELD_SET(*this, FIX::Account);
        FIELD_SET(*this, AccReqID);
        FIELD_SET(*this, SubAccountInfoRequestType);
    };

}

#endif // FIXBTC_ACCOUNTINFOREQUEST_H__