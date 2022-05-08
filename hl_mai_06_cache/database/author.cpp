#include "author.h"
#include "database.h"
#include "cache.h"
#include "../config/config.h"

#include <Poco/Data/MySQL/Connector.h>
#include <Poco/Data/MySQL/MySQLException.h>
#include <Poco/Data/SessionFactory.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Dynamic/Var.h>

#include <sstream>
#include <exception>

using namespace Poco::Data::Keywords;
using Poco::Data::Session;
using Poco::Data::Statement;

namespace database
{

    void Author::init()
    {
        try
        {

            Poco::Data::Session session = database::Database::get().create_session();
            //*
            Statement drop_stmt(session);
            drop_stmt << "DROP TABLE IF EXISTS Author", now;
            //*/

            // (re)create table
            Statement create_stmt(session);
            create_stmt << "CREATE TABLE IF NOT EXISTS `Author` (`id` INT NOT NULL AUTO_INCREMENT,"
                        << "`first_name` VARCHAR(256) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL,"
                        << "`last_name` VARCHAR(256) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL,"
                        << "`email` VARCHAR(256) CHARACTER SET utf8 COLLATE utf8_unicode_ci NULL,"
                        << "`title` VARCHAR(1024) CHARACTER SET utf8 COLLATE utf8_unicode_ci NULL,"
                        << "PRIMARY KEY (`id`),KEY `fn` (`first_name`),KEY `ln` (`last_name`));",
                now;
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

    Poco::JSON::Object::Ptr Author::toJSON() const
    {
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();

        root->set("id", _id);
        root->set("first_name", _first_name);
        root->set("last_name", _last_name);
        root->set("email", _email);
        root->set("title", _title);

        return root;
    }

    Author Author::fromJSON(const std::string &str)
    {
        Author author;
        Poco::JSON::Parser parser;
        Poco::Dynamic::Var result = parser.parse(str);
        Poco::JSON::Object::Ptr object = result.extract<Poco::JSON::Object::Ptr>();

        author.id() = object->getValue<long>("id");
        author.first_name() = object->getValue<std::string>("first_name");
        author.last_name() = object->getValue<std::string>("last_name");
        author.email() = object->getValue<std::string>("email");
        author.title() = object->getValue<std::string>("title");

        return author;
    }

    Author Author::read_by_id(long id)
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement select(session);
            Author a;
            select << "SELECT id, first_name, last_name, email, title FROM Author where id=?",
                into(a._id),
                into(a._first_name),
                into(a._last_name),
                into(a._email),
                into(a._title),
                use(id),
                range(0, 1); //  iterate over result set one row at a time

            if (!select.done())
            if (!select.execute()) throw std::logic_error("Item not found");

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

    Author Author::read_from_cache_by_id(long id)
    {

        try
        {
            std::string result;
            if (database::Cache::get().get(id, result))
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
    void Author::warm_up_cache()
    {
        std::cout << "wharming up authors cache ...";
        auto array = read_all();
        long count = 0;
        for (auto &a : array)
        {
            a.save_to_cache();
            ++count;
        }
        std::cout << "done: " << count << std::endl;
    }


    size_t Author::size_of_cache(){
        return database::Cache::get().size();
    }

    void Author::save_to_cache()
    {
        std::stringstream ss;
        Poco::JSON::Stringifier::stringify(toJSON(), ss);
        std::string message = ss.str();
        database::Cache::get().put(_id, message);
    }

    std::vector<Author> Author::read_all()
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Statement select(session);
            std::vector<Author> result;
            Author a;
            select << "SELECT id, first_name, last_name, email, title FROM Author",
                into(a._id),
                into(a._first_name),
                into(a._last_name),
                into(a._email),
                into(a._title),
                range(0, 1); //  iterate over result set one row at a time

            while (!select.done())
            {
                if(select.execute())  result.push_back(a);
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

    std::vector<Author> Author::search(std::string first_name, std::string last_name)
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Statement select(session);
            std::vector<Author> result;
            Author a;
            first_name+="%";
            last_name+="%";
            select << "SELECT id, first_name, last_name, email, title FROM Author where first_name LIKE ? and last_name LIKE ?",
                into(a._id),
                into(a._first_name),
                into(a._last_name),
                into(a._email),
                into(a._title),
                use(first_name),
                use(last_name),
                range(0, 1); //  iterate over result set one row at a time

            while (!select.done())
            {
                if(select.execute()) result.push_back(a);
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

   
    void Author::save_to_mysql()
    {

        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement insert(session);

            insert << "INSERT INTO Author (first_name,last_name,email,title) VALUES(?, ?, ?, ?)",
                use(_first_name),
                use(_last_name),
                use(_email),
                use(_title),
                now;

            //insert.execute();

            Poco::Data::Statement select(session);
            select << "SELECT LAST_INSERT_ID()",
                into(_id),
                range(0, 1); //  iterate over result set one row at a time

            if (!select.done())  select.execute();
            
            std::cout << "inserted:" << _id << std::endl;
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

    long Author::get_id() const
    {
        return _id;
    }

    const std::string &Author::get_first_name() const
    {
        return _first_name;
    }

    const std::string &Author::get_last_name() const
    {
        return _last_name;
    }

    const std::string &Author::get_email() const
    {
        return _email;
    }

    const std::string &Author::get_title() const
    {
        return _title;
    }

    long &Author::id()
    {
        return _id;
    }

    std::string &Author::first_name()
    {
        return _first_name;
    }

    std::string &Author::last_name()
    {
        return _last_name;
    }

    std::string &Author::email()
    {
        return _email;
    }

    std::string &Author::title()
    {
        return _title;
    }
}