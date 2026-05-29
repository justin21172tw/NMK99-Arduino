import { defineStore, acceptHMRUpdate } from 'pinia';

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
    isSimulating: false,
    simulationIntervalId: null as number | null,
    deviceId: 'DHT22-Node-01',
    thresholds: {
      tempMax: 30,
      tempMin: 18,
      humidMax: 70,
      humidMin: 30
    }
  }),

  getters: {
    latestReadings: (state) => {
      // Returns last 50 readings for real-time graph
      return state.history.slice(-50);
    },
    // Statistics for the current history
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
    // Generate simulated data for the past N hours
    generateHistoricalData(hours: number = 24, intervalMinutes: number = 10) {
      const data: SensorReading[] = [];
      const now = Date.now();
      const totalPoints = (hours * 60) / intervalMinutes;

      let temp = 24.5;
      let humid = 55.0;

      for (let i = totalPoints; i >= 0; i--) {
        const timestamp = now - i * intervalMinutes * 60 * 1000;
        const hourOfDay = new Date(timestamp).getHours();
        const diurnalTempFactor = Math.sin(((hourOfDay - 8) / 24) * 2 * Math.PI);
        const targetTemp = 25.0 + diurnalTempFactor * 4.0;
        const targetHumid = 60.0 - diurnalTempFactor * 15.0;

        temp = temp + (targetTemp - temp) * 0.1 + (Math.random() - 0.5) * 0.5;
        humid = humid + (targetHumid - humid) * 0.1 + (Math.random() - 0.5) * 1.5;

        temp = Math.max(10, Math.min(45, temp));
        humid = Math.max(10, Math.min(100, humid));

        data.push({
          timestamp,
          temperature: parseFloat(temp.toFixed(1)),
          humidity: parseFloat(humid.toFixed(1)),
          deviceId: this.deviceId,
        });
      }

      this.history = data;
      if (data.length > 0) {
        this.currentReading = data[data.length - 1] ?? null;
      }
    },

    addReading(reading: SensorReading) {
      this.history.push(reading);
      this.currentReading = reading;
      if (this.history.length > 5000) {
        this.history.shift();
      }
    },

    startSimulation() {
      if (this.isSimulating) return;
      this.isSimulating = true;
      if (this.history.length === 0) {
        this.generateHistoricalData(24, 10);
      }

      this.simulationIntervalId = window.setInterval(() => {
        if (!this.currentReading) {
          this.currentReading = {
            timestamp: Date.now(),
            temperature: 25.0,
            humidity: 50.0,
            deviceId: this.deviceId,
          };
        }

        const lastTemp = this.currentReading.temperature;
        const lastHumid = this.currentReading.humidity;
        const deltaTemp = (Math.random() - 0.5) * 0.4;
        const deltaHumid = (Math.random() - 0.5) * 1.0;

        const newTemp = parseFloat(Math.max(15, Math.min(40, lastTemp + deltaTemp)).toFixed(1));
        const newHumid = parseFloat(Math.max(20, Math.min(95, lastHumid + deltaHumid)).toFixed(1));

        this.addReading({
          timestamp: Date.now(),
          temperature: newTemp,
          humidity: newHumid,
          deviceId: this.deviceId,
        });
      }, 2000);
    },

    stopSimulation() {
      if (!this.isSimulating) return;
      if (this.simulationIntervalId !== null) {
        clearInterval(this.simulationIntervalId);
        this.simulationIntervalId = null;
      }
      this.isSimulating = false;
    },

    resetData() {
      this.history = [];
      this.currentReading = null;
      if (this.isSimulating) {
        this.stopSimulation();
        this.startSimulation();
      }    },

    updateThresholds(newThresholds: { tempMax: number, tempMin: number, humidMax: number, humidMin: number }) {
      this.thresholds = { ...this.thresholds, ...newThresholds };    }
  },
});

if (import.meta.hot) {
  import.meta.hot.accept(acceptHMRUpdate(useSensorStore, import.meta.hot));
}
