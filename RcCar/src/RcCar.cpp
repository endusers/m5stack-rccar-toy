/**
 * @file RcCar.cpp
 * 
 * @brief       RcCar
 * @note        なし
 * 
 * @version     1.2.0
 * @date        2023/04/10
 * 
 * @copyright   (C) 2022-2023 Motoyuki Endo
 */
#include "RcCar.h"


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

#define RCLRETUNUSED(ret) (void)(ret)
#define RCLRETCHECK(ret) if( ret != RCL_RET_OK ){ return false; }

#define ROS_INFO(...)                                                            \
do{                                                                              \
    rcl_ret_t ret;                                                               \
    snprintf( _logMsg.msg.data, _logMsg.msg.capacity, __VA_ARGS__ );             \
    _logMsg.msg.size = strlen(_logMsg.msg.data);                                 \
    ret = rcl_publish( &_pubLog, &_logMsg, NULL );                               \
    RCLRETUNUSED( ret );                                                         \
}while(0)                                                                        \


//----------------------------------------------------------------
//  <constant>
//----------------------------------------------------------------
const LedDisBuf RcCar::DISP_DISCONNECTED =
{
	5 , 5 ,
	{
		0x000000, 0x00FF00, 0x000000, 0x00FF00, 0x000000,
		0x00FF00, 0x00FF00, 0x00FF00, 0x00FF00, 0x00FF00,
		0x00FF00, 0x00FF00, 0x00FF00, 0x00FF00, 0x00FF00,
		0x000000, 0x00FF00, 0x00FF00, 0x00FF00, 0x000000,
		0x000000, 0x000000, 0x00FF00, 0x000000, 0x000000,
	} ,
};

const LedDisBuf RcCar::DISP_BTCONNECTED =
{
	5 , 5 ,
	{
		0x000000, 0xFF0000, 0x000000, 0xFF0000, 0x000000,
		0xFF0000, 0xFF0000, 0xFF0000, 0xFF0000, 0xFF0000,
		0xFF0000, 0xFF0000, 0xFF0000, 0xFF0000, 0xFF0000,
		0x000000, 0xFF0000, 0xFF0000, 0xFF0000, 0x000000,
		0x000000, 0x000000, 0xFF0000, 0x000000, 0x000000,
	} ,
};

const LedDisBuf RcCar::DISP_ROSCONNECTED =
{
	5 , 5 ,
	{
		0x000000, 0x0000FF, 0x000000, 0x0000FF, 0x000000,
		0x0000FF, 0x0000FF, 0x0000FF, 0x0000FF, 0x0000FF,
		0x0000FF, 0x0000FF, 0x0000FF, 0x0000FF, 0x0000FF,
		0x000000, 0x0000FF, 0x0000FF, 0x0000FF, 0x000000,
		0x000000, 0x000000, 0x0000FF, 0x000000, 0x000000,
	} ,
};


//----------------------------------------------------------------
//  <function>
//----------------------------------------------------------------
/**
 * @brief       コンストラクタ
 * @note        なし
 * @param       なし
 * @retval      なし
 */
