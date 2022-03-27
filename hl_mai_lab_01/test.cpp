#include <gtest/gtest.h>
#include <Poco/Net/SocketAddress.h>
#include <Poco/Net/StreamSocket.h>
#include <Poco/Net/SocketStream.h>
#include <Poco/Net/DatagramSocket.h>
#include <Poco/StreamCopier.h>

#include "database/database.h"
#include "database/person.h"
#include "config/config.h"

bool isIinited = false;

void setDatabase() {
    if (isIinited) return;

    Config::get().host() = "localhost";
    Config::get().port() = "3306";
    Config::get().database() = "stud";
    Config::get().login() = "stud";
    Config::get().password() = "stud";
    
    isIinited = true;
}

TEST(check_person_login, person_table_set) {
    database::Person person;
    person.login() = "Theodor";
    EXPECT_EQ("Theodor", person.get_login()) << "логины должны совпадать";
}

TEST(init_person_table, person_table_set) {

    setDatabase();

    try
    {
        if (database::Person::InitDefaultPersonTable() == "true") ASSERT_TRUE(true);
        else ASSERT_TRUE(false);
    }

    catch (Poco::Data::MySQL::ConnectionException &e)
    {
        ASSERT_TRUE(false);
    }
    catch (Poco::Data::MySQL::StatementException &e)
    {
        ASSERT_TRUE(false);
    }
}

TEST(create_new_person, person_creation_set) {

    setDatabase();

    database::Person person;
    person.first_name() = "Theodor";
    person.last_name()  = "Zalupkin";
    person.login()      = "ZaluTH";
    person.age()        = 23;

    try
    {
        person.save_to_mysql();
        ASSERT_TRUE(true);
    }
    catch (...)
    {
        ASSERT_TRUE(false);
    }
}

TEST(search_by_login, basic_test_set) {

    setDatabase();

    // auto results = database::Person::search("fn","ln");

    try
    {
        database::Person result = database::Person::find_by_login("lulex.py");
        EXPECT_EQ("lulex.py", result.get_login()) << "логины должны совпадать";
    }
    catch (...)
    {
        ASSERT_TRUE(false);
    } 

    

    // Poco::Net::SocketAddress sa("192.168.31.238", 80);
    // Poco::Net::StreamSocket socket(sa);

    // Poco::Net::SocketStream str(socket);
    // str << "GET /person HTTP/2.1\r\n" <<
    //        "\r\n";
    // str.flush();

    // Poco::StreamCopier::copyStream(str, std::cout);
    
    // -----------------------
    // Poco::Net::HTMLForm form(hreq, hreq.stream());


    // Poco::Net::SocketAddress sa("192.168.31.238", 80);
    // Poco::Net::DatagramSocket dgs(sa);
    // char buffer[1024];
    // for (;;){
    //     Poco::Net::SocketAddress sender;
    //     int n = dgs.receiveFrom(buffer, sizeof(buffer)-1, sender);
    //     buffer[n] = '\0';
    //     std::cout << sender.toString() << ": " << buffer << std::endl;
    // }


    
}

TEST(get_all_persons, person_table_set) {
    auto results = database::Person::read_all();

    if (results.size() <= 0) {
        ASSERT_TRUE(false);
    }

    for (auto s : results) {
        EXPECT_EQ(23, s.get_age()) << "Все возраста должны быть равны 23";
    }
}

int main ([[maybe_unused]] int argc, [[maybe_unused]] char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}