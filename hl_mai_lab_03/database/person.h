#ifndef PERSON_H
#define PERSON_H

#include <string>
#include <vector>
#include "Poco/JSON/Object.h"
#include "Poco/Net/HTMLForm.h"

namespace database
{
    class Person 
    {
        private:
            long _u_id;
            std::string _login;
            std::string _first_name;
            std::string _last_name;
            int _age;
        
        public:

            static Person fromJSON(const std::string & str);

            long                get_u_id() const;
            const std::string   &get_first_name() const;
            const std::string   &get_last_name() const;
            const std::string   &get_login() const;
            const int           &get_age() const;

            long        &u_id();
            std::string &first_name();
            std::string &last_name();
            std::string &login();
            int         &age();

            static Person read_by_id(long id);
            static Person find_by_login(std::string login);
            static std::vector<Person> read_all();
            static std::vector<Person> search(std::string first_name,std::string last_name);
            void save_to_mysql();

            static void AddNewPerson(std::ostream &ostr, Poco::Net::HTMLForm &form);
            static void init(std::ostream &ostr);
            static void searchPerson(std::ostream &ostr, Poco::Net::HTMLForm &form);
            static void searchPersonById(std::ostream &ostr, Poco::Net::HTMLForm &form);
            static void searchByLogin(std::ostream &ostr, Poco::Net::HTMLForm &form);

            static std::string InitDefaultPersonTable();

            Poco::JSON::Object::Ptr toJSON() const;


            static Person read_from_cache_by_login(std::string login);
            static void warm_up_cache();
            void save_to_cache();
            static size_t size_of_cache();
    };
}

#endif