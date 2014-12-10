#include "records.h"

class Student :public FixSize
{
	FixField recordManager;
public:
	Student(string FileName = "test.bin") :FixSize(60, FileName)
	{
	}

	int pkey;


	virtual void* getKey()
	{
		int* key = new int;
		*key = pkey;
		return key;

	}


	virtual int cmp(const void* key)
	{
		int k = *(int*)key;
		return pkey - k;
	}

	bool unPack()
	{
		int index = 0;
		index += recordManager.detach(buffer, (char*)&pkey, sizeof(pkey));
		return true;
	}

	int pack()
	{
		int recordSize = 0;
		link = InitAddress;
		recordSize += recordManager.attach(buffer, (char*)&ID, sizeof(ID), sizeof(ID));
		recordSize += recordManager.attach(buffer + recordSize, name, strlen(name), sizeof(name)-1);
		recordSize += recordManager.attach(buffer + recordSize, lastName, strlen(lastName), sizeof(name)-1);
		recordSize += recordManager.attach(buffer + recordSize, (char*)&grade, sizeof(grade), sizeof(grade));
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
