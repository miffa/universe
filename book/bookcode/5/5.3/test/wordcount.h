#ifndef WORD_COUNT_H
#define WORD_COUNT_H

#include<stdlib.h>
#include<dirent.h>
#include<iostream>
#include"../MessageHead.h"

using namespace std;

#define BUFFSIZE 4096

class WordCount 
{
public:
	WordCount(string );
	int DirWordCount(map<string,unsigned int> &);

private:
	int FileWordCount(fstream &,map<string,unsigned int> &);

private:
    string dirname;
};

#endif
