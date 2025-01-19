from flask import Flask, jsonify, request, render_template
from flask_sqlalchemy import SQLAlchemy
from geoalchemy2 import Geometry
import json
# from datetime import datetime

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


@app.route("/api/device-data", methods=["GET"])
def get_device_data():
    # Leaflet map bounds
    print(request.args)
    print(request.args.get("min_lat", type=float))
    min_lat = request.args.get("min_lat", type=float)
    max_lat = request.args.get("max_lat", type=float)
    min_lng = request.args.get("min_lng", type=float)
    max_lng = request.args.get("max_lng", type=float)
    # Leaflet map timeline plugin
    #start_date = request.args.get("start_date", type=str)
    #end_date = request.args.get("end_date", type=str)

    if not min_lat or not max_lat or not min_lng or not max_lng:
        return jsonify({"error": "Missing map bounds"}), 400

    # if not start_date or not end_date:
    #     return jsonify({"error": "Missing date range"}), 400

    # try:
    #     start_date = datetime.fromisoformat(start_date)
    #     end_date = datetime.fromisoformat(end_date)
    # except ValueError:
    #     return jsonify({"error": "Invalid date format"}), 400

    device_data = DeviceData.query.filter(
        db.func.ST_Within(
            DeviceData.location,
            db.func.ST_MakeEnvelope(min_lng, min_lat, max_lng, max_lat, 4326),
        ),
        #DeviceData.recorded.between(start_date, end_date),
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