#include <gtest/gtest.h>
#include <Poco/Net/SocketAddress.h>
#include <Poco/Net/StreamSocket.h>
#include <Poco/Net/SocketStream.h>
#include <Poco/Net/DatagramSocket.h>
#include <Poco/StreamCopier.h>

TEST(test_basic, basic_test_set) {
    // ASSERT_TRUE(4 == 4);


    Poco::Net::SocketAddress sa("192.168.31.238", 80);
    Poco::Net::StreamSocket socket(sa);

    Poco::Net::SocketStream str(socket);
    str << "GET /person HTTP/2.1\r\n" <<
           "\r\n";
    str.flush();

    Poco::StreamCopier::copyStream(str, std::cout);
    
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


    EXPECT_EQ(4, 2+2) << "Two plus two must equal four";
}

int main ([[maybe_unused]] int argc, [[maybe_unused]] char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}