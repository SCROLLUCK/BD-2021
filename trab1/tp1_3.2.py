import psycopg2
import os
import time
import datetime
import math

all_products = []
all_reviews = []
all_resembling = []
all_categories = []
all_product_category = []
all_customers = []
# lista auxiliar para evitar repetições na lista all_categories
categories_ids = []
length = 0

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

def drop_tables():
    print(" Destruindo tabelas ..")
    conn = connect()
    if conn is not None:
        commands = (
            """
            DROP TABLE IF EXISTS products CASCADE;
            """,
            """
            DROP TABLE IF EXISTS categories CASCADE;
            """,
            """
            DROP TABLE IF EXISTS product_category CASCADE;
            """,
            """
            DROP TABLE IF EXISTS resembling CASCADE;
            """,
            """
            DROP TABLE IF EXISTS customers CASCADE;
            """,
            """
            DROP TABLE IF EXISTS reviews CASCADE;
            """,
        )
        cur = conn.cursor()
        
        for command in commands:
            cur.execute(command)

        cur.close()
        conn.commit()
        conn.close()
        print(" - Bando de dados Limpo!")

def create_tables():
    conn = connect()
    print(" Criando tabelas ..")
    if conn is not None:
        commands = (
            """
            CREATE TABLE products (
            id INTEGER PRIMARY KEY,
            asin CHAR(10) UNIQUE NOT NULL,
            title VARCHAR(500),
            _group VARCHAR(100),
            salesrank INTEGER,
            discontinued BOOLEAN NOT NULL
            )
            """,
            """
            CREATE TABLE categories (
            id INTEGER PRIMARY KEY,
            name VARCHAR(500),
            subcategory_of INTEGER,
            FOREIGN KEY (subcategory_of) REFERENCES categories (id) ON UPDATE CASCADE ON DELETE CASCADE
            )
            """,
            """
            CREATE TABLE product_category (
            product_asin CHAR(10) NOT NULL,
            category_id INTEGER NOT NULL,
            FOREIGN KEY (product_asin) REFERENCES products (asin) ON UPDATE CASCADE ON DELETE CASCADE,
            FOREIGN KEY (category_id) REFERENCES categories (id) ON UPDATE CASCADE ON DELETE CASCADE
            )
            """,
            """
            CREATE TABLE resembling (
            asin CHAR(10),
            asin_resembling CHAR(10),
            PRIMARY KEY (asin,asin_resembling),
            FOREIGN KEY (asin) REFERENCES products (asin) ON UPDATE CASCADE ON DELETE CASCADE,
            FOREIGN KEY (asin_resembling) REFERENCES products (asin) ON UPDATE CASCADE ON DELETE CASCADE
            )
            """,
            """
            CREATE TABLE customers (
            id VARCHAR(14) PRIMARY KEY
            )
            """,
             """
            CREATE TABLE reviews (
            id SERIAL PRIMARY KEY,
            date DATE NOT NULL DEFAULT CURRENT_DATE,
            asin CHAR(10) NOT NULL,
            customer VARCHAR(14) NOT NULL,
            rating INTEGER NOT NULL DEFAULT 0,
            votes INTEGER NOT NULL DEFAULT 0,
            helpful INTEGER NOT NULL DEFAULT 0,
            FOREIGN KEY (asin) REFERENCES products (asin) ON UPDATE CASCADE ON DELETE CASCADE
            )
            """,
        )
         
        cur = conn.cursor()
        
        for command in commands:
            cur.execute(command)

        cur.close()
        conn.commit()
        conn.close()
        print(" - Criação de tabelas finalzada!")

