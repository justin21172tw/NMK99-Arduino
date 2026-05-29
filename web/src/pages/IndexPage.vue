<template>
  <q-page class="q-pa-md">
    <!-- Page Header -->
    <div class="row q-col-gutter-sm items-center q-mb-md">
      <div class="col-12 col-md-6">
        <h5 class="q-my-none text-weight-bold row items-center">
          <q-icon name="dashboard" class="q-mr-sm text-primary" />
          即時數據監控看板
        </h5>
        <div class="text-caption text-grey-6">顯示當前裝置傳送之溫濕度即時資訊</div>
      </div>
      <div class="col-12 col-md-6 text-right">
        <span class="text-caption text-grey-6 q-mr-sm">最後更新時間: {{ formatTime(sensorStore.currentReading?.timestamp) }}</span>
        <q-btn
          flat
          round
          dense
          color="primary"
          icon="refresh"
          @click="forceUpdate"
          class="q-mr-xs"
        >
          <q-tooltip>立即整理</q-tooltip>
        </q-btn>
      </div>
    </div>

    <!-- Quick Overview Cards -->
    <div class="row q-col-gutter-md q-mb-lg">
      <!-- Temperature Card -->
      <div class="col-12 col-sm-6 col-md-4">
        <q-card flat bordered class="metric-card overflow-hidden">
          <div class="card-edge bg-red-5"></div>
          <q-card-section class="q-pa-md">
            <div class="row items-center justify-between no-wrap q-mb-xs">
              <span class="text-subtitle2 text-grey-7 text-weight-medium">當前環境溫度</span>
              <q-icon name="thermostat" size="24px" color="red-5" />
            </div>

            <div class="row items-baseline q-my-sm">
              <span class="text-h3 text-weight-bolder text-grey-9">
                {{ sensorStore.currentReading ? sensorStore.currentReading.temperature : '--' }}
              </span>
              <span class="text-subtitle1 text-grey-6 q-ml-xs">°C</span>
            </div>

            <q-linear-progress
              :value="getPercentage(sensorStore.currentReading?.temperature, 10, 45)"
              color="red-5"
              class="q-mt-md"
              rounded
              track-color="red-1"
            />
          </q-card-section>
        </q-card>
      </div>

      <!-- Humidity Card -->
      <div class="col-12 col-sm-6 col-md-4">
        <q-card flat bordered class="metric-card overflow-hidden">
          <div class="card-edge bg-blue-5"></div>
          <q-card-section class="q-pa-md">
            <div class="row items-center justify-between no-wrap q-mb-xs">
              <span class="text-subtitle2 text-grey-7 text-weight-medium">當前環境濕度</span>
              <q-icon name="water_drop" size="24px" color="blue-5" />
            </div>

            <div class="row items-baseline q-my-sm">
              <span class="text-h3 text-weight-bolder text-grey-9">
                {{ sensorStore.currentReading ? sensorStore.currentReading.humidity : '--' }}
              </span>
              <span class="text-subtitle1 text-grey-6 q-ml-xs">% RH</span>
            </div>

            <q-linear-progress
              :value="getPercentage(sensorStore.currentReading?.humidity, 0, 100)"
              color="blue-5"
              class="q-mt-md"
              rounded
              track-color="blue-1"
            />
          </q-card-section>
        </q-card>
      </div>

      <!-- System Status Card -->
      <div class="col-12 col-md-4">
        <q-card flat bordered class="metric-card bg-primary text-white">
          <q-card-section class="q-pa-md height-100 column justify-between">
            <div>
              <div class="row items-center justify-between no-wrap q-mb-sm">
                <span class="text-subtitle2 text-grey-3 text-weight-medium">數據串流狀態</span>
                <q-icon name="dns" size="24px" color="grey-3" />
              </div>
              <div class="text-h6 text-weight-bold q-my-none">
                {{ sensorStore.currentReading ? '系統正常監測中' : '等待裝置數據...' }}
              </div>
              <div class="text-caption text-blue-1 q-mt-xs">
                Firebase 即時同步中 | 樣本數: <span class="text-weight-bold text-white">{{ sensorStore.history.length }}</span>
              </div>
            </div>

            <div class="row q-gutter-sm q-mt-md">
              <q-btn
                dense
                color="white"
                text-color="primary"
                icon="sync"
                label="重新連線"
                class="col text-weight-bold"
                @click="forceUpdate"
              />
              <q-btn
                dense
                flat
                color="white"
                icon="analytics"
                label="查看歷史"
                class="col text-weight-bold border-white"
                :to="{ name: 'history' }"
              />
            </div>
          </q-card-section>
        </q-card>
      </div>
    </div>

    <!-- Chart Panel -->
    <div class="row q-col-gutter-md">
      <div class="col-12 col-lg-8">
        <q-card flat bordered class="chart-card">
          <q-card-section class="row items-center justify-between q-pb-none">
            <div>
              <div class="text-subtitle1 text-weight-bold text-grey-9">即時溫濕度動態走勢</div>
              <div class="text-caption text-grey-6">每 2 秒自動更新最新觀測數值</div>
            </div>
            <div class="row items-center q-gutter-xs">
              <q-badge color="red-5" class="q-mr-xs">溫度 (°C)</q-badge>
              <q-badge color="blue-5">濕度 (%)</q-badge>
            </div>
          </q-card-section>

          <q-card-section class="q-pt-xs">
            <div v-if="sensorStore.history.length === 0" class="row items-center justify-center" style="height: 350px">
              <div class="text-center">
                <q-spinner color="primary" size="3em" />
                <div class="text-grey-6 q-mt-md">正在建立通訊鏈路...</div>
              </div>
            </div>
            <div v-else>
              <apexchart
                type="line"
                height="350"
                ref="realtimeChart"
                :options="chartOptions"
                :series="chartSeries"
              />
            </div>
          </q-card-section>
        </q-card>
      </div>

      <!-- Live Log -->
      <div class="col-12 col-lg-4">
        <q-card flat bordered class="column" style="height: 100%; min-height: 422px;">
          <q-card-section class="q-pb-none">
            <div class="text-subtitle1 text-weight-bold text-grey-9">硬體傳輸日誌</div>
            <div class="text-caption text-grey-6">最近 6 筆數據封包詳情</div>
          </q-card-section>

          <q-card-section class="col q-pt-md">
            <q-timeline color="primary">
              <q-timeline-entry
                v-for="(log, idx) in latestLogs"
                :key="idx"
                color="blue-grey-3"
                icon="sync"
                :subtitle="log.time"
              >
                <div class="text-weight-bold text-grey-8">數據接收成功</div>
                <div class="text-caption text-grey-6">T: {{ log.temp }}°C | H: {{ log.humid }}% | ID: {{ log.id }}</div>
              </q-timeline-entry>
            </q-timeline>
          </q-card-section>
        </q-card>
      </div>
    </div>
  </q-page>
