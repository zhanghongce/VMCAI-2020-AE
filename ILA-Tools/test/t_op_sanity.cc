/// \file
/// Unit test for operation construction, z3 formula, hash, print, etc.

#include "unit-include/util.h"
#include <ilang/ilang++.h>

namespace ilang {

class TestOpSanity : public ::testing::Test {
public:
  TestOpSanity() {
    m = new Ila("host");
    // TODO
  }

  ~TestOpSanity() {
    if (m)
      delete m;
  }

  void SetUp() {
    DebugLog::Enable("OpSanity");
    SetToStdErr(1);
  }

  void TearDown() {
    DebugLog::Disable("OpSanity");
    SetToStdErr(0);
  }

  Ila* m = NULL;

}; // class TestOpSanity

TEST_F(TestOpSanity, AndBool) {
  // TODO
  // Need to have a API for node equivalence (semantically).
}

} // namespace ilang
