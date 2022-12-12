# 咩咩房间的灯sg90舵机控制器
造价不到10元，接入HomeKit和小爱
## 如何使用
1. 先下点灯（blinker）的app，创建一个独立设备，进入设备右上角三个点，拷贝秘钥填写到程序中，并在程序中填写你的wifi名字和密码  
2. 界面配置，把这些贴进去保存，界面就有了
```
{¨version¨¨2.0.0¨¨config¨{¨headerColor¨¨transparent¨¨headerStyle¨¨dark¨¨background¨{¨img¨¨assets/img/headerbg.jpg¨¨isFull¨«}}¨dashboard¨|{¨type¨¨btn¨¨ico¨¨fad fa-lightbulb-on¨¨mode¨Ê¨t0¨¨开灯¨¨t1¨¨文本2¨¨bg¨Ì¨cols¨Í¨rows¨Í¨key¨¨switch¨´x´Ë´y´Ò¨lstyle¨Ë¨speech¨|÷}{ßC¨ran¨ßH¨角度¨¨clr¨¨#389BEE¨¨max¨¢2u¨min¨ÉßLÉßMÑßNËßO¨ran-deg¨´x´É´y´¤EßR|÷}{ßC¨deb¨ßGÉßLÉßMÑßNÌßO¨debug¨´x´É´y´Ë}{ßCßDßE¨fal fa-power-off¨ßGÊßH¨小蓝灯¨ßJßKßLÉßMËßNËßO¨btn-onoff¨´x´Ì´y´ÏßQÊ}÷¨actions¨|¦¨cmd¨¦ßP‡¨text¨‡¨on¨¨打开开灯¨¨off¨¨关闭开灯¨—{ßf{ßdßh}ßg¨打开小蓝灯¨}{ßf{ßdßj}ßg¨关闭小蓝灯¨}÷¨triggers¨|÷¨rt¨|÷}
```
3. 动作配置点 自动生成 并保存
4. 安装好arduino的esp8266支持，不懂百度或google查
5. 安装这两个依赖库 [Arduino-HomeKit-ESP8266](https://github.com/Mixiaoxiao/Arduino-HomeKit-ESP8266/archive/refs/heads/master.zip) [blinker](https://github.com/blinker-iot/blinker-library/archive/refs/heads/master.zip) 在arduino点加载zip库选择下载的zip就行了
6. 编译烧录