</template>

<script setup lang="ts">
import { ref, computed } from 'vue';
import { useSensorStore } from 'stores/sensor-store';
import apexchart from 'vue3-apexcharts';
import type { ApexOptions } from 'apexcharts';

const sensorStore = useSensorStore();
const realtimeChart = ref<any>(null); // eslint-disable-line @typescript-eslint/no-explicit-any

function formatTime(timestamp: number | undefined): string {
  if (!timestamp) return '--:--:--';
  const d = new Date(timestamp);
  return d.toTimeString().split(' ')[0] ?? '--:--:--';
}

function getPercentage(val: number | undefined, min: number, max: number): number {
  if (val === undefined) return 0;
  const pct = (val - min) / (max - min);
  return Math.max(0, Math.min(1, pct));
}

function forceUpdate() {
  sensorStore.initFirebaseSync();
}

const latestLogs = computed(() => {
  return sensorStore.history.slice(-6).reverse().map(r => ({
    time: formatTime(r.timestamp),
    temp: r.temperature,
    humid: r.humidity,
    id: r.deviceId
  }));
});

const chartSeries = computed(() => {
  const readings = sensorStore.latestReadings;
  return [
    { name: '溫度 (°C)', data: readings.map((r) => r.temperature) },
    { name: '濕度 (%)', data: readings.map((r) => r.humidity) },
  ];
});

const chartOptions = computed<ApexOptions>(() => {
  const readings = sensorStore.latestReadings;
  const categories = readings.map((r) => formatTime(r.timestamp));

  return {
    chart: {
      id: 'realtime-sensor-chart',
      type: 'line',
      animations: {
        enabled: true,
        easing: 'linear',
        dynamicAnimation: { speed: 1000 },
      },
      toolbar: { show: false },
      zoom: { enabled: false },
    },
    colors: ['#ef5350', '#42a5f5'],
    stroke: { curve: 'smooth', width: 3 },
    dataLabels: { enabled: false },
    xaxis: {
      categories,
      labels: { show: true, style: { fontSize: '10px' }, rotate: -45 },
      tickAmount: 10,
    },
    yaxis: [
      {
        title: { text: '溫度 (°C)', style: { color: '#ef5350' } },
        labels: { style: { colors: '#ef5350' }, formatter: (val: number) => `${val.toFixed(1)}°C` },
        min: 15, max: 40,
      },
      {
        opposite: true,
        title: { text: '濕度 (% RH)', style: { color: '#42a5f5' } },
        labels: { style: { colors: '#42a5f5' }, formatter: (val: number) => `${val.toFixed(0)}%` },
        min: 20, max: 95,
      },
    ],
    grid: { borderColor: '#f1f1f1' },
    tooltip: { shared: true, intersect: false },
  };
});
</script>

<style scoped>
.metric-card {
  position: relative;
  border-radius: 8px;
  box-shadow: 0 4px 15px rgba(0, 0, 0, 0.05);
  transition: transform 0.2s ease;
  height: 100%;
}
.metric-card:hover { transform: translateY(-2px); }
.card-edge { position: absolute; top: 0; left: 0; width: 4px; height: 100%; }
.chart-card { border-radius: 8px; box-shadow: 0 4px 15px rgba(0, 0, 0, 0.05); }
.height-100 { height: 100%; }
.border-white { border: 1px solid rgba(255, 255, 255, 0.4); }
</style>
