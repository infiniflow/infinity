////////////////////////////////////////////////////////////////////////////////
/// DISCLAIMER
///
/// Copyright 2016 by EMC Corporation, All Rights Reserved
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///
///     http://www.apache.org/licenses/LICENSE-2.0
///
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
///
/// Copyright holder is EMC Corporation
///
/// @author Andrey Abramov
////////////////////////////////////////////////////////////////////////////////
#pragma once

namespace fst {

template <unsigned Bit, typename T>
inline constexpr void UnsetBit(T &value) noexcept {
    value &= ~(T(1) << (Bit));
}

template <unsigned Bit, typename T>
inline constexpr void SetBit(T &value) noexcept {
    value |= (T(1) << (Bit));
}

template <typename T>
inline constexpr void SetBit(T &value, size_t bit) noexcept {
    value |= (T(1) << bit);
}

template <unsigned Bit, typename T>
inline constexpr void SetBit(bool set, T &value) noexcept {
    set ? SetBit<Bit>(value) : UnsetBit<Bit>(value);
}

template <typename T>
inline constexpr uint32_t BitsRequired() noexcept {
    return sizeof(T) * 8U;
}

template <typename T>
inline constexpr bool CheckBit(T value, size_t bit) noexcept {
    return (value & (T(1) << bit)) != 0;
}

template <unsigned Bit, typename T>
inline constexpr bool CheckBit(T value) noexcept {
    return (value & (T(1) << (Bit))) != 0;
}

} // namespace fst