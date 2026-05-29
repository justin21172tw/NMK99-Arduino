<template>
  <q-page class="q-pa-md">
    <!-- Page Header -->
    <div class="row q-col-gutter-sm items-center q-mb-md">
      <div class="col-12 col-md-6">
        <h5 class="q-my-none text-weight-bold row items-center">
          <q-icon name="analytics" class="q-mr-sm text-primary" />
          歷史數據分析與報表
        </h5>
        <div class="text-caption text-grey-6">查詢、統計並導出裝置歷史傳輸紀錄</div>
      </div>
      <div class="col-12 col-md-6 text-right">
        <q-btn-group outline>
          <q-btn outline color="primary" icon="file_download" label="導出 CSV" @click="exportCSV" />
          <q-btn outline color="primary" icon="print" label="列印報表" />
        </q-btn-group>
      </div>
    </div>

    <!-- Statistics Summary -->
    <div class="row q-col-gutter-md q-mb-lg">
      <div class="col-12 col-sm-6">
        <q-card flat bordered>
          <q-card-section class="q-pb-none">
            <div class="text-subtitle2 text-red-8 text-weight-bold">溫度統計摘要 (°C)</div>
          </q-card-section>
          <q-card-section class="row justify-between text-center">
            <div class="col">
              <div class="text-h6 text-weight-bold">{{ sensorStore.statistics.temp.min }}</div>
              <div class="text-caption text-grey-6">最小值</div>
            </div>
            <q-separator vertical inset />
            <div class="col">
              <div class="text-h6 text-weight-bold text-primary">{{ sensorStore.statistics.temp.avg }}</div>
              <div class="text-caption text-grey-6">平均值</div>
            </div>
            <q-separator vertical inset />
            <div class="col">
              <div class="text-h6 text-weight-bold">{{ sensorStore.statistics.temp.max }}</div>
              <div class="text-caption text-grey-6">最大值</div>
            </div>
          </q-card-section>
        </q-card>
      </div>

      <div class="col-12 col-sm-6">
        <q-card flat bordered>
          <q-card-section class="q-pb-none">
            <div class="text-subtitle2 text-blue-8 text-weight-bold">濕度統計摘要 (%)</div>
          </q-card-section>
          <q-card-section class="row justify-between text-center">
            <div class="col">
              <div class="text-h6 text-weight-bold">{{ sensorStore.statistics.humid.min }}</div>
              <div class="text-caption text-grey-6">最小值</div>
            </div>
            <q-separator vertical inset />
            <div class="col">
              <div class="text-h6 text-weight-bold text-primary">{{ sensorStore.statistics.humid.avg }}</div>
              <div class="text-caption text-grey-6">平均值</div>
            </div>
            <q-separator vertical inset />
            <div class="col">
              <div class="text-h6 text-weight-bold">{{ sensorStore.statistics.humid.max }}</div>
              <div class="text-caption text-grey-6">最大值</div>
            </div>
          </q-card-section>
        </q-card>
      </div>
    </div>

    <!-- Historical Chart -->
    <q-card flat bordered class="q-mb-lg">
      <q-card-section class="row items-center justify-between">
        <div class="text-subtitle1 text-weight-bold text-grey-9">數據趨勢分析</div>
        <div class="row q-gutter-sm">
          <q-select
            v-model="timeRange"
            :options="timeOptions"
            label="時間範圍"
            dense
            outlined
            style="width: 150px"
            emit-value
            map-options
            @update:model-value="onRangeChange"
          />
        </div>
      </q-card-section>
      <q-card-section>
        <apexchart
          type="area"
          height="300"
          :options="chartOptions"
          :series="chartSeries"
        />
      </q-card-section>
    </q-card>

    <!-- Data Table -->
    <q-card flat bordered>
      <q-table
        title="原始數據列表"
        :rows="sensorStore.history"
        :columns="columns"
        row-key="timestamp"
        flat
        :pagination="pagination"
        class="no-shadow"
      >
        <template v-slot:body-cell-temperature="props">
          <q-td :props="props">
            <q-badge :color="getTempColor(props.value)" class="q-px-sm">
              {{ props.value }} °C
            </q-badge>
          </q-td>
        </template>

        <template v-slot:body-cell-humidity="props">
          <q-td :props="props">
            <q-badge :color="getHumidColor(props.value)" class="q-px-sm">
              {{ props.value }} %
            </q-badge>
          </q-td>
        </template>
      </q-table>
    </q-card>
  </q-page>
