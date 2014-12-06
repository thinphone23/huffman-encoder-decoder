//============================================================================
// Name        : hfmEncoderDecoder.cpp
// Author      : Ruan Sijie
// Version     :
// Copyright   :
// Description : A Huffman encoder and decoder system.
//============================================================================

#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <string.h>
#include <iomanip>
#define MAX_INT  32767
using namespace std;
typedef int Status;
typedef struct {
	int num;
	char ch;
	int weight;
	int parent, lchild, rchild;
}HTNode, * HuffmanTree;          // 动态分配数组存储赫夫曼树
typedef char * * HuffmanCode;    // 动态分配数组存储赫夫曼编码
int numb = 0;

void Select(HuffmanTree HT, int pos, int &s1, int &s2) {
	// 选择最小、次小权值对应的下标。权值小的位于左侧。
	int j, m1, m2;
	m1 = m2 = MAX_INT;
	for(j = 1; j <= pos; j++)
	{
		if(HT[j].weight < m1 && HT[j].parent == 0) {
			m2 = m1;
			s2 = s1;
			s1 = j;
			m1 = HT[j].weight;
		}
		else if(HT[j].weight < m2 && HT[j].parent == 0) {
			m2 = HT[j].weight;
			s2 = j;
		}

	}
}

HuffmanTree ImportHuff(int &n) {
	//将哈夫曼树从文件hfmTree.txt读入内存。
	HuffmanTree HT = NULL;
	int i, m = 0;
	char ch;
	FILE * ImportHuff = fopen("hfmTree.txt", "r");
	while((ch = fgetc(ImportHuff)) != EOF) {
		if(ch == '\n')
			m++;
	}
	n = (m + 1) / 2;
	fclose(ImportHuff);
	HT = (HuffmanTree)malloc((m + 1) * sizeof(HTNode));   // 0号单元未使用
	ImportHuff = fopen("hfmTree.txt", "r");
	for(i = 1; i <= m; i++)
		fscanf(ImportHuff, "%d %c %d %d %d %d",
				&HT[i].num, &HT[i].ch, &HT[i].weight, &HT[i].parent, &HT[i].lchild, &HT[i].rchild);
	fclose(ImportHuff);
	return HT;
}

void Initialization(HuffmanTree &HT, int n) {
	// c存放n个字符，w存放n个字符的权值（均>0），构造赫夫曼树HT。
	int * w, i, m = 2 * n - 1, s1 = 1, s2 = 1;
	char * c;
	HuffmanTree p;

	w = (int *)malloc((n + 1) * sizeof(int));
	c = (char *)malloc((n + 1) * sizeof(char));
	cout << "请输入n个字符c（空格用~表示）" << endl;
	for(i = 1; i <= n; i++)
		cin >> c[i];
	cout << "请输入n个字符对应的权值w" << endl;
	for(i = 1; i <= n; i++)
		cin >> w[i];
	if(n <= 1)
		return;
	for(p = &HT[1], i = 1; i <= n; i++, p++) {
		p->num = i;
		p->ch = c[i];
		p->weight = w[i];
		p->parent = 0;
		p->lchild = 0;
		p->rchild = 0;
	}
	for(; i <= m; i++, p++) {
		p->num = i;
		p->ch = '*';
		p->weight = 0;
		p->parent = 0;
		p->lchild = 0;
		p->rchild = 0;
	}
	for(i = n + 1; i <= m; i++) {     // 建赫夫曼树
		// 在HT[1...i - 1]选择parent为0且weight最小的两个节点，其序号分别为s1和s2。
		Select(HT, i - 1, s1, s2);
		HT[s1].parent = i; HT[s2].parent = i;
		HT[i].lchild = s1; HT[i].rchild = s2;
		HT[i].weight = HT[s1].weight + HT[s2].weight;
	}
	ofstream SavehfmTree("hfmTree.txt");
	for(i = 1; i <= m ; i++)
		SavehfmTree << HT[i].num << "  "<< HT[i].ch << "  " << HT[i].weight <<"  "<< HT[i].parent <<"  "<< HT[i].lchild <<"  "<< HT[i].rchild << endl;
	SavehfmTree.close();
	return;
}

