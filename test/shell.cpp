
#include "gtest/gtest.h"

extern "C" {
#include <unistd.h>
#include <stdio.h>
#include "../shell/shell.h"
}

class ShellTest : public ::testing::Test {
    protected:

  // You can remove any or all of the following functions if its body
  // is empty.

  ShellTest() {
    // You can do set-up work for each test here.
  }

  virtual ~ShellTest() {
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


TEST(Shell, prompt) {
    char c;
    int fds[2];
    pipe(fds);

    FILE *ostrm = fdopen(fds[1], "w");
    FILE *istrm = fdopen(fds[0], "r");

    print_prompt(ostrm);
    fclose(ostrm);

    c = fgetc(istrm);
    fclose(istrm);

    EXPECT_EQ('$', c);
}

TEST(Shell, argcount) {
    int argcount;
    argcount = sh_argc();
    EXPECT_EQ(0, argcount);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
