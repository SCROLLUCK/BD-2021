import psycopg2
import os
import time
import datetime
import math
import gc

id = 0
asin = ""
title = ""
group = ""
salesrank = 0
similars = ""
discontinued = False

all_products = []
all_reviews = []
all_resembling = []
all_categories = []
all_product_category = []
all_customers = []

inserted_=0

def connect():
    try:
        conn = psycopg2.connect(
            host="localhost",
            database="root",
            user="root",
            password="password")
        return conn
    except (Exception, psycopg2.DatabaseError) as error:
        print(error)

def progress_bar(iteration, total, prefix = '', suffix = '', decimals = 1, length = 100, fill = '█', printEnd = "\r"):
    percent = ("{0:." + str(decimals) + "f}").format(100 * (iteration / float(total)))
    filledLength = int(length * iteration // total)
    bar = fill * filledLength + '-' * (length - filledLength)
    print(f'\r   {prefix} |{bar}| {percent}% {suffix}', end = printEnd)
    if iteration == total: 
        print()

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

def populate_tables():
    
    global all_products
    global all_reviews
    global all_resembling
    global all_categories
    global all_product_category
    global all_customers
    conn = connect()
    
    if conn is not None:

        print("\n Cadastrando Produtos .. Tempo estimado: 0:00:30")
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
            all_products.clear()
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
            all_customers.clear()
            gc.collect()

        print(" Cadastrando Reviews .. Tempo estimado: 0:06:00")
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
            all_reviews.clear()
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
            all_categories.clear()
            gc.collect()

        print(" Cadastrando Categorias dos Produtos .. 0:03:32\n")
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
            all_product_category.clear()
            gc.collect()

        if inserted_ == 274276:
            print(" Continuando leitura dos dados ..")
        conn.close()

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


def read_line_file():

    global inserted_
    global all_resembling
    global id
    global asin
    global title
    global group
    global salesrank
    global similars
    global discontinued

    # discionarios auxiliares para evitar repetições
    all_products_disc = {}
    all_customers_disc = {}
    all_categories_disc = {}
    aux_resembling = []

    conn = connect()
    cur = conn.cursor()

    with open('amazon-meta.txt') as f:

        print(" - Arquivo encontrado!\n Lendo arquivo .. \n")
        start = time.time()
        #pula cabeçalho do arquivo
        f.seek(80,0)
        try:
            for line in f:
                
                if line.__contains__("Id:   "):
                    id = int(line[line.find("Id:   ")+6:line.find("\n")].strip("\n "))    
                elif line.__contains__("ASIN:"):
                    asin = line[line.find("ASIN: ")+6:line.find("\n")].strip("\n ")
                elif line.__contains__("discontinued product"):  
                    discontinued = True
                elif line.__contains__("title:"):
                    title = line[line.find("title: ")+7:line.find("\n")].strip("\n ")
                elif line.__contains__("group:"):
                    group = line[line.find(" group: ")+7:line.find("\n")].strip("\n ")
                elif line.__contains__("salesrank:"):
                    salesrank = int(line[line.find("salesrank: ")+11:line.find("\n")].strip("\n "))
                elif line.__contains__("similar: "):
                    #OBTER SIMILARIDADES DO PRODUTO
                    line = line[line.find("similar: ")+9:line.find("\n ")].split("  ")
                    line.pop(0)
                    for similar in line:
                        aux_resembling.append((asin,similar))

                elif line.__contains__("categories: "):
                    continue
                elif not line.__contains__("title: ") and line.__contains__("|"):
                    #OBTEM CATEGORIAS DO PRODUTO
                    hierarchy_categories=[]  
                    line = line.strip("   |").split("|")   
                    for c, category in enumerate(line):
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
                        if c == len(line)-1:
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
                
                elif line.__contains__("reviews: "):
                    continue
                elif line.__contains__("cutomer: "):
                    #OBTEM REVIEWS DO PRODUTO 
                    date = line[line.find("    ")+4:line.find("  cutomer:")].strip(" ")
                    customer = line[line.find("cutomer: ")+9:line.find("  rating:")].strip(" ")
                    rating = line[line.find("rating: ")+8:line.find("  votes:")].strip(" ")
                    votes = line[line.find("votes: ")+7:line.find("  helpful:")].strip(" ")
                    helpful = line[line.find("helpful: ")+9:].strip(" ")
                    try:
                        all_customers_disc[customer]
                    except KeyError:
                        all_customers_disc[customer] = customer
                        all_customers.append((customer,))
                    all_reviews.append((date,asin,customer,int(rating),int(votes),int(helpful)))
                
                else:
                    inserted_+=1
                    #FINALIZA A LEITURA DO PRODUTO
                    if discontinued:
                        all_products.append((id,asin,None,None,0,True))
                    else:
                        all_products.append((id,asin,title,group,salesrank,False))

                    discontinued = False
                    
                    all_products_disc[asin] = asin
                    now = time.time()
                    progress_bar(inserted_, 548552, prefix = 'Progresso:', suffix = f" {inserted_} de 548552 em {str(datetime.timedelta(seconds=math.floor(now - start)))}", length = 30)

                    if len(all_products) == 274276:
                        if inserted_ == 274276:
                            print("\n\n Inserindo parte dos dados para evitar uso demasiado de memória ..                     \n")
                            print(" - Informações encontradas até o momento:")
                        else: 
                            print("\n Inserindo parte final dos dados ..                                                    \n")
                            print(" - Informações encontradas:")

                        print(f" - {len(all_products)} Produtos")
                        print(f" - {len(all_reviews)} Reviews")
                        print(f" - {len(all_categories)} Categorias")
                        print(f" - {len(aux_resembling)} Produtos Similares")
                        print(f" - {len(all_product_category)} Categorias de Produtos")
                        print(f" - {len(all_customers)} Clientes")
                        populate_tables()

        except (Exception) as error:
            print(error)

        print(" Removendo similaridade de produtos não presentes no arquivo .. Tempo estimado: 0:00:23")
        start = time.time()
        remove_invalid_asins=0
        for ii, similar in enumerate(aux_resembling):   
            try:
                all_products_disc[similar[1]]
                all_resembling.append(similar)
            except KeyError:
                remove_invalid_asins+=1
            progress_bar(ii + 1, len(aux_resembling), prefix = 'Progresso:', suffix = f" (lidos:{ii+1}, removidos: {remove_invalid_asins})", length = 30)
       
        done = time.time()
        print(f" - {len(all_resembling)} Similaridades restantes")
        print(f" - {remove_invalid_asins} Similaridades inválidas removidas em: {str(datetime.timedelta(seconds=math.floor(done - start)))}")

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
        
        all_products_disc = None
        aux_resembling = None
        gc.collect()


if __name__ == '__main__':
    os.system("service postgresql start")
    print("\n (OBS: Olá caro usuário! É possivel acessar o banco de dados deste container direto da sua máquina.)\n")
    print(" Os dados para o acesso:\n  host: localhost\n  port: 5432\n  user: root \n  password: password\n  database: root \n")
    start = time.time()
    drop_tables()
    create_tables()
    read_line_file()
    # populate_tables()
    done = time.time()
    print(f" Tudo Pronto para testar o tp1_3.3! Tempo total: {str(datetime.timedelta(seconds=math.floor(done - start)))}")