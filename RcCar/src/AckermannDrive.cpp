/**
 * @file AckermannDrive.cpp
 * 
 * @brief       AckermannDrive
 * @note        なし
 * 
 * @version     1.3.1
 * @date        2024/05/03
 * 
 * @copyright   (C) 2022-2025 Motoyuki Endo
 */
#include "AckermannDrive.h"


//----------------------------------------------------------------
//  <macro>
//----------------------------------------------------------------
#define ARRAY_LENGTH(in_array)      (sizeof(in_array)/sizeof(in_array[0]))
#define UNUSED_VARIABLE(in_x)       (void)(in_x)

#define SAFE_DELETE(x) if(x){ delete(x); x = NULL;}           //delete開放処理
#define SAFE_DELETE_ARRAY(x) if(x){ delete [] (x); x = NULL;} //delete[]開放処理

#define OFF                         (0)
#define ON                          (1)

#define MAPF(x,in_min,in_max,out_min,out_max)\
    ((x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min)


//----------------------------------------------------------------
//  <function>
//----------------------------------------------------------------
/**
 * @brief       コンストラクタ
 * @note        なし
 * @param       なし
 * @retval      なし
 */
AckermannDrive::AckermannDrive( AckermannDriveConfig i_config )
	: steering( i_config.steering.servo ) , throttle( i_config.throttle.servo )
{
	_mutex = xSemaphoreCreateMutex();
	xSemaphoreGive( _mutex );

	sangle = 0.0;
	tspeed = 0.0;
	tposition = 0.0;

	_config = i_config;

	_minAngle = _config.steering.minAngle;
	_maxAngle = _config.steering.maxAngle;
	_wheelbase = _config.throttle.wheelbase;
	_radius = _config.throttle.diameter / 2;
	_minRpm = _config.throttle.minRpm;
	_maxRpm = _config.throttle.maxRpm;
	_forwardGain = _config.throttle.forwardGain;
	_reverseGain = _config.throttle.reverseGain;
	_minSteerMap = &_config.throttle.minMap.steer[0];
	_minThrotMap = &_config.throttle.minMap.throt[0];
}


/**
 * @brief       デストラクタ
 * @note        なし
 * @param       なし
 * @retval      なし
 */
AckermannDrive::~AckermannDrive( void )
{
	// DO_NOTHING
}


/**
 * @brief       イニシャライズ
 * @note        なし
 * @param       なし
 * @retval      なし
 */
void AckermannDrive::Init( void )
{
	xSemaphoreTake( _mutex , portMAX_DELAY );
	steering.Init();
	throttle.Init();
	xSemaphoreGive( _mutex );
}


/**
 * @brief       制御周期ハンドラ
 * @note        なし
 * @param       なし
 * @retval      なし
 */
void AckermannDrive::ControlCycle( void )
{
	xSemaphoreTake( _mutex , portMAX_DELAY );
	steering.ControlCycle();
	throttle.ControlCycle();
	xSemaphoreGive( _mutex );
}


/**
 * @brief       ステアリング設定
 * @note        なし
 * @param[in]   i_angle : ステアリング角度(degree)
 * @retval      なし
 */
void AckermannDrive::SetSteering( float i_angle )
{
	float position;

	if( i_angle < _minAngle )
	{
		sangle = (float)_minAngle;
	}
	else if( i_angle > _maxAngle )
	{
		sangle = (float)_maxAngle;
	}
	else
	{
		sangle = i_angle;
	}

	position = MAPF( sangle, (float)_minAngle, (float)_maxAngle, 0.0, 180.0 );

	xSemaphoreTake( _mutex , portMAX_DELAY );
	steering.SetAngle( (int32_t)position );
	xSemaphoreGive( _mutex );
}


/**
 * @brief       スピード設定
 * @note        なし
 * @param[in]   i_speed : 速度(m/s)
 * @retval      なし
 */
void AckermannDrive::SetSpeed( float i_speed )
{
	float rpm;
	float position;
	float angle;
	int32_t lhs;
	int32_t rhs;
	float min;

	if( i_speed < 0.0 )
	{
		tspeed = i_speed * _reverseGain;
	}
	else
	{
		tspeed = i_speed * _forwardGain;
	}

	rpm = tspeed / ( 2 * M_PI * _radius ) * 60;

	if( rpm < _minRpm )
	{
		rpm = (float)_minRpm;
	}
	if( rpm > _maxRpm )
	{
		rpm = (float)_maxRpm;
	}

	position = MAPF( rpm, (float)_minRpm, (float)_maxRpm, 0.0, 180.0 );

	angle = abs(sangle);
	lhs = (int32_t)( angle / ACKERMANNDRIVE_TMINMAP_STEP );
	rhs = (int32_t)( ( (lhs + 1) >= ACKERMANNDRIVE_TMINMAP_SIZE ) ? lhs : lhs + 1 );
	min = MAPF( angle, _minSteerMap[lhs], _minSteerMap[rhs], _minThrotMap[lhs], _minThrotMap[rhs] );

	if( (position != 90.0) and (position > (90 - min)) and (position < (90 + min)) )
	{
		tposition = ( position < 90.0 ) ? (90 - min) : (90 + min);
	}
	else
	{
		tposition = position;
	}

	xSemaphoreTake( _mutex , portMAX_DELAY );
	throttle.SetAngle( (int32_t)tposition );
	xSemaphoreGive( _mutex );
}


/**
 * @brief       スロットル設定
 * @note        なし
 * @param[in]   i_position : スロットル(%)
 * @retval      なし
 */
void AckermannDrive::SetThrottle( float i_position )
{
	tposition = MAPF( i_position, -100.0, 100.0, 0.0, 180.0 );

	xSemaphoreTake( _mutex , portMAX_DELAY );
	throttle.SetAngle( (int32_t)tposition );
	xSemaphoreGive( _mutex );
}


/**
 * @brief       角度変換
 * @note        なし
 * @param[in]   i_speed : 速度(m/s)
 * @param[in]   i_omega : 角速度
 * @retval      なし
 */
float AckermannDrive::ConvertOmega2Degree( float i_speed, float i_omega )
{
	float angle;
	float radius;
	float degree;

	if( i_speed == 0.0 or i_omega == 0.0 )
	{
		angle = 0.0;
	}
	else
	{
		
		radius = fabs(i_speed) / -i_omega;
		angle =  atan( _wheelbase / radius );
	}

	degree = angle * ( 180.0 / M_PI );	// RAD_TO_DEG

	return degree;
}
