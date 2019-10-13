/// \file
/// Unit test for the class Z3ExprAdapter

#include "unit-include/util.h"
#include <ilang/ila/expr_fuse.h>
#include <ilang/ila/z3_expr_adapter.h>

namespace ilang {

TEST(TestZ3Adapter, Construct) {
  DebugLog::Enable("z3_adapter");

  z3::context c;
  z3::solver s(c);
  Z3ExprAdapter adapter(c);

  auto reg_x = ExprFuse::NewBvVar("reg_x", 8);
  auto reg_y = ExprFuse::NewBvVar("reg_y", 8);

  auto const_true = ExprFuse::BoolConst(BoolVal("True"));
  auto const_bv0 = ExprFuse::BvConst(0, 8);

  // first expression check
  auto x_and_y = ExprFuse::And(reg_x, reg_y);
  auto x_and_y_or_y = ExprFuse::Or(x_and_y, reg_y);
  auto y_or_0 = ExprFuse::Or(reg_y, const_bv0);
  auto bv_equal = ExprFuse::Eq(x_and_y_or_y, y_or_0); // always true

  z3::expr expr_eq = adapter.GetExpr(bv_equal);

  // solve the validity
  s.add(!expr_eq);
  auto result = s.check();
  EXPECT_TRUE(result == z3::unsat);

  // remove previous assertions.
  s.reset();

  // second expression check
  auto const_false = ExprFuse::Not(const_true);
  auto bool_equal = ExprFuse::Eq(bv_equal, const_false); // always false

  z3::expr expr_bool_equal = adapter.GetExpr(bool_equal);

  // solve the validity
  s.add(expr_bool_equal);
  result = s.check();
  EXPECT_TRUE(result == z3::unsat);

  DebugLog::Disable("z3_adapter");
}

TEST(TestZ3Adapter, Suffix) {
  DebugLog::Enable("z3_adapter");

  z3::context c;
  z3::solver s(c);
  Z3ExprAdapter adapter(c);

  auto reg_x = ExprFuse::NewBvVar("reg_x", 8);
  auto reg_y = ExprFuse::NewBvVar("reg_y", 8);

  z3::expr expr_x_plus_y_frame_1_ =
      adapter.GetExpr(ExprFuse::Add(reg_x, reg_y), "_frame_1_");
  z3::expr expr_x_plus_y_frame_2_ =
      adapter.GetExpr(ExprFuse::Add(reg_x, reg_y), "_frame_2_");

  z3::expr two_frames_should_be_independent =
      expr_x_plus_y_frame_1_ != expr_x_plus_y_frame_2_;

  s.add(two_frames_should_be_independent);

  auto result = s.check();
  EXPECT_TRUE(result == z3::sat);

  DebugLog::Disable("z3_adapter");
}

// TODO simplify
// TODO change context
// TODO Clear

} // namespace ilang
