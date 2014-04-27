#include "gtest/gtest.h"

extern "C" {
#include "../util/strings.h"
}

class StringsTest : public ::testing::Test {
    protected:

  // You can remove any or all of the following functions if its body
  // is empty.


  StringsTest() {
    // You can do set-up work for each test here.
  }

  virtual ~StringsTest() {
    // You can do clean-up work that doesn't throw exceptions here.
  }

  // If the constructor and destructor are not enough for setting up
  // and cleaning up each test, you can define the following methods:

  virtual void SetUp() {
    // Code here will be called immediately after the constructor (right
    // before each test).
  }

  virtual void TearDown() {
    // Code here will be called immediately after each test (right
    // before the destructor).
  }
};

TEST_F(StringsTest, StringsEqual) {
    int res;
    char str1[] = "dog";
    char str2[] = "dog";
    char str3[] = "cat";
    char str4[] = "doggone";

    res = strings_equal(str1, str2);
    EXPECT_EQ(1, res);
    res = strings_equal(str1, str3);
    EXPECT_EQ(0, res);
    res = strings_equal(str1, str4);
    EXPECT_EQ(0, res);
}

TEST_F(StringsTest, StringLength) {
    char str1[] = "";
    char str2[] = "cat";
    char str3[] = "doggone";
    EXPECT_EQ(0, string_length(str1));
    EXPECT_EQ(3, string_length(str2));
    EXPECT_EQ(7, string_length(str3));

}

TEST_F(StringsTest, StringCopy) {
    char str1[] = "cat";
    char str2[4];
    string_copy(str1, str2);
    EXPECT_STREQ("cat", str2);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
