/**
 * @file main.cpp
 * 
 * @brief       main
 * @note        なし
 * 
 * @version     1.0.0
 * @date        2022/10/22
 * 
 * @copyright   (C) 2022 Motoyuki Endo
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


//----------------------------------------------------------------
//  <macro>
//----------------------------------------------------------------
#define EVENT_CTRLCYCLE             ( 1 << 0 )
#define EVENT_WAIT_BITS             ( EVENT_CTRLCYCLE )


//----------------------------------------------------------------
//  <enum>
//----------------------------------------------------------------


//----------------------------------------------------------------
//  <struct>
//----------------------------------------------------------------


//----------------------------------------------------------------
//  <function prototype>
//----------------------------------------------------------------
extern void IRAM_ATTR ControlCycle();
extern void ControlTask( void * pvParameters );
extern void BtCtrlTask( void * pvParameters );
extern void RosTask( void * pvParameters );
extern void RosMgrTask( void * pvParameters );


//----------------------------------------------------------------
//  <table>
//----------------------------------------------------------------


//----------------------------------------------------------------
//  <variable>
//----------------------------------------------------------------
hw_timer_t *timer = NULL;
TaskHandle_t xCtrlTask;
TaskHandle_t xBtCtrlTask;
TaskHandle_t xRosTask;
TaskHandle_t xRosMgrTask;
EventGroupHandle_t xEventGroup;
uint32_t cntBootCtrltask;
RcCar car;


//----------------------------------------------------------------
//  <function>
//----------------------------------------------------------------
/**
 * @brief      セットアップ
 * @note       なし
 * @param      なし
 * @retval     なし
 */
void setup() {
	// put your setup code here, to run once:

	cntBootCtrltask = 0;

	car.Init();

	xEventGroup = xEventGroupCreate();
	xEventGroupClearBits( xEventGroup, 0xFFFFFF );

	xTaskCreateUniversal(
		ControlTask,
		"ControlTask",
		2048,
		NULL,
		5,
		&xCtrlTask,
		APP_CPU_NUM
	);

#if JOYSTICK_BLUETOOTH_TYPE == JOYSTICK_BLUETOOTH_SUPPORT
	xTaskCreateUniversal(
		BtCtrlTask,
		"BtCtrlTask",
		4096,
		NULL,
		4,
		&xBtCtrlTask,
		APP_CPU_NUM
	);
#endif

	xTaskCreateUniversal(
		RosTask,
		"RosTask",
		4096,
		NULL,
		3,
		&xRosTask,
		APP_CPU_NUM
	);

	// xTaskCreateUniversal(
	// 	RosMgrTask,
	// 	"RosMgrTask",
	// 	4096,
	// 	NULL,
	// 	2,
	// 	&xRosMgrTask,
	// 	APP_CPU_NUM
	// );

	timer = timerBegin( 0, 80, true );
	timerAttachInterrupt( timer, &ControlCycle, true );
	timerAlarmWrite( timer, RCCAR_MAIN_CYCLE, true );
	timerAlarmEnable( timer );
}


/**
 * @brief      メインループ
 * @note       なし
 * @param      なし
 * @retval     なし
 */
void loop() {
	// put your main code here, to run repeatedly:
	car.MainLoop();
	delay(1);
}


/**
 * @brief      制御周期ハンドラ
 * @note       なし
 * @param      なし
 * @retval     なし
 */
void IRAM_ATTR ControlCycle()
{
	BaseType_t xHigherPriorityTaskWoken;
	xEventGroupSetBitsFromISR( xEventGroup, EVENT_CTRLCYCLE, &xHigherPriorityTaskWoken );

	cntBootCtrltask++;

	if( cntBootCtrltask >= RCCAR_MAINCYCLE_MISEVTCNT )
	{
		// ESP.restart();
	}
}


/**
 * @brief      制御周期タスク
 * @note       なし
 * @param      なし
 * @retval     なし
 */
void ControlTask( void * pvParameters )
{
	for(;;)
	{
		EventBits_t uxBits;

		uxBits = xEventGroupWaitBits(
			xEventGroup,
			EVENT_WAIT_BITS,
			pdTRUE,
			pdFALSE,
			portMAX_DELAY
		);

		if( ( uxBits & EVENT_CTRLCYCLE ) != 0 ){
			cntBootCtrltask = 0;
			car.MainCycle();
		}
	}
}


/**
 * @brief      BtCtrlタスク
 * @note       なし
 * @param      なし
 * @retval     なし
 */
#if JOYSTICK_BLUETOOTH_TYPE == JOYSTICK_BLUETOOTH_SUPPORT
void BtCtrlTask( void * pvParameters )
{
	for(;;)
	{
		car.BtJoyCtrlCycle();
		vTaskDelay(1);
	}
}
#endif


/**
 * @brief      ROSタスク
 * @note       なし
 * @param      なし
 * @retval     なし
 */
void RosTask( void * pvParameters )
{
	for(;;)
	{
		car.RosMgrCtrlCycle();
		car.RosCtrlCycle();
		vTaskDelay(1);
	}
}


/**
 * @brief      ROS管理タスク
 * @note       なし
 * @param      なし
 * @retval     なし
 */
// void RosMgrTask( void * pvParameters )
// {
// 	for(;;)
// 	{
// 		car.RosMgrCtrlCycle();
// 		vTaskDelay(1);
// 	}
// }
