#ifndef PERSON_H
#define PERSON_H

#include <string>
#include <vector>
#include "Poco/JSON/Object.h"
#include "Poco/Net/HTMLForm.h"

namespace database
{
    class Person{
        private:
            long _id;
            int _age;
            std::string _login;
            std::string _first_name;
            std::string _last_name;
            std::string _email;
            std::string _title;

        public:

            static Person fromJSON(const std::string & str);
            static void preload(const std::string & str);

            long               get_id() const;
            const std::string &get_first_name() const;
            const std::string &get_last_name() const;
            const std::string &get_email() const;
            const std::string &get_title() const;
            const std::string &get_login() const;
            const int           &get_age() const;

            long&        id();
            std::string &first_name();
            std::string &last_name();
            std::string &email();
            std::string &title();
            std::string &login();
            int         &age();

            // static Person init();
            static Person read_by_id(long id);
            static std::vector<Person> read_all();
            static std::vector<Person> search(std::string first_name,std::string last_name);
            void save_to_mysql();

            Poco::JSON::Object::Ptr toJSON() const;


            static void init(std::ostream &ostr);
            static void AddNewPerson(std::ostream &ostr, Poco::Net::HTMLForm &form);
            static Person find_by_login(std::string login);
            static void searchPerson(std::ostream &ostr, Poco::Net::HTMLForm &form);
            static void searchByLogin(std::ostream &ostr, Poco::Net::HTMLForm &form);
            static std::string InitDefaultPersonTable();

    };
}

#endif