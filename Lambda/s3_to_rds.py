import psycopg2
import pandas as pd 
import boto3
import os
import tempfile

s3_client = boto3.client('s3')

def download_file_from_s3(bucket, key, file_path):
    print('Downloading', bucket, key, file_path)
    with open(file_path, 'wb') as file:
        s3_client.download_fileobj(bucket, key, file)
    print('Downloaded', bucket, key)

def process_file(file_path):
    with open(file_path, 'r') as file:
        lines = file.readlines()
    dataframes = []
    current_data = []
    signal_count = 1
    for line in lines:
        if line.startswith('#,'):
            if current_data:
                df = pd.DataFrame(current_data, columns=[f'Signal {signal_count}', current_data[0][1]])
                df.reset_index(drop=True, inplace=True)
                dataframes.append(df)
                current_data = []
                signal_count += 1
            current_data.append(line.strip().split(','))
        else:
            current_data.append(line.strip().split(','))
    if current_data:
        df = pd.DataFrame(current_data, columns=[f'Signal {signal_count}', current_data[0][1]])
        df.reset_index(drop=True, inplace=True)
        dataframes.append(df)
    result = pd.concat(dataframes, axis=1)
    result = result.iloc[1:]
    return result

def clean_downloaded_file(file_path):
    os.remove(file_path)
    print('Temporary file deleted')

def sanitize_table_name(name):
    sanitized_name = name.replace(" ", "_").replace("-", "_")
    return sanitized_name

def lambda_handler(event, context):
    try:
        record = event['Records'][0]
        bucket = record['s3']['bucket']['name']
        key = record['s3']['object']['key']

        print('Converting', bucket, ':', key)

        _, extension = os.path.splitext(key)
        base_file_name = os.path.basename(key).replace(extension, '')
        table_name = sanitize_table_name(base_file_name)
        temp_file_path = tempfile.mktemp(suffix=extension)
        download_file_from_s3(bucket, key, temp_file_path)

        df = process_file(temp_file_path)

        clean_downloaded_file(temp_file_path)
        dbname = os.getenv('dbname')
        host = os.getenv('host')
        user = os.getenv('user')
        password = os.getenv('password')
        connection = psycopg2.connect(dbname = dbname,
                                       host = host,
                                       port = '5432',
                                       user = user,
                                       password = password)
        df.to_sql(table_name, connection, if_exists='replace')
        if connection is not None:
            connection.close()

    except Exception as e:
        print('Error:', str(e))