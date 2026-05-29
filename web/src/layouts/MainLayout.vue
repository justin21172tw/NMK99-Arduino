<template>
  <q-layout view="lHh Lpr lFf">
    <q-header elevated class="bg-primary text-white">
      <q-toolbar>
        <q-btn
          flat
          dense
          round
          icon="menu"
          aria-label="Menu"
          @click="toggleLeftDrawer"
        />

        <q-toolbar-title class="row items-center no-wrap">
          <q-icon name="thermostat" size="28px" class="q-mr-sm text-amber-5" />
          <span class="text-weight-bold">NMK99 溫濕度物聯網監控平台</span>
        </q-toolbar-title>

        <div class="row items-center q-gutter-md">
          <!-- Device status badge -->
          <q-badge :color="sensorStore.isSimulating ? 'green' : 'amber'" text-color="black" class="q-py-xs q-px-sm text-weight-medium">
            <q-icon :name="sensorStore.isSimulating ? 'sensors' : 'sensors_off'" size="14px" class="q-mr-xs" />
            {{ sensorStore.isSimulating ? '模擬數據串流中' : '連線暫停' }}
          </q-badge>

          <div class="text-caption text-grey-3 gt-xs">
            裝置 ID: <span class="text-weight-bold">{{ sensorStore.deviceId }}</span>
          </div>

          <q-btn
            flat
            round
            dense
            :icon="$q.dark.isActive ? 'light_mode' : 'dark_mode'"
            @click="toggleDarkMode"
          />
        </div>
      </q-toolbar>
    </q-header>

    <q-drawer
      v-model="leftDrawerOpen"
      show-if-above
      bordered
      :width="260"
    >
      <div class="q-py-md q-px-lg bg-grey-2 text-grey-9 text-subtitle2 text-weight-bold row items-center gt-sm">
        <q-avatar size="32px" color="primary" text-color="white" icon="memory" class="q-mr-md" />
        <div>
          <div>NMK99 IoT Hub</div>
          <div class="text-caption text-grey-6">v1.0.0 (Quasar Framework)</div>
        </div>
      </div>

      <q-divider />

      <q-list class="q-mt-md">
        <q-item-label header class="text-weight-bold text-grey-7">導覽選單</q-item-label>

        <q-item
          clickable
          v-ripple
          :to="{ name: 'dashboard' }"
          exact
          active-class="text-primary bg-blue-1"
        >
          <q-item-section avatar>
            <q-icon name="dashboard" />
          </q-item-section>
          <q-item-section>
            <q-item-label class="text-weight-medium">即時數據看板</q-item-label>
            <q-item-label caption>即時數值與動態圖表</q-item-label>
          </q-item-section>
        </q-item>

        <q-item
          clickable
          v-ripple
          :to="{ name: 'history' }"
          active-class="text-primary bg-blue-1"
        >
          <q-item-section avatar>
            <q-icon name="analytics" />
          </q-item-section>
          <q-item-section>
            <q-item-label class="text-weight-medium">歷史數據與報表</q-item-label>
            <q-item-label caption>區間查詢、統計、導出</q-item-label>
          </q-item-section>
        </q-item>
      </q-list>

      <div class="absolute-bottom q-pa-md text-center text-caption text-grey-6 border-top">
        © 2026 NMK99-Arduno
      </div>
    </q-drawer>

    <q-page-container class="bg-grey-1 text-grey-9">
      <router-view />
    </q-page-container>
  </q-layout>
</template>

<script setup lang="ts">
import { ref, onMounted } from 'vue';
import { useQuasar } from 'quasar';
import { useSensorStore } from 'stores/sensor-store';

const $q = useQuasar();
const sensorStore = useSensorStore();

const leftDrawerOpen = ref(false);

function toggleLeftDrawer() {
  leftDrawerOpen.value = !leftDrawerOpen.value;
}

function toggleDarkMode() {
  $q.dark.toggle();
}

onMounted(() => {
  // Automatically start simulating on load to make the app alive!
  sensorStore.startSimulation();
});
</script>

<style scoped>
.border-top {
  border-top: 1px solid #e0e0e0;
}
body.body--dark .border-top {
  border-top: 1px solid #333333;
}
body.body--dark .bg-grey-1 {
  background-color: #121212 !important;
}
body.body--dark .bg-grey-2 {
  background-color: #1d1d1d !important;
  color: #ffffff !important;
}
body.body--dark .bg-blue-1 {
  background-color: #0d47a1 !important;
  color: #90caf9 !important;
}
</style>
