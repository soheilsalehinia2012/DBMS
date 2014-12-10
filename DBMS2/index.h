#include "records.h"

class Index :public FixSize
{
	FixField recordManager;
public:
	Index(string FileName = "index.bin") :FixSize(60,FileName)
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
		recordSize += recordManager.attach(buffer, (char*)&pkey, sizeof(pkey), sizeof(pkey));
		return recordSize;
	}

	int deletePack(){
		return 0;
	}


	bool headerUnPack(){
		return true;
	}

	int headerPack(){
		return 0;
	}
	virtual int getLink(){
		return 0;
	}

	Record* getInstance(string FileName)
	{
		return new Index(FileName);
	}
};
