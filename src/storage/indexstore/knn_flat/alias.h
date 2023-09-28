//
// Created by jinhai on 23-9-27.
//

#pragma once

#include "new_result_handler.h"
#include "faiss/utils/distances.h"

namespace infinity {



using ReservoirResultHandler = NewReservoirResultHandler<faiss::CMin<float, CompoundID>>;
using ReservoirSingleResultHandler = ReservoirResultHandler::ReservoirSingleResultHandler;

}
