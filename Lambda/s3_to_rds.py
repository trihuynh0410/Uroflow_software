import psycopg2
import pandas as pd 
import boto3
import os
import tempfile
from sqlalchemy import create_engine

s3_client = boto3.client('s3')

def download_file_from_s3(bucket, key, file_path):
    print('Downloading', bucket, key, file_path)
    with open(file_path, 'wb') as file:
        s3_client.download_fileobj(bucket, key, file)
    print('Downloaded', bucket, key)

import pandas as pd

def process_file(file_path):
    with open(file_path, 'r') as file:
        lines = file.readlines()

    data = []
    current_signal_id = None
    for line in lines:
        if line.startswith('#,'):
            current_signal_id = line.strip().split(',')[1]
        else:
            time_point, value = line.strip().split(',')
            data.append([current_signal_id, time_point, value])

    df = pd.DataFrame(data, columns=['signal_id', 'timestamp', 'value'])
    return df

def clean_downloaded_file(file_path):
    os.remove(file_path)
    print('Temporary file deleted')

def lambda_handler(event, context):
    try:
        record = event['Records'][0]
        bucket = record['s3']['bucket']['name']
        key = record['s3']['object']['key']

        print('Converting', bucket, ':', key)

        _, extension = os.path.splitext(key)
        
        temp_file_path = tempfile.mktemp(suffix=extension)
        download_file_from_s3(bucket, key, temp_file_path)

        df = process_file(temp_file_path)

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
        
            # Check if the placeholder patient exists
            check_patient_query = "SELECT 1 FROM Patients WHERE patient_id = %s"
            placeholder_patient_id = 1
            cursor.execute(check_patient_query, (placeholder_patient_id,))
            patient_exists = cursor.fetchone()
        
            # If not, insert a temporary patient record
            if not patient_exists:
                insert_patient_query = "INSERT INTO Patients (patient_id, name, age) VALUES (%s, 'Temporary Name', 0)"
                cursor.execute(insert_patient_query, (placeholder_patient_id,))
                conn.commit()
        
            # Then proceed with your existing logic
            query = "SELECT 1 FROM Signals WHERE signal_id = %s"
            cursor.execute(query, (signal_id,))
            result = cursor.fetchone()
        
            if result is None:
                insert_query = "INSERT INTO Signals (signal_id, patient_id) VALUES (%s, %s)"
                cursor.execute(insert_query, (signal_id, placeholder_patient_id))
                conn.commit()
        
            cursor.close()
        
        conn.close()

        df.to_sql(name = 'time_series_data', con = engine, if_exists='append', index=False)
        clean_downloaded_file(temp_file_path)

    except Exception as e:
        print('Error:', str(e))
