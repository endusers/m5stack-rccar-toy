/**
 * @file System.h
 * 
 * @brief       System
 * @note        なし
 * 
 * @version     1.0.0
 * @date        2025/05/03
 * 
 * @copyright   (C) 2025 Motoyuki Endo
 */
#ifndef __SYSTEM_H
#define __SYSTEM_H

#include "Config.h"

#if MODULE_TYPE == MODULE_TYPE_M5ATOM
#include <Arduino.h>
#include <M5Atom.h>
#include "NvmConfig.h"
#endif
#if MODULE_TYPE == MODULE_TYPE_M5ATOMS3
#include <Arduino.h>
#include <M5Unified.h>
#include <M5AtomS3.h>
#include "NvmConfig.h"
#endif

#endif
