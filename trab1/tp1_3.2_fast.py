import psycopg2
import os
import time
import datetime
import math
import gc

all_products = []
all_reviews = []
all_resembling = []
all_categories = []
all_product_category = []
all_customers = []

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
            id INTEGER PRIMARY KEY NOT NULL,
            asin CHAR(10) UNIQUE NOT NULL,
            title VARCHAR(500),
            _group VARCHAR(100),
            salesrank INTEGER NOT NULL DEFAULT 0,
            discontinued BOOLEAN NOT NULL
            )
            """,
            """
            CREATE TABLE categories (
            id INTEGER PRIMARY KEY NOT NULL,
            name VARCHAR(500),
            subcategory_of INTEGER NOT NULL,
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
            asin CHAR(10) NOT NULL,
            asin_resembling CHAR(10) NOT NULL,
            PRIMARY KEY (asin,asin_resembling),
            FOREIGN KEY (asin) REFERENCES products (asin) ON UPDATE CASCADE ON DELETE CASCADE,
            FOREIGN KEY (asin_resembling) REFERENCES products (asin) ON UPDATE CASCADE ON DELETE CASCADE
            )
            """,
            """
            CREATE TABLE customers (
            id VARCHAR(14) PRIMARY KEY NOT NULL
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
            FOREIGN KEY (asin) REFERENCES products (asin) ON UPDATE CASCADE ON DELETE CASCADE,
            FOREIGN KEY (customer) REFERENCES customers (id) ON UPDATE CASCADE ON DELETE CASCADE
            )
            """,
        )
         
        cur = conn.cursor()
        
        for command in commands:
            cur.execute(command)

        cur.close()
        conn.commit()
        conn.close()
        print(" - Criação de tabelas finalizada!")

def progress_bar(iteration, total, prefix = '', suffix = '', decimals = 1, length = 100, fill = '█', printEnd = "\r"):
    percent = ("{0:." + str(decimals) + "f}").format(100 * (iteration / float(total)))
    filledLength = int(length * iteration // total)
    bar = fill * filledLength + '-' * (length - filledLength)
    print(f'\r   {prefix} |{bar}| {percent}% {suffix}', end = printEnd)
    if iteration == total: 
        print()

def populate_tables():
    
    global all_products
    global all_reviews
    global all_resembling
    global all_categories
    global all_product_category
    global all_customers
    conn = connect()
    
    print(" Populando tabelas ..")
    
    if conn is not None:

        print(" Cadastrando Produtos .. Tempo estimado: 0:01:00")
        start = time.time()
        try:
            query = """INSERT INTO products (id,asin,title,_group,salesrank,discontinued) VALUES (%s,%s,%s,%s,%s,%s)"""
            cur = conn.cursor()
            cur.executemany(query,all_products)
            conn.commit()
            cur.close()
        except (Exception, psycopg2.DatabaseError) as error:
            print(error)
            return
        finally:
            done = time.time()
            print(f" - {len(all_products)} produtos cadastrados em: {str(datetime.timedelta(seconds=math.floor(done - start)))}")
            all_products = None
            gc.collect()

        print(" Cadastrando Clientes .. Tempo estimado: 0:01:30")
        start = time.time()
        try:
            query = """INSERT INTO customers (id) VALUES (%s)"""
            cur = conn.cursor()
            cur.executemany(query,all_customers)
            conn.commit()
            cur.close()
        except (Exception, psycopg2.DatabaseError) as error:
            print(error)
            return
        finally:
            done = time.time()
            print(f" - {len(all_customers)} Clientes cadastrados em: {str(datetime.timedelta(seconds=math.floor(done - start)))}")
            all_customers = None
            gc.collect()

        print(" Cadastrando Reviews .. Tempo estimado: 0:15:00")
        start = time.time()
        try:
            query = """INSERT INTO reviews (date,asin,customer,rating,votes,helpful) VALUES (%s,%s,%s,%s,%s,%s)"""
            cur = conn.cursor()
            cur.executemany(query,all_reviews)
            conn.commit()
            cur.close()
        except (Exception, psycopg2.DatabaseError) as error:
            print(error)
            return
        finally:
            done = time.time()
            print(f" - {len(all_reviews)} reviews cadastrados em: {str(datetime.timedelta(seconds=math.floor(done - start)))}")
            all_reviews = None
            gc.collect()

        print(" Cadastrando Categorias .. Tempo estimado: 0:00:03")
        start = time.time()
        valid_insertions = len(all_categories)
        try:
            query = """INSERT INTO categories (id,name, subcategory_of) VALUES (%s,%s,%s)"""
            cur = conn.cursor()
            cur.executemany(query,all_categories)
            conn.commit()
            cur.close()
        except (Exception, psycopg2.DatabaseError) as error:
            valid_insertions=0
            print(error)
            return
        finally:
            done = time.time()
            print(f" - {valid_insertions} Categorias cadastradas em: {str(datetime.timedelta(seconds=math.floor(done - start)))}")
            all_categories = None
            gc.collect()

        print(" Cadastrando Categorias dos Produtos .. 0:03:32")
        start = time.time()
        try:
            query = """INSERT INTO product_category (product_asin,category_id) VALUES (%s,%s)"""
            cur = conn.cursor()
            cur.executemany(query,all_product_category)
            conn.commit()
            cur.close()
        except (Exception, psycopg2.DatabaseError) as error:
            print(error)
            return
        finally:
            done = time.time()
            print(f" - {len(all_product_category)} Categorias de produtos cadastrados em: {str(datetime.timedelta(seconds=math.floor(done - start)))}")
            all_product_category = None
            gc.collect()

        print(" Cadastrando Similaridade de Produtos .. Tempo estimado: 00:01:53")
        start = time.time()
        valid_insertions = len(all_resembling)
        try:
            query = """INSERT INTO resembling (asin,asin_resembling) VALUES (%s,%s)"""
            cur = conn.cursor()
            cur.executemany(query,all_resembling)
            conn.commit()
            cur.close()
        except (psycopg2.DatabaseError) as error:
            print(error)
            valid_insertions=0
            return
        finally:
            done = time.time()
            print(f" - {valid_insertions} Relações de Similaridade cadastradas em: {str(datetime.timedelta(seconds=math.floor(done - start)))}")
            all_resembling = None
            gc.collect()

        conn.close()    

def read_file():

    # discionarios auxiliares para evitar repetições
    all_products_disc = {}
    all_customers_disc = {}
    all_categories_disc = {}
    aux_resembling = []
    
    print(" Verificando arquivo de entrada ..")
    with open('amazon-meta.txt') as f:
        print(" - Arquivo encontrado!\n Iniciando leitura .. Tempo estimado: 0:01:30")
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
                    aux_resembling.append((asin,similar))
                
                # obtem categorias
                categories = product[product.find("categories: ")+12:product.find("reviews:")].strip("\n  ").split("\n")
                # ignora numero de categorias
                categories.pop(0)
                hierarchy_categories=[]
                for category_line in categories:
                    #limpa linha e divide em categorias
                    category_line = category_line.strip("   |").split("|")
                    for c, category in enumerate(category_line):
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
                        try:
                            all_categories_disc[category_id]
                        except KeyError:
                            all_categories_disc[category_id] = category_id
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
                        try:
                            all_customers_disc[customer]
                        except KeyError:
                            all_customers_disc[customer] = customer
                            all_customers.append((customer,))
                        all_reviews.append((date,asin,customer,int(rating),int(votes),int(helpful)))
                
                all_products.append((int(id),asin,title,group,int(salesrank),False))
                all_products_disc[asin] = asin
                
            else:
                discontinued_id = product[product.find(":   ")+4:product.find("ASIN:")].strip("\n ")
                discontinued_asin = product[product.find("ASIN: ")+6:product.find("\n ")]
                all_products.append((discontinued_id,discontinued_asin,"",None,0,True))
                all_products_disc[discontinued_asin] = discontinued_asin
                
            progress_bar(i + 1, 548552, prefix = 'Progresso:', suffix = f"Completo {i+1} de 548552", length = 40)
        
        all_categories_disc = {}
        all_customers_disc = {}

        done = time.time()
        print(f" Leitura do arquivo finalizada em {str(datetime.timedelta(seconds=math.floor(done - start)))} \n Informações encontradas:")
        print(f" - {len(all_products)} Produtos")
        print(f" - {len(all_reviews)} Reviews")
        print(f" - {len(all_categories)} Categorias")
        print(f" - {len(aux_resembling)} Produtos Similares")
        print(f" - {len(all_product_category)} Categorias de Produtos")
        print(f" - {len(all_customers)} Clientes")

        print(" Removendo similaridade de produtos não presentes no arquivo .. Tempo estimado: 0:00:23")
        start = time.time()
        remove_invalid_asins=0
        for ii, similar in enumerate(aux_resembling):   
            try:
                all_products_disc[similar[1]]
                all_resembling.append(similar)
            except KeyError:
                remove_invalid_asins+=1
            progress_bar(ii + 1, len(aux_resembling), prefix = 'Progresso:', suffix = f"Completo  (lidos:{ii+1}, removidos: {remove_invalid_asins})", length = 40)
       
        done = time.time()
        print(f" - {len(all_resembling)} Similaridades restantes")
        print(f" - {remove_invalid_asins} Similaridades inválidas removidas em: {str(datetime.timedelta(seconds=math.floor(done - start)))}")
        
        all_products_disc = {}
        aux_resembling = []
        gc.collect()
        
    

if __name__ == '__main__':
    os.system("service postgresql start")
    print("\n (OBS: Olá caro usuário! É possivel acessar o banco de dados deste container direto da sua máquina.)\n")
    print(" Os dados para o acesso:\n  host: localhost\n  port: 5432\n  user: root \n  password: password\n  database: root \n")
    start = time.time()
    read_file()
    drop_tables()
    create_tables()
    populate_tables()
    done = time.time()
    print(f" Tudo Pronto para testar o tp1_3.3! Tempo total: {str(datetime.timedelta(seconds=math.floor(done - start)))}")
