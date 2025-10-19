/**
 * @file PeripheralBase.cpp
 * 
 * @brief       PeripheralBase
 * @note        Created with reference to the NimBLE_Client.ino from NimBLE.
 * 
 * @version     1.0.0
 * @date        2025/10/19
 * 
 * @copyright   (C) 2025 Motoyuki Endo
 */
#include "PeripheralBase.h"


//----------------------------------------------------------------
//  <function>
//----------------------------------------------------------------
/**
 * @brief       コンストラクタ
 * @note        なし
 * @param       なし
 * @retval      なし
 */
PeripheralBase::PeripheralBase( void )
{
    doConnect = false;
    advDevice = nullptr;

    client = nullptr;
    address = {};
}

/**
 * @brief       デストラクタ
 * @note        なし
 * @param       なし
 * @retval      なし
 */
PeripheralBase::~PeripheralBase( void )
{
    // DO_NOTHING
}

/**
 * @brief       クライアント接続
 * @note        なし
 * @param[in]   pClient : クライアントオブジェクトへのポインタ
 * @retval      なし
 */
void PeripheralBase::onConnect( NimBLEClient* pClient )
{
#ifdef _SERIAL_DEBUG_BLECLIENT_
    Serial.printf("Connected\n");
#endif
    client = pClient;
}

/**
 * @brief       クライアント接続失敗
 * @note        なし
 * @param[in]   pClient : クライアントオブジェクトへのポインタ
 * @param[in]   reason : 理由コード
 * @retval      なし
 */
void PeripheralBase::onDisconnect( NimBLEClient* pClient, int reason )
{
#ifdef _SERIAL_DEBUG_BLECLIENT_
    Serial.printf("%s Disconnected, reason = %d - Starting scan\n", pClient->getPeerAddress().toString().c_str(), reason);
#endif
    NimBLEDevice::deleteBond( pClient->getPeerAddress() );
    client = nullptr;
}

/**
 * @brief       パスキー要求
 * @note        なし
 * @param[in]   connInfo : 接続情報インスタンスへの参照
 * @retval      なし
 */
void PeripheralBase::onPassKeyEntry( NimBLEConnInfo& connInfo )
{
#ifdef _SERIAL_DEBUG_BLECLIENT_
    Serial.printf("Server Passkey Entry\n");
#endif
    /**
     * This should prompt the user to enter the passkey displayed
     * on the peer device.
     */
    NimBLEDevice::injectPassKey( connInfo, 123456 );
}

/**
 * @brief       ペアリング数値比較要求
 * @note        なし
 * @param[in]   connInfo : 接続情報インスタンスへの参照
 * @param[in]   pass_key : 比較するピン
 * @retval      なし
 */
void PeripheralBase::onConfirmPasskey( NimBLEConnInfo& connInfo, uint32_t pass_key )
{
#ifdef _SERIAL_DEBUG_BLECLIENT_
    Serial.printf("The passkey YES/NO number: %" PRIu32 "\n", pass_key);
#endif
    /** Inject false if passkeys don't match. */
    NimBLEDevice::injectConfirmPasskey( connInfo, true );
}

/**
 * @brief       ペアリング完了
 * @note        なし
 * @param[in]   connInfo : 接続情報インスタンスへの参照
 * @retval      なし
 */
void PeripheralBase::onAuthenticationComplete( NimBLEConnInfo& connInfo )
{
    if( !connInfo.isEncrypted() )
    {
#ifdef _SERIAL_DEBUG_BLECLIENT_
        Serial.printf("Encrypt connection failed - disconnecting\n");
#endif
        /** Find the client with the connection handle provided in connInfo */
        NimBLEDevice::getClientByHandle( connInfo.getConnHandle() )->disconnect();
        return;
    }
}

/**
 * @brief       デバイスアドレス設定
 * @note        なし
 * @param[in]   addr : デバイスアドレスインスタンスへの参照
 * @retval      なし
 */
void PeripheralBase::setAddress( NimBLEAddress &addr )
{
    address = addr;
}

/**
 * @brief       接続中判定
 * @note        なし
 * @param       なし
 * @retval      true 接続中
 * @retval      false 接続中でない
 */
bool PeripheralBase::isConnected( void )
{
    return client and client->isConnected();
}
