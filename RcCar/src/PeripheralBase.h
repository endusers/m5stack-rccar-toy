/**
 * @file PeripheralBase.h
 * 
 * @brief       PeripheralBase
 * @note        Created with reference to the NimBLE_Client.ino from NimBLE.
 * 
 * @version     1.0.0
 * @date        2025/10/19
 * 
 * @copyright   (C) 2025 Motoyuki Endo
 */
#ifndef __PERIPHERALBASE_H
#define __PERIPHERALBASE_H

#include <Arduino.h>
#include <NimBLEDevice.h>


//----------------------------------------------------------------
//  <macro>
//----------------------------------------------------------------
// #define _SERIAL_DEBUG_BLECLIENT_


//----------------------------------------------------------------
//  <class>
//----------------------------------------------------------------
/**
 * @brief       PeripheralBase
 * @note        なし
 */
class PeripheralBase : public NimBLEClientCallbacks
{
public:
    bool doConnect;
    const NimBLEAdvertisedDevice* advDevice;

    PeripheralBase( void );                                     // コンストラクタ
    ~PeripheralBase( void );                                    // デストラクタ

    void onConnect( NimBLEClient* pClient ) override;
    void onDisconnect( NimBLEClient* pClient, int reason ) override;
    void onPassKeyEntry( NimBLEConnInfo& connInfo ) override;
    void onConfirmPasskey( NimBLEConnInfo& connInfo, uint32_t pass_key ) override;
    void onAuthenticationComplete( NimBLEConnInfo& connInfo ) override;

    virtual bool matchAdvertisedDevice( const NimBLEAdvertisedDevice* pDevice ) = 0;
    virtual bool connectDevice( const NimBLEAdvertisedDevice* pDevice ) = 0;

    void setAddress( NimBLEAddress &addr );
    bool isConnected( void );

protected:
    NimBLEClient* client;
    NimBLEAddress address;
};
#endif
