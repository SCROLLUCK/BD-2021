
FROM postgres:latest

ENV POSTGRES_PASSWORD=password
ENV POSTGRES_USER=root
ENV POSTGRES_DB=root

RUN apt-get update 
RUN apt-get install -y python3
RUN apt-get update 
RUN apt-get -y install python3-pip
RUN apt-get update 
RUN python3 -m pip install --upgrade pip
RUN apt-get update
RUN pip install psycopg2-binary
RUN apt-get update 

ADD . /

# CMD ["python3","tp1_3.2.py"]