RcCar::RcCar( void )
	: _rccar( Config )
{
	_mutex = xSemaphoreCreateMutex();
	xSemaphoreGive( _mutex );

	_mutex_joy = xSemaphoreCreateMutex();
	xSemaphoreGive( _mutex_joy );

	_mutex_ros = xSemaphoreCreateMutex();
	xSemaphoreGive( _mutex_ros );

	_isJoyCtl = false;
	_isLedUpdate = false;

	_JoyCtrlCycle = 0;

	_failCmdVelCnt = 0;
	_failJoyCtlCnt = 0;

	_imuInfPubCycle = 0;
	_imuMsg.header.stamp.sec = 0;
	_imuMsg.header.stamp.nanosec = 0;
	_imuMsg.header.frame_id.capacity = strlen("imu_link");
	_imuMsg.header.frame_id.data = (char *)"imu_link";
	_imuMsg.header.frame_id.size = strlen("imu_link");
	memset( &_imuMsg.orientation_covariance[0], 0, sizeof(_imuMsg.orientation_covariance) );
#if RCCAR_IMUINF_ORIENTATION_TYPE == RCCAR_IMUINF_ORIENTATION_NOTSUPPORT
	_imuMsg.orientation_covariance[0] = -1.0;
#endif
	_imuMsg.orientation.x = 0.0;
	_imuMsg.orientation.y = 0.0;
	_imuMsg.orientation.z = 0.0;
	_imuMsg.orientation.w = 0.0;
	memset( &_imuMsg.linear_acceleration_covariance[0], 0, sizeof(_imuMsg.linear_acceleration_covariance) );
	_imuMsg.linear_acceleration.x = 0.0;
	_imuMsg.linear_acceleration.y = 0.0;
	_imuMsg.linear_acceleration.z = 0.0;
	memset( &_imuMsg.angular_velocity_covariance[0], 0, sizeof(_imuMsg.angular_velocity_covariance) );
	_imuMsg.angular_velocity.x = 0.0;
	_imuMsg.angular_velocity.y = 0.0;
	_imuMsg.angular_velocity.z = 0.0;

	_logMsg.stamp.sec = 0;
	_logMsg.stamp.nanosec = 0;
	_logMsg.level = rcl_interfaces__msg__Log__INFO;
	_logMsg.name.capacity = strlen(RCCAR_NODE_NAME);
	_logMsg.name.data = (char *)RCCAR_NODE_NAME;
	_logMsg.name.size = strlen(_logMsg.name.data);
	_logMsg.msg.capacity = 128;
	_logMsg.msg.data = new char[_logMsg.msg.capacity];
	_logMsg.msg.size = 0;
	_logMsg.file.capacity = strlen("");
	_logMsg.file.data = (char *)"";
	_logMsg.file.size = strlen(_logMsg.file.data);
	_logMsg.function.capacity = strlen("");
	_logMsg.function.data = (char *)"";
	_logMsg.function.size = strlen(_logMsg.function.data);
	_logMsg.line = (uint32_t)NULL;

	_joyMsg.axes.capacity = JOYSTICK_JOYMSG_AXES_CAPACITY;
	_joyMsg.axes.data = new float[_joyMsg.axes.capacity];
	_joyMsg.axes.size = _joyMsg.axes.capacity;
	_joyMsg.buttons.capacity = JOYSTICK_JOYMSG_BUTTONS_CAPACITY;
	_joyMsg.buttons.data = new int32_t[_joyMsg.buttons.capacity];
	_joyMsg.buttons.size = _joyMsg.buttons.capacity;

	_twistMsg.linear.x = 0.0;
	_twistMsg.linear.y = 0.0;
	_twistMsg.linear.z = 0.0;
	_twistMsg.angular.x = 0.0;
	_twistMsg.angular.y = 0.0;
	_twistMsg.angular.z = 0.0;
}


/**
 * @brief       デストラクタ
 * @note        なし
 * @param       なし
 * @retval      なし
 */
RcCar::~RcCar( void )
{
	SAFE_DELETE_ARRAY( _logMsg.msg.data );
	SAFE_DELETE_ARRAY( _joyMsg.axes.data );
	SAFE_DELETE_ARRAY( _joyMsg.buttons.data );
}


/**
 * @brief       イニシャライズ
 * @note        なし
 * @param       なし
 * @retval      なし
 */
void RcCar::Init( void )
{
#ifdef _SERIAL_DEBUG_
	Serial.begin( 115200 );
	_ctrlCycleTime = (uint32_t)millis();
	_btCycleTime = (uint32_t)millis();
	_rosCycleTime = (uint32_t)millis();
	_ctrlCycleCnt = 0;
	_btCycleCnt = 0;
	_rosCycleCnt = 0;
#endif

	M5.begin( true , true , true );
	delay(50);

	M5.IMU.Init();

	// M5.IMU.SetGyroFsr( M5.IMU.GFS_500DPS ); 
	// M5.IMU.SetAccelFsr( M5.IMU.AFS_4G );

	_rccar.Init();

	xSemaphoreTake( _mutex , portMAX_DELAY );
	_rccar.SetSteering( 0.0 );
	_rccar.SetSpeed( 0.0 );
	xSemaphoreGive( _mutex );

	_joy.Init();
	_JoyCtrlCycle = 0;

	_wifiRetryTime = (uint32_t)millis();

	_rosConState = ROS_CNST_WIFI_DISCONNECTED;
	_rosAgentPingCnt = 0;
	_rosMgrCtrlCycle = (uint32_t)millis();
	_imuInfPubCycle = (uint32_t)millis();

	M5.dis.setWidthHeight( 5, 5 );
	M5.dis.displaybuff( (uint8_t *)&DISP_DISCONNECTED );
}


