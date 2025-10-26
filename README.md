# m5stack-rccar-toy

## はじめに

本ソフトは M5 ATOM Matrix / M5 ATOMS3R でRcCarを動かすソフトである

- micro-ROS接続(Serial/WiFi)の場合

  microROSのTwistメッセージ(cmd_vel)でRcCarを動かす  
  microROSのJoyメッセージ(joy)でRcCarを動かす(コンフィグ変更)  
  M5AtomのIMUのデータをmicroROSのImuメッセージ(rccar_imu)として配信する  
  Servoの出力情報をmicroROSのFloat32MultiArrayメッセージ(rccar_servo_info)として配信する  

- Bluetooth接続の場合

  - M5 ATOM Matrix (Bluetooth Classic)

    PS4コントローラをBluetoothで接続して動かす  

  - M5 ATOMS3R (Bluetooth Low Energy(BLE))

    XboxコントローラをBluetoothで接続して動かす  

## 動作環境

- ハードウェア

  - RCカー
    - 1/10RC フォード ブロンコ 2021 (CC-02シャーシ)
      - ホイール仕様
        - Goolsky AUSTAR 110mm 1.9インチ リム ラバー タイヤ ホイール
        - HOBBYWING COMBO-XR10 Justock G3S ESC & 3650 SD G2.1 ブラシレスモーター 25.5T
        - HOOYIJ DS3245SG 45kg サーボ 270°
        - M5 AtomS3R
      - トラックユニット仕様
        - OP.1948 トラックユニット コンバージョン
        - HOBBYWING QuicRUN 540 ブラシモーター 30T
        - HOBBYWING QuicRUN-WP-1080-G2-Brushed
        - FLASH HOBBY M35CHW 35KG サーボ 180°
        - M5 ATOM Matrix
    - 1/10RC メルセデス・ベンツ ウニモグ 406 (CC-02シャーシ)
      - OP.1895 タミヤ ブラシレスモーター 02 センサー付 21.5T
      - タミヤ ブラシレス エレクトロニック スピードコントローラー 04SR センサー付
      - M5 ATOM Matrix
    - バッテリー(G-FORCE BULLET LiPo 7.4V 3000mAh)
  - PS4 Controller
  - Xbox Wireless Controller

- ソフトウェア

  - Arduino IDE
  - Visual Studio Code
  - PlatformIO
  - SixaxisPairTool
  - ROS2 Humble
  - micro-ROS
  - micro-ROS for Arduino
  - Arduino Library
    - M5Atom
    - M5AtomS3
    - FastLED
    - M5Unified
    - M5GFX
    - PS4-esp32
    - ESP32Servo
    - Madgwick
    - NimBLE-Arduino
    - micro_ros_arduino
  - ROS2 Package
    - micro_ros_agent
    - joy_linux
    - teleop_twist_key
    - teleop_twist_joy

## 構成図

T.B.A

## ピン配置

