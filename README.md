# NMK99-Arduno

## 專案目的

本專案目標是使用 DHT 濕溫度感測器偵測環境濕度與溫度，並依濕度狀態提供即時警示：

- 濕度高於設定上限時，亮起對應 LED 顏色警示。
- 濕度低於設定下限時，亮起另一種 LED 顏色警示。
- 濕度在正常範圍內時，顯示正常狀態燈號。

此外，專案規劃會固定將測得的濕度與溫度資料上傳至網站進行紀錄，作為後續環境監控與查詢依據。

## 目前專案架構

### 1. 核心流程

- `src/main.cpp`
  - 系統啟動初始化（序列埠、LED、I2C、DHT、OLED 掃描）。
  - 依固定週期讀取濕溫度資料。
  - 判斷濕度狀態（LOW / NORMAL / HIGH / SENSOR_ERROR）。
  - 控制 LED 與 OLED 顯示，並輸出序列埠日誌。

### 2. 設定與型別

- `include/config.h`：集中管理腳位、顯示器參數、讀取週期與濕度門檻值。
- `include/app_types.h`：定義濕度狀態列舉型別 `HumidityStatus`。
- `include/logger.h`：提供簡單日誌介面（INFO / WARNING / ERROR）。

### 3. 功能模組（`lib/`）

- `lib/sensor/dht_manager.*`：封裝 DHT 感測器初始化與讀值邏輯。
- `lib/indicator/led_manager.*`：依狀態切換 RGB LED 顏色。
- `lib/display/display_manager.*`：管理 OLED 初始化、數值與錯誤訊息顯示。
- `lib/hardware/hardware_probe.*`：I2C 裝置掃描與 DHT 啟動探測。

### 4. 建置設定

- `platformio.ini`
  - 平台：`espressif32`
  - 板子：`esp32dev`
  - 框架：`arduino`
  - 主要相依套件：DHT、SSD1306、GFX 等 Adafruit 函式庫。

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