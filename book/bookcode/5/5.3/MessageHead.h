#ifndef MESSAGE_HEAD
#define MESSAGE_HEAD

#include<string.h>
#include<fstream>
#include<stdlib.h>
#include "LibExecutive.h"

#define CHILD_INIT_MESSAGE_ID 1
#define CHILD_SEND_REQUEST_MESSAGE_ID 2
#define CHILD_WORK_FINISH_MESSAGE_ID 3
#define FATHER_INIT_MESSAGE_ID 4
#define FATHER_ACK_MESSAGE_ID 5
#define INTERMEDIATE_RESULT_MESSAGE_ID 6
#define QUIT_MESSAGE_ID 7

typedef struct 
{
	string word;
    unsigned int count;
}data_type;

class CLChildInitMsg : public CLMessage
{
public:
	CLChildInitMsg() : CLMessage(CHILD_INIT_MESSAGE_ID)
   	{  
   	}

public:
	string childname;
};

class CLChildSendRequestMsg : public CLMessage
{ 
public:
	CLChildSendRequestMsg() : CLMessage(CHILD_SEND_REQUEST_MESSAGE_ID)
   	{  
   	}

public:
	string childname;
};

class CLChildWorkFinishMsg : public CLMessage
{ 
public:
	CLChildWorkFinishMsg() : CLMessage(CHILD_WORK_FINISH_MESSAGE_ID)
   	{  
   	}

public:
	string childname;
};

class CLFatherInitMsg : public CLMessage
{
public:
    CLFatherInitMsg() : CLMessage(FATHER_INIT_MESSAGE_ID)
    { 
   	}
   
public:
    string dirname;
};

class CLFatherAckMsg : public CLMessage
{
public:
	CLFatherAckMsg() : CLMessage(FATHER_ACK_MESSAGE_ID)
	{  
	}
};

class CLIntermediateResultMsg : public CLMessage
{
public:
	CLIntermediateResultMsg() : CLMessage(INTERMEDIATE_RESULT_MESSAGE_ID)
	{
	}

public:	
	data_type data;
};

class CLQuitMsg : public CLMessage
{
public:
	CLQuitMsg() : CLMessage(QUIT_MESSAGE_ID)
    {  
   	}
};

class CLChildInitMsgSerializer : public CLMessageSerializer
{
public:
	virtual char *Serialize(CLMessage *pMsg,unsigned int *pFullLength,unsigned int HeadLength)
	{
		CLChildInitMsg *p = dynamic_cast<CLChildInitMsg *>(pMsg);
		if( p == 0)
		{
			cout << "dynamic_cast error" << endl;
			return 0;
		}

		unsigned int t_len = p->childname.size();
        *pFullLength = HeadLength  + sizeof(long) + sizeof(unsigned int) + t_len;
		char *pBuf = new char[*pFullLength];

		long *pId = (long *)(pBuf + HeadLength);
		*pId = p->m_clMsgID;

		unsigned int *len = (unsigned int *)(pBuf + HeadLength + sizeof(long));
		*len = t_len;
     
        char *childname = (char *)(pBuf + HeadLength + sizeof(long) + sizeof(unsigned int));
		memcpy(childname,p->childname.c_str(),t_len);

		return pBuf;
	}
};

class CLChildSendRequestMsgSerializer : public CLMessageSerializer
{
public:
    virtual char *Serialize(CLMessage *pMsg,unsigned int *pFullLength,unsigned int HeadLength)
    {
        CLChildSendRequestMsg *p = dynamic_cast<CLChildSendRequestMsg *>(pMsg);
        if(p == 0)
        {
            cout << "dynamic_cast error" << endl;
            return 0;
        }
		
		unsigned int t_len = p->childname.size();
        *pFullLength = HeadLength  + sizeof(long) + sizeof(unsigned int) + t_len;
		char *pBuf = new char[*pFullLength];

		long *pId = (long *)(pBuf + HeadLength);
		*pId = p->m_clMsgID;

		unsigned int *len = (unsigned int *)(pBuf + HeadLength + sizeof(long));
		*len = t_len;
     
        char *childname = (char *)(pBuf + HeadLength + sizeof(long) + sizeof(unsigned int));
		memcpy(childname,p->childname.c_str(),t_len);

		return pBuf;
	}
};

