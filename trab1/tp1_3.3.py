# Comentários mais uteis e com menor avaliação
# SELECT *
# FROM reviews 
# WHERE id IN (
#   SELECT reviews.id 
#   FROM products,reviews 
#   WHERE products.asin = reviews.asin
#   AND products.asin = '1579550088'
# )
# ORDER BY reviews.rating, reviews.helpful DESC
#  LIMIT 5

# Comentários mais úteis e com mair avaliação 
# SELECT *
# FROM reviews 
# WHERE id IN (
#   SELECT reviews.id 
#   FROM products,reviews 
#   WHERE products.asin = reviews.asin
#   AND products.asin = '1579550088'    
# )
# ORDER BY reviews.rating DESC, reviews.helpful DESC
#  LIMIT 5

# Dado um produto, listar os produtos similares com maiores vendas do que ele
# SELECT * 
# FROM products
# WHERE asin IN(
#   (SELECT similars.asin_similar as asin FROM similars WHERE similars.asin = '0827229534')
# 		UNION 
# 	(SELECT similars.asin as asin FROM similars WHERE similars.asin_similar = '0827229534'))
# ORDER BY salesrank DESC

# Listar os 10 produtos líderes de venda em cada grupo de produtos
# SELECT *
# FROM products
# WHERE products._group != 'Book'
# ORDER BY salesrank DESC
# LIMIT 10

# Listar os 10 produtos com a maior média de avaliações úteis positivas por produto
# SELECT products.*
# FROM
#   (SELECT products.asin as asii, avg(reviews.helpful) as "avg", count(reviews.helpful) as total
#     FROM products, reviews
#     WHERE products.asin = reviews.asin
#     GROUP BY (products.asin)
#   ) tt JOIN products ON products.asin = tt.asii
# ORDER BY "avg" DESC
# LIMIT 10

# SELECT *
#   FROM 
#   (SELECT reviews.cutomer, products._group, count(reviews.asin) as _count
#     FROM products, reviews
#     WHERE products.asin = reviews.asin
#    	GROUP BY (products._group,reviews.id)
#    	ORDER BY _count DESC
#   ) tt ORDER BY _count ASC

import psycopg2
import os


class Product:

    def __init__(self, id, asin, title, group, salesrank, similar, categories, reviews, discontinued):
        self.id = int(id)
        self.discontinued = discontinued
        self.asin = asin
        self.title = title
        self.group = group
        self.salesrank = int(salesrank)
        self.similar = similar
        self.categories = categories
        self.reviews = reviews

    def show_product(self):
        
        print(f"id:{self.id}-")
        print(f"ASIN:{self.asin}-")
        if(not self.discontinued):
            print(f"title:{self.title}-")
            print(f"group:{self.group}-")
            print(f"salesrank:{self.salesrank}-")
            print(f"similar:{self.similar}-")
            print(f"categories:{self.categories}-")
            print(f"reviews:{self.reviews}-")
        else :
            print("Product discontinued")

    def get_id(self): return self.id
    def get_asin(self): return self.asin
    def get_discontinued(self): return self.discontinued
    def get_title(self): return self.title
    def get_group(self): return self.group
    def get_salesrank(self): return self.salesrank
    def get_similar(self): return self.similar
    def get_categories(self): return self.categories
    def get_reviews(self): return self.reviews

class Review:

    def __init__(self,asin,date,cutomer,rating,votes,helpful):
        self.asin = asin
        self.date = date
        self.cutomer = cutomer
        self.rating = rating
        self.votes = votes
        self.helpful = helpful

    def show_review(self):
        print(f" Date: -{self.date}-")
        print(f" Cutomer: -{self.cutomer}-")
        print(f" rating: -{self.rating}-")
        print(f" Votes: -{self.votes}-")
        print(f" Helpful: -{self.helpful}-")

    def get_date(self): return self.date
    def get_asin(self): return self.asin
    def get_cutomer(self): return self.cutomer
    def get_rating(self): return self.rating
    def get_votes(self): return self.votes
    def get_helpful(self): return self.helpful


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


def a():
    conn = connect()
    if conn is not None:
        try:
            cur = conn.cursor()
            cur.execute(f"SELECT * FROM reviews WHERE id IN ( SELECT reviews.id FROM products,reviews WHERE products.asin = reviews.asin AND products.asin = '1579550088') ORDER BY reviews.rating, reviews.helpful DESC LIMIT 5")
            print("The number of parts: ", cur.rowcount)
            rows = cur.fetchall()
            if rows is not None:
                for row in rows:
                    print(row)

            cur.close()
        except (Exception, psycopg2.DatabaseError) as error:
            print(error)
        
if __name__ == '__main__':
    a()