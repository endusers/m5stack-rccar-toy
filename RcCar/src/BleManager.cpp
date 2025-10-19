/**
 * @file BLEManager.cpp
 * 
 * @brief       BLEManager
 * @note        Created with reference to the NimBLE_Client.ino from NimBLE.
 * 
 * @version     1.0.0
 * @date        2025/10/19
 * 
 * @copyright   (C) 2025 Motoyuki Endo
 */
#include "BLEManager.h"


//----------------------------------------------------------------
//  <const>
//----------------------------------------------------------------
const uint32_t BLEManager::scanTimeMs = BLEMANAGER_SCAN_TIME;


//----------------------------------------------------------------
//  <function>
//----------------------------------------------------------------
/**
 * @brief       コンストラクタ
 * @note        なし
 * @param       なし
 * @retval      なし
 */
BLEManager::BLEManager( void )
{
    peripherals = {};
}

/**
 * @brief       デストラクタ
 * @note        なし
 * @param       なし
 * @retval      なし
 */
BLEManager::~BLEManager( void )
{
    // DO_NOTHING
}

/**
 * @brief       開始
 * @note        なし
 * @param       なし
 * @retval      なし
 */
void BLEManager::begin( void )
{
#ifdef _SERIAL_DEBUG_BLECLIENT_
    Serial.printf("Starting NimBLE Client\n");
#endif

    NimBLEDevice::init( "NimBLE-Client" );

    NimBLEDevice::setOwnAddrType( BLE_OWN_ADDR_PUBLIC );
    NimBLEDevice::setSecurityAuth( true, false, false );
    NimBLEDevice::setPower( ESP_PWR_LVL_P9 );

    NimBLEScan* pScan = NimBLEDevice::getScan();

    pScan->setScanCallbacks( this, false );

    pScan->setInterval( 100 );
    pScan->setWindow( 100 );

    pScan->start( scanTimeMs );

#ifdef _SERIAL_DEBUG_BLECLIENT_
    Serial.printf("Scanning for peripherals\n");
#endif
}

/**
 * @brief       更新
 * @note        なし
 * @param       なし
 * @retval      なし
 */
void BLEManager::update( void )
{
    bool needScan;

    for( auto p : peripherals )
    {
        if( !p->isConnected() )
        {
            if( p->doConnect )
            {
                p->doConnect = false;
                if( p->connectDevice( p->advDevice ) )
                {
#ifdef _SERIAL_DEBUG_BLECLIENT_
                    Serial.printf("Success! we should now be getting notifications, scanning for more!\n");
#endif
                }
                else
                {
                    NimBLEDevice::deleteBond( p->advDevice->getAddress() );
#ifdef _SERIAL_DEBUG_BLECLIENT_
                    Serial.printf("Failed to connect\n");
#endif
                }
                break;
            }
        }
    }

    needScan = false;
    for( auto p : peripherals )
    {
        if( !p->isConnected() )
        {
            needScan = true;
        }
    }

    if( needScan )
    {
        if( !NimBLEDevice::getScan()->isScanning() )
        {
            NimBLEDevice::getScan()->start( scanTimeMs, false, true );
        }
    }
}

/**
 * @brief       デバイス追加
 * @note        なし
 * @param[in]   device : ペリフェラルデバイスオブジェクトへのポインタ
 * @retval      なし
 */
void BLEManager::addDevice( PeripheralBase* device )
{
    peripherals.push_back( device );
}

/**
 * @brief       スキャン結果
 * @note        なし
 * @param[in]   advertisedDevice : アドバタイズデバイス
 * @retval      なし
 */
void BLEManager::onResult( const NimBLEAdvertisedDevice* advertisedDevice )
{
#ifdef _SERIAL_DEBUG_BLECLIENT_
    Serial.printf("Advertised Device found: %s", advertisedDevice->toString().c_str());
    Serial.printf(" | %s %d\n", advertisedDevice->getAddress().toString().c_str(), advertisedDevice->getAddress().getType());
#endif

    for( auto p : peripherals )
    {
        if( !p->isConnected() )
        {
            if( p->matchAdvertisedDevice( advertisedDevice ) )
            {
#ifdef _SERIAL_DEBUG_BLECLIENT_
                Serial.printf("Found Our Service\n");
#endif
                NimBLEDevice::getScan()->stop();
                p->advDevice = advertisedDevice;
                p->doConnect = true;
                break;
            }
        }
    }
}

/**
 * @brief       スキャン終了
 * @note        なし
 * @param[in]   results : 終了したスキャン結果
 * @param[in]   reason : 終了した理由コード
 * @retval      なし
 */
void BLEManager::onScanEnd( const NimBLEScanResults& results, int reason )
{
#ifdef _SERIAL_DEBUG_BLECLIENT_
    Serial.printf("Scan Ended, reason: %d, device count: %d; Restarting scan\n", reason, results.getCount());
#endif
    NimBLEDevice::getScan()->start( scanTimeMs, false, true );
}
