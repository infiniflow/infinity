//
// Created by jinhai on 23-9-27.
//

#pragma once

#include "faiss/utils/distances.h"
#include "new_result_handler.h"

namespace infinity {

using ReservoirResultHandler = NewReservoirResultHandler<faiss::CMin<float, RowID>>;
using ReservoirSingleResultHandler = ReservoirResultHandler::ReservoirSingleResultHandler;

} // namespace infinity
