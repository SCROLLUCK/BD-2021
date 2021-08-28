import psycopg2
import os

def connect():
    conn = None
    try:
        conn = psycopg2.connect(
            host="localhost",
            database="root",
            user="root",
            password="password")
        return conn
    except (Exception, psycopg2.DatabaseError) as error:
        print(error)

def a(product_asin):
    conn = connect()
    print("\n (a) Dado produto, listar os 5 comentários mais úteis e com maior avaliação e os 5 comentários mais úteis e com menor avaliação")
    print(f"  - Produto utilizado: {product_asin}")
    print("  (id, date, asin, customer, rating, votes, helpful)\n")
    if conn is not None:
        try:
            cur = conn.cursor()
            query = f"""
            SELECT tt.*
                FROM (
                (SELECT reviews.*
                FROM products,reviews 
                WHERE products.asin = reviews.asin
                AND products.asin = '{product_asin}'
                ORDER BY reviews.rating DESC, reviews.helpful DESC
                LIMIT 5)
                    UNION ALL
                (SELECT reviews.* 
                FROM products,reviews 
                WHERE products.asin = reviews.asin
                AND products.asin = '{product_asin}'
                ORDER BY reviews.rating, reviews.helpful DESC
                LIMIT 5)
                ) tt
            """
            cur.execute(query)
            rows = cur.fetchall()
            if rows is not None:
                for row in rows:
                    print(f"  {row}")
            
            cur.close()
        except (Exception, psycopg2.DatabaseError) as error:
            print(error)

def b(product_asin):
    conn = connect()
    print("\n (b) Dado um produto, listar os produtos similares com maiores vendas do que ele")
    print(f"   - Produto utilizado: {product_asin}")
    print("  (id, asin, group, salesrank, title)\n")
    if conn is not None:
        try:
            
            cur = conn.cursor()
            query = f"""
            SELECT id, asin, _group, salesrank, title 
            FROM products
            WHERE asin IN ( SELECT resembling.asin_resembling as asin FROM resembling WHERE resembling.asin = '{product_asin}')
            AND products.salesrank > (SELECT salesrank FROM products where products.asin = '{product_asin}')
            ORDER BY salesrank DESC
            """
            cur.execute(query)
            rows = cur.fetchall()
            if rows is not None:
                for row in rows:
                    print(f"  {row}")
            
            cur.close()
        except (Exception, psycopg2.DatabaseError) as error:
            print(error)

def d():
    conn = connect()
    print("\n (d) Listar os 10 produtos líderes de venda em cada grupo de produtos")
    print("  (ranking, group, asin, salesrank, title)\n")
    if conn is not None:
        try:
            cur = conn.cursor()
            query = """
            SELECT x.ranking, x._group, x.asin, x.salesrank, x.title
            FROM 
                (SELECT
                ROW_NUMBER() OVER (PARTITION BY _group ORDER BY salesrank DESC) AS ranking,
                t.*
                FROM products t) x
            WHERE x.ranking <= 10 AND x._group != '';
            """
            cur.execute(query)
            rows = cur.fetchall()
            if rows is not None:
                for row in rows:
                    print(f"  {row}")
            
            cur.close()
        except (Exception, psycopg2.DatabaseError) as error:
            print(error)

def e():
    conn = connect()
    print("\n (e) Listar os 10 produtos com a maior média de avaliações úteis positivas por produto")
    print("  (asin, avg_helpful, total reviews)\n")
    if conn is not None:
        try:
            cur = conn.cursor()
            query = """ 
            SELECT tt.* 
            FROM
                (SELECT reviews.asin as asii, avg(reviews.helpful) as avg_helpful, count(reviews.asin) as total 
                    FROM products, reviews
                    WHERE reviews.asin = products.asin
                    GROUP BY asii
                    ORDER BY total DESC
                ) tt
            ORDER BY tt.avg_helpful DESC
            LIMIT 10
            """
            cur.execute(query)
            rows = cur.fetchall()
            if rows is not None:
                for row in rows:
                    print(f"  {row}")
            
            cur.close()
        except (Exception, psycopg2.DatabaseError) as error:
            print(error)

def g():
    conn = connect()
    print("\n (g) Listar os 10 clientes que mais fizeram comentários por grupo de produto")
    print("  (ranking, group, customer, number of comments)\n")
    if conn is not None:
        try:
            cur = conn.cursor()
            query = """ 
            SELECT * 
            FROM 
                (SELECT
                ROW_NUMBER() OVER (PARTITION BY _group ORDER BY number_coments DESC) AS ranking,
                t.*
                FROM (
                    SELECT tt._group, tt.customer, tt.number_coments
                    FROM 
                        (SELECT products._group, reviews.customer, count(reviews.customer) as number_coments
                        FROM products, reviews 
                        WHERE products.asin = reviews.asin 
                        GROUP BY (products._group, reviews.customer)
                        ) tt
                    )t
                ) x
            WHERE x.ranking <= 10;
            """
            cur.execute(query)
            rows = cur.fetchall()
            if rows is not None:
                for row in rows:
                    print(f"  {row}")
            
            cur.close()
        except (Exception, psycopg2.DatabaseError) as error:
            print(error)

if __name__ == '__main__':
    a('B000067DNF')
    b('B00004VXDB')
    d()
    e()
    g()