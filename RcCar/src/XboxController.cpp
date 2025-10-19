/**
 * @file XboxController.cpp
 * 
 * @brief       XboxController
 * @note        Created with reference to the NimBLE_Client.ino from NimBLE.
 * 
 * @version     1.0.0
 * @date        2025/10/19
 * 
 * @copyright   (C) 2025 Motoyuki Endo
 */
#include "XboxController.h"


//----------------------------------------------------------------
//  <macro>
//----------------------------------------------------------------


//----------------------------------------------------------------
//  <variable>
//----------------------------------------------------------------
xbox_t XboxController::data;
uint8_t XboxController::battery;
NimBLEUUID XboxController::uuidServiceHid = NimBLEUUID("1812");
NimBLEUUID XboxController::uuidServiceBattery = NimBLEUUID("180f");


//----------------------------------------------------------------
//  <function>
//----------------------------------------------------------------
/**
 * @brief       コンストラクタ
 * @note        なし
 * @param       なし
 * @retval      なし
 */
XboxController::XboxController( void )
{
    XboxController::data.axes[0] = (uint16_t)( XBOXCONTROLLER_STICK_MAX / 2 );  // STICKLH
    XboxController::data.axes[1] = (uint16_t)( XBOXCONTROLLER_STICK_MAX / 2 );  // STICKLV
    XboxController::data.axes[2] = (uint16_t)( XBOXCONTROLLER_STICK_MAX / 2 );  // STICKRH
    XboxController::data.axes[3] = (uint16_t)( XBOXCONTROLLER_STICK_MAX / 2 );  // STICKRV
    XboxController::data.axes[4] = 0;                                           // TRIGL
    XboxController::data.axes[5] = 0;                                           // TRIGR

    XboxController::data.buttons[0] = 0;    // UP
    XboxController::data.buttons[1] = 0;    // UPRIGHT
    XboxController::data.buttons[2] = 0;    // RIGHT
    XboxController::data.buttons[3] = 0;    // DOWNRIGHT
    XboxController::data.buttons[4] = 0;    // DOWN
    XboxController::data.buttons[5] = 0;    // DOWNLEFT
    XboxController::data.buttons[6] = 0;    // LEFT
    XboxController::data.buttons[7] = 0;    // UPLEFT
    XboxController::data.buttons[8] = 0;    // A
    XboxController::data.buttons[9] = 0;    // B
    XboxController::data.buttons[10] = 0;   // X
    XboxController::data.buttons[11] = 0;   // Y
    XboxController::data.buttons[12] = 0;   // LB
    XboxController::data.buttons[13] = 0;   // RB
    XboxController::data.buttons[14] = 0;   // VIEW
    XboxController::data.buttons[15] = 0;   // MENU
    XboxController::data.buttons[16] = 0;   // XBOX
    XboxController::data.buttons[17] = 0;   // SHARE

    XboxController::battery = 0;
}

/**
 * @brief       デストラクタ
 * @note        なし
 * @param       なし
 * @retval      なし
 */
XboxController::~XboxController( void )
{
    // DO_NOTHING
}

/**
 * @brief       デバイス一致判定
 * @note        なし
 * @param[in]   pDevice : アドバタイズデバイス
 * @retval      true 一致
 * @retval      false 不一致
 */
bool XboxController::matchAdvertisedDevice( const NimBLEAdvertisedDevice* pDevice )
{
    if( pDevice->getAddress().equals( address ) )
    {
        return true;
    }
    return false;
}

/**
 * @brief       デバイス接続
 * @note        なし
 * @param[in]   pDevice : アドバタイズデバイス
 * @retval      true 接続OK
 * @retval      false 接続NG
 */
