/**
 * @file RcCar_Config.cpp
 * 
 * @brief       RcCar_Config
 * @note        なし
 * 
 * @version     1.4.0
 * @date        2024/04/10
 * 
 * @copyright   (C) 2022-2024 Motoyuki Endo
 */
#include "RcCar_Config.h"


//----------------------------------------------------------------
//  <table>
//----------------------------------------------------------------
const AckermannDriveConfig Config =
{
#if RCCAR_TYPE == RCCAR_UNIMOG
//  steering
    {
//    | minAngle | maxAngle |
        -35 ,      35 ,
//      servo
        {
            //  portConfig
            //  | pinNo | hertz | minWidth | maxWidth | isReverse |
                { 19 ,    50 ,    1000 ,     2000 ,     true      } ,
            //  smoothConfig
            //  | isSmoothEnable | coefficient |
                { false ,          0.2         } ,
        } ,
    } ,
//  throttle
    {
//    | wheelbase | diameter | minRpm | maxRpm | forwardGain | reverseGain |
        0.235 ,     0.09 ,     -450 ,   450 ,    1.0 ,         2.0 ,            // TAMIYA OP.1895 21.5T
//      minMap
        {
            // steer
            {  0.0,  5.0, 10.0, 15.0, 20.0, 25.0, 30.0, 35.0, 40.0, 45.0, 50.0, 55.0, 60.0, 65.0, 70.0, 75.0, 80.0, 85.0, 90.0 },
            // throt
            { 12.0, 12.0, 12.0, 13.0, 13.0, 14.0, 14.0, 15.0, 15.0, 15.0, 15.0, 15.0, 15.0, 15.0, 15.0, 15.0, 15.0, 15.0, 15.0 },
        },
//      servo
        {
            //  portConfig
            //  | pinNo | hertz | minWidth | maxWidth | isReverse |
                { 33 ,    50 ,    1000 ,     2000 ,     true      } ,
            //  smoothConfig
            //  | isSmoothEnable | coefficient |
                { false ,          0.2         } ,
        } ,
    } ,
#endif
#if RCCAR_TYPE == RCCAR_BRONCO
//  steering
    {
//    | minAngle | maxAngle |
        -35 ,      35 ,
//      servo
        {
            //  portConfig
            //  | pinNo | hertz | minWidth | maxWidth | isReverse |
                { 19 ,    50 ,    1000 ,     2000 ,     true      } ,
            //  smoothConfig
            //  | isSmoothEnable | coefficient |
                { false ,          0.2         } ,
        } ,
    } ,
//  throttle
    {
//    | wheelbase | diameter | minRpm | maxRpm | forwardGain | reverseGain |
        // 0.250 ,    0.095 ,     -440 ,   440 ,    1.0 ,         2.0 ,            // TAMIYA OP.1114 35T
        0.250 ,    0.095 ,     -520 ,   520 ,    1.0 ,         1.0 ,            // HOBBYWING QUICRUN 540 30T
//      minMap
        {
            // steer
            {  0.0,  5.0, 10.0, 15.0, 20.0, 25.0, 30.0, 35.0, 40.0, 45.0, 50.0, 55.0, 60.0, 65.0, 70.0, 75.0, 80.0, 85.0, 90.0 },
            // throt
            { 14.0, 14.0, 14.0, 16.0, 17.0, 18.0, 19.0, 20.0, 20.0, 20.0, 20.0, 20.0, 20.0, 20.0, 20.0, 20.0, 20.0, 20.0, 20.0 },
        },
//      servo
        {
            //  portConfig
            //  | pinNo | hertz | minWidth | maxWidth | isReverse |
                { 33 ,    50 ,    1000 ,     2000 ,     false     } ,
            //  smoothConfig
            //  | isSmoothEnable | coefficient |
                { false ,          0.2         } ,
        } ,
    } ,
#endif
#if RCCAR_TYPE == RCCAR_BRONCO_TRACKUINT
//  steering
    {
//    | minAngle | maxAngle |
        -35 ,      35 ,
//      servo
        {
            //  portConfig
            //  | pinNo | hertz | minWidth | maxWidth | isReverse |
                { 19 ,    50 ,    1000 ,     2000 ,     true      } ,
            //  smoothConfig
            //  | isSmoothEnable | coefficient |
                { false ,          0.2         } ,
        } ,
    } ,
//  throttle
    {
//    | wheelbase | diameter | minRpm | maxRpm | forwardGain | reverseGain |
        0.250 ,     0.11 ,     -165 ,   165 ,    1.0 ,         1.0 ,            // HOBBYWING QUICRUN 540 30T
//      minMap
        {
            // steer
            {  0.0,  5.0, 10.0, 15.0, 20.0, 25.0, 30.0, 35.0, 40.0, 45.0, 50.0, 55.0, 60.0, 65.0, 70.0, 75.0, 80.0, 85.0, 90.0 },
            // throt
            { 14.0, 14.0, 14.0, 16.0, 17.0, 18.0, 19.0, 20.0, 20.0, 20.0, 20.0, 20.0, 20.0, 20.0, 20.0, 20.0, 20.0, 20.0, 20.0 },
        },
//      servo
        {
            //  portConfig
            //  | pinNo | hertz | minWidth | maxWidth | isReverse |
                { 33 ,    50 ,    1000 ,     2000 ,     false     } ,
            //  smoothConfig
            //  | isSmoothEnable | coefficient |
                { false ,          0.2         } ,
        } ,
    } ,
#endif
};
