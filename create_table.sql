--Create table patient
CREATE TABLE Patients (
    patient_id SERIAL PRIMARY KEY,
    name VARCHAR(255),
    dob Date,
    gender integer
);

--Create table signal id
CREATE TABLE Signals (
    signal_id SERIAL PRIMARY KEY,
    patient_id INTEGER REFERENCES Patients(patient_id)
    status INTEGER DEFAULT 0
);

CREATE TABLE Time_Series_Data (
    signal_id INTEGER REFERENCES Signals(signal_id),
    volume varchar,
    flow varchar,
    median_volume float,
    voided_volume float,
    max_flow_rate float,
    avg_flow_rate float,
    time_to_max_flow_rate float,
    flow_time float
    comment text
    
);
ALTER TABLE time_series_data
ALTER COLUMN comment SET DEFAULT 'No Comment',
ALTER COLUMN comment SET NOT NULL;