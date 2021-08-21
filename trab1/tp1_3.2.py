import psycopg2
import os
def connect():
    os.system("service postgresql start")
    conn = None
    try:

        print('Connecting to the PostgreSQL database...')
        conn = conn = psycopg2.connect(
    		host="localhost",
    		database="root",
    		user="root",
    		password="password")

        cur = conn.cursor()

        print('PostgreSQL database version:')
        cur.execute('SELECT version()')

        print(cur)
        cur.close()
    except (Exception, psycopg2.DatabaseError) as error:
        print(error)
    finally:
        if conn is not None:
            conn.close()
            print('Database connection closed.')


if __name__ == '__main__':
    connect()
