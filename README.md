# 🌤️ Smart Clock - STM32 智能天气时钟

本项目是一个基于 **STM32F103C8T6** 的嵌入式智能天气时钟系统，集成了 **RTC 实时时钟显示、WiFi 联网天气获取、温湿度传感器检测、加速度感应切换界面** 等功能，展示效果清晰美观，可作为嵌入式综合实践或毕业设计项目。

---

## 📦 项目简介

Smart Clock 通过 STM32 单片机驱动 **ST7735 彩色 LCD 屏幕**，实时显示时间、日期、天气和温湿度信息。  
系统通过 **ESP32-C3 模块** 连接互联网，从心知天气（Seniverse API）获取最新天气数据，并在屏幕上动态展示。

---

## ⚙️ 硬件组成

| 模块名称 | 功能描述 |
|-----------|-----------|
| STM32F103C8T6 | 主控芯片，负责逻辑与显示控制 |
| ST7735 1.77" LCD | 显示时间、日期、天气等信息 |
| ESP32-C3 (AT模式) | 通过 WiFi 获取网络天气数据 |
| DS1302 / RTC | 提供实时时钟 |
| NTC 热敏电阻 | 温度采集 |
| MPU6050 | 加速度传感器，用于姿态检测或界面切换 |
| 杜邦线 / 电源模块 | 硬件连接与供电 |

---

## 🧠 软件结构

项目采用分层设计，驱动层与应用层分离，代码结构清晰、可维护性强。


stm32f103c8-project/
├── app/ # 应用层逻辑
│ ├── main.c
│ ├── weather.c
│ ├── rtc.c
│ ├── mpu6050.c
│ └── led.c
├── drivers/ # 底层驱动层
│ ├── st7735.c
│ ├── lcd_spi.c
│ ├── esp_at.c
│ └── delay.c
├── fonts/ # 字库与字体绘制
├── images/ # 图片资源
├── inc/ # 头文件
├── utils/ # 工具函数
└── main.h


---

## 🛰️ 功能特性

✅ 实时时钟显示（年月日时分秒）  
✅ 通过 WiFi 获取实时天气（温度、天气状态）  
✅ 自动切换 °C 单位显示  
✅ 加速度检测触发界面变化  
✅ 液晶屏动态刷新、自适应显示布局  
✅ 断电保存时间数据  
✅ 模块化驱动、结构清晰

---

## 🌐 天气 API 配置

在 `weather.c` 中配置天气 API：
```c
static const char *weather_uri = "https://api.seniverse.com/v3/weather/now.json?key=YOUR_KEY&location=shanghai&language=zh-Hans&unit=c";


🧩 编译与下载

开发环境：

Keil µVision 5 或 STM32CubeIDE

GCC ARM 工具链

连接 ST-Link / J-Link 烧录器

编译步骤：

打开 project.uvprojx 或 Makefile 工程；

选择目标芯片 STM32F103C8；

编译生成 .hex 文件；

使用 ST-Link Utility 或 CubeProgrammer 下载至开发板。

完整展示
![91641ca5e62de702f843554c74120ecd](https://github.com/user-attachments/assets/1355c7b9-64da-44f6-95b4-403c7d483a5b)

