# Arduino GPS Project

This project is an ESP32-based GPS and sensor tracking system that logs location and environmental data, then visualises it on a Flask webserver with an interactive Leaflet map.

![Arduino](https://img.shields.io/badge/-Arduino-00979D?style=for-the-badge&logo=Arduino&logoColor=white) ![ESP32](https://img.shields.io/badge/ESP32-000000?style=for-the-badge&logo=espressif&logoColor=white) ![Python](https://img.shields.io/badge/python-3670A0?style=for-the-badge&logo=python&logoColor=ffdd54) ![Flask](https://img.shields.io/badge/Flask-000000?style=for-the-badge&logo=flask&logoColor=white) ![PostgreSQL](https://img.shields.io/badge/PostgreSQL-316192?style=for-the-badge&logo=postgresql&logoColor=white) ![PostGIS](https://img.shields.io/badge/PostGIS-006B3A?style=for-the-badge&logo=postgis&logoColor=white) ![Leaflet](https://img.shields.io/badge/Leaflet-199900?style=for-the-badge&logo=leaflet&logoColor=white) ![Bootstrap](https://img.shields.io/badge/Bootstrap-7952B3?style=for-the-badge&logo=bootstrap&logoColor=white) ![TypeScript](https://img.shields.io/badge/TypeScript-007ACC?style=for-the-badge&logo=typescript&logoColor=white) ![HTML](https://img.shields.io/badge/HTML-E34F26?style=for-the-badge&logo=html5&logoColor=white) ![CSS](https://img.shields.io/badge/CSS-1572B6?style=for-the-badge&logo=css3&logoColor=white) ![Webpack](https://img.shields.io/badge/Webpack-8DD6F9?style=for-the-badge&logo=webpack&logoColor=black)

## Features

- **ESP32 hardware**
  - **NEO-6M GPS module**: Captures location data
  - **DHT22 sensor**: Measures temperature and humidity
  - **SD card module**: Stores data while offline, provides configuration options
  - **Portable Battery and Project case**: Designed for outdoor use and mobility
- **Backend**
  - **Python Flask webserver**: Serves the frontend and data endpoints
  - **PostgreSQL with PostGIS**: Stores location data and performs spatial queries
- **Frontend**
  - **Leaflet.js**: Displays location data on an interactive map
  - **Bootstrap**: Provides clean UI with rapid prototyping
  - **flatpickr**: Date picker for filtering data
  - **TypeScript, HTML, CSS**: Frontend languages built with Webpack

A project for me to learn a geospatial stack and build a full-stack application from scratch, bringing in my love for microcontrollers and IoT.

## Further information

- [Hardware](hardware/README.md): Details on the hardware components and how they are set up.
- [Webserver](webserver/README.md): Installation instructions for the webserver.

## Photos

|![Hardware](hardware/docs/project_in_box.png)|
|:--:|
| *The project in its box, DHT22 sensor on a long wire* |

|![Webserver](webserver/docs/webpage.png)|
|:--:|
| *The Flask webserver displaying the map and data* |

|![Collecting data](docs/collecting_data.png)|
|:--:|
| *Out in the field collecting data and checking it is functioning correctly* |

|![Collecting data 2](docs/collecting_data_2.png)|
|:--:|
| *Collecting data in my car, connected to laptop for debugging* |
