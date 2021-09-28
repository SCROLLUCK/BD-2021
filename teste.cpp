#include <iostream>
#include <fstream>
#include <string>
#include "tp2/Encadeada.h"
using namespace std;



int main( ){

	char *buf;
    Elemento* teste = new Elemento(3333,"teste",2021,"lucas|brenda",5555,"2016-07-28 09:36:29","cheirodepneuqueimado");
	string titulo = "", autores="", atualizacao="", snipet ="";
	int id, ano, citacoes, size=0;
	ofstream output ("output.dat", ios::out | ios::binary);

	output.write(reinterpret_cast<char *>(&teste->ID), sizeof(int));
    
    size = teste->Titulo.length();
    output.write(reinterpret_cast<char *>(&size), sizeof(int));
	output.write(teste->Titulo.c_str(), size);
    
    output.write(reinterpret_cast<char *>(&teste->Ano), sizeof(int));
    
    size = teste->Autores.length();
    output.write(reinterpret_cast<char *>(&size), sizeof(int));
    output.write(teste->Autores.c_str(), size);
    
    output.write(reinterpret_cast<char *>(&teste->Citacoes), sizeof(int));
    
    output.write(teste->Atualizacao.c_str(), teste->Atualizacao.length());
    
    size = teste->Snipet.length();
    output.write(reinterpret_cast<char *>(&size), sizeof(int));
    output.write(teste->Snipet.c_str(), size);
	
    output.flush();
	output.close();

	ifstream input("output.dat", ios::in | ios::binary);
	if (!output){
		cout << "can't open file" << endl;
		exit(1);
	}

	input.read(reinterpret_cast<char *>(&id), sizeof(int));
    
    input.read(reinterpret_cast<char *>(&size), sizeof(int));
	buf = new char[size];
	input.read( buf, size);
	titulo.append(buf, size);

    input.read(reinterpret_cast<char *>(&ano), sizeof(int));
    
    input.read(reinterpret_cast<char *>(&size), sizeof(int));
	buf = new char[size];
	input.read( buf, size);
	autores.append(buf, size);

    input.read(reinterpret_cast<char *>(&citacoes), sizeof(int));

	buf = new char[19];
	input.read( buf, 19);
	atualizacao.append(buf, 19);

    input.read(reinterpret_cast<char *>(&size), sizeof(int));
	buf = new char[size];
	input.read( buf, size);
	snipet.append(buf, size);

	cout << id << " " << titulo << " " << ano << " " << autores << " " << citacoes << " " << atualizacao << " " << snipet << endl;

	return 0;
}