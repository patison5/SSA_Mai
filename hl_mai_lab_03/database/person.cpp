#include "person.h"
#include "database.h"
#include "cache.h"
#include "../config/config.h"

#include <Poco/Data/MySQL/Connector.h>
#include <Poco/Data/MySQL/MySQLException.h>
#include <Poco/Data/SessionFactory.h>
#include <Poco/Data/RecordSet.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Dynamic/Var.h>

#include <sstream>
#include <exception>

using namespace Poco::Data::Keywords;
using Poco::Data::Session;
using Poco::Data::Statement;
using Poco::Net::HTMLForm;

namespace database
{
    Person Person::read_from_cache_by_login(std::string login)
    {
        try
        {
            std::string result;
            size_t data = std::hash<std::string>{}(login);
            long id = static_cast<long>(data);

            if (database::Cache::get().get(Cache::CacheType::persons, id, result))
                return fromJSON(result);
            else
                throw std::logic_error("key not found in the cache");
        }
        catch (std::exception* err)
        {
            std::cerr << "error:" << err->what() << std::endl;
            throw;
        }
    }

    void Person::warm_up_cache()
    {
        std::cout << "wharming up persons cache ...";
        auto array = read_all();
        long count = 0;
        for (auto &a : array)
        {
            a.save_to_cache();
            ++count;
        }
        std::cout << "done: " << count << std::endl;
    }


    size_t Person::size_of_cache(){
        return database::Cache::get().size(Cache::CacheType::persons);
    }

    void Person::save_to_cache()
    {
        std::stringstream ss;
        Poco::JSON::Stringifier::stringify(this->toJSON(), ss);
        std::string message = ss.str();

        size_t data = std::hash<std::string>{}(_login);
        long id = static_cast<long>(data);

        database::Cache::get().put(Cache::CacheType::persons, id, message);
    }


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
        Poco::Data::Session session = database::Database::get().create_session();
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

        person.u_id() = object->getValue<long>("id");
        person.first_name() = object->getValue<std::string>("first_name");
        person.last_name() = object->getValue<std::string>("last_name");
        person.age() = object->getValue<int>("age");
        person.login() = object->getValue<std::string>("login");

        return person;
    }

    Person Person::read_by_id(long id)
    {
       try
        {
            Poco::Data::Session session = database::Database::get().create_session();
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
            Poco::Data::Session session = database::Database::get().create_session();
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
            Poco::Data::Session session = database::Database::get().create_session();
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
            Poco::Data::Session session = database::Database::get().create_session();
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
            Poco::Data::Session session = database::Database::get().create_session();
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

    long &Person::u_id()
    {
        return _u_id;
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
                person.save_to_cache();
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

        bool no_cache = false;
        if (form.has("no_cache"))
            no_cache = true;

        if (!no_cache) {
            try
            {
                database::Person result = database::Person::read_from_cache_by_login(login);
                std::cout << "item from cache:" << login << std::endl;
                Poco::JSON::Stringifier::stringify(result.toJSON(), ostr);
                return;
            }
            catch (...)
            {
                std::cout << "cache missed for login: " << login << std::endl;
            }
        }

        try
        {
            database::Person result = database::Person::find_by_login(login);
            if (!no_cache) {
                std::cout << "saving to cache: " << login << std::endl;
                result.save_to_cache();
            }
            Poco::JSON::Stringifier::stringify(result.toJSON(), ostr);
            return;
        }
        catch (...)
        {
            ostr << "{ \"result\": false , \"reason\": \"not found\" }";
            return;
        } 
    }
}