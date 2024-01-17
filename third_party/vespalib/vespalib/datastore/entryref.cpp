// Copyright Vespa.ai. Licensed under the terms of the Apache 2.0 license. See LICENSE in the project root.

#include "entryref.hpp"
#include <sstream>

namespace vespalib::datastore {

template EntryRefT<24u, 8u>::EntryRefT(size_t, uint32_t);
template EntryRefT<31u, 1u>::EntryRefT(size_t, uint32_t);
template EntryRefT<22u, 10u>::EntryRefT(size_t, uint32_t);
template EntryRefT<20u, 12u>::EntryRefT(size_t, uint32_t);
template EntryRefT<19u, 13u>::EntryRefT(size_t, uint32_t);
template EntryRefT<18u, 6u>::EntryRefT(size_t, uint32_t);
template EntryRefT<15u, 17u>::EntryRefT(size_t, uint32_t);
template EntryRefT<10u, 22u>::EntryRefT(size_t, uint32_t);
template EntryRefT<10u, 10u>::EntryRefT(size_t, uint32_t);
template EntryRefT<3u, 2u>::EntryRefT(size_t, uint32_t);

} // namespace vespalib::datastore