/**
 * @brief       WiFiイニシャライズ
 * @note        なし
 * @param       なし
 * @retval      なし
 */
void RcCar::WiFiInit( void )
{
	NvmConfig nvm;

	WiFi.begin( (char *)nvm.ssid.c_str(), (char *)nvm.pass.c_str() );
}


/**
 * @brief       Rosイニシャライズ
 * @note        なし
 * @param       なし
 * @retval      なし
 */
void RcCar::RosInit( void )
{
#if ROS_AGENT_COMMODE == ROS_AGENT_COMMODE_UDP
	NvmConfig nvm;

	_locator.address.fromString( (char *)nvm.rosAgentIp.c_str() );
	_locator.port = atoi((char *)nvm.rosAgentPort.c_str());

	rmw_uros_set_custom_transport(
		false,
		(void *) &_locator,
		arduino_wifi_transport_open,
		arduino_wifi_transport_close,
		arduino_wifi_transport_write,
		arduino_wifi_transport_read
	);
#endif
#if ROS_AGENT_COMMODE == ROS_AGENT_COMMODE_SERIAL
	rmw_uros_set_custom_transport(
		true,
		NULL,
		arduino_uart_transport_open,
		arduino_uart_transport_close,
		arduino_uart_transport_write,
		arduino_uart_transport_read
	);
#endif
}


/**
 * @brief       Rosエンティティ生成
 * @note        なし
 * @param       なし
 * @retval      true 正常終了
 * @retval      false 異常終了
 */
boolean RcCar::RosCreateEntities( void )
{
	rcl_ret_t ret;
	rcl_node_options_t node_ops;

	_imuInfPubCycle = (uint32_t)millis();

	_allocator = rcl_get_default_allocator();
	ret = rclc_support_init( &_support, 0, NULL, &_allocator );
	RCLRETCHECK( ret );

	node_ops = rcl_node_get_default_options();

	node_ops.domain_id = (size_t)(RCCAR_ROS_DOMAIN_ID);

	ret = rclc_node_init_with_options( &_node, (const char *)RCCAR_NODE_NAME, "", &_support, &node_ops );
	RCLRETCHECK( ret );

	// ret = rclc_node_init_default( &_node, (const char *)RCCAR_NODE_NAME, "", &_support );
	// RCLRETCHECK( ret );

	ret = rclc_publisher_init_best_effort(
		&_pubLog,
		&_node,
		ROSIDL_GET_MSG_TYPE_SUPPORT( rcl_interfaces, msg, Log ),
		"rosout" );
	RCLRETCHECK( ret );

	ret = rclc_publisher_init_best_effort(
		&_pubImu,
		&_node,
		ROSIDL_GET_MSG_TYPE_SUPPORT( sensor_msgs, msg, Imu ),
		"rccar_imu" );
	RCLRETCHECK( ret );

#if JOYSTICK_ROS2_TYPE == JOYSTICK_ROS2_SUPPORT
	ret = rclc_subscription_init_default(
		&_subJoy,
		&_node,
		ROSIDL_GET_MSG_TYPE_SUPPORT( sensor_msgs, msg, Joy ),
		"joy" );
	RCLRETCHECK( ret );
#endif

	ret = rclc_subscription_init_default(
		&_subTwist,
		&_node,
		ROSIDL_GET_MSG_TYPE_SUPPORT( geometry_msgs, msg, Twist ),
		"cmd_vel" );
	RCLRETCHECK( ret );

	ret = rclc_executor_init( &_executor, &_support.context, 2, &_allocator );
	RCLRETCHECK( ret );

#if JOYSTICK_ROS2_TYPE == JOYSTICK_ROS2_SUPPORT
	ret = rclc_executor_add_subscription_with_context(
		&_executor,
		&_subJoy,
		&_joyMsg,
		&RcCar::SubscribeJoyCbkWrap, this,
		ON_NEW_DATA );
	RCLRETCHECK( ret );
#endif

	ret = rclc_executor_add_subscription_with_context(
		&_executor,
		&_subTwist,
		&_twistMsg,
		&RcCar::SubscribeTwistCbkWrap, this,
		ON_NEW_DATA );
	RCLRETCHECK( ret );

	return true;
}


