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
}HTNode, * HuffmanTree;          // ��̬��������洢�շ�����
typedef char * * HuffmanCode;    // ��̬��������洢�շ�������
int numb = 0;

void Select(HuffmanTree HT, int pos, int &s1, int &s2) {
	// ѡ����С����СȨֵ��Ӧ���±ꡣȨֵС��λ����ࡣ
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
	//�������������ļ�hfmTree.txt�����ڴ档
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
	HT = (HuffmanTree)malloc((m + 1) * sizeof(HTNode));   // 0�ŵ�Ԫδʹ��
	ImportHuff = fopen("hfmTree.txt", "r");
	for(i = 1; i <= m; i++)
		fscanf(ImportHuff, "%d %c %d %d %d %d",
				&HT[i].num, &HT[i].ch, &HT[i].weight, &HT[i].parent, &HT[i].lchild, &HT[i].rchild);
	fclose(ImportHuff);
	return HT;
}

void Initialization(HuffmanTree &HT, int n) {
	// c���n���ַ���w���n���ַ���Ȩֵ����>0��������շ�����HT��
	int * w, i, m = 2 * n - 1, s1 = 1, s2 = 1;
	char * c;
	HuffmanTree p;

	w = (int *)malloc((n + 1) * sizeof(int));
	c = (char *)malloc((n + 1) * sizeof(char));
	cout << "������n���ַ�c���ո���~��ʾ��" << endl;
	for(i = 1; i <= n; i++)
		cin >> c[i];
	cout << "������n���ַ���Ӧ��Ȩֵw" << endl;
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
	for(i = n + 1; i <= m; i++) {     // ���շ�����
		// ��HT[1...i - 1]ѡ��parentΪ0��weight��С�������ڵ㣬����ŷֱ�Ϊs1��s2��
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
	//�����ѽ��õĹ������������ļ�ToBeTran�е����Ľ��б��룬Ȼ�󽫽�������ļ�CodeFile�С�
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
			str[k] = '~';                    //����ո�
		while(HT[i].ch != str[k]) i++;
		SaveCode << HC[i];
	}
	SaveCode.close();
}

void Decoding(HuffmanTree HT, int n){
	//�����ѽ��õĹ����������ļ�CodeFile�еĴ���������룬��������ļ�TextFile�С�
	int p,i = 1;
	char code[1000],c;
	p = 2 * n - 1;
	FILE *CodeFile,*TextFile;
	if(( CodeFile = fopen("CodeFile.txt","r")) == NULL ){ //���ļ�
		printf("Open file fail......\n");
		exit(0);
	}
	if(( TextFile = fopen("TextFile.txt","w")) == NULL ){ //���ļ�
		printf("Open file fail......\n");
		exit(0);
	}
	c = fgetc(CodeFile);
	while(c != EOF){
		code[i] = c;
		i++;
		c = fgetc(CodeFile);
	}
	code[i] = '\0';                       //���ļ���ȡ�ַ����洢��code[]������
	i = 1;
	while ( code[i] != '\0' && p != 0 ){  //������code[]�еĺշ��������������
		if ( code[i] == '0' )
			p=HT[p].lchild;               //�������֧
		else
			p = HT[p].rchild;             //�����ҷ�֧
		if (!HT[p].lchild && !HT[p].rchild) {  //����Ҷ�ӽ��
			if(HT[p].ch == '~')
				fputc(' ', TextFile);           //����ո�
			else
				fputc(HT[p].ch, TextFile);    //����Ӧ���ַ�д�뵽�ļ���
			p = 2 * n - 1;                //���´�����������������
		}
		i++;
	}
	fclose(CodeFile);
	fclose(TextFile);
}

void Print(void) {
	//���ļ�CodeFile�Խ��ո�ʽ��ʾ���ն��ϣ�ÿ��50�����롣ͬʱ�����ַ���ʽ�ı���д���ļ�CodePrin�С�
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
	//�������ڴ��еĹ���������ֱ�۵ķ�ʽ��������ʾ���ն��ϣ�ͬʱ�����ַ���ʽ�Ĺ�������д���ļ�TreePrint�С�
	char t = ' ';
	if(start != HT) {
		ofstream TreePrint ("TreePrin.txt", ios::app);
        numb++;                    //ȫ�ֱ���
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
	cout << "I:��ʼ�� E:���� D:���� P:ӡ�����ļ� T:ӡ�������� Q:�˳�"<<endl;
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
				cout << "�������ַ����Ĵ�Сn" << endl;
				cin >> n;
				m = 2 * n - 1;
				HT = (HuffmanTree)malloc((m + 1) * sizeof(HTNode));   // 0�ŵ�Ԫδʹ��
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


