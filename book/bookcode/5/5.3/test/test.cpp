#include <iostream>
#include "../MessageHead.h"
#include "wordcount.h"

using namespace std;

class CLChildWordCountObserver : public CLMessageObserver
{
public:
	CLChildWordCountObserver(string t_name = ""):childname(t_name)
	{  
	}
	
    virtual ~CLChildWordCountObserver()
	{  
	}

	virtual CLStatus Initialize(CLMessageLoopManager *pMessageLoop,void* pContext)
	{
		pMessageLoop->Register(FATHER_INIT_MESSAGE_ID,(CallBackForMessageLoop)(&CLChildWordCountObserver::On_ReadDir));
		pMessageLoop->Register(FATHER_ACK_MESSAGE_ID,(CallBackForMessageLoop)(&CLChildWordCountObserver::On_Father_Ack));
		pMessageLoop->Register(QUIT_MESSAGE_ID,(CallBackForMessageLoop)(&CLChildWordCountObserver::On_Quit));
		
		CLSharedExecutiveCommunicationByNamedPipe *pSender = new CLSharedExecutiveCommunicationByNamedPipe("father_pipe");
		pSender->RegisterSerializer(CHILD_INIT_MESSAGE_ID, new CLChildInitMsgSerializer);
		pSender->RegisterSerializer(CHILD_SEND_REQUEST_MESSAGE_ID, new CLChildSendRequestMsgSerializer);
		pSender->RegisterSerializer(INTERMEDIATE_RESULT_MESSAGE_ID, new CLIntermediateResultMsgSerializer);
        pSender->RegisterSerializer(CHILD_WORK_FINISH_MESSAGE_ID, new CLChildWorkFinishMsgSerializer);
		pSender->RegisterSerializer(QUIT_MESSAGE_ID, new CLQuitMsgSerializer);
		
		CLExecutiveNameServer::GetInstance()->Register("father_pipe", pSender);

        CLChildInitMsg *pChildInitMsg = new CLChildInitMsg;
		pChildInitMsg->childname = childname;
        cout << "Child " << childname << ":: send init msg !!!!!!!!!!!!!!!!! " << endl << endl;
		CLExecutiveNameServer::PostExecutiveMessage("father_pipe",pChildInitMsg);
        
		return CLStatus(0,0);
	}

	CLStatus On_ReadDir(CLMessage *pm)
	{
		CLFatherInitMsg *p = dynamic_cast<CLFatherInitMsg *>(pm);
		if(p == 0)
			return CLStatus(0,0);
        
		cout << "Child "<< childname << ":: receive father init ok !!!!!!!!!!!!!!" << endl << endl;    
		
		string dirname = p->dirname;
		WordCount wc(dirname);
		
		wc.DirWordCount(word_table);
         
		CLChildSendRequestMsg *pChildSendRequestMsg = new CLChildSendRequestMsg;
		pChildSendRequestMsg->childname = childname;  
        cout << "Child " << childname << ":: send requtst meg !!!!!!!!!!!!!!!" << endl << endl;
		CLExecutiveNameServer::PostExecutiveMessage("father_pipe",pChildSendRequestMsg);

		return CLStatus(0,0);
	}
    
	CLStatus On_Father_Ack(CLMessage *pm)
	{
		CLFatherAckMsg *p = dynamic_cast<CLFatherAckMsg *>(pm);
		if(p == 0)
			return CLStatus(0,0);

        cout << "Child " << childname << ":: receive father ack msg !!!!!!!!!!!!!!!!" << endl << endl;
		
		for(iter = word_table.begin(); iter != word_table.end(); iter++)
		{  
		      CLIntermediateResultMsg *pIntermediateResultMsg = new CLIntermediateResultMsg;
			  pIntermediateResultMsg->data.word = iter->first;
			  pIntermediateResultMsg->data.count = iter->second;

	       	  CLExecutiveNameServer::PostExecutiveMessage("father_pipe", pIntermediateResultMsg);
        }

        CLChildWorkFinishMsg *pChildWorkFinishMsg = new CLChildWorkFinishMsg;
		pChildWorkFinishMsg->childname = childname;
        cout << "Child " << childname <<":: send  work finish msg !!!!!!!!!!!!!!!" << endl << endl;
		
		CLExecutiveNameServer::PostExecutiveMessage("father_pipe",pChildWorkFinishMsg);
		
		return CLStatus(0,0);
	}

	CLStatus On_Quit(CLMessage *pm)
	{
		CLQuitMsg *p = dynamic_cast<CLQuitMsg*>(pm);
		if(p == 0)
			return CLStatus(0,0);

        cout << "Child :: receive quit msg !!!!!!!!!!!!!!!!" << endl << endl;
		
		CLExecutiveNameServer::GetInstance()->ReleaseCommunicationPtr("father_pipe");
		
		return CLStatus(QUIT_MESSAGE_LOOP,0);
	}

private:
	string childname;
	map<string,unsigned int> word_table;
	map<string,unsigned int>::iterator iter;
};

int main(int argc,char *argv[])
{
    if(argc != 2)
	{
		cout << "usage:./a.out childname " << endl;
		exit(-1);
	}

	try
	{
		if(!CLLibExecutiveInitializer::Initialize().IsSuccess())
		{
			cout << "Initialize error" << endl;
			return 0;
		}

        CLNonThreadForMsgLoop child_nonthread(new CLChildWordCountObserver(argv[1]),argv[1],EXECUTIVE_BETWEEN_PROCESS_USE_PIPE_QUEUE);
		child_nonthread.RegisterDeserializer(FATHER_INIT_MESSAGE_ID, new CLFatherInitMsgDeserializer);
        child_nonthread.RegisterDeserializer(FATHER_ACK_MESSAGE_ID, new CLFatherAckMsgDeserializer);
		child_nonthread.RegisterDeserializer(QUIT_MESSAGE_ID, new CLQuitMsgDeserializer);

		child_nonthread.Run(0);
        
		throw CLStatus(0, 0);
	}
	catch(CLStatus& s)
	{
		if(!CLLibExecutiveInitializer::Destroy().IsSuccess())
			cout << "Destroy error" << endl;

		return 0;
	}
}
