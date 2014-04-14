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
    char f1[] = "/dev/fs/data";
    EXPECT_EQ(1, filename_valid(f1));
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
