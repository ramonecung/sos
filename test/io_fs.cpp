extern "C" {
#include "../freescaleK70/io.h"
#include "../freescaleK70/io_fs.h"
}

#include "gtest/gtest.h"
#include "../third-party/fff.h"
DEFINE_FFF_GLOBALS;

class IOFSTest : public ::testing::Test {
  protected:

  // You can remove any or all of the following functions if its body
  // is empty.

  Stream *test_stream;
  Stream ts;
  Device d;

  IOFSTest() {
    // You can do set-up work for each test here.
  }

  virtual ~IOFSTest() {
    // You can do clean-up work that doesn't throw exceptions here.
  }

  // If the constructor and destructor are not enough for setting up
  // and cleaning up each test, you can define the following methods:

  virtual void SetUp() {
    // Code here will be called immediately after the constructor (right
    // before each test).
    FFF_RESET_HISTORY();

    ts.device = &d;
    test_stream = &ts;
  }

  virtual void TearDown() {
    // Code here will be called immediately after each test (right
    // before the destructor).
  }
};

TEST_F(IOFSTest, FilenameValid) {
    char v1[] = "/dev/fs/data";

    char iv1[] = "data";
    char iv2[] = "/dev/fs/a c";
    char iv3[] = "/dev/fs/";

    EXPECT_EQ(1, filename_valid(v1));

    EXPECT_EQ(0, filename_valid(iv1));
    EXPECT_EQ(0, filename_valid(iv2));
    EXPECT_EQ(0, filename_valid(iv3));
}

TEST_F(IOFSTest, BasenameValid) {
    char v1[] = "data";
    char v2[] = "1";
    char v3[] = "dot.txt";
    char v4[] = "under_score";
    char v5[] = "hyphen-";
    char v6[] = "12345678901234";

    char iv1[] = "a c";
    char iv2[] = "-not-OK";
    char iv3[] = "123456789012345";
    char iv4[] = "ä";
    char iv5[] = "/usr";
    char iv6[] = "[data]";

    EXPECT_EQ(1, basename_valid(v1));
    EXPECT_EQ(1, basename_valid(v2));
    EXPECT_EQ(1, basename_valid(v3));
    EXPECT_EQ(1, basename_valid(v4));
    EXPECT_EQ(1, basename_valid(v5));
    EXPECT_EQ(1, basename_valid(v6));

    EXPECT_EQ(0, basename_valid(iv1));
    EXPECT_EQ(0, basename_valid(iv2));
    EXPECT_EQ(0, basename_valid(iv3));
    EXPECT_EQ(0, basename_valid(iv4));
    EXPECT_EQ(0, basename_valid(iv5));
    EXPECT_EQ(0, basename_valid(iv6));
}

TEST_F(IOFSTest, PrefixValid) {
    char v1[] = "/dev/fs/data";
    char v2[] = "/dev/fs/";

    char iv1[] = "data";
    char iv2[] = "/data";
    char iv3[] = "/dev";
    char iv4[] = "/dev/fs";
    char iv5[] = "/dev/files/x";
    char iv6[] = "";

    EXPECT_EQ(1, prefix_valid(v1));
    EXPECT_EQ(1, prefix_valid(v2));

    EXPECT_EQ(0, prefix_valid(iv1));
    EXPECT_EQ(0, prefix_valid(iv2));
    EXPECT_EQ(0, prefix_valid(iv3));
    EXPECT_EQ(0, prefix_valid(iv4));
    EXPECT_EQ(0, prefix_valid(iv5));
    EXPECT_EQ(0, prefix_valid(iv6));
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
