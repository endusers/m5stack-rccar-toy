/**
 * @file RcCar.h
 * 
 * @brief       RcCar
 * @note        なし
 * 
 * @version     1.1.0
 * @date        2023/03/22
 * 
 * @copyright   (C) 2022-2023 Motoyuki Endo
 */
#ifndef __RCCAR_H
#define __RCCAR_H

#include <Arduino.h>
#include <M5Atom.h>
#include <micro_ros_arduino.h>
#include <stdio.h>
#include <time.h>
#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <rcl_interfaces/msg/log.h>
#include <sensor_msgs/msg/imu.h>
#include <sensor_msgs/msg/joy.h>
#include <geometry_msgs/msg/twist.h>
#include <rmw_microros/rmw_microros.h>
#include "NvmConfig.h"
#include "RcCar_Config.h"
#include "JoyStick.h"


//----------------------------------------------------------------
//  <macro>
//----------------------------------------------------------------
#define RCCAR_MAIN_CYCLE						(10 * 1000)						// 10ms
#define RCCAR_JOYCTRL_CYCLE						(100)							// 100ms
#define RCCAR_ROSMGRCTRL_CYCLE					(1000)							// 1000ms
#define RCCAR_MAINCYCLE_MISEVTCNT				(200 / 10)						// 200ms

#define RCCAR_IMUINF_SENDCYCLE					(100)							// 100ms

#define RCCAR_IMUINF_ORIENTATION_NOTSUPPORT		1
#define RCCAR_IMUINF_ORIENTATION_SUPPORT		2
#define RCCAR_IMUINF_ORIENTATION_TYPE			RCCAR_IMUINF_ORIENTATION_NOTSUPPORT

#define RCCAR_CMDVEL_COMFAILTIME				(1000 / 10)						// 1000ms
#define RCCAR_JOYCTL_COMFAILTIME				(1000 / 10)						// 1000ms

#define RCCAR_NODE_NAME							"micro_ros_rccar_node"
#define RCCAR_ROS_DOMAIN_ID						(100)

#define ROS_AGENT_PING_TIMEOUT					(50)							// 50ms
#define ROS_AGENT_PING_RETRY_CNTMAX				(5)								// 5count(5 * RCCAR_ROSMGRCTRL_CYCLE ms)

#define ROS_AGENT_COMMODE_SERIAL				1
#define ROS_AGENT_COMMODE_UDP					2
#define ROS_AGENT_COMMODE						ROS_AGENT_COMMODE_UDP


//----------------------------------------------------------------
//  <enum>
//----------------------------------------------------------------
enum RosConnectionState_Tag
{
	ROS_CNST_WIFI_DISCONNECTED			= 0	,
	ROS_CNST_WAITING_WIFI					,
	ROS_CNST_WIFI_CONNECTED					,
	ROS_CNST_WAITING_AGENT					,
	ROS_CNST_AGENT_AVAILABLE				,
	ROS_CNST_AGENT_CONNECTED				,
	ROS_CNST_AGENT_DISCONNECTED				,
};
typedef enum RosConnectionState_Tag			RosConnectionState;


//----------------------------------------------------------------
//  <union>
//----------------------------------------------------------------


//----------------------------------------------------------------
//  <struct>
//----------------------------------------------------------------
struct LedDisBuf_Tag
{
	uint8_t width;
	uint8_t height;
	CRGB buff[5 * 5];
};
typedef struct LedDisBuf_Tag				LedDisBuf;


//----------------------------------------------------------------
//  <class>
//----------------------------------------------------------------
/**
 * @brief       RcCar
 * @note        なし
 */
class RcCar
{
private:
	static const LedDisBuf DISP_DISCONNECTED;
	static const LedDisBuf DISP_BTCONNECTED;
	static const LedDisBuf DISP_ROSCONNECTED;

	SemaphoreHandle_t _mutex;
	SemaphoreHandle_t _mutex_joy;
	SemaphoreHandle_t _mutex_ros;

	AckermannDrive _rccar;

	boolean _isJoyCtl;
	boolean _isLedUpdate;

	uint32_t _failCmdVelCnt;
	uint32_t _failJoyCtlCnt;

	JoyStick _joy;
	uint32_t _JoyCtrlCycle;

	uint32_t _wifiRetryTime;

	struct micro_ros_agent_locator _locator;
	rcl_allocator_t _allocator;
	rclc_support_t _support;
	rcl_node_t _node;
	rclc_executor_t _executor;
	rcl_publisher_t _pubImu;
	rcl_publisher_t _pubLog;
	rcl_subscription_t _subJoy;
	rcl_subscription_t _subTwist;

	rcl_interfaces__msg__Log _logMsg;
	sensor_msgs__msg__Imu _imuMsg;
	sensor_msgs__msg__Joy _joyMsg;
	geometry_msgs__msg__Twist _twistMsg;

	RosConnectionState _rosConState;
	uint32_t _rosAgentPingCnt;
	uint32_t _rosMgrCtrlCycle;
	uint32_t _imuInfPubCycle;

	void PublishImuInfo( void );                                        // IMUセンサ情報配信
#if JOYSTICK_ROS2_TYPE == JOYSTICK_ROS2_SUPPORT
	static void SubscribeJoyCbkWrap( const void *arg, void *obj );      // Joy情報購読ハンドラ
	void SubscribeJoyCbk( const void *msgin );                          // Joy情報購読
#endif
	static void SubscribeTwistCbkWrap( const void *arg, void *obj );    // Twist情報購読ハンドラ
	void SubscribeTwistCbk( const void *msgin );                        // Twist情報購読
	void JoyControl( JoyStickConnectType i_type );                      // JoyStickコントロール
	void CommunicationFail( void );                                     // 通信途絶
#ifdef _SERIAL_DEBUG_
	void SerialDebug( void );                                           // シリアルデバッグ
	volatile uint32_t _ctrlCycleTime;	// DEBUG
	volatile uint32_t _btCycleTime;		// DEBUG
	volatile uint32_t _rosCycleTime;	// DEBUG
	volatile uint32_t _ctrlCycleCnt;	// DEBUG
	volatile uint32_t _btCycleCnt;		// DEBUG
	volatile uint32_t _rosCycleCnt;		// DEBUG
#endif

public:
	RcCar( void );                                                      // コンストラクタ
	~RcCar( void );                                                     // デストラクタ

	void Init( void );                                                  // イニシャライズ
	void WiFiInit( void );                                              // WiFiイニシャライズ
	void RosInit( void );                                               // Rosイニシャライズ
	boolean RosCreateEntities( void );                                  // Rosエンティティ生成
	void RosDestroyEntities( void );                                    // Rosエンティティ破棄
	void MainLoop( void );                                              // メインループ
	void MainCycle( void );                                             // 制御周期ハンドラ
#if JOYSTICK_BLUETOOTH_TYPE == JOYSTICK_BLUETOOTH_SUPPORT
	void BtJoyCtrlCycle( void );                                        // JoyStickコントロール周期
#endif
	void RosCtrlCycle( void );                                          // ROSコントロール周期
	void RosMgrCtrlCycle( void );                                       // ROS管理コントロール周期
};

#endif
