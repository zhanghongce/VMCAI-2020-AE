/// \file
/// Some short cut for set operations

#ifndef MCM_SET_OP_H__
#define MCM_SET_OP_H__

#include <algorithm>
#include <iterator>
#include <set>

#define UNION(a, b, r)                                                         \
  (std::set_union((a).begin(), (a).end(), (b).begin(), (b).end(),              \
                  std::inserter((r), (r).end())))
#define INTERSECT(a, b, r)                                                     \
  (std::set_intersection((a).begin(), (a).end(), (b).begin(), (b).end(),       \
                         std::inserter((r), (r).end())))
#define DIFFERENCE(a, b, r)                                                    \
  (std::set_difference((a).begin(), (a).end(), (b).begin(), (b).end(),         \
                       std::inserter((r), (r).end())))
#define SYMDIFF(a, b, r)                                                       \
  (std::set_symmetric_difference((a).begin(), (a).end(), (b).begin(),          \
                                 (b).end(), std::inserter((r), (r).end())))

#define IN(e, s) ((s).find(e) != (s).end())
#define IN_p(e, s) ((s)->find(e) != (s)->end())

#endif // MCM_SET_OP_H__
