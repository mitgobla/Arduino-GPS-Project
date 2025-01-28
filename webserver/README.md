# Arduino GPS Project Webserver

Flask webserver used to display Postgres data from the hardware of the project. Uses Leaflet.js to display the data on a map. Bootstrap for styling.

Backend written in Python, frontend in TypeScript, HTML and CSS with Webpack.

## Install

```bash
npm install
python -m venv .venv
source .venv/bin/activate
pip install -r requirements.txt
```

## Build

```bash
npm run build
```

## Run

```bash
flask --app src/app.py run
```
