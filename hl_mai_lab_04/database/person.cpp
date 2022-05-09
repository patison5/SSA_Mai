#include "person.h"
#include "database.h"

#include "../config/config.h"
#include <Poco/Data/MySQL/Connector.h>
#include <Poco/Data/MySQL/MySQLException.h>
#include <Poco/Data/SessionFactory.h>
#include <Poco/Data/RecordSet.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Dynamic/Var.h>
#include <cppkafka/cppkafka.h>

#include <sstream>
#include <exception>

using namespace Poco::Data::Keywords;
using Poco::Data::Session;
using Poco::Data::Statement;
using Poco::Net::HTMLForm;

namespace database
{
    void Person::init(std::ostream &ostr)
    {
       try
        {
            std::string res = InitDefaultPersonTable();

            if (res == "true") {
                ostr << "{ \"result\": true, \"tableInited\": true, \"testUsersInited\": true }";
            } else {
                ostr << "{ \"result\": false , \"tableInited\": true, \"testUsersInited\": false, \"message\": \"" << res << "\" }";  
            }
        }

        catch (Poco::Data::MySQL::ConnectionException &e)
        {
            ostr << "{ \"result\": false , \"tableInited\": false, \"testUsersInited\": false, \"message\": \"" << e.displayText() << "\" }";
            return;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {
            ostr << "{ \"result\": false , \"tableInited\": false, \"testUsersInited\": false, \"message\": \"" << e.displayText() << "\" }";
            return;
        }
    }

    std::string Person::InitDefaultPersonTable() {
        Poco::Data::Session session = database::Database::get().create_session_write();
        Statement drop_stmt(session);
        drop_stmt << "DROP TABLE IF EXISTS Users", now;

        Statement create_stmt(session);
        create_stmt << "CREATE TABLE IF NOT EXISTS `Users` ("
                << "    u_id int NOT NULL AUTO_INCREMENT, "
                << "    first_name VARCHAR(30), "
                << "    login VARCHAR(30) NOT NULL UNIQUE, "
                << "    last_name VARCHAR(30), "
                << "    age int, "
                << "    PRIMARY KEY (u_id)"
                << ");",
            now;
        try {
            Poco::Data::Statement insert(session);
            insert << "INSERT INTO Users (first_name,last_name,login,age) VALUES('Fedor', 'Penin', 'lulex.py', '23'), ('NeFedor', 'Penin', 'Nelulex.py', '23'); ";
            insert.execute();
            return "true";
        } catch (Poco::Data::MySQL::ConnectionException &e) {
            return e.displayText();
        } catch (Poco::Data::MySQL::StatementException &e) {
            return e.displayText();
        }
    }

    Poco::JSON::Object::Ptr Person::toJSON() const
    {
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
        root->set("id", _u_id);
        root->set("first_name", _first_name);
        root->set("last_name", _last_name);
        root->set("login", _login);
        root->set("age", _age);

        return root;
    }

    Person Person::fromJSON(const std::string &str)
    {
        Person person; 
        Poco::JSON::Parser parser;
        Poco::Dynamic::Var result = parser.parse(str);
        Poco::JSON::Object::Ptr object = result.extract<Poco::JSON::Object::Ptr>();
        return person;
    }

    Person Person::read_by_id(long id)
    {
       try
        {
            Poco::Data::Session session = database::Database::get().create_session_write();
            Poco::Data::Statement select(session);
            Person a;
            select << "SELECT u_id, first_name, last_name, login, age FROM Users where u_id=?",
                into(a._u_id),
                into(a._first_name),
                into(a._last_name),
                into(a._login),
                into(a._age),
                use(id),
                range(0, 1); //  iterate over result set one row at a time
  
            select.execute();
            Poco::Data::RecordSet rs(select);
            if (!rs.moveFirst()) throw std::logic_error("not found");

            return a;
        }

        catch (Poco::Data::MySQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
            throw;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {

            std::cout << "statement:" << e.what() << std::endl;
            throw;
        }
    }

    Person Person::find_by_login(std::string login) 
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session_read();
            Poco::Data::Statement select(session);
            Person a;
            select << "SELECT u_id, first_name, last_name, login, age FROM Users where login=?",
                into(a._u_id),
                into(a._first_name),
                into(a._last_name),
                into(a._login),
                into(a._age),
                use(login),
                range(0, 1); //  iterate over result set one row at a time
  
            select.execute();
            Poco::Data::RecordSet rs(select);
            if (!rs.moveFirst()) throw std::logic_error("not found");

            return a;
        }

        catch (Poco::Data::MySQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
            throw;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {

            std::cout << "statement:" << e.what() << std::endl;
            throw;
        }
    }

    std::vector<Person> Person::read_all()
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session_read();
            Statement select(session);
            std::vector<Person> result;
            Person a;
            select << "SELECT u_id, first_name, last_name, login, age FROM Users",
                into(a._u_id),
                into(a._first_name),
                into(a._last_name),
                into(a._login),
                into(a._age),
                range(0, 1); //  iterate over result set one row at a time

            while (!select.done())
            {
                if(select.execute())
                result.push_back(a);
            }
            return result;
        }

