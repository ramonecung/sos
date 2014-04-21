extern "C" {
#include "../freescaleK70/io.h"
#include "../freescaleK70/io_fs.h"
#include "../include/constants.h"
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

TEST_F(IOFSTest, CreateFs) {
    int result;
    initialize_io_fs();
    EXPECT_FALSE(file_exists("/dev/fs/fake"));

    result = create_fs("/dev/fs/data");
    EXPECT_EQ(SUCCESS, result);
    create_fs("/dev/fs/whale");
    create_fs("/dev/fs/manatee");

    EXPECT_TRUE(file_exists("/dev/fs/data"));
    EXPECT_TRUE(file_exists("/dev/fs/whale"));
    EXPECT_TRUE(file_exists("/dev/fs/manatee"));

    EXPECT_FALSE(file_exists("/dev/fs/fake"));
}

TEST_F(IOFSTest, DeleteFs) {
    int result;
    initialize_io_fs();
    result = delete_fs("/dev/fs/fake");
    EXPECT_EQ(CANNOT_DELETE_FILE, result);

    create_fs("/dev/fs/data");
    EXPECT_TRUE(file_exists("/dev/fs/data"));
    result = delete_fs("/dev/fs/data");
    EXPECT_FALSE(file_exists("/dev/fs/data"));
    EXPECT_EQ(SUCCESS, result);

    create_fs("/dev/fs/whale");
    create_fs("/dev/fs/manatee");

    result = delete_fs("/dev/fs/manatee");
    EXPECT_EQ(SUCCESS, result);
    EXPECT_TRUE(file_exists("/dev/fs/whale"));
    EXPECT_FALSE(file_exists("/dev/fs/manatee"));
}

TEST_F(IOFSTest, FopenFs) {
    Stream *s1, *s2, *s3;
    int i;
    initialize_io_fs();
    s1 = fopen_fs("/dev/fs/data");
    EXPECT_STREQ(s1->filename, "/dev/fs/data");
    EXPECT_EQ(FILE_SYSTEM, s1->device_instance);
    EXPECT_EQ(0, s1->file_id);
    s2 = fopen_fs("/dev/fs/data");
    EXPECT_EQ(FILE_SYSTEM, s2->device_instance);
    EXPECT_EQ(1, s2->file_id);
    fclose_fs(s1);
    s3 = fopen_fs("/dev/fs/data");
    EXPECT_EQ(FILE_SYSTEM, s3->device_instance);
    EXPECT_EQ(0, s3->file_id);
    s1 = fopen_fs("/dev/fs/data");
    EXPECT_EQ(FILE_SYSTEM, s1->device_instance);
    EXPECT_EQ(2, s1->file_id);
    fclose_fs(s1);
    fclose_fs(s2);
    fclose_fs(s3);
    /* intentionally fill up the max number of open files */
    for (i = 0; i < MAX_OPEN_FILES; i++) {
      test_stream = fopen_fs("/dev/fs/data");
    }
    test_stream = fopen_fs("/dev/fs/data");
    EXPECT_EQ(NULL_STREAM, test_stream);
    purge_open_files();
}

TEST_F(IOFSTest, FopenFsInvalid) {
    test_stream = fopen_fs("data");
    EXPECT_EQ(NULL_STREAM, test_stream);
}

TEST_F(IOFSTest, FputcFs) {
    int c;
    Stream *s = fopen_fs("/dev/fs/data");
    EXPECT_EQ(s->data, s->last_byte);
    EXPECT_EQ(s->data, s->next_byte_to_read);
    c = fputc_fs('c', s);
    EXPECT_EQ((s->data + 1), s->last_byte);
    EXPECT_EQ('c', c);
}

TEST_F(IOFSTest, FgetcFs) {
    Stream *s = fopen_fs("/dev/fs/data");
    int c, d;
    c = fputc_fs('x', s);
    c = fputc_fs('y', s);
    c = fputc_fs('z', s);

    d = fgetc_fs(s);
    EXPECT_EQ('x', d);
    EXPECT_EQ((s->data + 1), s->next_byte_to_read);

    d = fgetc_fs(s);
    EXPECT_EQ('y', d);
    EXPECT_EQ((s->data + 2), s->next_byte_to_read);

    d = fgetc_fs(s);
    EXPECT_EQ('z', d);
    EXPECT_EQ((s->data + 3), s->next_byte_to_read);
}

TEST_F(IOFSTest, FilenameValid) {
    char v1[] = "/dev/fs/data";

    char iv1[] = "data";
    char iv2[] = "/dev/fs/a c";
    char iv3[] = "/dev/fs/";

    EXPECT_TRUE(filename_valid(v1));

    EXPECT_FALSE(filename_valid(iv1));
    EXPECT_FALSE(filename_valid(iv2));
    EXPECT_FALSE(filename_valid(iv3));
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

    EXPECT_TRUE(basename_valid(v1));
    EXPECT_TRUE(basename_valid(v2));
    EXPECT_TRUE(basename_valid(v3));
    EXPECT_TRUE(basename_valid(v4));
    EXPECT_TRUE(basename_valid(v5));
    EXPECT_TRUE(basename_valid(v6));

    EXPECT_FALSE(basename_valid(iv1));
    EXPECT_FALSE(basename_valid(iv2));
    EXPECT_FALSE(basename_valid(iv3));
    EXPECT_FALSE(basename_valid(iv4));
    EXPECT_FALSE(basename_valid(iv5));
    EXPECT_FALSE(basename_valid(iv6));
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

    EXPECT_TRUE(prefix_valid(v1));
    EXPECT_TRUE(prefix_valid(v2));

    EXPECT_FALSE(prefix_valid(iv1));
    EXPECT_FALSE(prefix_valid(iv2));
    EXPECT_FALSE(prefix_valid(iv3));
    EXPECT_FALSE(prefix_valid(iv4));
    EXPECT_FALSE(prefix_valid(iv5));
    EXPECT_FALSE(prefix_valid(iv6));
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