def progress_bar(iteration, total, prefix = '', suffix = '', decimals = 1, length = 100, fill = '█', printEnd = "\r"):
    percent = ("{0:." + str(decimals) + "f}").format(100 * (iteration / float(total)))
    filledLength = int(length * iteration // total)
    bar = fill * filledLength + '-' * (length - filledLength)
    print(f'\r   {prefix} |{bar}| {percent}% {suffix}', end = printEnd)
    if iteration == total: 
        print()

def populate_tables():
    conn = connect()
    print(" Populando tabelas ..")
    
    if conn is not None:

        print(" Cadastrando Clientes .. Tempo estimado: 5 minutos")
        start = time.time()
        cur = conn.cursor()
        valid_insertions = 0
        for c, customer in enumerate(all_customers):
            try:
                query = """INSERT INTO customers (id) VALUES (%s,)"""
                cur.execute(query,customer)
                conn.commit()
                valid_insertions+=1
            except (Exception, psycopg2.DatabaseError) as error:
                conn.rollback()
                print(customer)
                print(error)
                return
            
            progress_bar(c + 1, len(all_customers), prefix = 'Progresso:', suffix = f"Completo {c+1} de {len(all_customers)}", length = 40)
                
        cur.close()
        done = time.time()
        print(f" - {valid_insertions} Clientes cadastrados em: {str(datetime.timedelta(seconds=math.floor(done - start)))}")
        
        # print(" - Cadastrando Produtos .. Tempo estimado: 1 minuto")
        # start = time.time()
        # try:
        #     query = """INSERT INTO products (id,asin,title,_group,salesrank,discontinued) VALUES (%s,%s,%s,%s,%s,%s)"""
        #     cur = conn.cursor()
        #     cur.executemany(query,all_products)
        #     conn.commit()
        #     cur.close()
        # except (Exception, psycopg2.DatabaseError) as error:
        #     print(error)
        #     return

        # done = time.time()
        # print(f" - {len(all_products)} produtos cadastrados em: {str(datetime.timedelta(seconds=math.floor(done - start)))}")

        # print(" Cadastrando Categorias .. Tempo estimado: 2 minutos")
        # start = time.time()
        # cur = conn.cursor()
        # valid_insertions = 0
        # for j, category in enumerate(all_categories):
        #     try:
        #         query = """INSERT INTO categories (id,name, subcategory_of) VALUES (%s,%s,%s)"""
        #         cur.execute(query,category)
        #         conn.commit()
        #         valid_insertions+=1
        #     except (Exception, psycopg2.DatabaseError) as error:
        #         conn.rollback()
        #         print(error)
        #         return
            
        #     progress_bar(j + 1, len(all_categories), prefix = 'Progresso:', suffix = f"Completo {j+1} de {len(all_categories)}", length = 40)
                
        # cur.close()
        # done = time.time()
        # print(f" - {valid_insertions} Categorias cadastradas em: {str(datetime.timedelta(seconds=math.floor(done - start)))}")

        # print(" - Cadastrando Categorias dos Produtos .. ")
        # start = time.time()
        # try:
        #     query = """INSERT INTO product_category (product_asin,category_id) VALUES (%s,%s)"""
        #     cur = conn.cursor()
        #     cur.executemany(query,all_product_category)
        #     conn.commit()
        #     cur.close()
        # except (Exception, psycopg2.DatabaseError) as error:
        #     print(error)
        #     return

        # done = time.time()
        # print(f" - {len(all_product_category)} Categorias de produtos cadastrados em: {str(datetime.timedelta(seconds=math.floor(done - start)))}")


        # print(" Cadastrando Reviews .. Tempo estimado: 15 minutos")
        # start = time.time()
        # try:
        #     query = """INSERT INTO reviews (date,asin,customer,rating,votes,helpful) VALUES (%s,%s,%s,%s,%s,%s)"""
        #     cur = conn.cursor()
        #     cur.executemany(query,all_reviews)
        #     conn.commit()
        #     cur.close()
        # except (Exception, psycopg2.DatabaseError) as error:
        #     print(error)
        #     return
        # done = time.time()
        # print(f" - {len(all_reviews)} reviews cadastrados em: {str(datetime.timedelta(seconds=math.floor(done - start)))}")

        # print(" - Cadastrando Similaridade de Produtos .. Tempo estimado: 1h")
        # start = time.time()
        # cur = conn.cursor()
        # valid_insertions = 0
        # for i, similar in enumerate(all_resembling):
        #     try:
        #         query = """INSERT INTO resembling (asin,asin_resembling) VALUES (%s,%s)"""
        #         cur.execute(query,(similar[0],similar[1]))
        #         conn.commit()
        #     except (psycopg2.DatabaseError) as error:
        #         conn.rollback()

        #     progress_bar(i + 1, len(all_resembling), prefix = 'Progresso:', suffix = f"Completo {i+1} de {len(all_resembling)}", length = 40)

        # cur.close()
        # done = time.time()
        # print(f" - {len(valid_insertions)} Relações de Similaridade cadastradas em: {str(datetime.timedelta(seconds=math.floor(done - start)))}")


        conn.close()    

def read_file():

    print("Verificando arquivo de entrada ..")
    with open('amazon-meta.txt') as f:
        print(" - Arquivo encontrado!\n Iniciando leitura .. Tempo estimado: 10 minutos")
        start = time.time()
        #pula cabeçalho do arquivo
        f.seek(80,0)

        contents = f.read()
        contents = contents.split("\nId")

        for i, product in enumerate(contents):
            
            if(product.find("discontinued product") == -1): 
                id = product[product.find(":   ")+4:product.find("ASIN:")].strip("\n ")
                asin = product[product.find("ASIN: ")+6:product.find("title:")].strip("\n ")
                title = product[product.find("title: ")+7:product.find(" group:")].strip("\n ")
                group = product[product.find(" group: ")+7:product.find("salesrank:")].strip("\n ")
                salesrank = product[product.find("salesrank: ")+11:product.find("similar:")].strip("\n ")
                similars = product[product.find("similar: ")+9:product.find("categories:")].strip("\n ").split("  ")
                # ignora numero de similares
                similars.pop(0)
                # obtem similares
                for similar in similars:
                    all_resembling.append((asin,similar))
                
                # obtem categorias
                categories = product[product.find("categories: ")+12:product.find("reviews:")].strip("\n  ").split("\n")
                # ignora numero de categorias
                categories.pop(0)
                hierarchy_categories=[]
                for category_line in categories:
                    #limpa linha e divide em categorias
                    category_line = category_line.strip("   |").split("|")
                    for c, category in enumerate(category_line):
                        # adiciona categoria em uma lista de strings para facilitar a busca por categorias repetidas
                        name = ""
                        if not category.__contains__("[guitar]"):
                            category = category.split("[")
                            name = category[0]
                        else:
                            category = category.split("][")
                            name = category[0]+"]"

                        category_id = category[1].strip("\n   ")
                        category_id = int(category_id.strip("]"))
                        
                        # verifica se é a ultima categoria da linha
                        if c == len(category_line)-1:
                            all_product_category.append((asin,category_id))
                        
                        #constroi hierarquia de categoria do produto
                        hierarchy_categories.append(category_id)
                        
                        # verifica se categoria atual ja foi inserida
                        if category_id not in categories_ids:
                            categories_ids.append(category_id)
                            if c > 0:
                                all_categories.append((category_id,name,hierarchy_categories[c-1]))
                            else:
                                all_categories.append((category_id,name,category_id))
                
                # obtem reviews
                reviews = product[product.find("reviews: ")+9:].split("\n")
                # ignora status dos reviews
                reviews.pop(0)
                for review in reviews:
                    if(len(review) > 10):
                        date = review[review.find("    ")+4:review.find("  cutomer:")].strip(" ")
                        customer = review[review.find("cutomer: ")+9:review.find("  rating:")].strip(" ")
                        rating = review[review.find("rating: ")+8:review.find("  votes:")].strip(" ")
                        votes = review[review.find("votes: ")+7:review.find("  helpful:")].strip(" ")
                        helpful = review[review.find("helpful: ")+9:].strip(" ")
                        # if customer not in all_customers:
                        all_customers.append(customer)
                        all_reviews.append((date,asin,customer,int(rating),int(votes),int(helpful)))
                
                all_products.append((int(id),asin,title,group,int(salesrank),False))
                
            else:
                discontinued_id = product[product.find(":   ")+4:product.find("ASIN:")].strip("\n ")
                discontinued_asin = product[product.find("ASIN: ")+6:product.find("\n ")]
                all_products.append((discontinued_id,discontinued_asin,"","",0,True))
                
            progress_bar(i + 1, length, prefix = 'Progresso:', suffix = f"Completo {i+1} de 548552", length = 40)

        done = time.time()
        print(f" Leitura do arquivo finalizada! em {str(datetime.timedelta(seconds=math.floor(done - start)))} \n Informações encontradas:")
        print(f" - {len(all_products)} Produtos")
        print(f" - {len(all_reviews)} Reviews")
        print(f" - {len(all_categories)} Categorias")
        print(f" - {len(all_resembling)} Produtos Similares")
        print(f" - {len(all_product_category)} Categorias de Produtos")
        print(f" - {len(all_customers)} Clientes")


if __name__ == '__main__':
    os.system("service postgresql start")
    
    items = list(range(0, 548551))
    length = len(items)
    read_file()

    drop_tables()
    create_tables()
    
    items = list(range(0, 548551))
    length = len(items)
    populate_tables()