        catch (Poco::Data::MySQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.displayText() << std::endl;
            throw;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {
            std::cout << "statement:" << e.displayText() << std::endl;
            throw;
        }
    }

    std::vector<Person> Person::search(std::string first_name, std::string last_name)
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session_read();
            Statement select(session);
            std::vector<Person> result;
            Person a;
            first_name+="%";
            last_name+="%";
            select << "SELECT u_id, first_name, last_name, login, age FROM Users where first_name LIKE ? and last_name LIKE ?",
                into(a._u_id),
                into(a._first_name),
                into(a._last_name),
                into(a._login),
                into(a._age),
                use(first_name),
                use(last_name),
                range(0, 1);

            while (!select.done())
            {
                if(select.execute())  result.push_back(a);
            }
            return result;
        }

        catch (Poco::Data::MySQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.displayText() << std::endl;
            throw;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {
            std::cout << "statement:" << e.displayText() << std::endl;
            throw;
        }

        catch (...) {
            std::cout << "some shit happens" << std::endl;
            throw;
        }
    }

   
    void Person::save_to_mysql()
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session_write();
            Poco::Data::Statement insert(session);

            insert << "INSERT INTO Users (first_name,last_name,login,age) VALUES(?, ?, ?, ?)",
                use(_first_name),
                use(_last_name),
                use(_login),
                use(_age);

            insert.execute();

            Poco::Data::Statement select(session);
            select << "SELECT LAST_INSERT_ID()",
                into(_u_id),
                range(0, 1);

            if (!select.done())
            {
                select.execute();
            }
            std::cout << "inserted:" << _u_id << std::endl;
        }
        catch (Poco::Data::MySQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.displayText() << std::endl;
            throw;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {

            std::cout << "statement:" << e.displayText() << std::endl;
            throw;
        }
    }

    long Person::get_u_id() const
    {
        return _u_id;
    }

    const std::string &Person::get_first_name() const
    {
        return _first_name;
    }

    const std::string &Person::get_last_name() const
    {
        return _last_name;
    }

    const std::string &Person::get_login() const
    {
        return _login;
    }

    const int &Person::get_age() const
    {
        return _age;
    }

    long &Person::u_id()
    {
        return _u_id;
    }

    std::string &Person::first_name()
    {
        return _first_name;
    }

    std::string &Person::last_name()
    {
        return _last_name;
    }

    std::string &Person::login()
    {
        return _login;
    }

    int &Person::age()
    {
        return _age;
    }

    void Person::AddNewPerson(std::ostream &ostr, HTMLForm &form) {
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
                static int i=0;
                person.send_to_queue();
                std::cout << "send to queue: " << std::to_string(++i)  << std::endl;
                ostr << "{ \"result\": true, \"message\": \"New Person added\" }";
                return;
            }
            catch (...)
            {
                ostr << "{ \"result\": false , \"reason\": \" database error\" }";
                return;
            }
        }
    }

    void Person::searchPerson(std::ostream &ostr, Poco::Net::HTMLForm &form) {
        try
        {
            std::string  fn = form.get("first_name");
            std::string  ln = form.get("last_name");

            auto results = search(fn,ln);
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
    }

    void Person::searchPersonById(std::ostream &ostr, Poco::Net::HTMLForm &form) {
        long id = atol(form.get("id").c_str());

        try
        {
            database::Person result = read_by_id(id);
            Poco::JSON::Stringifier::stringify(result.toJSON(), ostr);
            return;
        }
        catch (...)
        {
            ostr << "{ \"result\": false , \"reason\": \"not found\" }";
            return;
        }
    }


    void Person::searchByLogin(std::ostream &ostr, Poco::Net::HTMLForm &form) {
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
    }

    #include <mutex>
    void Person::send_to_queue()
    {
        static cppkafka::Configuration config = {
            {"metadata.broker.list", Config::get().get_queue_host()}};

        static cppkafka::Producer producer(config);
        static std::mutex mtx;
        std::lock_guard<std::mutex> lock(mtx);
        std::stringstream ss;
        Poco::JSON::Stringifier::stringify(toJSON(), ss);
        std::string message = ss.str();
        bool not_sent = true;
        while (not_sent)
        {
            try
            {
                producer.produce(cppkafka::MessageBuilder(Config::get().get_queue_topic()).partition(0).payload(message));
                //producer.flush(); // из-за этого тормозило
                not_sent = false;
            }
            catch (...)
            {
            }
        }
    }
}