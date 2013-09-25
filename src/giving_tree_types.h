// Copyright (C) 2013 iFunFactory Inc. All Rights Reserved.
//
// This work is confidential and proprietary to iFunFactory Inc. and
// must not be used, disclosed, copied, or distributed without the prior
// consent of iFunFactory Inc.

#ifndef SRC_GIVING_TREE_TYPES_H_
#define SRC_GIVING_TREE_TYPES_H_

#include <funapi/common/types.h>
#include "giving_tree.h"


namespace google { namespace protobuf {} }


namespace giving_tree {

using fun::string;
using fun::shared_ptr;
using fun::Uuid;
using fun::GivingTree;
using fun::GivingTreePtr;
using fun::GivingTreePtrVector;
using fun::GivingTreePtrMap;

namespace protobuf = google::protobuf;
}  // namespace giving_tree

#endif  // SRC_GIVING_TREE_TYPES_H_