</template>

<script setup lang="ts">
import { ref, computed } from 'vue';
import { useSensorStore } from 'stores/sensor-store';
import apexchart from 'vue3-apexcharts';
import type { ApexOptions } from 'apexcharts';
import type { QTableColumn } from 'quasar';
import { date, exportFile, useQuasar } from 'quasar';

const sensorStore = useSensorStore();
const $q = useQuasar();

const timeRange = ref(24);
const timeOptions = [
  { label: '最近 1 小時', value: 1 },
  { label: '最近 6 小時', value: 6 },
  { label: '最近 12 小時', value: 12 },
  { label: '最近 24 小時', value: 24 },
];

const pagination = ref({
  rowsPerPage: 10,
  sortBy: 'timestamp',
  descending: true
});

const columns: QTableColumn[] = [
  {
    name: 'timestamp',
    required: true,
    label: '測量時間',
    align: 'left',
    field: 'timestamp',
    format: (val: number) => date.formatDate(val, 'YYYY-MM-DD HH:mm:ss'),
    sortable: true,
  },
  {
    name: 'temperature',
    label: '溫度 (°C)',
    field: 'temperature',
    sortable: true,
  },
  {
    name: 'humidity',
    label: '濕度 (%)',
    field: 'humidity',
    sortable: true,
  },
  {
    name: 'deviceId',
    label: '裝置來源',
    field: 'deviceId',
    sortable: true,
  },
];

function onRangeChange(val: number) {
  // In a real app, this would trigger a Firebase query
  // Here we just re-generate the simulation for that range
  sensorStore.generateHistoricalData(val, val > 6 ? 10 : 2);
}

function getTempColor(temp: number) {
  if (temp > sensorStore.thresholds.tempMax) return 'red-8';
  if (temp < sensorStore.thresholds.tempMin) return 'orange-8';
  return 'green-7';
}

function getHumidColor(humid: number) {
  if (humid > sensorStore.thresholds.humidMax) return 'red-8';
  if (humid < sensorStore.thresholds.humidMin) return 'orange-8';
  return 'blue-7';
}

function exportCSV() {
  const content = [columns.map(col => col.label).join(',')]
    .concat(
      sensorStore.history.map(row => [
        date.formatDate(row.timestamp, 'YYYY-MM-DD HH:mm:ss'),
        row.temperature,
        row.humidity,
        row.deviceId
      ].join(','))
    )
    .join('\r\n');

  const status = exportFile(
    `sensor-report-${date.formatDate(Date.now(), 'YYYYMMDD-HHmm')}.csv`,
    content,
    'text/csv'
  );

  if (status !== true) {
    $q.notify({
      message: '瀏覽器不支援導出檔案',
      color: 'negative',
      icon: 'warning'
    });
  }
}

// Chart series and options
const chartSeries = computed(() => {
  return [
    {
      name: '溫度 (°C)',
      data: sensorStore.history.map(r => ({ x: r.timestamp, y: r.temperature }))
    },
    {
      name: '濕度 (%)',
      data: sensorStore.history.map(r => ({ x: r.timestamp, y: r.humidity }))
    }
  ];
});

const chartOptions = computed<ApexOptions>(() => {
  return {
    chart: {
      type: 'area',
      toolbar: { show: true },
      zoom: { type: 'x', enabled: true, autoScaleYaxis: true }
    },
    dataLabels: { enabled: false },
    stroke: { curve: 'smooth', width: 2 },
    colors: ['#ef5350', '#42a5f5'],
    fill: {
      type: 'gradient',
      gradient: {
        shadeIntensity: 1,
        opacityFrom: 0.45,
        opacityTo: 0.05,
        stops: [20, 100, 100, 100]
      }
    },
    xaxis: {
      type: 'datetime',
      labels: {
        datetimeUTC: false,
        style: { colors: '#9e9e9e' }
      }
    },
    yaxis: [
      {
        title: { text: '溫度 (°C)' },
        labels: { formatter: (v: number) => v.toFixed(1) }
      },
      {
        opposite: true,
        title: { text: '濕度 (%)' },
        labels: { formatter: (v: number) => v.toFixed(0) }
      }
    ],
    tooltip: { x: { format: 'yyyy-MM-dd HH:mm' } }
  };
});
</script>
