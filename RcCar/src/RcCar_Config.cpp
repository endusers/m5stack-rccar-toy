/**
 * @file RcCar_Config.cpp
 * 
 * @brief       RcCar_Config
 * @note        なし
 * 
 * @version     1.1.0
 * @date        2022/12/28
 * 
 * @copyright   (C) 2022 Motoyuki Endo
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
        0.235 ,     0.09 ,     -620 ,   620 ,    1.0 ,         2.0 ,
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
