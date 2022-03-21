#include <string>
#include <iostream>
#include <fstream>

#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/HTTPServerParams.h"
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

#include <Poco/Data/MySQL/Connector.h>
#include <Poco/Data/MySQL/MySQLException.h>
#include <Poco/Data/SessionFactory.h>
#include <Poco/Data/RecordSet.h>

using Poco::DateTimeFormat;
using Poco::DateTimeFormatter;
using Poco::ThreadPool;
using Poco::Timestamp;
using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPRequestHandlerFactory;
using Poco::Net::HTTPServer;
using Poco::Net::HTTPServerParams;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;
using Poco::Net::ServerSocket;
using Poco::Util::Application;
using Poco::Util::HelpFormatter;
using Poco::Util::Option;
using Poco::Util::OptionCallback;
using Poco::Util::OptionSet;
using Poco::Util::ServerApplication;

class WebPageHandler : public HTTPRequestHandler
{
public:
    WebPageHandler(const std::string &format) : _format(format)
    {
    }

    void handleRequest(HTTPServerRequest &request,
                       HTTPServerResponse &response)
    {
        std::cout << "webpage request" << std::endl;
        response.setChunkedTransferEncoding(true);
        response.setContentType("text/html");

        std::ostream &ostr = response.send();
        std::ifstream file;

        auto pos = request.getURI().find('?');
        std::string uri = request.getURI();
        if (pos != std::string::npos)
            uri = uri.substr(0, pos);
        std::string name = "content" + uri;


        std::cout << name << std::endl;

        file.open(name, std::ifstream::binary);

        if (file.is_open())
            while (file.good())
            {
                int sign = file.get();
                if (sign > 0)
                    ostr << (char)sign;
            }

        file.close();
    }

private:
    std::string _format;
};

#include "Poco/JSON/Object.h"
#include "Poco/Net/HTMLForm.h"
class RequestHandler : public HTTPRequestHandler
{
public:
    RequestHandler(const std::string &format) : _format(format)
    {
    }

    void handleRequest(HTTPServerRequest &request,
                       HTTPServerResponse &response)
    {
        //std::cout << "handle request" << std::endl;
        Poco::Net::HTMLForm form(request);
        response.setChunkedTransferEncoding(true);
        response.setContentType("application/json");
        std::ostream &ostr = response.send();
        
        if (form.has("session_id"))
        {
            std::string session_str = form.get("session_id");
            //std::cout << "session_id:" << session_str << std::endl;
                try
                {
                    Poco::JSON::Array arr;
                    Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
                    root->set("id", session_str);
                    root->set("some_text", "Some Text");
                    arr.add(root);
                    root = new Poco::JSON::Object();
                    root->set("id", session_str);
                    root->set("some_text", "Another Text");
                    arr.add(root);
                    Poco::JSON::Stringifier::stringify(arr, ostr);
                }
                catch (...)
                {
                    std::cout << "exception" << std::endl;
                }
        }

        if (form.has("login"))
        {
            std::string login = form.get("login");
            try {
                    Poco::JSON::Array arr;
                    Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
                    root->set("login", login);
                    root->set("some_text", "Some Text");
                    arr.add(root);
                    root = new Poco::JSON::Object();
                    root->set("login", login);
                    root->set("some_text", "Another Text");
                    arr.add(root);
                    Poco::JSON::Stringifier::stringify(arr, ostr);
            } catch(...) {

            }
        }

        Poco::Data::MySQL::Connector::registerConnector();
        std::cout << "connector registered" << std::endl;

        std::string connection_str;
        connection_str += "host=localhost;user=stud;db=stud;password=stud";

        Poco::Data::Session session(
            Poco::Data::SessionFactory::instance().create(
                Poco::Data::MySQL::Connector::KEY, 
                connection_str
            )
        );

        try
        {
            Poco::Data::Statement create_stmt(session);

        //    create_stmt  << "CREATE TABLE IF NOT EXISTS Users ("
        //                 << "u_id int NOT NULL AUTO_INCREMENT, "
        //                 << "login VARCHAR(30) NOT NULL UNIQUE, "
        //                 << "first_name VARCHAR(30), "
        //                 << "last_name VARCHAR(30), "
        //                 << "age int,"
        //                 << "PRIMARY KEY (u_id)"
        //                 << ");";

        //     create_stmt.execute();
        //     std::cout << "table created" << std::endl;

            Poco::Data::Statement insert(session);

            insert << "INSERT INTO Users (login, first_name, last_name, age) VALUES('patison55', 'Fedor', 'Penin', 23)";
            
            insert.execute();

            // Poco::Data::Statement select(session);
            // long id;
            // select << "SELECT id, first_name, last_name, email, title FROM Author",
            //     Poco::Data::Keywords::into(id),
            //     Poco::Data::Keywords::into(first_name),
            //     Poco::Data::Keywords::into(last_name),
            //     Poco::Data::Keywords::into(email),
            //     Poco::Data::Keywords::into(title),
            //     Poco::Data::Keywords::range(0, 1); //  iterate over result set one row at a time


            // while (!select.done())
            // {
            //     if(select.execute()){
            //     std::cout << id << ":[" << first_name << "," << last_name << "," << email << "," << title << "]" << std::endl;
            //     }
            // };
        }
        catch (Poco::Data::MySQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl << e.displayText() << std::endl;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {
            std::cout << "statement:" << e.what() << std::endl << e.displayText() << std::endl;
        }
        catch (std::exception *ex)
        {
            std::cout << "exception:" << ex->what() << std::endl;
        }
        // response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_INTERNAL_SERVER_ERROR);
    }

private:
    std::string _format;
};


class PersonHandler : public HTTPRequestHandler
{
public:
    PersonHandler(const std::string &format) : _format(format)
    {
    }