/**
 * @brief       Rosエンティティ破棄
 * @note        なし
 * @param       なし
 * @retval      なし
 */
void RcCar::RosDestroyEntities( void )
{
	rcl_ret_t ret;
	rmw_context_t *rmw_context;

	rmw_context = rcl_context_get_rmw_context( &_support.context );
	(void)rmw_uros_set_context_entity_destroy_session_timeout( rmw_context, 0 );

	ret = rcl_publisher_fini( &_pubLog, &_node );
	RCLRETUNUSED( ret );
	ret = rcl_publisher_fini( &_pubImu, &_node );
	RCLRETUNUSED( ret );
#if JOYSTICK_ROS2_TYPE == JOYSTICK_ROS2_SUPPORT
	ret = rcl_subscription_fini( &_subJoy, &_node );
	RCLRETUNUSED( ret );
#endif
	ret = rcl_subscription_fini( &_subTwist, &_node );
	RCLRETUNUSED( ret );
	ret = rclc_executor_fini( &_executor );
	RCLRETUNUSED( ret );
	ret = rcl_node_fini( &_node );
	RCLRETUNUSED( ret );
	ret = rclc_support_fini( &_support );
	RCLRETUNUSED( ret );
}


/**
 * @brief       メインループ
 * @note        なし
 * @param       なし
 * @retval      なし
 */
void RcCar::MainLoop( void )
{
	LedDisBuf *disp;
	boolean isStop;
#ifdef _SERIAL_DEBUG_
	SerialDebug();
#endif

	if( _isLedUpdate )
	{
		_isLedUpdate = false;
		disp = (LedDisBuf *)&DISP_DISCONNECTED;
		isStop = true;

		if( _joy.isConnectedBt )
		{
			disp = (LedDisBuf *)&DISP_BTCONNECTED;
			isStop = false;
		}

		if( _rosConState == ROS_CNST_AGENT_CONNECTED )
		{
			disp = (LedDisBuf *)&DISP_ROSCONNECTED;
			isStop = false;
		}

		M5.dis.displaybuff( (uint8_t *)disp );

		if( isStop )
		{
			xSemaphoreTake( _mutex , portMAX_DELAY );
			_rccar.SetSpeed( 0.0 );
			xSemaphoreGive( _mutex );
		}
	}
}


/**
 * @brief       制御周期ハンドラ
 * @note        なし
 * @param       なし
 * @retval      なし
 */
void RcCar::MainCycle( void )
{
	xSemaphoreTake( _mutex , portMAX_DELAY );
	_rccar.ControlCycle();
	xSemaphoreGive( _mutex );

	CommunicationFail();

#ifdef _SERIAL_DEBUG_
	{	// DEBUG
		uint32_t time;
		time = (uint32_t)millis();
		_ctrlCycleCnt = time - _ctrlCycleTime;
		_ctrlCycleTime = time;
	}
#endif
}


/**
 * @brief       JoyStickコントロール周期
 * @note        なし
 * @param       なし
 * @retval      なし
 */
#if JOYSTICK_BLUETOOTH_TYPE == JOYSTICK_BLUETOOTH_SUPPORT
void RcCar::BtJoyCtrlCycle( void )
{
	uint32_t getTime;

	getTime = (uint32_t)millis();

	if( getTime > _JoyCtrlCycle )
	{
		_JoyCtrlCycle = getTime + RCCAR_JOYCTRL_CYCLE;

		xSemaphoreTake( _mutex_joy , portMAX_DELAY );
		_joy.UpdateJoyStickInfoBt( &PS4.data );
		if( _joy.isConnectedBt )
		{
			JoyControl( JOYSTKCONTYPE_BT );
		}
		xSemaphoreGive( _mutex_joy );

		if( _joy.isBeforeConnectedBt != _joy.isConnectedBt )
		{
			_isLedUpdate = true;
		}

#ifdef _SERIAL_DEBUG_
		{	// DEBUG
			uint32_t time;
			time = (uint32_t)millis();
			_btCycleCnt = time - _btCycleTime;
			_btCycleTime = time;
		}
#endif
	}
}
#endif