void Encoding(HuffmanTree &HT, int n) {
	//利用已建好的哈夫曼树，对文件ToBeTran中的正文进行编码，然后将结果存入文件CodeFile中。
	int i, k, start, c ,f;
	string s;
	HuffmanCode HC = (HuffmanCode)malloc((n + 1) * sizeof (char *));
	char * cd =(char *)malloc(n * sizeof(char));
	cd[n - 1] = '\0';
	for(i = 1; i <= n; i++) {
		start = n - 1;
		for(c = i, f = HT[i].parent; f!= 0; c =f, f = HT[f].parent) {
			if(HT[f].lchild == c)
				cd[--start] = '0';
			else cd[--start] = '1';
		}
		HC[i] = (char *)malloc((n - start) * sizeof(char));
		strcpy(HC[i], &cd[start]);
	}
	free(cd);
	ifstream Codingtext("ToBeTran.txt", ios::in);
	getline(Codingtext, s);
	char * str = &s[0];
	ofstream SaveCode("CodeFile.txt");
	for(k = 0; str[k] != '\0'; k++) {
		i = 1;
		if(str[k] == ' ')
			str[k] = '~';                    //处理空格
		while(HT[i].ch != str[k]) i++;
		SaveCode << HC[i];
	}
	SaveCode.close();
}

void Decoding(HuffmanTree HT, int n){
	//利用已建好的哈夫曼树将文件CodeFile中的代码进行译码，结果存入文件TextFile中。
	int p,i = 1;
	char code[1000],c;
	p = 2 * n - 1;
	FILE *CodeFile,*TextFile;
	if(( CodeFile = fopen("CodeFile.txt","r")) == NULL ){ //打开文件
		printf("Open file fail......\n");
		exit(0);
	}
	if(( TextFile = fopen("TextFile.txt","w")) == NULL ){ //打开文件
		printf("Open file fail......\n");
		exit(0);
	}
	c = fgetc(CodeFile);
	while(c != EOF){
		code[i] = c;
		i++;
		c = fgetc(CodeFile);
	}
	code[i] = '\0';                       //从文件读取字符，存储在code[]数组中
	i = 1;
	while ( code[i] != '\0' && p != 0 ){  //对数组code[]中的赫夫曼编码进行译码
		if ( code[i] == '0' )
			p=HT[p].lchild;               //进入左分支
		else
			p = HT[p].rchild;             //进入右分支
		if (!HT[p].lchild && !HT[p].rchild) {  //进入叶子结点
			if(HT[p].ch == '~')
				fputc(' ', TextFile);           //处理空格
			else
				fputc(HT[p].ch, TextFile);    //将相应的字符写入到文件中
			p = 2 * n - 1;                //重新从树根出发进行译码
		}
		i++;
	}
	fclose(CodeFile);
	fclose(TextFile);
}

void Print(void) {
	//将文件CodeFile以紧凑格式显示在终端上，每行50个代码。同时将此字符形式的编码写入文件CodePrin中。
	string s;
	int i;
	ifstream CodeFile("CodeFile.txt", ios::in);
	ofstream CodePrin("CodePrin.txt", ios::out);
	getline(CodeFile, s);
	char * str = &s[0];
	for(i = 0; str[i] != '\0'; i++) {
		cout << str[i];
		CodePrin << str[i];
		if((i + 1) % 50 == 0) {
			cout << endl;
			CodePrin << endl;
		}
	}
	cout << endl;
	CodeFile.close();
	CodePrin.close();
}

void TreePrinting(HuffmanTree start, HuffmanTree HT) {
	//将已在内存中的哈夫曼树以直观的方式（树）显示在终端上，同时将此字符形式的哈夫曼树写在文件TreePrint中。
	char t = ' ';
	if(start != HT) {
		ofstream TreePrint ("TreePrin.txt", ios::app);
        numb++;                    //全局变量
        TreePrinting(HT+start->rchild, HT);
        if(start->lchild != 0 && start->rchild != 0)
             t='<';
        cout << setw(5*numb) << start -> weight<< t << endl;
        TreePrint << setw(5*numb) << start -> weight<< t << endl;
        TreePrinting(HT+start->lchild, HT);
        numb--;
        TreePrint.close();
	}
}

char printMenu(){
	char option;
	cout << "I:初始化 E:编码 D:译码 P:印代码文件 T:印哈夫曼树 Q:退出"<<endl;
	cin >> option;
	return option;
}

int main(void) {
	char option = '\0';
	int n = 0, m = 0;
	HuffmanTree HT = NULL;
	while(option != 'Q') {
		option = printMenu();
		switch(option) {
			case 'I':
				cout << "请输入字符集的大小n" << endl;
				cin >> n;
				m = 2 * n - 1;
				HT = (HuffmanTree)malloc((m + 1) * sizeof(HTNode));   // 0号单元未使用
				Initialization(HT, n);
				break;
			case 'E':
				if(HT == NULL)
					HT = ImportHuff(n);
				Encoding(HT, n);
				break;
			case 'D':
				if(HT == NULL)
					HT = ImportHuff(n);
				Decoding(HT, n);
				break;
			case 'P': Print();break;
			case 'T': TreePrinting(HT + 2 * n - 1, HT);break;
		}
	}
	free(HT);
	return 0;
}


