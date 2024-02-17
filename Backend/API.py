from fastapi import FastAPI
from mangum import Mangum
from pydantic import BaseModel
import time
from datetime import datetime
import os
import psycopg2
from psycopg2.extras import RealDictCursor
from fastapi.middleware.cors import CORSMiddleware

POSTGRESQL_URL = os.environ["POSTGRESQL_URL"]
EXPIRE_TIME = int(os.environ["EXPIRE_TIME"])

app = FastAPI()
app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],  # Allows all origins
    allow_credentials=True,
    allow_methods=["*"],  # Allows all methods
    allow_headers=["*"],  # Allows all headers
)
handler = Mangum(app)

class Patient(BaseModel):
    name: str
    dob: str
    gender: int

class Comment(BaseModel):
    comment: str

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
            cur.execute("SELECT signal_id, patient_id FROM signals WHERE patient_id = %s", (patient_id,))
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
        "comment": signal_data["comment"]
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

@app.put("/patient/modify/{patient_id}")
def modify_patient(patient_id: int, patient: Patient):
    update_fields = {key: value for key, value in patient.dict().items() if value is not None}
    if not update_fields:
        return {"message": "No fields to update"}

    with psycopg2.connect(POSTGRESQL_URL) as conn:
        with conn.cursor() as cur:
            for key, value in update_fields.items():
                if key == 'dob':
                    value = datetime.strptime(value, "%Y-%m-%d").date()
                cur.execute(f"UPDATE patients SET {key} = %s WHERE patient_id = %s", (value, patient_id))
    return {"message": "Patient information updated successfully"}

@app.delete("/patient/delete/{patient_id}")
def delete_patient(patient_id: int):
    with psycopg2.connect(POSTGRESQL_URL) as conn:
        with conn.cursor() as cur:

            cur.execute("SELECT signal_id FROM signals WHERE patient_id = %s", (patient_id,))
            signal_ids = [row[0] for row in cur.fetchall()]

            for signal_id in signal_ids:
                cur.execute("DELETE FROM time_series_data WHERE signal_id = %s", (signal_id,))

            cur.execute("DELETE FROM signals WHERE patient_id = %s", (patient_id,))

            cur.execute("DELETE FROM patients WHERE patient_id = %s", (patient_id,))

    return {"message": "Patient and related data deleted successfully"}

@app.put("/signal/activate/{patient_id}")
def activate_signals(patient_id: int):
    with psycopg2.connect(POSTGRESQL_URL) as conn:
        with conn.cursor() as cur:
            cur.execute("DELETE FROM signals WHERE signal_id = 1")
            
            cur.execute("UPDATE signals SET status = 0")
            
            cur.execute("SELECT 1 FROM signals WHERE patient_id = %s", (patient_id,))
            has_signals = cur.fetchone() is not None
            
            if has_signals:
                cur.execute("UPDATE signals SET status = 1 WHERE patient_id = %s", (patient_id,))
            else:
                cur.execute("INSERT INTO signals (signal_id, patient_id, status) VALUES (1, %s, 1)", (patient_id,))
            
            conn.commit()
    return {"message": "Signals activated for patient"}

@app.delete("/signal/delete/{signal_id}")
def delete_signals(signal_id: int):
    with psycopg2.connect(POSTGRESQL_URL) as conn:
        with conn.cursor() as cur:

            cur.execute("DELETE FROM time_series_data WHERE signal_id = %s", (signal_id,))

            cur.execute("DELETE FROM signals WHERE signal_id = %s", (signal_id,))

    return {"message": "Signal and related data deleted successfully"}

@app.put("/signal/comment/{signal_id}")
def modify_comment(
    signal_id: int,
    comment: Comment,
    ):
    with psycopg2.connect(POSTGRESQL_URL) as conn:
        with conn.cursor() as cur:

            cur.execute("UPDATE time_series_data SET comment = %s WHERE signal_id = %s", (comment.comment, signal_id))

    return {"message": "Comment modified successfully"}





