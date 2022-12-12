
/**
 * 咩咩房间的灯sg90舵机控制器
 * Sparkle
 * 20221130
 * v1.0
 */

#define BLINKER_WIFI
#define BLINKER_MIOT_OUTLET

#include <Blinker.h>
#include <Servo.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <arduino_homekit_server.h>


char auth[] = "yourBlinkerDeviceKey";
char ssid[] = "wifiSsid";
char pswd[] = "wifiPwd";
const int sgIo = 4;
const int onDeg = 115;
const int offDeg = 100;
const int stayDeg = 45;

bool oState = false;
Servo sg;
ESP8266WebServer httpServer(80);
ESP8266HTTPUpdateServer httpUpdater;


BlinkerButton onoffButton("btn-onoff");
BlinkerSlider degSlider("ran-deg");

// 因为用C语法写的配置所以
extern "C" homekit_server_config_t config;
extern "C" homekit_characteristic_t cha_switch_on;

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
    if (isOn) {
        BUILTIN_SWITCH.print("on");
//        onoffButton.print("on");
        BlinkerMIOT.powerState("on");
        BlinkerMIOT.print();
        cha_switch_on.value.bool_value = true;
        sg.write(onDeg);
    } else {
        BUILTIN_SWITCH.print("off");
//        onoffButton.print("off");
        BlinkerMIOT.powerState("off");
        BlinkerMIOT.print();
        cha_switch_on.value.bool_value = false;
        sg.write(offDeg);
    }
    delay(100);
    sg.write(stayDeg);
    homekit_characteristic_notify(&cha_switch_on, cha_switch_on.value);
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
    sg.write(value);
    Blinker.vibrate();
}

String summary() {
    String data = "online, switch: " + STRING_format(oState ? "on" : "off");
    return data;
}

void cha_switch_on_setter(const homekit_value_t value) {
    setOnOff(value.bool_value); 
}

void setup() {
    Serial.begin(115200);
    BLINKER_DEBUG.stream(Serial);

    sg.attach(sgIo);
    sg.write(stayDeg);

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

    cha_switch_on.setter = cha_switch_on_setter;
    arduino_homekit_setup(&config);

    httpUpdater.setup(&httpServer);
    httpServer.begin();

}

void loop() {
    Blinker.run();
    arduino_homekit_loop();
    httpServer.handleClient();
}
