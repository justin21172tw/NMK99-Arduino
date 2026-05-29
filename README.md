# NMK99-Arduno

## 專案目的

本專案目標是使用 DHT 濕溫度感測器偵測環境濕度與溫度，並依濕度狀態提供即時警示：

- 濕度高於設定上限時，亮起對應 LED 顏色警示。
- 濕度低於設定下限時，亮起另一種 LED 顏色警示。
- 濕度在正常範圍內時，顯示正常狀態燈號。

此外，專案規劃會固定將測得的濕度與溫度資料上傳至網站進行紀錄，作為後續環境監控與查詢依據。

## 硬體清單 (Hardware List)

| 元件名稱 | 規格描述 | 接腳 (GPIO) |
| :--- | :--- | :--- |
| **主控板** | ESP32 Dev Module | N/A |
| **溫濕度感測器** | DHT11 | 33 |
| **顯示器** | 0.96" OLED (SSD1306, 128x64) | SDA:21, SCL:22 |
| **狀態指示燈** | 內建 WS2812B RGB LED | 32 |

## 軟體與開發環境 (Software & Environment)

### 開發工具
- **IDE**: Visual Studio Code + PlatformIO (Core 6.1.19)
- **Framework**: Arduino ESP32 (v3.2.0)
- **Platform**: Espressif 32 (v7.0.1)

### 依賴函式庫 (Dependencies)
- **DHT sensor library** (v1.4.7)
- **Adafruit Unified Sensor** (v1.1.15)
- **Adafruit SSD1306** (v2.5.16)
- **Adafruit GFX Library** (v1.12.6)
- **Adafruit NeoPixel** (v1.15.5)

## 專案結構 (Project Structure)

本倉庫採用分層架構，將硬體韌體與網頁系統分離：

- **`firmware/`**: 包含 ESP32 的 PlatformIO 專案。
  - `src/`: 核心程式碼。
  - `include/`: 全域設定檔。
  - `lib/`: 自定義功能模組。
- **`web/`**: 存放網頁系統相關檔案（即將開發）。
- **根目錄**: 存放共用文檔與 Git 配置。

## 目前專案架構 (Firmware)

### 1. 核心流程

- `firmware/src/main.cpp`
  - 系統啟動初始化（序列埠、LED、I2C、DHT、OLED 掃描）。
  - 依固定週期讀取濕溫度資料。
  - 判斷濕度狀態（LOW / NORMAL / HIGH / SENSOR_ERROR）。
  - 控制 LED 與 OLED 顯示，並輸出序列埠日誌。

### 2. 設定與型別

- `firmware/include/config.h`：集中管理腳位、顯示器參數、讀取週期與濕度門檻值。
- `firmware/include/app_types.h`：定義濕度狀態列舉型別 `HumidityStatus`。
- `firmware/include/logger.h`：提供簡單日誌介面（INFO / WARNING / ERROR）。

### 3. 功能模組（`firmware/lib/`）

- `firmware/lib/sensor/dht_manager.*`：封裝 DHT 感測器初始化與讀值邏輯。
- `firmware/lib/indicator/led_manager.*`：管理 WS2812B 指示燈顏色切換。
- `firmware/lib/display/display_manager.*`：管理 OLED 初始化、大字體佈局與訊息顯示。
- `firmware/lib/hardware/hardware_probe.*`：I2C 裝置掃描與 DHT 啟動探測。

### 4. 建置設定

- `firmware/platformio.ini`：專案建置配置與函式庫依賴管理。

## 目前尚未實作的部分

以下功能已在需求中規劃，但目前程式碼尚未完成：

1. **資料上傳網站**
   - 尚未實作 Wi-Fi 連線流程與網路通訊（HTTP/MQTT 等）。
   - 尚未將濕度/溫度定期送出到遠端網站或伺服器。

2. **雲端資料紀錄整合**
   - 尚未定義後端 API 格式、驗證方式與錯誤重送機制。
   - 尚未建立本地暫存/補傳機制（網路中斷時避免資料遺失）。

3. **遠端監控擴充**
   - 目前僅有本地 LED/OLED/Serial 顯示，尚無網站端圖表與歷史查詢串接。