/**
 * @brief       ROSコントロール周期
 * @note        なし
 * @param       なし
 * @retval      なし
 */
void RcCar::RosCtrlCycle( void )
{
	xSemaphoreTake( _mutex_ros , portMAX_DELAY );
	if( _rosConState == ROS_CNST_AGENT_CONNECTED  )
	{
		PublishImuInfo();
		rclc_executor_spin_some( &_executor, RCL_MS_TO_NS(10) );
	}
	xSemaphoreGive( _mutex_ros );

#ifdef _SERIAL_DEBUG_
	{	// DEBUG
		uint32_t time;
		time = (uint32_t)millis();
		_rosCycleCnt = time - _rosCycleTime;
		_rosCycleTime = time;
	}
#endif
}


/**
 * @brief       ROS管理コントール周期
 * @note        なし
 * @param       なし
 * @retval      なし
 */
void RcCar::RosMgrCtrlCycle( void )
{
	uint32_t getTime;
	RosConnectionState beforeState;

	getTime = (uint32_t)millis();

	if( getTime > _rosMgrCtrlCycle )
	{
		_rosMgrCtrlCycle = getTime + RCCAR_ROSMGRCTRL_CYCLE;
		beforeState = _rosConState;

		switch( _rosConState )
		{
			case ROS_CNST_WIFI_DISCONNECTED :
#if RCCAR_JOYCONNECT_MODE == RCCAR_JOYCONNECT_PREFERBLUETOOTH
				if( !_joy.isConnectedBt )
				{
#endif
#if ROS_AGENT_COMMODE == ROS_AGENT_COMMODE_UDP
					WiFiInit();
					_wifiRetryTime = (uint32_t)millis() + 3000;
					_rosConState = ROS_CNST_WAITING_WIFI;
#endif
#if ROS_AGENT_COMMODE == ROS_AGENT_COMMODE_SERIAL
					_rosConState = ROS_CNST_WIFI_CONNECTED;
#endif
#if RC_JOYCONNECT_MODE == RCCAR_JOYCONNECT_PREFERBLUETOOTH
				}
#endif
				break;
			case ROS_CNST_WAITING_WIFI :
#if ROS_AGENT_COMMODE == ROS_AGENT_COMMODE_UDP
				if( WiFi.status() == WL_CONNECTED )
				{
					configTime( 0, 0, "ntp.nict.jp" );
					_rosConState = ROS_CNST_WIFI_CONNECTED;
				}
				else
				{
					if( (uint32_t)millis() > _wifiRetryTime )
					{
						WiFi.disconnect( true, true );
						_rosConState = ROS_CNST_WIFI_DISCONNECTED;
					}
				}
#endif
				break;
			case ROS_CNST_WIFI_CONNECTED :
				RosInit();
				_rosConState = ROS_CNST_WAITING_AGENT;
				break;
			case ROS_CNST_WAITING_AGENT :
				if( rmw_uros_ping_agent(ROS_AGENT_PING_TIMEOUT, 10) == RMW_RET_OK )
				{
					_rosAgentPingCnt = 0;
					_rosConState = ROS_CNST_AGENT_AVAILABLE;
				}
				else{
#if ROS_AGENT_COMMODE == ROS_AGENT_COMMODE_UDP
					if( WiFi.status() != WL_CONNECTED )
					{
						WiFi.disconnect( true, true );
						_rosConState = ROS_CNST_WIFI_DISCONNECTED;
					}
#endif
#if RCCAR_JOYCONNECT_MODE == RCCAR_JOYCONNECT_PREFERBLUETOOTH
					if( _joy.isConnectedBt )
					{
#if ROS_AGENT_COMMODE == ROS_AGENT_COMMODE_UDP
						WiFi.disconnect( true, true );
#endif
						_rosConState = ROS_CNST_WIFI_DISCONNECTED;
					}
#endif
				}
				break;
			case ROS_CNST_AGENT_AVAILABLE :
				if( RosCreateEntities() )
				{
					_rosConState = ROS_CNST_AGENT_CONNECTED;
				}
				else
				{
					_rosConState = ROS_CNST_AGENT_DISCONNECTED;
				}
				break;
			case ROS_CNST_AGENT_CONNECTED :
				if( rmw_uros_ping_agent(ROS_AGENT_PING_TIMEOUT, 3) != RMW_RET_OK )
				{
					_rosAgentPingCnt++;
					if( _rosAgentPingCnt >= ROS_AGENT_PING_RETRY_CNTMAX )
					{
						_rosConState = ROS_CNST_AGENT_DISCONNECTED;
					}
				}
				else
				{
					xSemaphoreTake( _mutex_ros , portMAX_DELAY );
#if ROS_AGENT_COMMODE == ROS_AGENT_COMMODE_UDP
					if( WiFi.status() != WL_CONNECTED )
					{
						RosDestroyEntities();
						WiFi.disconnect( true, true );
						_rosConState = ROS_CNST_WIFI_DISCONNECTED;
					}
#endif
#if RCCAR_JOYCONNECT_MODE == RCCAR_JOYCONNECT_PREFERBLUETOOTH
					if( _joy.isConnectedBt )
					{
						RosDestroyEntities();
#if ROS_AGENT_COMMODE == ROS_AGENT_COMMODE_UDP
						WiFi.disconnect( true, true );
#endif
						_rosConState = ROS_CNST_WIFI_DISCONNECTED;
					}
#endif
					xSemaphoreGive( _mutex_ros );
					_rosAgentPingCnt = 0;
				}
				break;
			case ROS_CNST_AGENT_DISCONNECTED :
				RosDestroyEntities();
				_rosConState = ROS_CNST_WAITING_AGENT;
				break;
			default :
				// DO_NOTHING
				break;
		}
		if( beforeState != _rosConState )
		{
			if( _rosConState == ROS_CNST_AGENT_CONNECTED )
			{
				_isLedUpdate = true;
			}
			if( beforeState == ROS_CNST_AGENT_CONNECTED )
			{
				_isLedUpdate = true;
			}
		}
	}
}


