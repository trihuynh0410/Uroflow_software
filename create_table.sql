--Create table patient
CREATE TABLE Patients (
    patient_id SERIAL PRIMARY KEY,
    name VARCHAR(255),
    age INTEGER
);

--Create table signal id
CREATE TABLE Signals (
    signal_id SERIAL PRIMARY KEY,
    patient_id INTEGER REFERENCES Patients(patient_id)
);

--Create table data
CREATE TABLE Time_Series_Data (
    signal_id INTEGER REFERENCES Signals(signal_id),
    timestamp BIGINT NOT NULL,
    value FLOAT NOT NULL
);
--SELECT create_hypertable('Time_Series_Data', 'timestamp');