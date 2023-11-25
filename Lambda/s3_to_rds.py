import psycopg2
import pandas as pd 
conn = psycopg2.connect(
    dbname="academic_online_dev", 
    user="academic_dev_viewer", 
    password="hdq2106", 
    host="103.3.245.20",
    port="5433"
)

query = """
WITH paper_count AS (
    SELECT keyphrase_id, COUNT(paper_id) AS paper_count
    FROM keyphrases_papers
    GROUP BY keyphrase_id
)
SELECT c.name, c.definition, pc.paper_count
FROM keyphrases AS c
JOIN keyphrases_domains AS d ON c.id = d.keyphrase_id
JOIN paper_count AS pc ON c.id = pc.keyphrase_id
WHERE c.level = 2 AND d.domain_id = 'phys'
GROUP BY (c.name, c.definition, pc.paper_count)
"""
df_level2 = pd.read_sql_query(query, conn)
if conn is not None:
    conn.close()
df_level2.reset_index(drop=True, inplace=True)
df_level2.sort_values(by='paper_count', ascending=False, inplace=True)