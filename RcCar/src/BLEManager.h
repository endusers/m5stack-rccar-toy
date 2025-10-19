/**
 * @file BLEManager.h
 * 
 * @brief       BLEManager
 * @note        Created with reference to the NimBLE_Client.ino from NimBLE.
 * 
 * @version     1.0.0
 * @date        2025/10/19
 * 
 * @copyright   (C) 2025 Motoyuki Endo
 */
#ifndef __BLEMANAGER_H
#define __BLEMANAGER_H

#include <vector>
#include <Arduino.h>
#include <NimBLEDevice.h>
#include "PeripheralBase.h"


//----------------------------------------------------------------
//  <macro>
//----------------------------------------------------------------
#define BLEMANAGER_SCAN_TIME                    (5000)


//----------------------------------------------------------------
//  <class>
//----------------------------------------------------------------
/**
 * @brief       BLEManager
 * @note        なし
 */
class BLEManager : public NimBLEScanCallbacks
{
public:
    BLEManager( void );                                         // コンストラクタ
    ~BLEManager( void );                                        // デストラクタ

    void begin( void );
    void update( void );

    void addDevice( PeripheralBase* device );

    void onResult( const NimBLEAdvertisedDevice* advertisedDevice ) override;
    void onScanEnd( const NimBLEScanResults& results, int reason ) override;

private:
    static const uint32_t scanTimeMs;

    std::vector<PeripheralBase*> peripherals;
};
#endif