/**
 * @brief       IMUセンサ情報配信
 * @note        なし
 * @param       なし
 * @retval      なし
 */
void RcCar::PublishImuInfo( void )
{
	rcl_ret_t ret;
	uint32_t getTime;
	float accX;
	float accY;
	float accZ;
	float gyroX;
	float gyroY;
	float gyroZ;
	float pitch;
	float roll;
	float yaw;
	float quat[4];
	time_t now;

	getTime = (uint32_t)millis();

	if( getTime > _imuInfPubCycle )
	{
		M5.IMU.getGyroData( &gyroX, &gyroY, &gyroZ );
		M5.IMU.getAccelData( &accX, &accY, &accZ );
		M5.IMU.getAhrsData( &pitch, &roll, &yaw );	// Madgwick Filter

#if RCCAR_IMUINF_ORIENTATION_TYPE == RCCAR_IMUINF_ORIENTATION_SUPPORT
		{
			// rpy * DEG_TO_RAD * 0.5
			float cr = cos( (roll * (M_PI/180.0)) / 2 );
			float sr = sin( (roll * (M_PI/180.0)) / 2 );
			float cp = cos( (pitch * (M_PI/180.0)) / 2 );
			float sp = sin( (pitch * (M_PI/180.0)) / 2 );
			float cy = cos( (yaw * (M_PI/180.0)) / 2 );
			float sy = sin( (yaw * (M_PI/180.0)) / 2 );

			quat[0] = sr * cp * cy - cr * sp * sy;
			quat[1] = cr * sp * cy + sr * cp * sy;
			quat[2] = cr * cp * sy - sr * sp * cy;
			quat[3] = cr * cp * cy + sr * sp * sy;
		}
#endif

		time( &now );

		_imuMsg.header.stamp.sec = (int32_t)now;
#if RCCAR_IMUINF_ORIENTATION_TYPE == RCCAR_IMUINF_ORIENTATION_SUPPORT
		_imuMsg.orientation.x = quat[0];
		_imuMsg.orientation.y = quat[1];
		_imuMsg.orientation.z = quat[2];
		_imuMsg.orientation.w = quat[3];
#endif
		_imuMsg.linear_acceleration.x = accX;
		_imuMsg.linear_acceleration.y = accY;
		_imuMsg.linear_acceleration.z = accZ;
		_imuMsg.angular_velocity.x = gyroX;
		_imuMsg.angular_velocity.y = gyroY;
		_imuMsg.angular_velocity.z = gyroZ;

		ret = rcl_publish( &_pubImu, &_imuMsg, NULL );
		RCLRETUNUSED( ret );

		_imuInfPubCycle = getTime + RCCAR_IMUINF_SENDCYCLE;
	}
}


