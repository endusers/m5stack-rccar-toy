/**
 * @file ServoCtrl.cpp
 * 
 * @brief       ServoCtrl
 * @note        なし
 * 
 * @version     1.0.0
 * @date        2022/10/22
 * 
 * @copyright   (C) 2022 Motoyuki Endo
 */
#include "ServoCtrl.h"


//----------------------------------------------------------------
//  <macro>
//----------------------------------------------------------------
#define ARRAY_LENGTH(in_array)      (sizeof(in_array)/sizeof(in_array[0]))
#define UNUSED_VARIABLE(in_x)       (void)(in_x)

#define SAFE_DELETE(x) if(x){ delete(x); x = NULL;}           //delete開放処理
#define SAFE_DELETE_ARRAY(x) if(x){ delete [] (x); x = NULL;} //delete[]開放処理

#define OFF                         (0)
#define ON                          (1)

#define SIGN(x) ((x>0)-(x<0))


//----------------------------------------------------------------
//  <function>
//----------------------------------------------------------------
/**
 * @brief       コンストラクタ
 * @note        なし
 * @param[in]   i_config : コンフィグ設定
 * @retval      なし
 */
ServoCtrl::ServoCtrl( ServoConfig i_config )
{
	_mutex = xSemaphoreCreateMutex();
	xSemaphoreGive( _mutex );

	port = i_config.portConfig;
	smooth = i_config.smoothConfig;
	_isSmoothEnable = &smooth.isSmoothEnable;
	_smtCoefficient = &smooth.coefficient;

	reqAngle = 0;
	targetAngle = 0.0;
	outAngle = 0;
}


/**
 * @brief       デストラクタ
 * @note        なし
 * @param       なし
 * @retval      なし
 */
ServoCtrl::~ServoCtrl( void )
{
	// DO_NOTHING
}


/**
 * @brief       イニシャライズ
 * @note        なし
 * @param       なし
 * @retval      なし
 */
void ServoCtrl::Init( void )
{
	xSemaphoreTake( _mutex , portMAX_DELAY );
	_servo.setPeriodHertz( port.frequency );
	_servo.attach( port.pinNo, port.minWidth, port.maxWidth );
	_servo.write( 90 );
	xSemaphoreGive( _mutex );
}


/**
 * @brief       制御周期ハンドラ
 * @note        なし
 * @param       なし
 * @retval      なし
 */
void ServoCtrl::ControlCycle( void )
{
	uint32_t getTime;

	getTime = (uint32_t)millis();

	if( getTime > _servoCtrlCycle )
	{
		SmoothControl();

		if( *_isSmoothEnable )
		{
			WriteAngle( (int32_t)targetAngle );
		}
		else
		{
			// DO_NOTHING
		}

		_servoCtrlCycle = getTime + SERVOCTRL_CTRLCYCLE;
	}
}


/**
 * @brief       アングル設定
 * @note        なし
 * @param[in]   i_angle : アングル
 * @retval      なし
 */
void ServoCtrl::SetAngle( int32_t i_angle )
{
	xSemaphoreTake( _mutex , portMAX_DELAY );
	reqAngle = i_angle;
	xSemaphoreGive( _mutex );

	if( *_isSmoothEnable )
	{
		// DO_NOTHING
	}
	else
	{
		WriteAngle( (int32_t)reqAngle );
	}
}


/**
 * @brief       アングル書き込み
 * @note        なし
 * @param[in]   i_angle : アングル
 * @retval      なし
 */
void ServoCtrl::WriteAngle( int32_t i_angle )
{
	outAngle = constrain( i_angle, 0, 180 );
	_servo.write( (int)outAngle );
}


/**
 * @brief       なまし処理
 * @note        なし
 * @param       なし
 * @retval      なし
 */
void ServoCtrl::SmoothControl( void )
{
	xSemaphoreTake( _mutex , portMAX_DELAY );
	if( *_isSmoothEnable )
	{
		targetAngle = targetAngle + ( ( (float)reqAngle - targetAngle ) * (*_smtCoefficient) );
		if( (int32_t)targetAngle == 0 )
		{
			targetAngle = 0.0;
		}
		else if( (int32_t)targetAngle == 90 )
		{
			targetAngle = 90.0;
		}
		else if( (int32_t)targetAngle == 180 )
		{
			targetAngle = 180.0;
		}
		else
		{
			// DO_NOTHING
		}
	}
	else{
		targetAngle = (float)reqAngle;
	}
	xSemaphoreGive( _mutex );
}