bool XboxController::connectDevice( const NimBLEAdvertisedDevice* pDevice )
{
    NimBLEClient* pClient = nullptr;

    /** Check if we have a client we should reuse first **/
    if( NimBLEDevice::getCreatedClientCount() )
    {
        /**
         *  Special case when we already know this device, we send false as the
         *  second argument in connect() to prevent refreshing the service database.
         *  This saves considerable time and power.
         */
        pClient = NimBLEDevice::getClientByPeerAddress( pDevice->getAddress() );
        if( pClient ) {
            if( !pClient->connect(pDevice, false) )
            {
#ifdef _SERIAL_DEBUG_BLECLIENT_
                Serial.printf("Reconnect failed\n");
#endif
                return false;
            }
#ifdef _SERIAL_DEBUG_BLECLIENT_
            Serial.printf("Reconnected client\n");
#endif
        } else {
            /**
             *  We don't already have a client that knows this device,
             *  check for a client that is disconnected that we can use.
             */
            pClient = NimBLEDevice::getDisconnectedClient();
        }
    }

    /** No client to reuse? Create a new one. */
    if( !pClient )
    {
        if( NimBLEDevice::getCreatedClientCount() >= NIMBLE_MAX_CONNECTIONS )
        {
#ifdef _SERIAL_DEBUG_BLECLIENT_
            Serial.printf("Max clients reached - no more connections available\n");
#endif
            return false;
        }

        pClient = NimBLEDevice::createClient();

#ifdef _SERIAL_DEBUG_BLECLIENT_
        Serial.printf("New client created\n");
#endif

        pClient->setClientCallbacks( this, false );
        /**
         *  Set initial connection parameters:
         *  These settings are safe for 3 clients to connect reliably, can go faster if you have less
         *  connections. Timeout should be a multiple of the interval, minimum is 100ms.
         *  Min interval: 12 * 1.25ms = 15, Max interval: 12 * 1.25ms = 15, 0 latency, 150 * 10ms = 1500ms timeout
         */
        pClient->setConnectionParams( 12, 12, 0, 150 );

        /** Set how long we are willing to wait for the connection to complete (milliseconds), default is 30000. */
        pClient->setConnectTimeout( 5 * 1000 );

        if( !pClient->connect( pDevice ) )
        {
            /** Created a client but failed to connect, don't need to keep it as it has no data */
            NimBLEDevice::deleteClient( pClient );
#ifdef _SERIAL_DEBUG_BLECLIENT_
            Serial.printf("Failed to connect, deleted client\n");
#endif
            return false;
        }
    }

    if( !pClient->isConnected() )
    {
        if( !pClient->connect( pDevice ) )
        {
#ifdef _SERIAL_DEBUG_BLECLIENT_
            Serial.printf("Failed to connect\n");
#endif
            return false;
        }
    }

#ifdef _SERIAL_DEBUG_BLECLIENT_
    Serial.printf("Connected to: %s RSSI: %d\n", pClient->getPeerAddress().toString().c_str(), pClient->getRssi());
#endif

    for( auto pSvc : pClient->getServices( true ) )
    {
        auto sUuid = pSvc->getUUID();
        if( sUuid.equals(uuidServiceHid) or sUuid.equals(uuidServiceBattery) )
        {
            for( auto pChr : pSvc->getCharacteristics( true ) )
            {

                if( pChr->canWrite() )
                {
                    ;
                }

                if( pChr->canRead() )
                {
                    for( auto v : pChr->readValue() )
                    {
                        ;
                    }
                }

                if( pChr->canNotify() )
                {
                    if( !pChr->subscribe( true, notifyCB ) )
                    {
                        pClient->disconnect();
                        return false;
                    }
                }
            }
        }
    }

#ifdef _SERIAL_DEBUG_BLECLIENT_
    Serial.printf("Done with this device!\n");
#endif
    return true;
}

/**
 * @brief       通知ハンドラ
 * @note        なし
 * @param[in]   pRemoteCharacteristic : キャラクタリスティック
 * @param[in]   pData : データ
 * @param[in]   length : サイズ
 * @param[in]   isNotify : Notification(true) or Indication(false)
 * @retval      なし
 */
