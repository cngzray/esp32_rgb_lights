
# ESP32 + WS2812B（NeoPixel）网页独立控制RGB灯带项目
单灯珠独立开关、自定义调色，手机/电脑同WiFi局域网实时控制。

## 📌 项目简介
本项目基于ESP32 Arduino框架，内置Web服务，无需额外APP。浏览器访问ESP32局域网IP即可打开可视化控制面板：
- 每一颗WS2812灯珠独立开关控制
- 点击圆形预览框弹出取色器，单灯自定义RGB颜色
- 前端纯原生HTML+JS，无第三方JS依赖，体积轻量
- 串口实时打印每颗灯珠开关、颜色变更日志
- 全局状态数组保存所有灯珠开关与颜色配置

## ✨ 核心功能
1. **单灯独立控制**：每个灯珠配备独立复选框，单独点亮/熄灭互不干扰
2. **网页取色器**：内置原生color类型取色面板，实时预览颜色，确认后同步硬件
3. **实时HTTP通信**：前端AJAX异步请求，修改后灯带立即刷新，无页面重载
4. **状态持久缓存**：程序运行期间保存每颗灯珠开关状态与色值
5. **串口调试输出**：灯珠操作日志打印至串口监视器，方便排错
6. **可灵活配置**：灯带引脚、灯珠数量一键宏定义修改

## 🧰 硬件清单
| 器材 | 规格说明 |
|------|--------|
| ESP32开发板 | ESP32-DevKitC / ESP32-S3 均可 |
| WS2812B灯带 | GRB时序，5V NeoPixel灯带 |
| 杜邦线 | 3根（GND、DATA、5V） |
| 电源建议 | 灯珠少于10颗可用ESP32 USB供电；大于10颗建议外部5V独立电源，共地 |

### 硬件接线
1. WS2812 DATA信号线 → ESP32 GPIO13（代码`LED_PIN`可修改）
2. WS2812 GND → ESP32 GND（必须共地，否则灯带异常）
3. WS2812 5V → ESP32 5V / 外部5V电源

## 📦 依赖库安装
打开Arduino IDE库管理器，搜索并安装以下库：
1. **Adafruit NeoPixel**（Adafruit官方NeoPixel驱动）
2. ESP32核心自带库，无需额外安装：`WiFi`、`WebServer`

### ESP32 Arduino核心配置
1. 文件 → 首选项 → 附加开发板管理器网址填入：
```
https://dl.espressif.com/dl/package_esp32_index.json
```
2. 工具 → 开发板 → 选择对应ESP32开发板型号

## ⚙️ 代码配置修改
打开`src/main.cpp`，修改以下参数适配你的硬件与WiFi：
```cpp
// WiFi信息
const char* ssid = "你的WiFi名称";
const char* password = "你的WiFi密码";

// 灯带硬件配置
#define LED_PIN 13      // WS2812数据引脚
#define NUM_LEDS 10     // 你的灯带灯珠总数
```

<img width="474" height="570" alt="image" src="https://github.com/user-attachments/assets/a124c17e-d72e-4764-b691-eb29068d715a" />

### 网页操作说明
1. **复选框**：勾选点亮对应灯珠，取消勾选熄灭
2. **圆形彩色预览框**：点击弹出颜色选择器，自定义该灯珠颜色
3. 取色器点击「确认」保存颜色并同步灯带；「取消」放弃修改

## 📡 接口说明（二次开发可用）
内置HTTP接口，可直接通过GET请求控制单灯，第三方设备/脚本可调用：
```
http://ESP32_IP/led?index=灯珠序号&enabled=1/0&color=#RRGGBB
```
参数说明：
- `index`：灯珠编号，从0开始（0=灯珠1，1=灯珠2…）
- `enabled`：1=点亮，0=熄灭
- `color`：十六进制RGB颜色，格式`#FFFFFF`

示例：点亮第1颗灯珠为红色
```
http://192.168.1.105/led?index=0&enabled=1&color=#FF0000
```

