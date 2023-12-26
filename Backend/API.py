from fastapi import FastAPI
from mangum import Mangum
from pydantic import BaseModel
import time
from datetime import datetime
import os
import psycopg2
from psycopg2.extras import RealDictCursor

POSTGRESQL_URL = os.environ["POSTGRESQL_URL"]
EXPIRE_TIME = int(os.environ["EXPIRE_TIME"])

app = FastAPI()
handler = Mangum(app)

class Patient(BaseModel):
    name: str
    dob: str
    gender: int

def split_and_convert(data_str):
    data_list = data_str.split()
    time = [float(pair.split(',')[0]) for pair in data_list]
    val = [float(pair.split(',')[1]) for pair in data_list]
    return time, val

@app.get("/patient/get")
def fetch_patient_info():
    with psycopg2.connect(POSTGRESQL_URL) as conn:
        with conn.cursor(cursor_factory=RealDictCursor) as cur:
            cur.execute("SELECT * FROM patients")
            patient_info = cur.fetchall()
    return patient_info

@app.get("/signal_meta/{patient_id}")
def fetch_signal_metadata(patient_id: int):
    with psycopg2.connect(POSTGRESQL_URL) as conn:
        with conn.cursor(cursor_factory=RealDictCursor) as cur:
            cur.execute("SELECT * FROM signals WHERE patient_id = %s", (patient_id,))
            signal_meta = cur.fetchall()
    return signal_meta

@app.get("/signal_info/{signal_id}")
def fetch_times_series(signal_id: int):
    with psycopg2.connect(POSTGRESQL_URL) as conn:
        with conn.cursor(cursor_factory=RealDictCursor) as cur:
            cur.execute("SELECT * FROM time_series_data WHERE signal_id = %s", (signal_id,))
            data = cur.fetchall()
    signal_data = data[0]
    print(signal_data["signal_id"])
    date_measured = signal_data["signal_id"]
    dt_object = datetime.fromtimestamp(date_measured)
    date_time_str = dt_object.strftime("%Y-%m-%d, %H:%M:%S")

    vol_time, vol_value = split_and_convert(signal_data["volume"])
    fr_time, fr_value = split_and_convert(signal_data["flow"])

    res = {
        "signal_id": signal_id,
        "date_time": date_time_str,
        "volume": {
            "vol_time": vol_time,
            "vol_value": vol_value
        },
        "flow_rate": {
            "fr_time": fr_time,
            "fr_value": fr_value
        },
        "median_volume": signal_data["median_volume"],
        "voided_volume": signal_data["voided_volume"],
        "max_flow_rate": signal_data["max_flow_rate"],
        "avg_flow_rate": signal_data["avg_flow_rate"],
        "time_to_max_flow_rate": signal_data["time_to_max_flow_rate"],
        "flow_time": signal_data["flow_time"],
    }
    return res

@app.post("/patient/add")
def add_patient(patient: Patient):
    patient_id = int(time.time())
    dob = datetime.strptime(patient.dob, "%Y-%m-%d").date()
    with psycopg2.connect(POSTGRESQL_URL) as conn:
        with conn.cursor() as cur:
            cur.execute("INSERT INTO patients (patient_id, name, dob, gender) VALUES (%s, %s, %s, %s)", 
                        (patient_id, patient.name, dob, patient.gender))
    return {"message": "Patient added successfully"}




