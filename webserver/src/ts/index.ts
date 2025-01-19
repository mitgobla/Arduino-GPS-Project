// filepath: /home/mitgobla/Arduino-GPS-Project/webserver/src/ts/index.ts
import L from 'leaflet';
import chroma from 'chroma-js';
import Split from 'split.js';
import flatpickr from 'flatpickr';

Split(['#map', '#sidebar'], {
    minSize: 100
});

const map = L.map('map').setView([51.4545, -2.5879], 16);

L.tileLayer('https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png', {
    maxZoom: 19,
    attribution: '© OpenStreetMap contributors'
}).addTo(map);

let temperatureLayer = L.layerGroup();
let humidityLayer = L.layerGroup();


interface ValueRanges {
    temperature: [number, number];
    humidity: [number, number];
    date: [string, string];
}

async function getValueRanges(): Promise<ValueRanges> {
    const response = await fetch('/api/value-ranges');
    const data: ValueRanges = await response.json();
    return data;
}

let valueRanges: ValueRanges;
let startDate: string;
let endDate: string;

getValueRanges().then(ranges => {
    valueRanges = ranges;
    startDate = ranges.date[0];
    endDate = ranges.date[1];
    console.log(ranges);
    console.log(startDate);
    console.log(endDate);

    // Initialize flatpickr for start-date
    flatpickr("#start-date", {
        defaultDate: startDate,
        animate: true,
        enableTime: true,
        dateFormat: "Y-m-d H:i",
        onChange: function(selectedDates, dateStr, instance) {
            startDate = selectedDates[0].toISOString();
        }
    });

    // Initialize flatpickr for end-date
    flatpickr("#end-date", {
        defaultDate: endDate,
        animate: true,
        enableTime: true,
        dateFormat: "Y-m-d H:i",
        onChange: function(selectedDates, dateStr, instance) {
            endDate = selectedDates[0].toISOString();
        }
    });
});

function normalize(value: number, range: [number, number]): number {
    return (value - range[0]) / (range[1] - range[0]);
}

function fetchAndUpdateData() {
    const bounds = map.getBounds();
    let url = `/api/device-data?min_lng=${bounds.getWest()}&min_lat=${bounds.getSouth()}&max_lng=${bounds.getEast()}&max_lat=${bounds.getNorth()}`;
    if (startDate && endDate) {
        url += `&start_date=${encodeURIComponent(startDate)}&end_date=${encodeURIComponent(endDate)}`;
    }
    fetch(url)
    .then(response => response.json())
    .then(data => {
        document.getElementById('data-count')!.innerText = `Data count: ${data.features.length.toString()}`;

        temperatureLayer.clearLayers();
        humidityLayer.clearLayers();

        L.geoJSON(data, {
            pointToLayer: (feature, latlng) => {
                return L.circleMarker(latlng, {
                    radius: 8,
                    fillColor: chroma.scale(['lightblue', 'cyan', 'lime', 'yellow', 'orange', 'red'])(normalize(feature.properties.temperature, valueRanges.temperature)).hex(),
                    color: "#000",
                    weight: 1,
                    opacity: 1,
                    fillOpacity: 0.8
                }).bindPopup(`
                    <strong>Device:</strong> ${feature.properties.device}<br>
                    <strong>Temperature:</strong> ${feature.properties.temperature}°C<br>
                    <strong>Humidity:</strong> ${feature.properties.humidity}%<br>
                    <strong>Altitude:</strong> ${feature.properties.altitude}m<br>
                    <strong>Recorded:</strong> ${feature.properties.recorded}
                `);
            }
        }).addTo(temperatureLayer);

        L.geoJSON(data, {
            pointToLayer: (feature, latlng) => {
                return L.circleMarker(latlng, {
                    radius: 8,
                    fillColor: chroma.scale(['white', 'lightblue', 'cyan', 'blue'])(normalize(feature.properties.humidity, valueRanges.humidity)).hex(),
                    color: "#000",
                    weight: 1,
                    opacity: 1,
                    fillOpacity: 0.8
                }).bindPopup(`
                    <strong>Device:</strong> ${feature.properties.device}<br>
                    <strong>Temperature:</strong> ${feature.properties.temperature}°C<br>
                    <strong>Humidity:</strong> ${feature.properties.humidity}%<br>
                    <strong>Altitude:</strong> ${feature.properties.altitude}m<br>
                    <strong>Recorded:</strong> ${feature.properties.recorded}
                `);
            }
        }).addTo(humidityLayer);
    });
}

map.on('moveend', () => fetchAndUpdateData());
fetchAndUpdateData();

const baseLayers = {
    "Temperature": temperatureLayer,
    "Humidity": humidityLayer
};
const overlays = {};
L.control.layers(baseLayers, overlays).addTo(map);
temperatureLayer.addTo(map);





document.getElementById('apply-date-range')?.addEventListener('click', () => {
    fetchAndUpdateData();
});