class CLChildWorkFinishMsgSerializer : public CLMessageSerializer
{
public:
    virtual char *Serialize(CLMessage *pMsg,unsigned int *pFullLength,unsigned int HeadLength)
    {
        CLChildWorkFinishMsg *p = dynamic_cast<CLChildWorkFinishMsg *>(pMsg);
        if(p == 0)
        {
            cout << "dynamic_cast error" << endl;
            return 0;
        }
		
		unsigned int t_len = p->childname.size();
        *pFullLength = HeadLength  + sizeof(long) + sizeof(unsigned int) + t_len;
		char *pBuf = new char[*pFullLength];

		long *pId = (long *)(pBuf + HeadLength);
		*pId = p->m_clMsgID;

		unsigned int *len = (unsigned int *)(pBuf + HeadLength + sizeof(long));
		*len = t_len;
     
        char *childname = (char *)(pBuf + HeadLength + sizeof(long) + sizeof(unsigned int));
		memcpy(childname,p->childname.c_str(),t_len);

		return pBuf;
	}
};

class CLFatherInitMsgSerializer : public CLMessageSerializer
{
public:
	virtual char *Serialize(CLMessage *pMsg,unsigned int *pFullLength,unsigned int HeadLength)
	{
		CLFatherInitMsg *p = dynamic_cast<CLFatherInitMsg *>(pMsg);
		if(p == 0)
		{
			cout << "dynamic_cast error" <<endl;
			return 0;
		}

		unsigned int dir_len = p->dirname.size();

		*pFullLength = HeadLength + sizeof(long) + sizeof(unsigned int) + dir_len;
		char *pBuf = new char[*pFullLength];

		long *pId = (long *)(pBuf + HeadLength);
		*pId = p->m_clMsgID;
        
	    unsigned int *len = (unsigned int *)(pBuf + HeadLength + sizeof(long));
	    *len = dir_len;
			
        char *dirname = (char *)(pBuf + HeadLength + sizeof(long) + sizeof(unsigned int));
	    memcpy(dirname,p->dirname.c_str(),dir_len);
		
		return pBuf;
	}
};

class CLFatherAckMsgSerializer : public CLMessageSerializer
{
public:
	virtual char *Serialize(CLMessage *pMsg, unsigned int *pFullLength, unsigned int HeadLength)
	{
		CLFatherAckMsg *p = dynamic_cast<CLFatherAckMsg *>(pMsg);
		if(p == 0)
		{
			cout << "dynamic_cast error" << endl;
			return 0;
		}
		
		*pFullLength = HeadLength  + sizeof(long);
		char *pBuf = new char[*pFullLength];

		long *pId = (long *)(pBuf + HeadLength);
		*pId = p->m_clMsgID;

		return pBuf;
	}
};

class CLIntermediateResultMsgSerializer : public CLMessageSerializer
{
public:
	virtual char *Serialize(CLMessage *pMsg, unsigned int *pFullLength, unsigned int HeadLength)
	{
		CLIntermediateResultMsg *p = dynamic_cast<CLIntermediateResultMsg *>(pMsg);
		if(p == 0)
		{
			cout << "dynamic_cast error" << endl;
			return 0;
		}

		unsigned int word_len = p->data.word.size();
		*pFullLength = HeadLength + sizeof(long) + sizeof(unsigned int) + word_len + sizeof(unsigned int);
		char *pBuf = new char[*pFullLength];

		long *pID = (long *)(pBuf + HeadLength);
		*pID = p->m_clMsgID;

        unsigned int *len = (unsigned int *)(pBuf + HeadLength + sizeof(long)); 
		*len = word_len;
		
		char *word = (char *)( pBuf + HeadLength + sizeof(long) + sizeof(unsigned int));
        memcpy(word,p->data.word.c_str(),word_len);

		unsigned int *count = (unsigned int *)(pBuf + HeadLength + sizeof(long) + sizeof(unsigned int) + word_len);
		*count = p->data.count;

		return pBuf;
	}
};

class CLQuitMsgSerializer : public CLMessageSerializer
{  
public:
	virtual char *Serialize(CLMessage *pMsg,unsigned int *pFullLength,unsigned int HeadLength)
	{
		CLQuitMsg *p = dynamic_cast<CLQuitMsg *>(pMsg);
		if( p == 0)
		{
			cout << "dynamic_cast error" << endl;
			return 0;
		}

		*pFullLength = HeadLength  + sizeof(long);
		char *pBuf = new char[*pFullLength];

		long *pId = (long *)(pBuf + HeadLength);
		*pId = p->m_clMsgID;

		return pBuf;
	}
};

