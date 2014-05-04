extern "C" {
#include "../freescaleK70/io.h"
#include "../freescaleK70/io_fs.h"
#include "../include/constants.h"
#include "../memory/memory.h"
#include "../util/util.h"
}

#include "gtest/gtest.h"
#include "../third-party/fff.h"
DEFINE_FFF_GLOBALS;
FAKE_VOID_FUNC(initialize_io_button);
FAKE_VALUE_FUNC(int, fgetc_button, Stream *);
FAKE_VALUE_FUNC(int, fputc_button, int, Stream *);

FAKE_VOID_FUNC(initialize_io_led);
FAKE_VALUE_FUNC(int, fgetc_led);
FAKE_VALUE_FUNC(int, fputc_led, int, Stream *);

class IOFSTest : public ::testing::Test {
  protected:

  // You can remove any or all of the following functions if its body
  // is empty.

  Stream *test_stream;
  Stream ts;

  IOFSTest() {
    // You can do set-up work for each test here.
    initialize_memory();
    initialize_io();
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
    RESET_FAKE(initialize_io_button);
    RESET_FAKE(fgetc_button);
    RESET_FAKE(fputc_button);

    RESET_FAKE(initialize_io_led);
    RESET_FAKE(fgetc_led);
    RESET_FAKE(fputc_led);

    test_stream = &ts;
  }

  virtual void TearDown() {
    // Code here will be called immediately after each test (right
    // before the destructor).
  }
};

TEST_F(IOFSTest, CreateFs) {
    int result;
    NamedFile *file;
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

    file = find_file("/dev/fs/data");
    EXPECT_STREQ(file->filename, "/dev/fs/data");
}

TEST_F(IOFSTest, CreateFsInvalidName) {
    int result;
    result = create_fs("data");
    EXPECT_EQ(CANNOT_CREATE_FILE, result);
    EXPECT_FALSE(file_exists("data"));
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

TEST_F(IOFSTest, FopenFsNonExistentFile) {
    Stream *s;
    s = myFopen("/dev/fs/data");
    EXPECT_EQ(NULL, s);
}

TEST_F(IOFSTest, FputcFs) {
    int c;
    NamedFile *file;

    /* need file to exist */
    create_fs("/dev/fs/data");
    file = find_file("/dev/fs/data");

    Stream *s = myFopen("/dev/fs/data");
    EXPECT_EQ(file->first_block->data, s->next_byte_to_write);
    EXPECT_EQ(file->first_block->data, s->next_byte_to_read);
    c = fputc_fs('c', s);
    EXPECT_EQ((file->first_block->data + 1), s->next_byte_to_write);
    EXPECT_EQ('c', c);

    myFclose(s);
    delete_fs("/dev/fs/data");
}

TEST_F(IOFSTest, FgetcFs) {
    Stream *s;
    int c, d;
    NamedFile *file;

    /* need file to exist */
    create_fs("/dev/fs/data");
    file = find_file("/dev/fs/data");
    s = myFopen("/dev/fs/data");

    /* haven't put any chars in the file yet */
    EXPECT_EQ(EOF, fgetc_fs(s));

    c = fputc_fs('x', s);
    c = fputc_fs('y', s);
    c = fputc_fs('z', s);

    d = fgetc_fs(s);
    EXPECT_EQ('x', d);
    EXPECT_EQ((file->first_block->data + 1), s->next_byte_to_read);

    d = fgetc_fs(s);
    EXPECT_EQ('y', d);
    EXPECT_EQ((file->first_block->data + 2), s->next_byte_to_read);

    d = fgetc_fs(s);
    EXPECT_EQ('z', d);
    EXPECT_EQ((file->first_block->data + 3), s->next_byte_to_read);

    EXPECT_EQ(EOF, fgetc_fs(s));

    myFclose(s);
    delete_fs("/dev/fs/data");
}

TEST_F(IOFSTest, FsFirstBlock) {
    Stream *s;
    int c;
    int i;
    NamedFile *file;
    Block *file_block, *stream_block;

    /* need file to exist */
    create_fs("/dev/fs/data");

    file = find_file("/dev/fs/data");
    s = myFopen("/dev/fs/data");
    file_block = file->first_block;
    stream_block = s->current_block;
    EXPECT_EQ(0, file->size);
    EXPECT_EQ(file_block, stream_block);


    c = fputc_fs('x', s);
    EXPECT_EQ(1, file->size);

    /* start at 1 not 0 since we already put a char */
    for (i = 1; i < BLOCK_SIZE; i++) {
        c = fputc_fs('x', s);
    }
    EXPECT_EQ(BLOCK_SIZE, file->size);

    myFclose(s);
    delete_fs("/dev/fs/data");
}

TEST_F(IOFSTest, FsSecondBlock) {
    Stream *s;
    int c;
    int i;
    NamedFile *file;
    Block *file_block, *stream_block;

    /* need file to exist */
    create_fs("/dev/fs/data");

    file = find_file("/dev/fs/data");
    s = myFopen("/dev/fs/data");
    file_block = file->first_block;
    stream_block = s->current_block;

    /* fill the first block */
    for (i = 0; i < BLOCK_SIZE; i++) {
        c = fputc_fs('x', s);
    }

    c = fputc_fs('x', s);
    EXPECT_EQ(BLOCK_SIZE + 1, file->size);

    stream_block = s->current_block;

    EXPECT_NE(file_block, stream_block);
    EXPECT_EQ(file_block->next, stream_block);

    myFclose(s);
    delete_fs("/dev/fs/data");
}

TEST_F(IOFSTest, FsTwoStreams) {
    Stream *s1, *s2;
    int c;
    int i;
    NamedFile *file;

    /* need file to exist */
    create_fs("/dev/fs/data");

    file = find_file("/dev/fs/data");
    s1 = myFopen("/dev/fs/data");
    /* open a new stream on the same file */
    s2 = myFopen("/dev/fs/data");
    EXPECT_EQ(s1->current_block, s2->current_block);

    /* fill the first block of the file using the first stream */
    for (i = 0; i < BLOCK_SIZE; i++) {
        c = fputc_fs('x', s1);
    }

    c = fputc_fs('x', s1);
    EXPECT_EQ(BLOCK_SIZE + 1, file->size);
    EXPECT_NE(s1->current_block, s2->current_block);
    EXPECT_EQ(s1->current_block, s2->current_block->next);

    /* fill the first block of the file using the second stream */
    for (i = 0; i < BLOCK_SIZE; i++) {
        c = fputc_fs('y', s2);
    }
    c = fputc_fs('y', s2);
    EXPECT_EQ(BLOCK_SIZE + 1, file->size);
    /* s2 should now point to the next block s1 created */
    /* not a brand new block */
    EXPECT_EQ(s1->current_block, s2->current_block);

    myFclose(s1);
    myFclose(s2);
    delete_fs("/dev/fs/data");
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
