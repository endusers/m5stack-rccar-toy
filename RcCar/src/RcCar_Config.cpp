/**
 * @file RcCar_Config.cpp
 * 
 * @brief       RcCar_Config
 * @note        なし
 * 
 * @version     1.1.0
 * @date        2023/01/15
 * 
 * @copyright   (C) 2022-2023 Motoyuki Endo
 */
#include "RcCar_Config.h"


//----------------------------------------------------------------
//  <table>
//----------------------------------------------------------------
const AckermannDriveConfig Config =
{
//  steering
    {
//    | minAngle | maxAngle |
        -35 ,      35 ,
//      servo
        {
            //  portConfig
            //  | pinNo | hertz | minWidth | maxWidth |
                { 19 ,    50 ,    1000 ,     2000     } ,
            //  smoothConfig
            //  | isSmoothEnable | coefficient |
                { false ,          0.2         } ,
        } ,
    } ,
//  throttle
    {
//    | wheelbase | diameter | minRpm | maxRpm | forwardGain | reverseGain |
        0.235 ,     0.09 ,     -450 ,   450 ,    1.0 ,         2.0 ,
//      servo
        {
            //  portConfig
            //  | pinNo | hertz | minWidth | maxWidth |
                { 33 ,    50 ,    1000 ,     2000     } ,
            //  smoothConfig
            //  | isSmoothEnable | coefficient |
                { false ,          0.2         } ,
        } ,
    } ,
};