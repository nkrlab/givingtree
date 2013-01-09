// Copyright (C) 2012 Nexon Korea Corporation All Rights Reserved.
//
// This work is confidential and proprietary to Nexon Korea Corporation and
// must not be used, disclosed, copied, or distributed without the prior
// consent of Nexon Korea Corporation.

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
