from flask import Flask, jsonify
from flask_sqlalchemy import SQLAlchemy
from geoalchemy2 import Geometry
import json

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


@app.route("/")
def index():
    query = DeviceData.query.count()
    return f"Hello World! There are {query} records in the database."