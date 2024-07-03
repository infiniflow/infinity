/*
    nanobind/nb_enums.h: enumerations used in nanobind (just rv_policy atm.)

    Copyright (c) 2022 Wenzel Jakob

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE file.
*/

NAMESPACE_BEGIN(NB_NAMESPACE)

// Approach used to cast a previously unknown C++ instance into a Python object
enum class rv_policy {
    automatic,
    automatic_reference,
    take_ownership,
    copy,
    move,
    reference,
    reference_internal,
    none
    /* Note to self: nb_func.h assumes that this value fits into 3 bits,
       hence no further policies can be added. */
};

NAMESPACE_END(NB_NAMESPACE)
