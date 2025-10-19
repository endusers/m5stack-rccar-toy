/**
 * @file XboxController.h
 * 
 * @brief       XboxController
 * @note        Created with reference to the NimBLE_Client.ino from NimBLE.
 * 
 * @version     1.0.0
 * @date        2025/10/19
 * 
 * @copyright   (C) 2025 Motoyuki Endo
 */
#ifndef __XBOXCONTROLLER_H
#define __XBOXCONTROLLER_H

#include <Arduino.h>
#include "PeripheralBase.h"


//----------------------------------------------------------------
//  <macro>
//----------------------------------------------------------------
#define XBOXCONTROLLER_AXES_NUM                 (6)
#define XBOXCONTROLLER_BUTTONS_NUM              (18)
#define XBOXCONTROLLER_STICK_MAX                (0xFFFF)
#define XBOXCONTROLLER_TRIG_MAX                 (0x03FF)


//----------------------------------------------------------------
//  <enum>
//----------------------------------------------------------------


//----------------------------------------------------------------
//  <struct>
//----------------------------------------------------------------
struct Xbox_Tag
{
    uint16_t axes[XBOXCONTROLLER_AXES_NUM];
    uint8_t buttons[XBOXCONTROLLER_BUTTONS_NUM];
};
typedef struct Xbox_Tag                         xbox_t;


//----------------------------------------------------------------
//  <class>
//----------------------------------------------------------------
/**
 * @brief       XboxController
 * @note        なし
 */
class XboxController : public PeripheralBase
{
public:
    static xbox_t data;
    static uint8_t battery;

    XboxController( void );                                     // コンストラクタ
    ~XboxController( void );                                    // デストラクタ

    bool matchAdvertisedDevice( const NimBLEAdvertisedDevice* pDevice );
    bool connectDevice( const NimBLEAdvertisedDevice* pDevice );

private:
    static NimBLEUUID uuidServiceHid;
    static NimBLEUUID uuidServiceBattery;

    static void notifyCB( NimBLERemoteCharacteristic* pRemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify );
    static void parseHidData( uint8_t* pData, size_t length );
    static void parseBatteryData( uint8_t* pData, size_t length );
};
#endif
