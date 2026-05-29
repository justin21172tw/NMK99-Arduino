import { defineStore, acceptHMRUpdate } from 'pinia';
import { db } from '../firebase';
import { ref as dbRef, onValue, query, limitToLast, orderByKey } from 'firebase/database';

export interface SensorReading {
  timestamp: number; // millisecond timestamp
  temperature: number; // °C
  humidity: number; // %
  deviceId: string;
}

export const useSensorStore = defineStore('sensor', {
  state: () => ({
    currentReading: null as SensorReading | null,
    history: [] as SensorReading[],
    isLoading: false,
    deviceId: 'NMK99-Node-01',
    thresholds: {
      tempMax: 30,
      tempMin: 18,
      humidMax: 70,
      humidMin: 30
    }
  }),

  getters: {
    latestReadings: (state) => {
      return state.history.slice(-50);
    },
    statistics: (state) => {
      if (state.history.length === 0) {
        return {
          temp: { min: 0, max: 0, avg: 0 },
          humid: { min: 0, max: 0, avg: 0 },
          count: 0,
        };
      }
      const temps = state.history.map((r) => r.temperature);
      const humids = state.history.map((r) => r.humidity);

      const sumTemp = temps.reduce((a, b) => a + b, 0);
      const sumHumid = humids.reduce((a, b) => a + b, 0);

      return {
        temp: {
          min: parseFloat(Math.min(...temps).toFixed(1)),
          max: parseFloat(Math.max(...temps).toFixed(1)),
          avg: parseFloat((sumTemp / temps.length).toFixed(1)),
        },
        humid: {
          min: parseFloat(Math.min(...humids).toFixed(1)),
          max: parseFloat(Math.max(...humids).toFixed(1)),
          avg: parseFloat((sumHumid / humids.length).toFixed(1)),
        },
        count: state.history.length,
      };
    },
  },

  actions: {
    // Initialize Firebase listeners
    initFirebaseSync() {
      this.isLoading = true;
      const devicePath = `devices/${this.deviceId}`;
      
      // 1. Listen for current reading
      const currentRef = dbRef(db, `${devicePath}/current`);
      onValue(currentRef, (snapshot) => {
        const data = snapshot.val();
        if (data) {
          const newReading: SensorReading = {
            timestamp: data.ts,
            temperature: data.t,
            humidity: data.h,
            deviceId: this.deviceId
          };
          this.currentReading = newReading;
          this.isLoading = false;

          // Add to local history if it's a new timestamp
          const isDuplicate = this.history.some(r => r.timestamp === newReading.timestamp);
          if (!isDuplicate) {
            this.history.push(newReading);
            // Sort and limit to 100 points
            this.history.sort((a, b) => a.timestamp - b.timestamp);
            if (this.history.length > 100) {
              this.history.shift();
            }
          }
        }
      });

      // 2. Listen for history (last 100 points for performance)
      const historyRef = query(
        dbRef(db, `${devicePath}/history`),
        orderByKey(),
        limitToLast(100)
      );
      
      onValue(historyRef, (snapshot) => {
        const data = snapshot.val();
        if (data) {
          const list: SensorReading[] = Object.keys(data).map(key => ({
            timestamp: data[key].ts,
            temperature: data[key].t,
            humidity: data[key].h,
            deviceId: this.deviceId
          }));
          // Sort by timestamp just in case
          this.history = list.sort((a, b) => a.timestamp - b.timestamp);
        }
      });
    },

    updateThresholds(newThresholds: { tempMax: number, tempMin: number, humidMax: number, humidMin: number }) {
      this.thresholds = { ...this.thresholds, ...newThresholds };
    }
  },
});

if (import.meta.hot) {
  import.meta.hot.accept(acceptHMRUpdate(useSensorStore, import.meta.hot));
}
