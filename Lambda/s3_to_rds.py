import os
import tempfile

import psycopg2
import pandas as pd 
import boto3
from sqlalchemy import create_engine
from utils import *

s3_client = boto3.client('s3')

def lambda_handler(event, context):
    try:
        record = event['Records'][0]
        bucket = record['s3']['bucket']['name']
        key = record['s3']['object']['key']

        print('Converting', bucket, ':', key)

        _, extension = os.path.splitext(key)
        
        temp_file_path = tempfile.mktemp(suffix=extension)
        download_file_from_s3(bucket, key, temp_file_path, s3_client)

        df = create_df(temp_file_path)
        window_size = 50
        for i in df.index:
            volume_string, time, volume = adjust_timestamps(df.loc[i, 'volume'], window_size)
            flow_rate_string, fr_time, flow_rate = flow_rate_gen(time, volume)
            median_volume = np.median(volume)
            max_flowrate = np.max(flow_rate)

            step = fr_time[1] - fr_time[0]
            voided_volume_per_interval = np.sum(flow_rate)
            voided_volume = voided_volume_per_interval*step

            average_flow_rate = voided_volume / max(fr_time)
            time_to_max_flow_rate = time[np.argmax(flow_rate)] - time[0]
            flow_time = max(time)
            df.loc[i, ["volume", "flow", "median_volume", "voided_volume",
                       "max_flow_rate", "avg_flow_rate", 
                       "time_to_max_flow_rate", "flow_time"]] = [volume_string, flow_rate_string, median_volume, voided_volume, max_flowrate, average_flow_rate, time_to_max_flow_rate, flow_time]

        insert_to_db(df)

        clean_downloaded_file(temp_file_path)

    except Exception as e:
        print('Error:', str(e))
