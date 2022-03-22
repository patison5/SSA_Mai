#include <gtest/gtest.h>

TEST(test_basic, basic_test_set) {
    // ASSERT_TRUE(4 == 4);
    EXPECT_EQ(4, 2+3) << "Two plus two must equal four";
}

int main ([[maybe_unused]] int argc, [[maybe_unused]] char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}