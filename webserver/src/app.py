from flask import Flask, jsonify, request, render_template
from flask_sqlalchemy import SQLAlchemy
from geoalchemy2 import Geometry
import json
from dateutil.parser import isoparse

app = Flask(__name__)
app.config.from_object("src.config.Config")
app.config["SQLALCHEMY_TRACK_MODIFICATIONS"] = False
db = SQLAlchemy(app)

class DeviceData(db.Model):
    __tablename__ = "device_data"
    id = db.Column(db.Integer, primary_key=True)
    location = db.Column(Geometry(geometry_type="POINT", srid=4326))
    # timestamp with timezone
    recorded = db.Column(db.DateTime(timezone=True))
    altitude = db.Column(db.Float)
    temperature = db.Column(db.Float)
    humidity = db.Column(db.Float)
    device = db.Column(db.String(255))
    # timestamp no timezone
    created_at = db.Column(db.DateTime, server_default=db.func.now())


@app.route("/api/value-ranges", methods=["GET"])
def get_value_ranges():
    ranges: list| None = db.session.query(
        db.func.min(DeviceData.temperature),
        db.func.max(DeviceData.temperature),
        db.func.min(DeviceData.humidity),
        db.func.max(DeviceData.humidity),
        db.func.min(DeviceData.recorded),
        db.func.max(DeviceData.recorded),
    ).first()
    if not ranges:
        return jsonify({"error": "No data available"}), 404

    out = {
        "temperature": [ranges[0], ranges[1]],
        "humidity": [ranges[2], ranges[3]],
        "date": [ranges[4].isoformat(), ranges[5].isoformat()],
    }
    return jsonify(out)

@app.route("/api/device-data", methods=["GET"])
def get_device_data():
    # Leaflet map bounds
    print(request.args)
    print(request.args.get("min_lat", type=float))
    min_lat = request.args.get("min_lat", type=float)
    max_lat = request.args.get("max_lat", type=float)
    min_lng = request.args.get("min_lng", type=float)
    max_lng = request.args.get("max_lng", type=float)

    start_date = request.args.get("start_date", type=str)
    end_date = request.args.get("end_date", type=str)

    if not min_lat or not max_lat or not min_lng or not max_lng:
        return jsonify({"error": "Missing map bounds"}), 400

    if start_date and end_date:
        print(start_date)
        print(end_date)
        try:
            start_date = isoparse(start_date)
            end_date = isoparse(end_date)
        except ValueError:
            return jsonify({"error": "Invalid date format"}), 400
    else:
        # query min and max date
        dates = db.session.query(
            db.func.min(DeviceData.recorded),
            db.func.max(DeviceData.recorded)
        ).first()
        if not dates:
            return jsonify({"error": "No date data available"}), 404
        start_date, end_date = dates

    device_data = DeviceData.query.filter(
        db.func.ST_Within(
            DeviceData.location,
            db.func.ST_MakeEnvelope(min_lng, min_lat, max_lng, max_lat, 4326),
        ),
        DeviceData.recorded.between(start_date, end_date),
    )

    # Convert to GeoJSON for Leaflet map
    features = []
    for data in device_data:
        feature = {
            "type": "Feature",
            "geometry": json.loads(db.session.scalar(db.func.ST_AsGeoJSON(data.location, 6))),
            "properties": {
                "id": data.id,
                "recorded": data.recorded.isoformat(),
                "altitude": data.altitude,
                "temperature": data.temperature,
                "humidity": data.humidity,
                "device": data.device,
            },
        }
        features.append(feature)

    return jsonify({"type": "FeatureCollection", "features": features})


@app.route("/")
def index():
    return render_template("index.html")