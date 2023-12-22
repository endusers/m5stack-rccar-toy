/**
 * @file AckermannDrive.h
 * 
 * @brief       AckermannDrive
 * @note        なし
 * 
 * @version     1.1.1
 * @date        2023/06/25
 * 
 * @copyright   (C) 2022-2023 Motoyuki Endo
 */
#ifndef __ACKERMANNDRIVE_H
#define __ACKERMANNDRIVE_H

#include <Arduino.h>
#include <M5Atom.h>
#include "ServoCtrl.h"


//----------------------------------------------------------------
//  <macro>
//----------------------------------------------------------------


//----------------------------------------------------------------
//  <enum>
//----------------------------------------------------------------


//----------------------------------------------------------------
//  <struct>
//----------------------------------------------------------------
struct SteeringConfig_Tag
{
	int32_t minAngle;
	int32_t maxAngle;
	ServoConfig servo;
};
typedef struct SteeringConfig_Tag					SteeringConfig;

struct ThrottleConfig_Tag
{
	float wheelbase;
	float diameter;
	int32_t minRpm;
	int32_t maxRpm;
	float forwardGain;
	float reverseGain;
	ServoConfig servo;
};
typedef struct ThrottleConfig_Tag					ThrottleConfig;

struct AckermannDriveConfig_Tag
{
	SteeringConfig steering;
	ThrottleConfig throttle;
};
typedef struct AckermannDriveConfig_Tag				AckermannDriveConfig;


//----------------------------------------------------------------
//  <class>
//----------------------------------------------------------------
/**
 * @brief       アッカーマン型ドライブ
 * @note        なし
 */
class AckermannDrive
{
private:
	SemaphoreHandle_t _mutex;

	AckermannDriveConfig _config;

	int32_t _minAngle;
	int32_t _maxAngle;
	float _wheelbase;
	float _radius;
	int32_t _minRpm;
	int32_t _maxRpm;
	float _forwardGain;
	float _reverseGain;

public:
	ServoCtrl steering;
	ServoCtrl throttle;

	AckermannDrive( AckermannDriveConfig i_config );                // コンストラクタ
	~AckermannDrive( void );                                        // デストラクタ

	void Init( void );                                              // イニシャライズ
	void ControlCycle( void );                                      // 制御周期ハンドラ
	void SetSteering( float i_angle );                              // ステアリング設定
	void SetSpeed( float i_speed );                                 // スピード設定
	void SetThrottle( float i_position );                           // スロットル設定
	float ConvertOmega2Degree( float i_speed, float i_omega );      // 角度変換
};

#endif
