
/**
 * 咩咩房间的灯sg90舵机控制器
 * Sparkle
 * 20221130
 * v1.0
 */

#define BLINKER_WIFI
#define BLINKER_MIOT_OUTLET
#define ZYYMEHOMEKIT

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
const int onDeg = 145;
const int offDeg = 140;
const int stayDeg = 112;

bool oState = false;
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
    // 启用输出舵机工作
    pinMode(sgIo, OUTPUT);
    if (isOn) {
        BUILTIN_SWITCH.print("on");
//        onoffButton.print("on");
        BlinkerMIOT.powerState("on");
        BlinkerMIOT.print();
        #ifdef ZYYMEHOMEKIT
        cha_switch_on.value.bool_value = true;
        #endif
        sg.write(onDeg);
    } else {
        BUILTIN_SWITCH.print("off");
//        onoffButton.print("off");
        BlinkerMIOT.powerState("off");
        BlinkerMIOT.print();
        #ifdef ZYYMEHOMEKIT
        cha_switch_on.value.bool_value = false;
        #endif
        sg.write(offDeg);
    }
    delay(250);
    sg.write(stayDeg);
    // 关闭输出给舵机延寿 问就是已经烧了一个舵机的电机
    delay(200);
    pinMode(sgIo, INPUT);
    #ifdef ZYYMEHOMEKIT
    homekit_characteristic_notify(&cha_switch_on, cha_switch_on.value);
    #endif
    oState = isOn;
}

void miotPowerState(const String & state) {
    BLINKER_LOG("need set power state: ", state);

    if (state == BLINKER_CMD_ON) {
//        digitalWrite(LED_BUILTIN, HIGH);
        setOnOff(true);
    }
    else if (state == BLINKER_CMD_OFF) {
//        digitalWrite(LED_BUILTIN, LOW);
        setOnOff(false);
    }
}

void setOnOffBtn(const String & state) {
    BLINKER_LOG("get button state: ", state);
    setOnOff(state == "on");
//    Blinker.vibrate();
}

void setOnOffBtnLed(const String & state) {
    BLINKER_LOG("get button state: ", state);
    digitalWrite(LED_BUILTIN, state != "on");
    onoffButton.print(state);
//    Blinker.vibrate();
}
  
void setDeg(int32_t value) {
    pinMode(sgIo, OUTPUT);
    sg.write(value);
    Blinker.vibrate();
}

String summary() {
    String data = "online, switch: " + STRING_format(oState ? "on" : "off");
    return data;
}

#ifdef ZYYMEHOMEKIT
void cha_switch_on_setter(const homekit_value_t value) {
    setOnOff(value.bool_value); 
}
#endif

void setup() {
    Serial.begin(115200);
    BLINKER_DEBUG.stream(Serial);

    // sg90的实际工作范围 这样初始化才能到180度
    sg.attach(sgIo, 500, 2400);
    sg.write(stayDeg);
    delay(200);
    pinMode(sgIo, INPUT);

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);

    BUILTIN_SWITCH.attach(setOnOffBtn);
    onoffButton.attach(setOnOffBtnLed);
    degSlider.attach(setDeg);

    Blinker.begin(auth, ssid, pswd);
    
    wifi_set_sleep_type(LIGHT_SLEEP_T);
    
    Blinker.attachData(dataRead);
    Blinker.attachSummary(summary);
    
    BlinkerMIOT.attachPowerState(miotPowerState);
    BlinkerMIOT.attachQuery(miotQuery);

    #ifdef ZYYMEHOMEKIT
    cha_switch_on.setter = cha_switch_on_setter;
    arduino_homekit_setup(&config);
    #endif

    httpUpdater.setup(&httpServer);
    httpServer.begin();

}

void loop() {
    Blinker.run();
    httpServer.handleClient();
    #ifdef ZYYMEHOMEKIT
    arduino_homekit_loop();
    #endif
}
