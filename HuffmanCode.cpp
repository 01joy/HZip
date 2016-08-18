/*
 * HuffmanCode.cpp
 *
 *  Created on: 2016Äê8ÔÂ18ÈÕ
 *      Author: zhenlin
 */

#include "HuffmanCode.h"
#include <algorithm>

HuffmanCode::HuffmanCode() {
	// TODO Auto-generated constructor stub

}

HuffmanCode::~HuffmanCode() {
	// TODO Auto-generated destructor stub
}

//  count letter frequency in file src
void HuffmanCode::CountLetter(string src) {
	ifstream is(src, ios::binary);
	char *buf = new char[MAX_LEN];
	while (is.peek() != EOF) {
		is.read(buf, MAX_LEN);
		int len = is.gcount();
		for (int i = 0; i < len; i++)
			letter_count[buf[i]]++;
	}
	is.close();
	delete[] buf;

	map<char, int>::iterator it = letter_count.begin();
	while (it != letter_count.end()) {
		Node nd(-1, true, it->first, -1, -1, -1);
		// count_node is multimap, already sort frequency ascending
		count_node.insert(pair<int, Node>(it->second, nd));
		it++;
	}
}

void HuffmanCode::ConstructHuffmanTree() {
	huffman_tree.resize(letter_count.size() * 2 - 1); // n=2n_0-1
	int k = 0;
	multimap<int, Node>::iterator it1, it2;
	while (count_node.size() > 1) {
		it2 = count_node.begin();
		it1 = it2;
		it2++;
		if ((it1->second).is_leaf) {
			(it1->second).id = k;
			(it1->second).parent = k + 1;
			huffman_tree[k++] = it1->second;
		} else
			huffman_tree[(it1->second).id].parent = k;

		int p = huffman_tree[(it1->second).id].parent; // the parent id
		if ((it2->second).is_leaf) {
			(it2->second).id = p + 1;
			(it2->second).parent = p;
			huffman_tree[p + 1] = it2->second;
			k = p + 2;
		} else {
			huffman_tree[(it2->second).id].parent = p;
			k = p + 1;
		}
		Node pnd(p, false, ' ', -1, (it1->second).id, (it2->second).id);
		huffman_tree[p] = pnd;
		// merge two node with lowest frequencies
		count_node.insert(pair<int, Node>(it1->first + it2->first, pnd));
		count_node.erase(it1);
		count_node.erase(it2);
	}
	it1 = count_node.begin();
	huffman_tree[(it1->second).id].parent = -1; // root of huffman tree
}

void HuffmanCode::GenerateHuffmanCode() {
	for (int i = 0; i < huffman_tree.size(); i++) {
		if (huffman_tree[i].is_leaf) {
			vector<char> inverse_code;
			int j = i, k;
			//get inverse huffman code by backtracing
			while (huffman_tree[j].parent != -1) {
				k = huffman_tree[j].parent;
				if (huffman_tree[k].lchild == j)
					inverse_code.push_back('0'); // 0 for left
				else
					inverse_code.push_back('1'); // 1 for right
				j = k;
			}
			reverse(inverse_code.begin(), inverse_code.end());
			letter_hcode[huffman_tree[i].letter] = inverse_code;
		}
	}
}

// first write huffmancode as meta data of compressed file
void HuffmanCode::WriteHuffmanCode(ofstream &os) {
	map<char, vector<char>>::iterator it = letter_hcode.begin();
	int cnt = letter_hcode.size();
	os.write((const char*) &cnt, sizeof(int)); // number of leaf nodes
	while (it != letter_hcode.end()) {
		os.write(&(it->first), sizeof(char)); // letter
		cnt = (it->second).size();
		os.write((const char*) &cnt, sizeof(int)); //length
		os.write(&((it->second)[0]), (it->second).size() * sizeof(char)); // code
		it++;
	}

	// end flag of huaffman code
	char c = '\n';
	cnt = -1;
	os.write(&c, sizeof(char));
	os.write((const char*) &cnt, sizeof(int));
}

void HuffmanCode::Compressing(string src, string dest) {
	ifstream is(src, ios::binary);
	ofstream os(dest, ios::binary);

	WriteHuffmanCode(os);

	char *is_buf = new char[MAX_LEN], *os_buf = new char[MAX_LEN];
	list<char> tmp_hcode;
	int start_pos = 0, i, j, k, len, t;
	char c, flag = 0x0; // flag for the last byte
	list<char>::iterator it;
	while (is.peek() != EOF) {
		is.read(is_buf, MAX_LEN);
		len = is.gcount();
		for (i = 0; i < len; i++)
			tmp_hcode.insert(tmp_hcode.end(), letter_hcode[is_buf[i]].begin(),
					letter_hcode[is_buf[i]].end());
		k = tmp_hcode.size() / 8;
		t = 0;
		i = 0;
		it = tmp_hcode.begin();
		while (i < 8 * k) {
			c = 0x0;
			for (j = i; j <= i + 7; j++) {
				c = (*it == '1') ? (c | (1 << (i + 7 - j))) : c; // char -> bit
				it++;
			}
			os_buf[t++] = c;
			i += 8;
		}
		os.write(os_buf, t * sizeof(char));
		tmp_hcode.erase(tmp_hcode.begin(), it);
	}
	c = 0x0;
	i = 7;
	bool done = true;
	while (it != tmp_hcode.end()) {
		done = false;
		c = (*it == '1') ? (c | (1 << i)) : c; // left bits
		i--;
		it++;
	}
	if (!done) {
		os.write(&c, sizeof(char));
		c = 7 - i; // only c bits used in the last byte
		os.write(&c, sizeof(char));
		flag = 0x1; // the last byte is incomplete
	}
	os.write(&flag, sizeof(char));
	is.close();
	os.close();
	delete[] is_buf;
	delete[] os_buf;

}

