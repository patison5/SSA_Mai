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

using namespace Poco::Data::Keywords;
using Poco::Data::Session;
using Poco::Data::Statement;

namespace database
{
    void Person::init()
    {
       std::cout << "Инициализируем таблицу для Person" << std::endl;

       try
        {
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
            } catch (Poco::Data::MySQL::ConnectionException &e) {
                std::cout << "connection:" << e.displayText() << std::endl;
                throw;
            } catch (Poco::Data::MySQL::StatementException &e) {
                std::cout << "statement:" << e.displayText() << std::endl;
                throw;
            }

            
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
            std::cout << "connection:" << e.what() << std::endl;
            throw;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {
            std::cout << "statement:" << e.what() << std::endl;
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
}