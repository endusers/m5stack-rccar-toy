/**
 * @file CustomTransport.h
 * 
 * @brief       CustomTransport
 * @note        なし
 * 
 * @version     1.1.0
 * @date        2025/05/03
 * 
 * @copyright   (C) 2023-2025 Motoyuki Endo
 */
#ifndef __CUSTOMTRANSPORT_H
#define __CUSTOMTRANSPORT_H

#include <Arduino.h>
#include "Config.h"


//----------------------------------------------------------------
//  <macro>
//----------------------------------------------------------------
#define ROS_TRANSPORT_SERIAL					1
#define ROS_TRANSPORT_SERIAL1					2
#define ROS_TRANSPORT_SERIAL2					3
#define ROS_TRANSPORT_SERIAL_SEL				ROS_TRANSPORT_SERIAL1


//----------------------------------------------------------------
//  <enum>
//----------------------------------------------------------------


//----------------------------------------------------------------
//  <struct>
//----------------------------------------------------------------


//----------------------------------------------------------------
//  <class>
//----------------------------------------------------------------


//----------------------------------------------------------------
//  <function>
//----------------------------------------------------------------
extern "C" bool arduino_uart_transport_open( struct uxrCustomTransport * transport );
extern "C" bool arduino_uart_transport_close( struct uxrCustomTransport * transport );
extern "C" size_t arduino_uart_transport_write( struct uxrCustomTransport * transport, const uint8_t *buf, size_t len, uint8_t *errcode );
extern "C" size_t arduino_uart_transport_read( struct uxrCustomTransport * transport, uint8_t *buf, size_t len, int timeout, uint8_t *errcode );

#endif
