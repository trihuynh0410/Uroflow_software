import os
import tempfile

import psycopg2
import numpy as np
import pandas as pd 
import boto3
from sqlalchemy import create_engine

def download_file_from_s3(bucket, key, file_path, s3_client):
    print('Downloading', bucket, key, file_path)
    with open(file_path, 'wb') as file:
        s3_client.download_fileobj(bucket, key, file)
    print('Downloaded', bucket, key)


def create_df(file_path):
    with open(file_path, 'r') as file:
        lines = file.readlines()

    data = {}
    current_signal_id = None
    for line in lines:
        if line.startswith('#,'):
            current_signal_id = line.strip().split(',')[1]
            if current_signal_id not in data:
                data[current_signal_id] = []
        else:
            volume = line.strip()
            data[current_signal_id].append(volume)

    data = [[signal_id, ' '.join(volumes)] for signal_id, volumes in data.items()]
    df = pd.DataFrame(data, columns=['signal_id', 'volume'])
    return df


def clean_downloaded_file(file_path):
    os.remove(file_path)
    print('Temporary file deleted')

def weighted_moving_average(data, timestamps, window_size):
    weights = np.arange(1, window_size + 1)
    smoothed_data = np.convolve(data, weights, 'valid') / np.sum(weights)
    smoothed_timestamps = timestamps[window_size - 1:]
    return smoothed_data, smoothed_timestamps

def enforce_monotonicity(values):
    corrected_values = [values[0]]
    for value in values[1:]:
        corrected_value = max(value, corrected_values[-1])
        corrected_values.append(corrected_value)
    return corrected_values

def connect_string(a,b):
    pairs = zip(a, b)
    result = ' '.join([f'{t},{v}' for t, v in pairs])
    return result

def input_to_df(content, index, col_name, df):
    df.loc[index, col_name] = content
    return df

def adjust_timestamps(data_str, window_size):
    data_list = data_str.split()

    timestamps = [float(pair.split(',')[0]) for pair in data_list]
    values = [float(pair.split(',')[1]) for pair in data_list]

    timestamps = pd.to_datetime(timestamps, unit='ms')
    average_distance = np.mean(np.diff(timestamps).astype('timedelta64[ms]'))
    adjusted_timestamps = timestamps[0] + pd.to_timedelta(np.arange(len(timestamps)) * average_distance)

    time = (adjusted_timestamps - adjusted_timestamps[0]).total_seconds() * 1000
    time = time.astype(int).tolist()

    smoothed_values, smoothed_timestamps = weighted_moving_average(values, time, window_size)
    pos_val = enforce_monotonicity(smoothed_values)
    sv, st = weighted_moving_average(pos_val, smoothed_timestamps, window_size)

    sv = sv/10
    sv = [round(val, 2) for val in sv]
    
    st = np.array(st)/1000
    st = st - min(st)
    st = [round(val, 4) for val in st]

    result = connect_string(st, sv)
    return result, st, sv

def flow_rate_gen(smoothed_timestamps, sc_values):

    volume_diff = np.array(sc_values[50:]) - np.array(sc_values[:-50])
    time_diff_seconds = np.array(smoothed_timestamps[50:]) - np.array(smoothed_timestamps[:-50])

    flow_rate_per_second = volume_diff / time_diff_seconds
    sm_time = smoothed_timestamps[50:]
    st =  sm_time - min(sm_time)
    st = [round(val, 4) for val in st]
    fr = [round(val, 2) for val in flow_rate_per_second]

    result = connect_string(st, fr)

    return result, st, fr

def insert_to_db(df):
    dbname = os.getenv('dbname')
    host = os.getenv('host')
    user = os.getenv('user')
    password = os.getenv('password')
    conn = psycopg2.connect(
        host=host,
        database=dbname,
        user=user,
        password=password)
    database_url = f'postgresql+psycopg2://{user}:{password}@{host}/{dbname}'
    engine = create_engine(database_url)
    
    for signal_id in df['signal_id'].unique():
        cursor = conn.cursor()
    
        get_patient_query = "SELECT patient_id FROM Signals WHERE status = 1"
        cursor.execute(get_patient_query)
        patient_id = cursor.fetchone()[0]
    
        query = "SELECT 1 FROM Signals WHERE signal_id = %s"
        cursor.execute(query, (signal_id,))
        result = cursor.fetchone()
    
        if result is None:
            insert_query = "INSERT INTO Signals (signal_id, patient_id) VALUES (%s, %s)"
            cursor.execute(insert_query, (signal_id, patient_id))
            conn.commit()
    
        cursor.close()
    
    with conn.cursor() as cur:
        delete_dummy_query = "DELETE FROM Signals WHERE signal_id = 1"
        cur.execute(delete_dummy_query)
        conn.commit()

    conn.close()

    df.to_sql(name = 'time_series_data', con = engine, if_exists='append', index=False)


