# NMK99-Arduno

ESP32 + DHT11 溫濕度監控專案，已串接 Firebase Realtime Database 與 Quasar 網頁儀表板。

## 目前已完成功能

### Firmware（`firmware/`）
- 啟動時初始化 Serial、I2C、DHT11、OLED、WS2812B。
- 週期性讀取溫濕度資料（預設每 3 秒）。
- 依濕度門檻切換 LED 狀態（LOW / NORMAL / HIGH）。
- OLED 顯示即時讀值與連線狀態。
- 連接 Wi-Fi 後登入 Firebase（匿名登入）。
- 定期上傳資料到 Firebase Realtime Database：
  - `devices/NMK99-Node-01/current`（約每 5 秒）
  - `devices/NMK99-Node-01/history`（約每 10 分鐘）

### Web（`web/`）
- 已建置 Quasar + Vue 3 + Pinia + ApexCharts 前端。
- Dashboard（`/`）可顯示：
  - 即時溫濕度卡片
  - 即時折線圖
  - 最新資料傳輸日誌
- History（`/history`）可顯示：
  - 溫濕度統計摘要（最小/平均/最大）
  - 歷史趨勢圖
  - 歷史資料表格
  - CSV 匯出
- 透過 `web/src/stores/sensor-store.ts` 監聽 Firebase current/history 節點並同步到畫面。

## 硬體清單

| 元件 | 規格 | GPIO |
| :--- | :--- | :--- |
| 主控板 | ESP32 Dev Module | N/A |
| 感測器 | DHT11 | 33 |
| 顯示器 | 0.96" OLED (SSD1306, 128x64) | SDA: 21, SCL: 22 |
| 指示燈 | WS2812B RGB LED | 32 |

## 資料流

1. ESP32 讀取 DHT11 溫濕度。
2. 本地端顯示（OLED / LED / Serial）。
3. ESP32 寫入 Firebase Realtime Database。
4. Web 前端監聽 Firebase，即時更新 Dashboard 與 History。

## 專案結構

- `firmware/`：PlatformIO Arduino 韌體
  - `src/main.cpp`：主流程（讀值、顯示、上傳）
  - `include/config.h`：腳位、週期、門檻與 Firebase/網路設定
  - `lib/`：感測器、顯示器、LED、硬體探測模組
- `web/`：Quasar 前端
  - `src/pages/IndexPage.vue`：即時看板
  - `src/pages/HistoryPage.vue`：歷史分析與報表
  - `src/stores/sensor-store.ts`：Firebase 資料同步與統計

## 環境設定

根目錄建立 `.env`（供 firmware 與 web 共用）：

```env
WIFI_SSID=your_wifi_ssid
WIFI_PASSWORD=your_wifi_password
VITE_FIREBASE_API_KEY=your_firebase_api_key
VITE_FIREBASE_AUTH_DOMAIN=your_project.firebaseapp.com
VITE_FIREBASE_PROJECT_ID=your_project_id
VITE_FIREBASE_STORAGE_BUCKET=your_project.appspot.com
VITE_FIREBASE_MESSAGING_SENDER_ID=your_sender_id
VITE_FIREBASE_APP_ID=your_app_id
VITE_FIREBASE_MEASUREMENT_ID=your_measurement_id
VITE_FIREBASE_DATABASE_URL=your_database_url
```

## 開發與建置

### Firmware
```bash
cd firmware
pio run
```

### Web
```bash
cd web
npm install
npm run lint
npm run test
npm run build
```

## 待補強項目（現況）

- History 頁面時間範圍切換目前僅完成 UI，尚未依區間重新查詢 Firebase。
- 尚未建立離線暫存/補傳機制（網路中斷時資料可靠性強化）。
