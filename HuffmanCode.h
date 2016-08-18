/*
 * HuffmanCode.h
 *
 *  Created on: 2016Äê8ÔÂ18ÈÕ
 *      Author: zhenlin
 */

#ifndef HUFFMANCODE_H_
#define HUFFMANCODE_H_

#include<string>
#include<vector>
#include<map>
#include<list>
#include<fstream>
using namespace std;

const int MAX_LEN = 100*1024*1024; // read 10MB every time
const unsigned char MARKS[8] = { 0x80,0x40,0x20,0x10,0x8,0x4,0x2,0x1 }; // x & MARKS[i] to get ith bit


class HuffmanCode {
public:
	HuffmanCode();

	void Compress(string src, string dest);
	void Decompress(string src, string dest);

	virtual ~HuffmanCode();

private:
	void CountLetter(string src);
	void ConstructHuffmanTree();
	void GenerateHuffmanCode();
	void WriteHuffmanCode(ofstream &os);
	void Compressing(string src, string dest);

	void InsertIntoHuffmanTree(char letter, string &code, int &k);
	void ConstructHuffmanTreeFromFile(ifstream &is);
	void Decompressing(ifstream &is, ofstream &os);

	map<char, int> letter_count;
	typedef struct Node {
		int id;
		bool is_leaf;
		char letter;
		int parent, lchild, rchild;
		Node() {
		}
		Node(int i, bool il, char lt, int p, int lc, int rc) :
				id(i), is_leaf(il), letter(lt), parent(p), lchild(lc), rchild(
						rc) {
		}
	};
	multimap<int, Node> count_node;
	vector<Node> huffman_tree;
	map<char, vector<char>> letter_hcode; // hufman code for each letter
};

#endif /* HUFFMANCODE_H_ */