void XboxController::notifyCB( NimBLERemoteCharacteristic* pRemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify )
{
#ifdef _SERIAL_DEBUG_BLECLIENT_
    std::string str  = (isNotify == true) ? "Notification" : "Indication";
    str             += " from ";
    str             += pRemoteCharacteristic->getClient()->getPeerAddress().toString();
    str             += ": Service = " + pRemoteCharacteristic->getRemoteService()->getUUID().toString();
    str             += ", Characteristic = " + pRemoteCharacteristic->getUUID().toString();
    str             += ", Value = ";
    Serial.printf("%s", str.c_str());
    for (int i = 0; i < length; ++i) {
        Serial.printf(" %02x", pData[i]);
    }
    Serial.printf("\n");
#endif

    auto uuid = pRemoteCharacteristic->getRemoteService()->getUUID();

    if( uuid.equals( uuidServiceHid ) )
    {
        parseHidData( pData, length );
    }

    if( uuid.equals( uuidServiceBattery ) )
    {
        parseBatteryData( pData, length );
    }
}

/**
 * @brief       Hidデータ解析
 * @note        なし
 * @param[in]   pData : データ
 * @param[in]   length : サイズ
 * @retval      なし
 */
void XboxController::parseHidData( uint8_t* pData, size_t length )
{
    if( length != 16 )
    {
        return;
    }

    XboxController::data.axes[0] = (uint16_t)( ((pData[1] << 8) | pData[0]) & XBOXCONTROLLER_STICK_MAX );   // STICKLH
    XboxController::data.axes[1] = (uint16_t)( ((pData[3] << 8) | pData[2]) & XBOXCONTROLLER_STICK_MAX );   // STICKLV
    XboxController::data.axes[2] = (uint16_t)( ((pData[5] << 8) | pData[4]) & XBOXCONTROLLER_STICK_MAX );   // STICKRH
    XboxController::data.axes[3] = (uint16_t)( ((pData[7] << 8) | pData[6]) & XBOXCONTROLLER_STICK_MAX );   // STICKRV
    XboxController::data.axes[4] = (uint16_t)( ((pData[9] << 8) | pData[8]) & XBOXCONTROLLER_TRIG_MAX );    // TRIGL
    XboxController::data.axes[5] = (uint16_t)( ((pData[11] << 8) | pData[10]) & XBOXCONTROLLER_TRIG_MAX );  // TRIGR

    uint8_t button;

    button = pData[12];
    XboxController::data.buttons[0] = ( (button == 1) ? 1 : 0 );    // UP
    XboxController::data.buttons[1] = ( (button == 2) ? 1 : 0 );    // UPRIGHT
    XboxController::data.buttons[2] = ( (button == 3) ? 1 : 0 );    // RIGHT
    XboxController::data.buttons[3] = ( (button == 4) ? 1 : 0 );    // DOWNRIGHT
    XboxController::data.buttons[4] = ( (button == 5) ? 1 : 0 );    // DOWN
    XboxController::data.buttons[5] = ( (button == 6) ? 1 : 0 );    // DOWNLEFT
    XboxController::data.buttons[6] = ( (button == 7) ? 1 : 0 );    // LEFT
    XboxController::data.buttons[7] = ( (button == 8) ? 1 : 0 );    // UPLEFT

    button = pData[13];
    XboxController::data.buttons[8] = ( (button >> 0) & 0x01 );     // A
    XboxController::data.buttons[9] = ( (button >> 1) & 0x01 );     // B
    XboxController::data.buttons[10] = ( (button >> 3) & 0x01 );    // X
    XboxController::data.buttons[11] = ( (button >> 4) & 0x01 );    // Y
    XboxController::data.buttons[12] = ( (button >> 6) & 0x01 );    // LB
    XboxController::data.buttons[13] = ( (button >> 7) & 0x01 );    // RB

    button = pData[14];
    XboxController::data.buttons[14] = ( (button >> 2) & 0x01 );    // VIEW
    XboxController::data.buttons[15] = ( (button >> 3) & 0x01 );    // MENU
    XboxController::data.buttons[16] = ( (button >> 4) & 0x01 );    // XBOX

    button = pData[15];
    XboxController::data.buttons[17] = ( (button >> 0) & 0x01 );    // SHARE
}

/**
 * @brief       Batteryデータ解析
 * @note        なし
 * @param[in]   pData : データ
 * @param[in]   length : サイズ
 * @retval      なし
 */
void XboxController::parseBatteryData( uint8_t* pData, size_t length )
{
    if( length != 1 )
    {
        return;
    }

    XboxController::battery = pData[0];
}
