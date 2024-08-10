/**
 * @file RcCar_Config.h
 * 
 * @brief       RcCar_Config
 * @note        なし
 * 
 * @version     1.4.0
 * @date        2024/04/10
 * 
 * @copyright   (C) 2022-2024 Motoyuki Endo
 */
#ifndef __RCCAR_CONFIG_H
#define __RCCAR_CONFIG_H

#include <Arduino.h>
#include "AckermannDrive.h"


//----------------------------------------------------------------
//  <macro>
//----------------------------------------------------------------
#define _SERIAL_DEBUG_

#define RCCAR_UNIMOG							1
#define RCCAR_BRONCO							2
#define RCCAR_BRONCO_TRACKUINT					3
#define RCCAR_TYPE								RCCAR_UNIMOG

#define RCCAR_JOYCONNECT_PREFERBLUETOOTH		0
#define RCCAR_JOYCONNECT_DUAL					1
#define RCCAR_JOYCONNECT_MODE					RCCAR_JOYCONNECT_PREFERBLUETOOTH


//----------------------------------------------------------------
//  <enum>
//----------------------------------------------------------------


//----------------------------------------------------------------
//  <struct>
//----------------------------------------------------------------


//----------------------------------------------------------------
//  <table>
//----------------------------------------------------------------
extern const AckermannDriveConfig Config;

#endif
