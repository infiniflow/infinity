//
// Created by jinhai on 23-9-27.
//

#pragma once

#include "storage/indexstore/knn_distance.h"
#include "storage/indexstore/common/compound_id.h"
#include "storage/indexstore/knn_flat/alias.h"

namespace infinity {

using ReservoirResultHandler = NewReservoirResultHandler<faiss::CMin<float, CompoundID>>;
using ReservoirSingleResultHandler = ReservoirResultHandler::ReservoirSingleResultHandler;

//class KnnFlatIPBlasReservoir : public KnnDistance {
//
//};

}
