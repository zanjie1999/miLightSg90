
/**
 * 咩咩房间的灯sg90舵机控制器
 * Sparkle
 * 20221130
 * v2.0
 */

#define BLINKER_WIFI
#define BLINKER_MIOT_OUTLET
//#define ZYYMEHOMEKIT

#include <Blinker.h>
#include <Servo.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#ifdef ZYYMEHOMEKIT
#include <arduino_homekit_server.h>
#endif

char auth[] = "yourBlinkerDeviceKey";
char ssid[] = "wifiSsid";
char pswd[] = "wifiPwd";
const int sgIo = 4;
// 触发开角度
const int onDeg = 180;
// 触发关角度
const int offDeg = 135;
// 等待开角度
const int stay2onDeg = 75;
// 等待关角度
const int stay2offDeg = 100;
// 转动等待时间
const int degT = 190;
// 待机超时ms 控制完超时会关掉输出
const int sgSlpT = 10000;
unsigned long sgt = 0;

bool oState = true;
Servo sg;
ESP8266WebServer httpServer(80);
ESP8266HTTPUpdateServer httpUpdater;


BlinkerButton onoffButton("btn-onoff");
BlinkerSlider degSlider("ran-deg");

// 因为用C语法写的配置所以
#ifdef ZYYMEHOMEKIT
extern "C" homekit_server_config_t config;
extern "C" homekit_characteristic_t cha_switch_on;
#endif

void miotQuery(int32_t queryCode) {
    BLINKER_LOG("MIOT Query codes: ", queryCode);

    switch (queryCode)
    {
        case BLINKER_CMD_QUERY_ALL_NUMBER :
            BLINKER_LOG("MIOT Query All");
            BlinkerMIOT.powerState(oState ? "on" : "off");
            BlinkerMIOT.print();
            break;
        case BLINKER_CMD_QUERY_POWERSTATE_NUMBER :
            BLINKER_LOG("MIOT Query Power State");
            BlinkerMIOT.powerState(oState ? "on" : "off");
            BlinkerMIOT.print();
            break;
        default :
            BlinkerMIOT.powerState(oState ? "on" : "off");
            BlinkerMIOT.print();
            break;
    }
}

void dataRead(const String & data) {
    BLINKER_LOG("Blinker readString: ", data);

//    Blinker.vibrate();
    
    uint32_t BlinkerTime = millis();
    
    Blinker.print("millis", BlinkerTime);
}

void setOnOff(bool isOn) {
    oState = isOn;
    // 启用输出舵机工作
    if (sgt == 0) {
      pinMode(sgIo, OUTPUT);
      sgt = sgSlpT + millis();
      delay(100);
    }
    if (isOn) {
        sg.write(stay2onDeg);
        BUILTIN_SWITCH.print("on");
//        onoffButton.print("on");
        BlinkerMIOT.powerState("on");
        BlinkerMIOT.print();
        #ifdef ZYYMEHOMEKIT
        cha_switch_on.value.bool_value = true;
        #endif
        sg.write(onDeg);
        delay(degT);
        sg.write(stay2offDeg);
    } else {
        sg.write(stay2offDeg);
        BUILTIN_SWITCH.print("off");
//        onoffButton.print("off");
        BlinkerMIOT.powerState("off");
        BlinkerMIOT.print();
        #ifdef ZYYMEHOMEKIT
        cha_switch_on.value.bool_value = false;
        #endif
        sg.write(offDeg);
        delay(degT);
        sg.write(stay2onDeg);
    }
    #ifdef ZYYMEHOMEKIT
    homekit_characteristic_notify(&cha_switch_on, cha_switch_on.value);
    #endif
}
