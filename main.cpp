/*
 * main.cpp
 *
 *  Created on: 2016Äê8ÔÂ18ÈÕ
 *      Author: zhenlin
 */

#include"HuffmanCode.h"
#include<iostream>
#include<fstream>
#include<string>
#include<cstring> // for strcmp
#include<ctime>
#include<iomanip> // for setprecision
using namespace std;

bool DoesFileExist(char *path){
	ifstream is(path);
	if(!is)return false;
	else return true;
}

int main(int argc, char *argv[])
{
	if(argc != 4 || (strcmp(argv[1],"-c") && strcmp(argv[1],"-x"))){
		cout << "\nUsage:\n\n"
				<< "-c for creating a compressed file\n\t"
				<< argv[0] << " -c original_file_path compressed_file_path\n\n"
				<< "-x for extracting a file from the compressed file\n\t"
				<< argv[0] << " -x compressed_file_path decompressed_file_path\n";
		return 0;
	}

	if(strcmp(argv[1],"-c") == 0){
		if(DoesFileExist(argv[2])){
			time_t start, end;
			start = clock();
			HuffmanCode hc;
			hc.Compress(argv[2], argv[3]);
			end = clock();
			cout << "Done! time: " << setprecision(2) << (end - start) / CLOCKS_PER_SEC << " seconds\n";
		}else{
			cout << "Error: " << argv[2] <<" doesn't exist!\n";
		}
	}else if(strcmp(argv[1],"-x") == 0){
		if(DoesFileExist(argv[2])){
			time_t start, end;
			start = clock();
			HuffmanCode hc;
			hc.Decompress(argv[2], argv[3]);
			end = clock();
			cout << "Done! time: " << setprecision(2) << (end - start) / CLOCKS_PER_SEC << " seconds\n";
		}else{
			cout << "Error: " << argv[2] <<" doesn't exist!\n";
		}
	}
	return 0;
}
