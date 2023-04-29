/**
 * @file CustomTransport.c
 * 
 * @brief       CustomTransport
 * @note        なし
 * 
 * @version     1.0.0
 * @date        2023/04/10
 * 
 * @copyright   (C) 2023 Motoyuki Endo
 */
#include "CustomTransport.h"


//----------------------------------------------------------------
//  <function>
//----------------------------------------------------------------
extern "C"
{
  bool arduino_uart_transport_open( struct uxrCustomTransport * transport )
  {
#if ROS_TRANSPORT_SERIAL_SEL == ROS_TRANSPORT_SERIAL
    Serial.begin( 115200 );
#endif
#if ROS_TRANSPORT_SERIAL_SEL == ROS_TRANSPORT_SERIAL1
    Serial1.begin( 115200, SERIAL_8N1, 32, 26 );
#endif
#if ROS_TRANSPORT_SERIAL_SEL == ROS_TRANSPORT_SERIAL2
    Serial2.begin( 115200, SERIAL_8N1, 23, 33 );
#endif
    return true;
  }

  bool arduino_uart_transport_close( struct uxrCustomTransport * transport )
  {
#if ROS_TRANSPORT_SERIAL_SEL == ROS_TRANSPORT_SERIAL
    Serial.end();
#endif
#if ROS_TRANSPORT_SERIAL_SEL == ROS_TRANSPORT_SERIAL1
    Serial1.end();
#endif
#if ROS_TRANSPORT_SERIAL_SEL == ROS_TRANSPORT_SERIAL2
    Serial2.end();
#endif
    return true;
  }

  size_t arduino_uart_transport_write( struct uxrCustomTransport * transport, const uint8_t *buf, size_t len, uint8_t *errcode )
  {
    (void)errcode;
#if ROS_TRANSPORT_SERIAL_SEL == ROS_TRANSPORT_SERIAL
    size_t sent = Serial.write( buf, len );
#endif
#if ROS_TRANSPORT_SERIAL_SEL == ROS_TRANSPORT_SERIAL1
    size_t sent = Serial1.write( buf, len );
#endif
#if ROS_TRANSPORT_SERIAL_SEL == ROS_TRANSPORT_SERIAL2
    size_t sent = Serial2.write( buf, len );
#endif
    return sent;
  }

  size_t arduino_uart_transport_read( struct uxrCustomTransport * transport, uint8_t *buf, size_t len, int timeout, uint8_t *errcode )
  {
    (void)errcode;
#if ROS_TRANSPORT_SERIAL_SEL == ROS_TRANSPORT_SERIAL
    Serial.setTimeout( timeout );
    return Serial.readBytes( (char *)buf, len );
#endif
#if ROS_TRANSPORT_SERIAL_SEL == ROS_TRANSPORT_SERIAL1
    Serial1.setTimeout( timeout );
    return Serial1.readBytes( (char *)buf, len );
#endif
#if ROS_TRANSPORT_SERIAL_SEL == ROS_TRANSPORT_SERIAL2
    Serial2.setTimeout( timeout );
    return Serial2.readBytes( (char *)buf, len );
#endif
  }
}
