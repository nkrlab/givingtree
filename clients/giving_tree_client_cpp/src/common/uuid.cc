// Copyright (C) 2013 iFunFactory Inc. All Rights Reserved.
//
// This work is confidential and proprietary to iFunFactory Inc. and
// must not be used, disclosed, copied, or distributed without the prior
// consent of iFunFactory Inc.

#include "common/uuid.h"


namespace common {

const Uuid Uuid::kNil = {{
    0U, 0U, 0U, 0U,
    0U, 0U, 0U, 0U,
    0U, 0U, 0U, 0U,
    0U, 0U, 0U, 0U}};

}  // namespace common