/**
 * @brief       Joy情報購読ハンドラ
 * @note        なし
 * @param[in]   arg : Joy情報
 * @param[in]   obj : コールバックのthisポインタ
 * @retval      なし
 */
#if JOYSTICK_ROS2_TYPE == JOYSTICK_ROS2_SUPPORT
void RcCar::SubscribeJoyCbkWrap( const void *arg, void *obj )
{
	return reinterpret_cast<RcCar*>(obj)->SubscribeJoyCbk(arg);
}
#endif


/**
 * @brief       Joy情報購読
 * @note        なし
 * @param[in]   msgin : Joy情報
 * @retval      なし
 */
#if JOYSTICK_ROS2_TYPE == JOYSTICK_ROS2_SUPPORT
void RcCar::SubscribeJoyCbk( const void *msgin )
{
	boolean isUpdate;

	_failJoyCtlCnt = 0;
	_isJoyCtl = true;

	isUpdate = false;

	xSemaphoreTake( _mutex_joy , portMAX_DELAY );
	_joy.UpdateJoyStickInfoRos2( (sensor_msgs__msg__Joy *)msgin );
	JoyControl( JOYSTKCONTYPE_ROS2 );
	xSemaphoreGive( _mutex_joy );
}
#endif


/**
 * @brief       Twist情報購読ハンドラ
 * @note        なし
 * @param[in]   arg : Twist情報
 * @param[in]   obj : コールバックのthisポインタ
 * @retval      なし
 */
void RcCar::SubscribeTwistCbkWrap( const void *arg, void *obj )
{
	return reinterpret_cast<RcCar*>(obj)->SubscribeTwistCbk(arg);
}


/**
 * @brief       Twist情報購読
 * @note        なし
 * @param[in]   msgin : Twist情報
 * @retval      なし
 */
void RcCar::SubscribeTwistCbk( const void *msgin )
{
	const geometry_msgs__msg__Twist *msg = (const geometry_msgs__msg__Twist *)msgin;

	float v;
	float w;
	float angle;

	_failCmdVelCnt = 0;

	v = msg->linear.x;
	w = msg->angular.z;

	angle = _rccar.ConvertOmega2Degree( v, w );

	if( !_isJoyCtl )
	{
		xSemaphoreTake( _mutex , portMAX_DELAY );
		_rccar.SetSteering( angle );
		_rccar.SetSpeed( v );
		xSemaphoreGive( _mutex );
	}
}


/**
 * @brief       JoyStickコントロール
 * @note        なし
 * @param       なし
 * @retval      なし
 */
void RcCar::JoyControl( JoyStickConnectType i_type )
{
	JoyInfo *joyInf;
	JoyInfo *beforeJoyInf;
	float angle;
	float throttle;
	float minAngle;
	float maxAngle;

	_failJoyCtlCnt = 0;
	_isJoyCtl = true;

	minAngle = (float)Config.steering.minAngle;
	maxAngle = (float)Config.steering.maxAngle;

	if( i_type == JOYSTKCONTYPE_BT )
	{
		joyInf = &_joy.joyInfBt;
		beforeJoyInf = &_joy.beforeJoyInfBt;
	}
	else
	{
		joyInf = &_joy.joyInfRos2;
		beforeJoyInf = &_joy.beforeJoyInfRos2;
	}

	angle= MAPF( joyInf->rStickH , -1.0, 1.0, minAngle, maxAngle );
	throttle = MAPF( joyInf->lStickV, -1.0, 1.0, -100.0, 100.0 );

	xSemaphoreTake( _mutex , portMAX_DELAY );
	_rccar.SetSteering( angle );
	_rccar.SetThrottle( throttle );
	xSemaphoreGive( _mutex );
}


