#include "records.h"

class Student:public VariableRecord
{
	VarLenField recordManager;
public:
	Student(string FileName="test.bin"):VariableRecord(60,FileName)
	{
	}
	int ID;
	char name[21];
	char lastName[21];
	double grade;
	int link;
	

	virtual void* getKey()
	{
		int* key=new int;
		*key=ID;
		return key;

	}

	virtual int getLink(){
		int l;
		l = link;
		return l;
	}
	
	virtual int cmp(const void* key)
	{
		int k=*(int*)key;
		return ID-k;
	}

	bool unPack()
	{
		int index=0;
		index+=recordManager.detach(buffer,(char*)&ID,sizeof(ID));
		memset(name,0,sizeof(name));
		memset(lastName,0,sizeof(lastName));
		index+=recordManager.detach(buffer+index,name,sizeof(name)-1);
		index+=recordManager.detach(buffer+index,lastName,sizeof(lastName)-1);
		index+=recordManager.detach(buffer+index,(char*)&grade,sizeof(grade));
		index += recordManager.detach(buffer + index, (char*)&link, sizeof(link));
		return true;
	}

	int pack()
	{
		int recordSize=0;
		link = InitAddress;
		recordSize+=recordManager.attach(buffer,(char*)&ID,sizeof(ID),sizeof(ID));
		recordSize+=recordManager.attach(buffer+recordSize,name,strlen(name),sizeof(name)-1);
		recordSize+=recordManager.attach(buffer+recordSize,lastName,strlen(lastName),sizeof(name)-1);
		recordSize+=recordManager.attach(buffer+recordSize,(char*)&grade,sizeof(grade),sizeof(grade));
		recordSize += recordManager.attach(buffer + recordSize, (char*)&link, sizeof(link), sizeof(link));
		return recordSize;
	}

	int deletePack(){
		int recordSize = 0;
		link = headerLink;
		ID = -1;
		recordSize += recordManager.attach(buffer, (char*)&ID, sizeof(ID), sizeof(ID));
		recordSize += recordManager.attach(buffer + recordSize, name, strlen(name), sizeof(name)-1);
		recordSize += recordManager.attach(buffer + recordSize, lastName, strlen(lastName), sizeof(name)-1);
		recordSize += recordManager.attach(buffer + recordSize, (char*)&grade, sizeof(grade), sizeof(grade));
		recordSize += recordManager.attach(buffer + recordSize, (char*)&link, sizeof(link), sizeof(link));
		return recordSize;
	}


	bool headerUnPack(){
		int index = 0;
		index += recordManager.detach(buffer, (char*)&link, sizeof(link));
		headerLink = link;
		return true;
	}

	int headerPack(){
		int recordSize = 0;
		link = headerLink;
		recordSize += recordManager.attach(buffer, (char*)&link, sizeof(link), sizeof(link));
		return recordSize;
	}

	Record* getInstance(string FileName)
	{
		return new Student(FileName);
	}
};
