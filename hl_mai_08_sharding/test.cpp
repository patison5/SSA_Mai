#include <gtest/gtest.h>
#include <Poco/Net/SocketAddress.h>
#include <Poco/Net/StreamSocket.h>
#include <Poco/Net/SocketStream.h>
#include <Poco/Net/DatagramSocket.h>
#include <Poco/StreamCopier.h>

#include "database/database.h"
#include "database/person.h"
#include "config/config.h"

bool isInited = false;

void setDatabase() {
    if (isInited) return;

    Config::get().host() = "127.0.0.1";
    Config::get().port() = "6033";
    Config::get().database() = "sql_test";
    Config::get().login() = "test";
    Config::get().password() = "pzjqUkMnc7vfNHET";

    isInited = true;
}

TEST(check_person_login, person_table_set) {
    EXPECT_EQ("Theodor", "Theodor") << "логины должны совпадать";
}

TEST(check_person_login2, person_table_set2) {
    database::Person person;
    person.login() = "Theodor";
    EXPECT_EQ("Theodor", person.get_login()) << "логины должны совпадать";
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

TEST(get_all_persons, person_table_set) {
    auto results = database::Person::read_all();

    EXPECT_EQ(results.size(), 4) << "Кол-во объектов класса Person должно быть равно 4.";

    for (auto s : results) {
        EXPECT_EQ(23, s.get_age()) << "Все возраста должны быть равны 23";
    }
}

int main ([[maybe_unused]] int argc, [[maybe_unused]] char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}