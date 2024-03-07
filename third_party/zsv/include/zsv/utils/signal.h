/*
 * Copyright (C) 2021 Liquidaty and the zsv/lib contributors
 * All rights reserved
 *
 * This file is part of zsv/lib, distributed under the license defined at
 * https://opensource.org/licenses/MIT
 */

#ifndef ZSV_SIGNAL_H
#define ZSV_SIGNAL_H

#include <signal.h>

extern volatile sig_atomic_t zsv_signal_interrupted;

void zsv_handle_ctrl_c_signal();

#endif
