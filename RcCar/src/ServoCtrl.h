/**
 * @file ServoCtrl.h
 * 
 * @brief       ServoCtrl
 * @note        なし
 * 
 * @version     1.1.0
 * @date        2024/03/03
 * 
 * @copyright   (C) 2022-2024 Motoyuki Endo
 */
#ifndef __SERVOCTRL_H
#define __SERVOCTRL_H

#include <Arduino.h>
#include <ESP32Servo.h>
#include "ServoCtrl.h"


//----------------------------------------------------------------
//  <macro>
//----------------------------------------------------------------
#define SERVOCTRL_CTRLCYCLE						(30)							// 30ms


//----------------------------------------------------------------
//  <enum>
//----------------------------------------------------------------


//----------------------------------------------------------------
//  <struct>
//----------------------------------------------------------------
struct ServoPort_Tag
{
	int pinNo;
	int frequency;
	int minWidth;
	int maxWidth;
	boolean isReverse;
};
typedef struct ServoPort_Tag					ServoPort;

struct ServoSmooth_Tag
{
	boolean isSmoothEnable;
	float coefficient;
};
typedef struct ServoSmooth_Tag					ServoSmooth;

struct ServoConfig_Tag
{
	ServoPort portConfig;
	ServoSmooth smoothConfig;
};
typedef struct ServoConfig_Tag					ServoConfig;


//----------------------------------------------------------------
//  <class>
//----------------------------------------------------------------
/**
 * @brief       ServoCtrl
 * @note        なし
 */
class ServoCtrl
{
private:
	SemaphoreHandle_t _mutex;

	Servo _servo;
	uint32_t _servoCtrlCycle;

	boolean *_isReverse;
	boolean *_isSmoothEnable;
	float *_smtCoefficient;

	void WriteAngle( int32_t i_angle );                             // アングル書き込み
	void SmoothControl( void );                                     // なまし処理

public:
	int32_t reqAngle;
	float targetAngle;
	int32_t outAngle;

	ServoPort port;
	ServoSmooth smooth;

	ServoCtrl( ServoConfig i_config );                              // コンストラクタ
	~ServoCtrl( void );                                             // デストラクタ

	void Init( void );                                              // イニシャライズ
	void ControlCycle( void );                                      // 制御周期ハンドラ
	void SetAngle( int32_t i_angle );                               // アングル設定
};

#endif