class CLChildInitMsgDeserializer : public CLMessageDeserializer
{
public:
	virtual CLMessage *Deserialize(char *pBuffer)
	{
		long id = *((long *)pBuffer);
		if(id != CHILD_INIT_MESSAGE_ID)
			return 0;

		CLChildInitMsg *p = new CLChildInitMsg;
		
		unsigned int t_len = *((unsigned int *)(pBuffer + sizeof(id)));
		char str[t_len + 1];
		memset(str,0,sizeof(char) * (t_len + 1));

		const char* childname = (char *)(pBuffer + sizeof(id) + sizeof(t_len));
		memcpy(str,childname,t_len);
		p->childname = string(str);

		return p;
	}
};

class CLChildSendRequestMsgDeserializer : public CLMessageDeserializer
{
public:
	virtual CLMessage *Deserialize(char *pBuffer)
	{
        long id = *((long *)pBuffer);
		if(id != CHILD_SEND_REQUEST_MESSAGE_ID)
            return 0;

		CLChildSendRequestMsg *p = new CLChildSendRequestMsg;
		
		unsigned int t_len = *((unsigned int *)(pBuffer + sizeof(id)));
		char str[t_len + 1];
		memset(str,0,sizeof(char) * (t_len + 1));

		const char* childname = (char *)(pBuffer + sizeof(id) + sizeof(t_len));
		memcpy(str,childname,t_len);
		p->childname = string(str);
	
        return p;
	}
};

class CLChildWorkFinishMsgDeserializer : public CLMessageDeserializer
{
public:
	virtual CLMessage *Deserialize(char *pBuffer)
	{
        long id = *((long *)pBuffer);
		if(id != CHILD_WORK_FINISH_MESSAGE_ID)
            return 0;

		CLChildWorkFinishMsg *p = new CLChildWorkFinishMsg;
		
		unsigned int t_len = *((unsigned int *)(pBuffer + sizeof(id)));
		char str[t_len + 1];
		memset(str,0,sizeof(char) * (t_len + 1));

		const char* childname = (char *)(pBuffer + sizeof(id) + sizeof(t_len));
		memcpy(str,childname,t_len);
		p->childname = string(str);
	
        return p;
	}
};

class CLFatherInitMsgDeserializer : public CLMessageDeserializer
{
public:
	virtual CLMessage *Deserialize(char *pBuffer)
	{
	   long id = *((long *)pBuffer);
	   if(id != FATHER_INIT_MESSAGE_ID)
		   return 0;

	   CLFatherInitMsg *p = new CLFatherInitMsg;

	   unsigned int t_len = *((unsigned int *)(pBuffer + sizeof(id)));
	   char str[t_len + 1];
       memset(str,0,sizeof(char) * (t_len + 1));

       const char *dirname = (char *)(pBuffer + sizeof(id) + sizeof(t_len));
	   memcpy(str,dirname,t_len);
       p->dirname = string(str);

	   return p;
	}
};

class CLFatherAckMsgDeserializer : public CLMessageDeserializer
{
public:
	virtual CLMessage *Deserialize(char *pBuffer)
	{
		long id = *((long *)pBuffer);
		if(id != FATHER_ACK_MESSAGE_ID)
			return 0;

		CLFatherAckMsg *p = new CLFatherAckMsg;
		return p;
	}
};

class CLIntermediateResultMsgDeserializer : public CLMessageDeserializer
{
public:
	virtual CLMessage *Deserialize(char *pBuffer)
	{
		long id = *((long *)pBuffer);
		if(id != INTERMEDIATE_RESULT_MESSAGE_ID)
			return 0;

		CLIntermediateResultMsg *p = new CLIntermediateResultMsg;
		
		unsigned int t_len = *((unsigned int *)(pBuffer + sizeof( long)));
		char str[t_len + 1];
	    memset(str,0,sizeof(char) * (t_len + 1));

        const char *word = (char *)(pBuffer + sizeof(id) + sizeof(t_len));
		memcpy(str,word,t_len);
		p->data.word = string(str);
		
		p->data.count = *((unsigned int *)(pBuffer + sizeof(id) + sizeof(t_len) + t_len));

		return p;
	}
};

class CLQuitMsgDeserializer : public CLMessageDeserializer
{
public:
	virtual CLMessage *Deserialize(char *pBuffer)
	{
		long id = *((long *)pBuffer);
		if(id != QUIT_MESSAGE_ID)
			return 0;

		CLQuitMsg *p = new CLQuitMsg;
		return p;
	}
};

#endif
