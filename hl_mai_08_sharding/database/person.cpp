#include "person.h"
#include "database.h"
#include "../config/config.h"

#include <Poco/Data/MySQL/Connector.h>
#include <Poco/Data/MySQL/MySQLException.h>
#include <Poco/Data/SessionFactory.h>
#include <Poco/Data/RecordSet.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Dynamic/Var.h>

#include <sstream>
#include <exception>
#include <fstream>

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
        Poco::Data::Session session = database::Database::get().create_session();

        for (auto &hint : database::Database::get_all_hints())
        {
            Statement drop_stmt(session);
            drop_stmt << "DROP TABLE IF EXISTS Users" << hint, now;

            std::cout << hint << std::endl;        
            Statement create_stmt(session);
            create_stmt << "CREATE TABLE IF NOT EXISTS `Users` ("
                    << "    u_id int NOT NULL AUTO_INCREMENT, "
                    << "    first_name VARCHAR(30), "
                    << "    login VARCHAR(30) NOT NULL UNIQUE, "
                    << "    last_name VARCHAR(30), "
                    << "    age int, "
                    << "    PRIMARY KEY (u_id)"
                    << ");"
                    << hint,
                now;
        }

        try {
            for (auto &hint : database::Database::get_all_hints())
            {
                Poco::Data::Statement insert(session);
                insert << "INSERT INTO Users (first_name,last_name,login,age) VALUES('Fedor', 'Penin', 'lulex.py', '23'), ('NeFedor', 'Penin', 'Nelulex.py', '23'); " << hint;
                insert.execute();
            }
            return "true";
        } catch (Poco::Data::MySQL::ConnectionException &e) {
            return e.displayText();
        } catch (Poco::Data::MySQL::StatementException &e) {
            return e.displayText();
        }
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

    Poco::JSON::Object::Ptr Person::toJSON() const
    {
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();

        root->set("id", _id);
        root->set("first_name", _first_name);
        root->set("last_name", _last_name);
        root->set("age", _age);
        root->set("login", _login);

        return root;
    }

    Person Person::fromJSON(const std::string &str)
    {
        Person author;
        Poco::JSON::Parser parser;
        Poco::Dynamic::Var result = parser.parse(str);
        Poco::JSON::Object::Ptr object = result.extract<Poco::JSON::Object::Ptr>();

        author.id() = object->getValue<long>("id");
        author.first_name() = object->getValue<std::string>("first_name");
        author.last_name() = object->getValue<std::string>("last_name");
        author.login() = object->getValue<std::string>("login");
        author.age() = object->getValue<int>("age");

        return author;
    }

    std::vector<Person> Person::read_all()
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            std::vector<Person> result;

            for (auto &hint : database::Database::get_all_hints())
            {
                Statement select(session);
                Person a;
                select << "SELECT u_id, first_name, last_name, login, age FROM Users" << hint,
                    into(a._id),
                    into(a._first_name),
                    into(a._last_name),
                    into(a._login),
                    into(a._age),
                    range(0, 1); //  iterate over result set one row at a time

                while (!select.done())
                {
                    if (select.execute())
                        result.push_back(a);
                }
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

    Person Person::find_by_login(std::string login) 
    {
        try
        {
            std::string hint = database::Database::sharding_hint(login);

            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement select(session);
            Person a;
            select << "SELECT u_id, first_name, last_name, login, age FROM Users where login=?",
                into(a._id),
                into(a._first_name),
                into(a._last_name),
                into(a._login),
                into(a._age),
                use(login),
                range(0, 1);

            select << hint;
  
            select.execute();

            std::cout << "searching in: " << hint << std::endl;

            Poco::Data::RecordSet rs(select);
            if (!rs.moveFirst()) throw std::logic_error("not found");
    
            return a;
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

    std::vector<Person> Person::search(std::string first_name, std::string last_name)
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            std::vector<Person> result;

            for (auto &hint : database::Database::get_all_hints())
            {
                Statement select(session);
                Person a;
                first_name += "%";
                last_name += "%";
                select << "SELECT u_id, first_name, last_name, age, login FROM Users where first_name LIKE ? and last_name LIKE ?",
                    into(a._id),
                    into(a._first_name),
                    into(a._last_name),
                    into(a._age),
                    into(a._login),
                    use(first_name),
                    use(last_name),
                    range(0, 1); //  iterate over result set one row at a time

                select << hint;
                
                while (!select.done())
                {
                    if (select.execute())
                        result.push_back(a);
                }
            }
           
            return result;
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

    void Person::save_to_mysql()
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement insert(session);

            std::string hint = database::Database::sharding_hint(_login);

            std::cout << hint << std::endl;

            insert << "INSERT INTO Users (first_name,last_name,login,age) VALUES(?, ?, ?, ?)",
                use(_first_name),
                use(_last_name),
                use(_login),
                use(_age);

            insert << hint;

            insert.execute();

            Poco::Data::Statement select(session);
            select << "SELECT LAST_INSERT_ID()",
                into(_id),
                range(0, 1); //  iterate over result set one row at a time

            if (!select.done())
            {
                select.execute();
            }
            std::cout << "inserted:" << _id << std::endl;
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

    long Person::get_id() const
    {
        return _id;
    }

    const std::string &Person::get_first_name() const
    {
        return _first_name;
    }

    const std::string &Person::get_last_name() const
    {
        return _last_name;
    }

    const int &Person::get_age() const
    {
        return _age;
    }


    long &Person::id()
    {
        return _id;
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

    const std::string &Person::get_login() const
    {
        return _login;
    }


    int &Person::age()
    {
        return _age;
    }
}