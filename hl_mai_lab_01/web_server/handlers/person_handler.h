#ifndef PERSONHANDLER_H
#define PERSONHANDLER_H

#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTMLForm.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Timestamp.h"
#include "Poco/DateTimeFormatter.h"
#include "Poco/DateTimeFormat.h"
#include "Poco/Exception.h"
#include "Poco/ThreadPool.h"
#include "Poco/Util/ServerApplication.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/HelpFormatter.h"
#include <iostream>
#include <iostream>
#include <fstream>

using Poco::DateTimeFormat;
using Poco::DateTimeFormatter;
using Poco::ThreadPool;
using Poco::Timestamp;
using Poco::Net::HTMLForm;
using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPRequestHandlerFactory;
using Poco::Net::HTTPServer;
using Poco::Net::HTTPServerParams;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;
using Poco::Net::NameValueCollection;
using Poco::Net::ServerSocket;
using Poco::Util::Application;
using Poco::Util::HelpFormatter;
using Poco::Util::Option;
using Poco::Util::OptionCallback;
using Poco::Util::OptionSet;
using Poco::Util::ServerApplication;

#include "../../database/author.h"
#include "../../database/person.h"

class PersonHandler : public HTTPRequestHandler
{
public:
    PersonHandler(const std::string &format) : _format(format)
    {
    }

    void handleRequest(HTTPServerRequest &request,
                       HTTPServerResponse &response)
    {
        HTMLForm form(request, request.stream());
        response.setChunkedTransferEncoding(true);
        response.setContentType("application/json");
        std::ostream &ostr = response.send();

        if (form.has("init"))
        {
            database::Person::init();
            ostr << "{ \"result\": true , \"message\": \"Database inited\" }";
            return;
        } else if (form.has("add"))
        {
            if ((form.has("login")) && 
                (form.has("first_name")) && 
                (form.has("last_name")) && 
                (form.has("age"))) {
                
                database::Person person;
                person.first_name() = form.get("first_name");
                person.last_name()  = form.get("last_name");
                person.login()      = form.get("login");
                person.age()        = atol(form.get("age").c_str());

                try
                {
                    person.save_to_mysql();
                    ostr << "{ \"result\": true }";
                    return;
                }
                catch (...)
                {
                    ostr << "{ \"result\": false , \"reason\": \" database error\" }";
                    return;
                }
            }

            ostr << "{ \"result\": true, \"message\": \"New Person added\" }";
            return;
        } else if (form.has("search")) {
            try
            {
                std::string  fn = form.get("first_name");
                std::string  ln = form.get("last_name");
                auto results = database::Person::search(fn,ln);
                Poco::JSON::Array arr;
                for (auto s : results)
                    arr.add(s.toJSON());
                Poco::JSON::Stringifier::stringify(arr, ostr);
            }
            catch (...)
            {
                ostr << "{ \"result\": false , \"reason\": \"not gound\" }";
                return;
            }
            return;

        } else if (form.has("id")) {
            long id = atol(form.get("id").c_str());

            std::cout << id << std::endl;

            try
            {
                database::Person result = database::Person::read_by_id(id);
                Poco::JSON::Stringifier::stringify(result.toJSON(), ostr);
                return;
            }
            catch (...)
            {
                ostr << "{ \"result\": false , \"reason\": \"not found\" }";
                return;
            }
        } else if (form.has("login"))
        {
            std::string login = form.get("login");

            try
            {
                database::Person result = database::Person::find_by_login(login);
                Poco::JSON::Stringifier::stringify(result.toJSON(), ostr);
                return;
            }
            catch (...)
            {
                ostr << "{ \"result\": false , \"reason\": \"not found\" }";
                return;
            } 

            return;
        } 

        auto results = database::Person::read_all();
        Poco::JSON::Array arr;
        for (auto s : results)
            arr.add(s.toJSON());
        Poco::JSON::Stringifier::stringify(arr, ostr);
    }

private:
    std::string _format;
};
#endif // !PERSONHANDLER_H