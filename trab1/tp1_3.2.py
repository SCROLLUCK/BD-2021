import psycopg2
import os
products = []
l = 0

class Product:

    def __init__(self, id, asin, title, group, salesrank, similar, categories, reviews, discontinued):
        self.id = int(id)
        self.discontinued = discontinued
        self.asin = asin
        self.title = title
        self.group = group
        self.salesrank = salesrank
        self.similar = similar
        self.categories = categories
        self.reviews = reviews

    def show_product(self):
        
        print(f"id:{self.id}-")
        print(f"ASIN:{self.asin}-")
        if(not self.discontinued):
            print(f"title:{self.title}-")
            print(f"salesrank:{self.salesrank}-")
            print(f"similar:{self.similar}-")
            print(f"categories:{self.categories}-")
            print(f"reviews:{self.reviews}-")
        else :
            print("Product discontinued")

    def get_id(self): return self.id
    def get_asin(self): return self.asin
    def get_title(self): return self.title
    def get_salesrank(self): return self.salesrank
    def get_similar(self): return self.similar
    def get_categories(self): return self.categories
    def get_reviews(self): return self.reviews

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
    print(" - Destruindo tabelas ..")
    conn = connect()
    if conn is not None:
        commands = (
            """
            DROP TABLE IF EXISTS products;
            """,
        )
        cur = conn.cursor()
        
        for command in commands:
            cur.execute(command)

        cur.close()
        conn.commit()
        conn.close()
        print("  - Bando de dados Limpo!")

def create_tables():
    conn = connect()
    print(" - Criando tabelas ..")
    if conn is not None:
        commands = (
            """
            CREATE TABLE products (
            id INTEGER PRIMARY KEY,
            asin VARCHAR(10) UNIQUE NOT NULL,
            title VARCHAR (500)
            )
            """,
        )
        cur = conn.cursor()
        
        for command in commands:
            cur.execute(command)

        cur.close()
        conn.commit()
        conn.close()
        print("  - Criação de tabelas finalzada!")

def printProgressBar (iteration, total, prefix = '', suffix = '', decimals = 1, length = 100, fill = '█', printEnd = "\r"):
    percent = ("{0:." + str(decimals) + "f}").format(100 * (iteration / float(total)))
    filledLength = int(length * iteration // total)
    bar = fill * filledLength + '-' * (length - filledLength)
    print(f'\r   {prefix} |{bar}| {percent}% {suffix}', end = printEnd)
    if iteration == total: 
        print()

def populate_tables():
    conn = connect()
    print(" - Populando tabelas ..")
    if conn is not None:
        for i, product in enumerate(products):
            try:
                query = """INSERT INTO products (id,asin,title) VALUES (%s,%s,%s)"""
                cur = conn.cursor()
                cur.execute(query,(product.get_id(),product.get_asin(),product.get_title()))
                conn.commit()
                cur.close()
            except (Exception, psycopg2.DatabaseError) as error:
                print(error)
                return
            printProgressBar(i + 1, l, prefix = 'Progresso:', suffix = 'Completo', length = 50)
        conn.close()    
    

def read_file():
    print("Verificando arquivo de entrada ..")
    with open('amazon-meta.txt') as f:
        print(" - Arquivo encontrado! Iniciando leitura ..")
        #pula cabeçalho do arquivo
        f.seek(135,0)

        contents = f.read()
        contents = contents.split("\nId")
        for i, product in enumerate(contents):
            try:
                id = product[product.index(":   ")+4:product.index("ASIN:")].strip("\n ")
                asin = product[product.index("ASIN: ")+6:product.index("title:")].strip("\n ")
                title = product[product.index("title: ")+7:product.index("group:")].strip("\n ")
                group = product[product.index("group: ")+7:product.index("salesrank:")].strip("\n ")
                salesrank = product[product.index("salesrank: ")+11:product.index("similar:")].strip("\n ")
                similar = product[product.index("similar: ")+9:product.index("categories:")].strip("\n ").split("  ")
                categories = product[product.index("categories: ")+12:product.index("reviews:")].strip("\n ").split("|")
                reviews = product[product.index("reviews: ")+9:].split("\n")

                new_product = Product(id,asin,title,group,salesrank,similar,categories,reviews,False)
                products.append(new_product)

            except(Exception, ValueError) as error:
                discontinued_id = product[product.index(":   ")+4:product.index("ASIN:")].strip("\n ")
                discontinued_asin = product[product.index("ASIN: ")+6:product.index("\n ")]
                new_discontinued_product = Product(discontinued_id,discontinued_asin,"","","","","","",True)
                products.append(new_discontinued_product)

            printProgressBar(i + 1, l, prefix = 'Progresso:', suffix = 'Completo', length = 50)
        
        print(" - Leitura do arquivo finalizada!")

if __name__ == '__main__':
    os.system("service postgresql start")
    
    items = list(range(0, 548551))
    l = len(items)
    read_file()
   
    drop_tables()
    create_tables()
    
    items = list(range(0, len(products)))
    l = len(items)
    populate_tables()