    void handleRequest(HTTPServerRequest &request,
                       HTTPServerResponse &response)
    {
        Poco::Net::HTMLForm form(request);
        response.setChunkedTransferEncoding(true);
        response.setContentType("application/json");
        std::ostream &ostr = response.send();

        Poco::Data::MySQL::Connector::registerConnector();
        std::cout << "connector registered" << std::endl;

        std::string connection_str;
        connection_str += "host=localhost;user=stud;db=stud;password=stud";

        Poco::Data::Session session(
            Poco::Data::SessionFactory::instance().create(
                Poco::Data::MySQL::Connector::KEY, 
                connection_str
            )
        );

        if ( form.has("login") && form.has("firstname") && form.has("lastname") && form.has("age") ) {
            std::string login       { form.get("login")     };
            std::string firstname   { form.get("firstname") };
            std::string lastname    { form.get("lastname")  };
            std::string age         { form.get("age")       };

            try
            {
                Poco::Data::Statement insert(session);

                insert << "INSERT INTO Users (login, first_name, last_name, age) VALUES(?, ?, ?, ?)",
                    Poco::Data::Keywords::use(login),
                    Poco::Data::Keywords::use(firstname),
                    Poco::Data::Keywords::use(lastname),
                    Poco::Data::Keywords::use(age);

                std::cout << "добавляем юзверя" << std::endl;

                insert.execute();
            }
            catch (Poco::Data::MySQL::ConnectionException &e)
            {
                std::cout << "connection:" << e.what() << std::endl << e.displayText() << std::endl;
                sendErrorMessage(400, e.displayText(), ostr);
                return;
            }
            catch (Poco::Data::MySQL::StatementException &e)
            {
                std::cout << "statement:" << e.what() << std::endl << e.displayText() << std::endl;
                sendErrorMessage(400, e.displayText(), ostr);
                return;
            }
            catch (std::exception *ex)
            {
                std::cout << "exception:" << ex->what() << std::endl;
                sendErrorMessage(400, ex->what(), ostr);
                return;
            }

            try {
                    Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
                    root->set("status", "200");
                    root->set("login", login);
                    root->set("firstname", firstname);
                    root->set("lastname", lastname);
                    root->set("age", age);

                    Poco::JSON::Stringifier::stringify(root, ostr);
            } catch(...) {

            }
        }
    }

private:
    std::string _format;

    void sendErrorMessage(int status, std::string description, std::ostream &ostr)
    {
        try {
            Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
            root->set("status", status);
            root->set("desc", description);

            Poco::JSON::Stringifier::stringify(root, ostr);
        } catch(...) {

        }
    }
};



static bool startsWith(const std::string &str, const std::string &prefix)
{
    return str.size() >= prefix.size() && 0 == str.compare(0, prefix.size(), prefix);
}

class HTTPRequestFactory : public HTTPRequestHandlerFactory
{
public:
    HTTPRequestFactory(const std::string &format) : _format(format)
    {
    }

    HTTPRequestHandler *createRequestHandler([[maybe_unused]] const HTTPServerRequest &request)
    {
        std::string math = "/request";
        if (startsWith(request.getURI(), math))
            return new RequestHandler(_format);
        else if (startsWith(request.getURI(), "/Person"))
            return new PersonHandler(_format);
        else
            return new WebPageHandler(_format);
    }

private:
    std::string _format;
};

class HTTPWebServer : public Poco::Util::ServerApplication
{
public:
    HTTPWebServer() : _helpRequested(false)
    {
    }

    ~HTTPWebServer()
    {
    }

protected:
    void initialize(Application &self)
    {
        loadConfiguration();
        ServerApplication::initialize(self);
    }

    void uninitialize()
    {
        ServerApplication::uninitialize();
    }

    void defineOptions(OptionSet &options)
    {
        ServerApplication::defineOptions(options);
    }

    int main([[maybe_unused]] const std::vector<std::string> &args)
    {
        if (!_helpRequested)
        {
            unsigned short port = (unsigned short)
                                      config()
                                          .getInt("HTTPWebServer.port", 8080);
            std::string format(
                config().getString("HTTPWebServer.format",
                                   DateTimeFormat::SORTABLE_FORMAT));

            ServerSocket svs(Poco::Net::SocketAddress("0.0.0.0", port));
            HTTPServer srv(new HTTPRequestFactory(format),
                           svs, new HTTPServerParams);

            std::cout << "Started server on port: 8080" << std::endl;
            srv.start();

            waitForTerminationRequest();
            srv.stop();
        }
        return Application::EXIT_OK;
    }

private:
    bool _helpRequested;
};

int main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[])
{
    HTTPWebServer app;

    std::cout << "Starting server at port 8080 ..." << std::endl;
    return app.run(argc, argv);
    
}