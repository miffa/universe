#include"wordcount.h"

WordCount::WordCount(string name):dirname(name)
{
}

int WordCount::FileWordCount(fstream &infile,map<string,unsigned int> &word_table)
{
    char ch;
    string word;
    bool flag = false;
    
    while(infile.get(ch)&&!infile.eof())
    {
	   if(isalpha(ch))
       {
           ch = tolower(ch);
           if(!flag)
               flag = true;
           
		   word.append(1,ch);
       }
       else
       {
           if(flag)
           {
               flag == false;
               
               if(word.size() == 0)
               {
                   word.clear();
                   continue;
               }
               
			   map<string,unsigned int>::iterator iter = word_table.find(word);
               if(iter == word_table.end())
                   word_table.insert(pair<string,unsigned int>(word,1));
               else
                   ++iter->second;
                   
               word.clear();
           }
       }
    }

    infile.close();
    
    return 0;
}

int WordCount::DirWordCount(map<string,unsigned int> &word_table)
{
    DIR *dp;
    struct dirent* dirp;
    const char* filename = NULL;
    int n;
    fstream infile;
    char buf[BUFFSIZE],cwd[1024];
    memset(buf,0,sizeof(char)*BUFFSIZE);
    memset(cwd,0,sizeof(char)*1024);
    
    if(getcwd(cwd,1024) == NULL)
    {
        cerr << "get current work directory failed" << endl;
        return -1;
    }

    if((dp = opendir(dirname.c_str())) == NULL)
    {
        cerr << "can't open dir " << dirname << endl; 
        return -1;
    }

    if(chdir(dirname.c_str()) == -1)
    {
        cerr << "chdir error " << endl;
        return -1; 
    }
    
    while((dirp = readdir(dp)) != NULL)
    {
        filename = dirp->d_name;
        if(strcmp(filename,".") == 0 || strcmp(filename,"..") == 0)
        {
            continue;
        }
        
		infile.open(filename,fstream::in);
        if(infile.fail())
        {
            cerr << "open file " << filename << " erorr!"
                 << endl;
            return -1;
        }

       FileWordCount(infile,word_table);
    }
	
    chdir(cwd);
    closedir(dp);
    
    return 0;
}