void HuffmanCode::Compress(string src, string dest) {
	CountLetter(src);
	ConstructHuffmanTree();
	GenerateHuffmanCode();
	Compressing(src, dest);
}

void HuffmanCode::InsertIntoHuffmanTree(char letter, string &code, int &k) {
	int parent = 0;
	for (int i = 0; i < code.size(); i++) {
		if (code[i] == '0' && huffman_tree[parent].lchild == -1) // left
				{
			Node nd(k, false, ' ', parent, -1, -1);
			huffman_tree[k] = nd;
			huffman_tree[parent].lchild = k;
			parent = k++;
		} else if (code[i] == '1' && huffman_tree[parent].rchild == -1) // right
				{
			Node nd(k, false, ' ', parent, -1, -1);
			huffman_tree[k] = nd;
			huffman_tree[parent].rchild = k;
			parent = k++;
		} else
			parent =
					(code[i] == '0') ?
							huffman_tree[parent].lchild :
							huffman_tree[parent].rchild;
	}
	huffman_tree[parent].is_leaf = true;
	huffman_tree[parent].letter = letter;
}

void HuffmanCode::ConstructHuffmanTreeFromFile(ifstream &is) {
	char letter;
	int len;
	is.read((char*) &len, sizeof(int)); // first read number of leaf nodes
	huffman_tree.resize(2 * len - 1); // n=2n_0-1
	Node root(0, false, ' ', -1, -1, -1);
	huffman_tree[0] = root;
	int k = 1;
	while (true) {
		is.read(&letter, sizeof(char)); // letter
		is.read((char*) &len, sizeof(int)); //length
		if (letter == '\n' && len == -1) // reach end flag of huaffman code
			break;
		string code(len, '\0'); // char *tmp = new char [len + 1]; tmp[len] = '\0';
		is.read(&code[0], len * sizeof(char));
		InsertIntoHuffmanTree(letter, code, k);
	}
}
void HuffmanCode::Decompressing(ifstream &is, ofstream &os) {
	char *is_buf = new char[MAX_LEN], *os_buf = new char[MAX_LEN];
	list<char> tmp_hcode;
	list<char>::iterator it1, it2;
	int len, i, j, p, t;
	bool last_read = false; // the last batch of data
	char c;
	while (is.peek() != EOF) {
		is.read(is_buf, MAX_LEN);
		len = is.gcount();
		if (len < MAX_LEN)
			last_read = true;
		for (i = 0; i < len; i++) {
			if (last_read && (i == len - 3))
				break;
			c = (unsigned char) is_buf[i];
			for (j = 0; j < 8; j++)
				tmp_hcode.insert(tmp_hcode.end(),
						'0' + ((c & MARKS[j]) >> (7 - j)));
		}
		if (last_read) {
			if (is_buf[len - 1] == 0x1) { // incomplete
				int b = is_buf[len - 2]; // only b bits in (len-2)th byte used
				c = is_buf[len - 3];
				for (j = 0; j < b; j++)
					tmp_hcode.insert(tmp_hcode.end(),
							'0' + ((c & MARKS[j]) >> (7 - j)));
			} else if (is_buf[len - 1] == 0x0) { // complete
				for (i = len - 3; i <= len - 2; i++) {
					c = (unsigned char) is_buf[i];
					for (j = 0; j < 8; j++)
						tmp_hcode.insert(tmp_hcode.end(),
								'0' + ((c & MARKS[j]) >> (7 - j)));
				}
			}
		}
		it1 = tmp_hcode.begin();
		t = 0;
		while (it1 != tmp_hcode.end()) {
			p = 0;
			it2 = it1;
			while (!huffman_tree[p].is_leaf) {
				p = (*it1 == '0') ?
						huffman_tree[p].lchild : huffman_tree[p].rchild;
				it1++;
				if (it1 == tmp_hcode.end())
					break;
			}
			if (huffman_tree[p].is_leaf)
				os_buf[t++] = huffman_tree[p].letter;
			if (t >= MAX_LEN) {
				os.write(os_buf, t * sizeof(char));
				t = 0;
			}
			if (it1 == tmp_hcode.end()) {
				if (huffman_tree[p].is_leaf)
					tmp_hcode.clear();
				else
					tmp_hcode.erase(tmp_hcode.begin(), it2); // caution!
				break;
			}
		}
		os.write(os_buf, t * sizeof(char));
	}
	delete[] is_buf;
	delete[] os_buf;

}
void HuffmanCode::Decompress(string src, string dest) {
	ifstream is(src, ios::binary);
	ofstream os(dest, ios::binary);
	ConstructHuffmanTreeFromFile(is);
	Decompressing(is, os);
	is.close();
	os.close();
}
