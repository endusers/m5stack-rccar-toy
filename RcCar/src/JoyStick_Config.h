/**
 * @file JoyStick_Config.h
 * 
 * @brief       JoyStick_Config
 * @note        なし
 * 
 * @version     2.0.0
 * @date        2022/08/20
 * 
 * @copyright   (C) 2021-2022 Motoyuki Endo
 */
#ifndef __JOYSTICK_CONFIG_H
#define __JOYSTICK_CONFIG_H

//----------------------------------------------------------------
//  <macro>
//----------------------------------------------------------------
#define JOYSTICK_BLUETOOTH_SUPPORT				0
#define JOYSTICK_BLUETOOTH_NOTSUPPORT			1
#define JOYSTICK_BLUETOOTH_TYPE					JOYSTICK_BLUETOOTH_SUPPORT

#define JOYSTICK_ROS1_SUPPORT					0
#define JOYSTICK_ROS1_NOTSUPPORT				1
#define JOYSTICK_ROS1_TYPE						JOYSTICK_ROS1_NOTSUPPORT

#define JOYSTICK_ROS2_SUPPORT					0
#define JOYSTICK_ROS2_NOTSUPPORT				1
#define JOYSTICK_ROS2_TYPE						JOYSTICK_ROS2_NOTSUPPORT

#define JOYSTICK_JOYMSGTYPE_DS4DRV				0
#define JOYSTICK_JOYMSGTYPE_BLUEZ				1
#define JOYSTICK_JOYMSGTYPE						JOYSTICK_JOYMSGTYPE_DS4DRV

#define BLUETOOTH_MAC_ADDRESS					"1A:2B:3C:01:01:01"

#if JOYSTICK_JOYMSGTYPE == JOYSTICK_JOYMSGTYPE_DS4DRV
#define JOYSTICK_JOYMSG_AXES_CAPACITY			14
#define JOYSTICK_JOYMSG_BUTTONS_CAPACITY		14
#endif
#if JOYSTICK_JOYMSGTYPE == JOYSTICK_JOYMSGTYPE_BLUEZ
#define JOYSTICK_JOYMSG_AXES_CAPACITY			8
#define JOYSTICK_JOYMSG_BUTTONS_CAPACITY		13
#endif


//----------------------------------------------------------------
//  <enum>
//----------------------------------------------------------------


//----------------------------------------------------------------
//  <struct>
//----------------------------------------------------------------


//----------------------------------------------------------------
//  <table>
//----------------------------------------------------------------


#endif
