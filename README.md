# m5stack-rccar-toy

## はじめに

本ソフトはM5 ATOM MatrixでRcCarを動かすソフトである

- micro-ROS接続(Serial/WiFi)の場合

  microROSのTwistメッセージ(cmd_vel)でRcCarを動かす  
  microROSのJoyメッセージ(joy)でRcCarを動かす(コンフィグ変更)  
  M5AtomのIMUのデータをmicroROSのImuメッセージ(rccar_imu)として配信する  

- Bluetooth接続の場合

  PS4コントローラをBluetoothで接続して動かす

## 動作環境

- M5 ATOM Matrix
- 1/10RC メルセデス・ベンツ ウニモグ 406 (CC-02シャーシ)
- OP.1895 タミヤ ブラシレスモーター 02 センサー付 21.5T
- タミヤ ブラシレス エレクトロニック スピードコントローラー 04SR センサー付
- PS4 Controller
- Arduino IDE
- Visual Studio Code
- PlatformIO
- SixaxisPairTool
- ROS2 Foxy
- micro-ROS
- micro-ROS for Arduino

- Arduino Library
  - M5Atom
  - FastLED
  - PS4-esp32
  - ESP32Servo
  - micro_ros_arduino

- ROS2 Package
  - micro_ros_agent
  - joy_linux
  - teleop_twist_key
  - teleop_twist_joy

## 構成図

T.B.A

## ピン配置

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

[ATOM Matrix](https://docs.m5stack.com/en/core/atom_matrix)  
[ESP32-PICO Datasheet](https://m5stack.oss-cn-shenzhen.aliyuncs.com/resource/docs/datasheet/core/esp32-pico-d4_datasheet_en.pdf)  
[Peripherals Pin Map](https://github.com/m5stack/M5Atom#peripherals-pin-map)  

## ビルド

1. パーティションをPlatformIOのConfiguration Fileで指定する

    .\RcCar\platformio.ini

    ```ini
    board_build.partitions = C:\User\usr\Arduino IDE\portable\packages\m5stack\hardware\esp32\2.0.4\tools\partitions\min_spiffs.csv
    ```

    ※上記 board_build.partitions で指定するパスを書き換える  

1. 参照するライブラリのパスをPlatformIOのConfiguration Fileで指定する

    .\RcCar\platformio.ini

    ```ini
    lib_extra_dirs = 
      C:\User\usr\Arduino IDE\portable\packages\m5stack
      C:\User\usr\Arduino IDE\portable\sketchbook\libraries\M5Atom
      C:\User\usr\Arduino IDE\portable\sketchbook\libraries\FastLED
      C:\User\usr\Arduino IDE\portable\sketchbook\libraries\PS4-esp32
      C:\User\usr\Arduino IDE\portable\sketchbook\libraries\ESP32Servo
    ```

    ※上記 lib_extra_dirs で指定するパスを書き換える  

1. PlatformIOでソフトをビルドする

1. PlatformIOでソフトをアップロードする

## 起動方法

- microROSのTwistメッセージ(cmd_vel)でRcCarを動かす場合

  1. RcCarの電源を入れる(M5 ATOM Matrixの電源はESCから供給)

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
            ※デフォルトの設定の場合配信周期が速すぎてM5 ATOM Matrixが処理しきれないため上記パラメータにて起動する  

        1. Twistメッセージを配信するノードを起動する

            ```bash
            ros2 launch teleop_twist_joy teleop-launch.py joy_config:='ps3'
            ```

            ※RcCarをスムーズに動かすにはteleop_twist_joyのコンフィグの調整が必要

- Bluetooth接続の場合

  1. SixaxisPairToolでPS4コントローラにMasterのMACアドレスを書き込む

  1. RcCarの電源を入れる(M5 ATOM Matrixの電源はESCから供給)

  1. PS4コントローラのPSボタンを押す

## 操作方法

- microROSのTwistメッセージ(cmd_vel)でRcCarを動かす場合

  teleop_twist_keyまたはteleop_twist_joyの操作方法を参照

- Bluetooth接続の場合

  - スロットル : L stick Vertical
  - ステアリング : R stick Horizon

## コンフィグ

- Bluetooth接続の場合

  1. M5 ATOM MatrixのBluetoothのMACアドレスを確認しメモする

  1. メモしたMACアドレスを下記コンフィグファイルに記入する

      .\RcCar\src\JoyStick_Config.h

      ```c++
      #define BLUETOOTH_MAC_ADDRESS			"1A:2B:3C:01:01:01"
      ```

      ※上記 "1A:2B:3C:01:01:01" を書き換える

- micro-ROS接続(Serial)の場合

  1. micro-ROSの接続をSerialに設定する

      .\RcCar\src\RcCar.h

      ```c++
      #define ROS_AGENT_COMMODE				ROS_AGENT_COMMODE_SERIAL
      ```

      ※デフォルトの設定はシリアル接続(ROS_AGENT_COMMODE_SERIAL)

  1. Serialのポートを設定する

      .\RcCar\src\CustomTransport.h

      ```c++
      #define ROS_TRANSPORT_SERIAL_SEL			ROS_TRANSPORT_SERIAL1
      ```

      ※ROS_TRANSPORT_SERIAL:USB
      ※ROS_TRANSPORT_SERIAL1:RX(G32)TX(G26)
      ※ROS_TRANSPORT_SERIAL2:RX(G23)TX(G33)
      ※デフォルトの設定はROS_TRANSPORT_SERIAL1

- micro-ROS接続(WiFi)の場合

  1. micro-ROSの接続をWiFiに設定する

      .\RcCar\src\RcCar.h

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

      ※M5 ATOM Matrixの不揮発メモリに格納されるため、本コンフィグが不要となった場合はすみやかに削除ください  

## 参考サイト

- [PS4-esp32](https://github.com/aed3/PS4-esp32)
- [M5StackにPS4コントローラをBluetooth接続](https://qiita.com/Geek493/items/8402ad875b88822e75ab)
- [micro-ROS](https://micro.ros.org/)
- [micro-ROS for Arduino](https://github.com/micro-ROS/micro_ros_arduino)
