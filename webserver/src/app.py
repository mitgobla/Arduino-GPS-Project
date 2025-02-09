from flask import Flask, jsonify, request, render_template
from flask_sqlalchemy import SQLAlchemy
from sqlalchemy.ext.declarative import declarative_base
from sqlalchemy import Column, Integer, Float, DateTime, String, func
from geoalchemy2 import Geometry
import json
from dateutil.parser import isoparse
from datetime import datetime

app = Flask(__name__)
app.config.from_object("src.config.Config")
db = SQLAlchemy(app)

Base = declarative_base()

class DeviceData(Base):
    """Table representing device data.
    Columns:
    - id: unique identifier.
    - location: GPS location of the device in SRID 4326 format.
    - recorded: timestamp of the data recording.
    - altitude: altitude of the device in meters.
    - temperature: temperature reading in Celsius.
    - humidity: humidity reading in percentage.
    - device: device identifier.
    - created_at: timestamp of the data insertion.
    """
    __tablename__ = "device_data"
    id = Column(Integer, primary_key=True)
    location = Column(Geometry(geometry_type="POINT", srid=4326))
    recorded = Column(DateTime(timezone=True))
    altitude = Column(Float)
    temperature = Column(Float)
    humidity = Column(Float)
    device = Column(String(255))
    created_at = Column(DateTime, server_default=func.now())


@app.route("/api/value-ranges", methods=["GET"])
def get_value_ranges():
    """Get the minimum and maximum values for temperature, humidity, and date.

    Returns:
        Response: JSON response with the minimum and maximum values for temperature, humidity, and date.
    """
    ranges = db.session.query(
        func.min(DeviceData.temperature),
        func.max(DeviceData.temperature),
        func.min(DeviceData.humidity),
        func.max(DeviceData.humidity),
        func.min(DeviceData.recorded),
        func.max(DeviceData.recorded),
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
    """Get device data within a given map bounds and date range.

    Returns:
        Response: JSON response with the device data within the given map bounds and date range.
    """
    # Leaflet map bounds
    min_lat = request.args.get("min_lat", type=float)
    max_lat = request.args.get("max_lat", type=float)
    min_lng = request.args.get("min_lng", type=float)
    max_lng = request.args.get("max_lng", type=float)

    # Date range
    start_date: str | None | datetime = request.args.get("start_date", type=str)
    end_date: str | None | datetime = request.args.get("end_date", type=str)

    # Validate input
    if not min_lat or not max_lat or not min_lng or not max_lng:
        return jsonify({"error": "Missing map bounds"}), 400

    # Parse date range if provided
    if start_date and end_date:
        try:
            start_date = isoparse(str(start_date))
            end_date = isoparse(str(end_date))
        except ValueError:
            return jsonify({"error": "Invalid date format"}), 400
    else:
        # Query min and max date from the database
        dates = db.session.query(
            func.min(DeviceData.recorded),
            func.max(DeviceData.recorded)
        ).first()
        if not dates:
            return jsonify({"error": "No date data available"}), 404
        start_date, end_date = dates

    # Query device data within the given map bounds and date range
    device_data = db.session.query(DeviceData).filter(
        func.ST_Within(
            DeviceData.location,
            func.ST_MakeEnvelope(min_lng, min_lat, max_lng, max_lat, 4326),
        ),
        DeviceData.recorded.between(start_date, end_date),
    ).all()

    # Convert to GeoJSON for Leaflet map
    features = []
    for data in device_data:
        feature = {
            "type": "Feature",
            "geometry": json.loads(db.session.scalar(func.ST_AsGeoJSON(data.location, 6))),
            "properties": {
                "id": data.id,
                "recorded": data.recorded.isoformat() if data.recorded is not None else None,
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
    """Home page.

    Returns:
        Response: HTML response with the home page.
    """
    return render_template("index.html")