/**
 * @brief       通信途絶
 * @note        なし
 * @param       なし
 * @retval      なし
 */
void RcCar::CommunicationFail( void )
{
	_failCmdVelCnt++;
	_failJoyCtlCnt++;

	_failCmdVelCnt = constrain( _failCmdVelCnt , 0 , RCCAR_CMDVEL_COMFAILTIME );
	_failJoyCtlCnt = constrain( _failJoyCtlCnt , 0 , RCCAR_JOYCTL_COMFAILTIME );

	if( _isJoyCtl )
	{
		if( _failJoyCtlCnt >= RCCAR_JOYCTL_COMFAILTIME )
		{
			_isJoyCtl = false;
			xSemaphoreTake( _mutex , portMAX_DELAY );
			_rccar.SetSpeed( 0.0 );
			xSemaphoreGive( _mutex );
		}
	}
	else
	{
		if( _failCmdVelCnt >= RCCAR_CMDVEL_COMFAILTIME )
		{
			xSemaphoreTake( _mutex , portMAX_DELAY );
			_rccar.SetSpeed( 0.0 );
			xSemaphoreGive( _mutex );
		}
	}
}


#ifdef _SERIAL_DEBUG_
/**
 * @brief       シリアルデバッグ
 * @note        なし
 * @param       なし
 * @retval      なし
 */
void RcCar::SerialDebug( void )
{
	static uint32_t print_cycle = 0;
	uint32_t getTime;
	String read;
	int idx;
	String cmd;
	String data;
	int value_i;
	float value_f;

	if( Serial.available() )
	{
		read = Serial.readStringUntil( '\n' );
		idx = read.indexOf( '=' );

		if( idx <= 0 )
		{
			cmd = read.substring( 0 );
			data = "";
		}
		else{
			cmd = read.substring( 0 , idx );
			cmd.trim();
			data = read.substring( idx + 1 );
			data.trim();
		}

		if( cmd.equals( "smooth" ) )
		{
			value_f = data.toFloat();
			xSemaphoreTake( _mutex , portMAX_DELAY );
			_rccar.steering.smooth.coefficient = value_f;
			_rccar.throttle.smooth.coefficient = value_f;
			xSemaphoreGive( _mutex );
			Serial.print("smooth=");
			Serial.print(value_f);
			Serial.print("\n");
		}
		else
		{
			value_i = cmd.toInt();
			xSemaphoreTake( _mutex , portMAX_DELAY );
			// _rccar.SetSteering( (float)value_i );
			_rccar.SetThrottle( (float)value_i );
			xSemaphoreGive( _mutex );
		}
	}

	getTime = (uint32_t)millis();

	if( getTime > print_cycle )
	{
		// Serial.print(_rccar.steering.reqAngle);
		// Serial.print(",");
		// Serial.print(_rccar.steering.targetAngle);
		// Serial.print(",");
		// Serial.print(_rccar.steering.outAngle);
		// Serial.print(",");
		// Serial.print(_rccar.throttle.reqAngle);
		// Serial.print(",");
		// Serial.print(_rccar.throttle.targetAngle);
		// Serial.print(",");
		// Serial.print(_rccar.throttle.outAngle);
		// Serial.print("\n");

		// Serial.print(_joy.isConnectedBt);
		// Serial.print(",");
		// Serial.print(esp_get_free_heap_size());
		// Serial.print(",");
		// Serial.print(_rosAgentPingCnt);
		// Serial.print(",");
		// Serial.print(_rosConState);
		// Serial.print(",");
		// Serial.print(_ctrlCycleTime);
		// Serial.print(",");
		// Serial.print(_btCycleTime);
		// Serial.print(",");
		// Serial.print(_rosCycleTime);
		// Serial.print(",");
		// Serial.print(_ctrlCycleCnt);
		// Serial.print(",");
		// Serial.print(_btCycleCnt);
		// Serial.print(",");
		// Serial.print(_rosCycleCnt);
		// Serial.print("\n");

		print_cycle = getTime + 100;
	}
}
#endif
