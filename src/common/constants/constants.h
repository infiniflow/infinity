//
// Created by xwg on 23-9-27.
//
#pragma once

#include "string"

namespace infinity {
const int kWALFileSizeThreshold = 10 * 1024;


const std::string kWALFileTemp = "/wal.log";
const std::string kWALFilePrefix = "wal.log.";
} // namespace infinity
