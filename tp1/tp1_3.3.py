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
                (SELECT rr.*
                    FROM (SELECT * FROM reviews WHERE ( votes > 0 and ((helpful/100)*votes) > 50 and rating > 2) ) rr
                    WHERE rr.asin = '{product_asin}'
                    ORDER BY rr.rating DESC, rr.helpful DESC
                    LIMIT 5)
                UNION ALL
                (SELECT rr.* 
                    FROM (SELECT * FROM reviews WHERE ( votes > 0 and ((helpful/100)*votes) > 50 and rating > 2) ) rr
                    WHERE rr.asin = '{product_asin}'
                    ORDER BY rr.rating, rr.helpful DESC
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
            SELECT asin, _group, salesrank, title 
            FROM products
            WHERE asin IN ( SELECT resembling.asin_resembling as asin FROM resembling WHERE resembling.asin = '{product_asin}')
            AND products.salesrank < (SELECT salesrank FROM products where products.asin = '{product_asin}')
            ORDER BY salesrank ASC
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
                ROW_NUMBER() OVER (PARTITION BY _group ORDER BY salesrank ASC) AS ranking,
                t.*
                FROM products t) x
            WHERE x.ranking <= 10 AND x._group IS NOT null AND x.salesrank > 0;
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
    print("  (asin, title, avg_rating, total reviews)\n")
    if conn is not None:
        try:
            cur = conn.cursor()
            query = """ 
            SELECT tt.asin, products.title, tt.avg_rating, tt.total_reviews 
                FROM
                    (SELECT rr.asin as asin, avg(rr.rating) as avg_rating, count(rr.rating) as total_reviews
                        FROM (SELECT * FROM reviews WHERE ( votes > 0 and ((helpful/100)*votes) > 50 and rating > 2) ) rr
                        GROUP BY asin
                    ) tt, products
            WHERE products.asin = tt.asin
            ORDER BY tt.avg_rating DESC, tt.total_reviews DESC
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

def f():
    conn = connect()
    print("\n (f) Listar a 5 categorias de produto com a maior média de avaliações úteis positivas por produto")
    print("  (id, name)\n")
    if conn is not None:
        try:
            cur = conn.cursor()
            query = """ 
            SELECT categories.id, categories.name FROM
                categories,
                (SELECT pp.category_id, sum(pp.avg_rating) as most_avg_rating FROM
                    (SELECT * FROM
                        (SELECT asin, avg(rating) as avg_rating, count(asin) as total_reviews FROM
                            (SELECT * FROM reviews WHERE ( votes > 0 and ((helpful/100)*votes) > 50 and rating > 2) ) rr
                        GROUP BY asin
                        ) product_rating
                    ,
                    (SELECT products.asin, categories.name as category_name, categories.id as category_id FROM products, categories, product_category 
                        WHERE product_category.category_id = categories.id and product_category.product_asin = products.asin
                    ) product_category
                    WHERE product_rating.asin = product_category.asin
                    ORDER BY avg_rating DESC,total_reviews DESC
                ) pp
                GROUP BY pp.category_id
            ) category_rating
            WHERE categories.id = category_rating.category_id
            ORDER BY category_rating.most_avg_rating DESC
            LIMIT 5
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
    b('0679410759')
    d()
    e()
    f()
    g()