- M5 ATOM Matrix

  | Pin Number    | Pin Name | Mapped Pin Name                               | Schematic Pin ID | Input/Output | Function | Signal ID  | Signal Name(J)              |
  | ------------- | -------- | --------------------------------------------- | ---------------- | ------------ | -------- | ---------- | --------------------------- |
  | 14            | IO25     | GPIO25, DAC_1, ADC2_CH8, RTC_GPIO6, EMAC_RXD0 | G25              | -            | -        | -          | 未使用                      |
  | 33            | SD1      | GPIO8, SD_DATA1, SPID, HS1_DATA1, U2CTS       | G33              | O            | DO       | THROT      | スロットル(サーボ)          |
  | 36            | IO23     | GPIO23, VSPID, HS1_STROBE                     | G23              | -            | -        | -          | 未使用                      |
  | 38            | IO19     | GPIO19, VSPIQ, U0CTS, EMAC_TXD0               | G19              | O            | DO       | STEER      | ステアリング(サーボ)        |
  | 39            | IO22     | GPIO22, VSPIWP, U0RTS, EMAC_TXD1              | G22              | -            | -        | -          | 未使用                      |
  | 42            | IO21     | GPIO21, VSPIHD, EMAC_TX_EN                    | G21              | -            | -        | -          | 未使用                      |
  | 15            | IO26     | GPIO26, DAC_2, ADC2_CH9, RTC_GPIO7, EMAC_RXD1 | G26              | IO           | UART     | UROSTX     | microROSシリアル通信(TX)    |
  | 12            | IO32     | 32K_XP, ADC1_CH4, TOUCH9, RTC_GPIO9           | G32              | IO           | UART     | UROSRX     | microROSシリアル通信(RX)    |

  [ATOM Matrix](https://docs.m5stack.com/en/core/Atom-Matrix_v1.1)  
  [ESP32-PICO Datasheet](https://m5stack.oss-cn-shenzhen.aliyuncs.com/resource/docs/datasheet/core/esp32-pico-d4_datasheet_en.pdf)  

- M5 ATOMS3R

  | Pin Number    | Pin Name | Mapped Pin Name                               | Schematic Pin ID | Input/Output | Function | Signal ID  | Signal Name(J)              |
  | ------------- | -------- | --------------------------------------------- | ---------------- | ------------ | -------- | ---------- | --------------------------- |
  | 43            | GPIO38   | GPIO38, FSPIWP, SUBSPIWP                      | G38              | -            | -        | -          | 未使用                      |
  | 13            | GPIO8    | RTC_GPIO8, GPIO8, TOUCH8, ADC1_CH7, SUBSPICS1 | G8               | O            | DO       | THROT      | スロットル(サーボ)          |
  | 12            | GPIO7    | RTC_GPIO7, GPIO7, TOUCH7, ADC1_CH6            | G7               | -            | -        | -          | 未使用                      |
  | 11            | GPIO6    | RTC_GPIO6, GPIO6, TOUCH6, ADC1_CH5            | G6               | O            | DO       | STEER      | ステアリング(サーボ)        |
  | 10            | GPIO5    | RTC_GPIO5, GPIO5, TOUCH5, ADC1_CH4            | G5               | -            | -        | -          | 未使用                      |
  | 44            | MTCK     | MTCK, GPIO39, CLK_OUT3, SUBSPICS1             | G39              | -            | -        | -          | 未使用                      |
  | 7             | GPIO2    | RTC_GPIO2, GPIO2, TOUCH2, ADC1_CH1            | G2               | IO           | UART     | UROSTX     | microROSシリアル通信(TX)    |
  | 6             | GPIO1    | RTC_GPIO1, GPIO1, TOUCH1, ADC1_CH0            | G1               | IO           | UART     | UROSRX     | microROSシリアル通信(RX)    |

  [M5 ATOMS3R](https://docs.m5stack.com/en/core/AtomS3R)  
  [ESP32-S3-PICO-1-N8R8 Datasheet](https://m5stack.oss-cn-shenzhen.aliyuncs.com/resource/docs/products/core/AtomS3R/esp32-s3-pico-1_datasheet_en.pdf)

## ビルド

1. パーティションをPlatformIOのConfiguration Fileで指定する

    M5 ATOM Matrixを使用する場合のみ指定( M5 ATOMS3Rを使用する場合は不要 )  

    .\RcCar\platformio.ini

    ```ini
    board_build.partitions = C:\User\usr\Arduino IDE\portable\packages\m5stack\hardware\esp32\2.0.4\tools\partitions\min_spiffs.csv
    ```

    ※上記 board_build.partitions で指定するパスを書き換える  

1. 参照するライブラリのパスをPlatformIOのConfiguration Fileで指定する

    .\RcCar\platformio.ini

    - M5 ATOM Matrix

      ```ini
      lib_extra_dirs = 
        C:\User\usr\Arduino IDE\portable\packages\m5stack
        C:\User\usr\Arduino IDE\portable\sketchbook\libraries\M5Atom
        C:\User\usr\Arduino IDE\portable\sketchbook\libraries\FastLED
        C:\User\usr\Arduino IDE\portable\sketchbook\libraries\PS4-esp32
        C:\User\usr\Arduino IDE\portable\sketchbook\libraries\ESP32Servo
      ```

    - M5 ATOMS3R

      ```ini
      lib_extra_dirs = 
        C:\User\usr\Arduino IDE\portable\packages\m5stack
        C:\User\usr\Arduino IDE\portable\sketchbook\libraries\M5AtomS3
        C:\User\usr\Arduino IDE\portable\sketchbook\libraries\FastLED
        C:\User\usr\Arduino IDE\portable\sketchbook\libraries\M5Unified
        C:\User\usr\Arduino IDE\portable\sketchbook\libraries\M5GFX
        C:\User\usr\Arduino IDE\portable\sketchbook\libraries\PS4-esp32
        C:\User\usr\Arduino IDE\portable\sketchbook\libraries\ESP32Servo
        C:\User\usr\Arduino IDE\portable\sketchbook\libraries\Madgwick
        C:\User\usr\Arduino IDE\portable\sketchbook\libraries\NimBLE-Arduino
      ```

    ※上記 lib_extra_dirs で指定するパスを書き換える  

1. PlatformIOでソフトをビルドする

1. PlatformIOでソフトをアップロードする

## 起動方法

- microROSのTwistメッセージ(cmd_vel)でRcCarを動かす場合

  1. RcCarの電源を入れる( M5Atomの電源はESCから供給 )

  1. micro-ROS Agentを起動する

      - micro-ROS接続(Serial)の場合

        ```bash
        ros2 run micro_ros_agent micro_ros_agent serial --dev /dev/ttyTHS0 --baud 115200
        ```

      - micro-ROS接続(WiFi)の場合

        ```bash
        ros2 run micro_ros_agent micro_ros_agent udp4 --port 8888
        ```

      ※ROS2ネットワーク内のPCにて起動する

  1. Twistメッセージ(cmd_vel)を配信するノードを起動する

      - teleop_twist_keyの場合

        1. Twistメッセージを配信するノードを起動する

            ```bash
            ros2 run teleop_twist_keyboard teleop_twist_keyboard 
            ```

      - teleop_twist_joyの場合


        1. PS4コントローラのPSボタンを押す

            ※事前にROS2ネットワーク内のPCにPS4コントローラをペアリングしていること

        1. Joyメッセージを配信するノードを起動する

            ```bash
            ros2 run joy_linux joy_linux_node --ros-args -p autorepeat_rate:=10.0 -p coalesce_interval:=0.1
            ```

            ※PS4コントローラをペアリングしたPCにて起動する  
            ※デフォルトの設定の場合配信周期が速すぎてM5Atomが処理しきれないため上記パラメータにて起動する  

        1. Twistメッセージを配信するノードを起動する

            ```bash
            ros2 launch teleop_twist_joy teleop-launch.py joy_config:='ps3'
            ```

            ※RcCarをスムーズに動かすにはteleop_twist_joyのコンフィグの調整が必要

- Bluetooth接続の場合

  - M5 ATOM Matrix (Bluetooth Classic)

    PS4コントローラをBluetoothで接続して動かす  

    1. SixaxisPairToolでPS4コントローラにMasterのMACアドレスを書き込む

    1. RcCarの電源を入れる(M5 ATOM Matrixの電源はESCから供給)

    1. PS4コントローラのPSボタンを押す

  - M5 ATOMS3R (Bluetooth Low Energy(BLE))

    XboxコントローラをBluetoothで接続して動かす  

    1. RcCarの電源を入れる(M5 ATOMS3Rの電源はESCから供給)

    1. XboxコントローラのXBOXボタンを押す

## 操作方法

- microROSのTwistメッセージ(cmd_vel)でRcCarを動かす場合

  teleop_twist_keyまたはteleop_twist_joyの操作方法を参照

- Bluetooth接続の場合

  - スロットル : L stick Vertical
  - ステアリング : R stick Horizon

## コンフィグ

- Bluetooth接続の場合

  1. BluetoothのMACアドレスを確認しメモする

  - M5 ATOM Matrix (Bluetooth Classic)

    M5 ATOM MatrixのBluetoothのMACアドレスを確認しメモする  

  - M5 ATOMS3R (Bluetooth Low Energy(BLE))

    XboxコントローラのBluetoothのMACアドレスを確認しメモする  

  1. メモしたMACアドレスを下記コンフィグファイルに記入する

      .\RcCar\src\JoyStick_Config.h

      ```c++
      #define BLUETOOTH_MAC_ADDRESS			"1A:2B:3C:01:01:01"
      ```

      ※上記 "1A:2B:3C:01:01:01" を書き換える

- micro-ROS接続(Serial)の場合

  1. micro-ROSの接続をSerialに設定する

      .\RcCar\src\Config.h

      ```c++
      #define ROS_AGENT_COMMODE				ROS_AGENT_COMMODE_SERIAL
      ```

      ※デフォルトの設定はシリアル接続(ROS_AGENT_COMMODE_SERIAL)

  1. Serialのポートを設定する

      .\RcCar\src\CustomTransport.h

      ```c++
      #define ROS_TRANSPORT_SERIAL_SEL			ROS_TRANSPORT_SERIAL1
      ```

      - M5 ATOM Matrix

        ROS_TRANSPORT_SERIAL:USB  
        ROS_TRANSPORT_SERIAL1:RX(G32)TX(G26)  
        ROS_TRANSPORT_SERIAL2:RX(G23)TX(G33)  

      - M5 ATOMS3R

        ROS_TRANSPORT_SERIAL:USB  
        ROS_TRANSPORT_SERIAL1:RX(G1)TX(G2)  
        ROS_TRANSPORT_SERIAL2:RX(G7)TX(G8)  

      ※デフォルトの設定はROS_TRANSPORT_SERIAL1

- micro-ROS接続(WiFi)の場合

  1. micro-ROSの接続をWiFiに設定する

      .\RcCar\src\Config.h

      ```c++
      #define ROS_AGENT_COMMODE			ROS_AGENT_COMMODE_UDP
      ```

      ※デフォルトの設定はシリアル接続(ROS_AGENT_COMMODE_SERIAL)

  1. WiFiの設定とmicro-ROS Agentの設定を下記コンフィグファイルに記入する

      1行目から下記順番で記入する
      - SSID
      - PASS
      - micro-ROS Agent IP
      - micro-ROS Agent Port

      .\RcCar\data\config.txt

      ```txt
      SSID
      PASS
      192.168.1.100
      8888
      ```

      ※WiFiの設定は、上記 SSID と PASS を書き換える  
      ※micro-ROS Agentの設定は、上記 192.168.1.100 と 8888 を書き換える  
      ※SSIDやPASSは暗号化をしていないため自己責任でご使用ください  

  1. PlatformIOでコンフィグファイルを書き込む

      PlatformIOの下記メニューを選択してファイルを書き込む

      ```txt
      PROJECT TASKS -> m5stack-c -> Platform -> Upload Filesystem Image
      ```

      ※M5Atomの不揮発メモリに格納されるため、本コンフィグが不要となった場合はすみやかに削除ください  

## 参考サイト

- [PS4-esp32](https://github.com/aed3/PS4-esp32)
- [M5StackにPS4コントローラをBluetooth接続](https://qiita.com/Geek493/items/8402ad875b88822e75ab)
- [ESP32とXboxのコントローラーをBLEで接続する方法](https://asukiaaa.blogspot.com/2021/01/communicate-esp32-and-xbox-controller-on-ble.html)
- [micro-ROS](https://micro.ros.org/)
- [micro-ROS for Arduino](https://github.com/micro-ROS/micro_ros_arduino)
