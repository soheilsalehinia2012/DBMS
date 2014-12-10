#include <string>
#include <fstream>
using namespace std;

class FileRecord
{
	fstream FINOUT;
public:
	string fileName;
	fstream* finout;
	int InitAddress;
	FileRecord(string fileName)
	{
		this->fileName=fileName;
		InitAddress=0;
		if(fileName[0]){
			finout=&FINOUT;
			finout->open(fileName.c_str(),ios::in | ios::out | ios::binary);
			finout->seekg(0);
		}
	}
	~FileRecord()
	{
		if(fileName[0])
			finout->close();
	}

};



class Record:public FileRecord
{
public:
	int bufferSize;
	char* buffer;
	int recordSize, recordSize2;
	int headerLink;
	// attributes
	virtual int pack()=0;
	virtual bool unPack()=0;
	virtual int pureRead()=0;
	virtual int pureWrite()=0;
	virtual int deletePack() = 0;
	virtual int headerPack() = 0;
	virtual bool headerUnPack() = 0;
	virtual int read()
	{
		pureRead();
		unPack();
		return finout->fail()?1:0;
		
	}
	virtual int write()
	{
		recordSize=pack();
		int res=pureWrite();
		return res;
	}

	virtual bool find(int RRN)
	{
		finout->seekg(InitAddress);
		for(int i=0;i<RRN-1;i++)
		{
			if(pureRead())
				return false;
		}
		return true;
	}

	virtual int cmp(const void* key)=0;
		

	virtual bool search(void* key)
	{
		finout->seekg(InitAddress);int loc=InitAddress;
		while(!read())
		{
			int res=cmp(key);
			if(res<0)
			{
				loc=finout->tellg();
				continue;
			}
			finout->seekg(loc);
			return !res;
		}
		finout->clear();
		finout->seekg(loc);
		return false;
	}


	virtual bool append()
	{
		char* temp = new char[bufferSize];
		int loc = finout->tellg();
		recordSize2 = pack();
		memcpy(temp, buffer, bufferSize);
		finout->seekg(0);
		ReadHeader();
		if (headerLink == 0){
			finout->seekg(InitAddress);
			memcpy(buffer, temp, bufferSize);
			recordSize = recordSize2;
			pureWrite();
			finout->seekg(0, ios::end);
			headerLink = finout->tellg();
			recordSize = headerPack();
			finout->seekg(0);
			pureWrite();
		}
		else{
			finout->seekg(0, ios::end);
			int end = finout->tellg();
			if (end == headerLink){
				memcpy(buffer, temp, bufferSize);
				recordSize = recordSize2;
				pureWrite();
				finout->seekg(0, ios::end);
				headerLink = finout->tellg();
				recordSize = headerPack();
				finout->seekg(0);
				pureWrite();
			}
			else{
				finout->seekg(headerLink);
				loc = finout->tellg();
				read();
				headerLink = getLink();
				finout->seekg(loc);
				recordSize = recordSize2;
				memcpy(buffer, temp, bufferSize);
				appendUpdate();
				recordSize = headerPack();
				finout->seekg(0);
				pureWrite();
			}
		}
		finout->seekg(0, ios::end);
		return 0;
	}

	virtual int getSize()=0;

	void shift(int size)
	{
		int initLoc=finout->tellg();
		finout->seekg(0,ios::end);
		int curLoc=finout->tellg();
		curLoc--;
		char c;
		for(;curLoc>=initLoc;curLoc--)
		{
			finout->seekg(curLoc);
			finout->read(&c,1);
			finout->seekg(size-1,ios::cur);
			finout->write(&c,1);
		}
		finout->seekg(initLoc);
	}

	virtual int insert()
	{
		recordSize=pack();
		int size=getSize();
		shift(size);
		return pureWrite();// If it returns false, the file may be corrupted due to shifting.
	}

	virtual int insert(int RRN)
	{
		bool res=find(RRN);
		if(!res)return 1;  // RRN is not found
		return insert();
	}

	virtual void* getKey()=0;

	virtual int getLink() = 0;

	virtual int update()
	{
		int cur=finout->tellg();
		int res=pureRead();
		finout->seekg(cur);
		if(res)return res;
		int oldSize=recordSize;
		recordSize=pack();
		int newSize=recordSize;
		if(newSize>oldSize)
			shift(newSize-oldSize);
		else
			recordSize=oldSize;
		res=pureWrite();
		if(res)return res+10;
		return 0;
	}

	virtual int deleteUpdate(){
		int cur = finout->tellg();
		int res = pureRead();
		finout->seekg(cur);
		if (res)return res;
		int oldSize = recordSize;
		recordSize = deletePack();
		int newSize = recordSize;
		if (newSize>oldSize)
			shift(newSize - oldSize);
		else
			recordSize = oldSize;
		res = pureWrite();
		if (res)return res + 10;
		return 0;
	}

	virtual int appendUpdate(){
		char* temp = new char[bufferSize];
		memcpy(temp, buffer, bufferSize);
		int cur = finout->tellg();
		int res = pureRead();
		finout->seekg(cur);
		if (res)return res;
		int oldSize = recordSize;
		recordSize = recordSize2;
		int newSize = recordSize;
		if (newSize>oldSize)
			shift(newSize - oldSize);
		else
			recordSize = oldSize;
		memcpy(buffer, temp, bufferSize);
		res = pureWrite();
		if (res)return res + 10;
		return 0;
	}


	virtual int update(int RRN)
	{
		int res=find(RRN);
		if(res)return res;
		res=update();
		return res?10+res:0;
	}


	virtual int add()
	{
		void* key=getKey();
		int r=pack();
		char* temp=new char[bufferSize];
		memcpy(temp,buffer,bufferSize);
		bool res=search(key);
		memcpy(buffer,temp,bufferSize);
		recordSize=r;
		unPack();
		delete key;
		delete[] temp;

		if(res)
			return update()+10;
		return insert();
	}

	int deleteRec()
	{
		int loc=finout->tellg();
		fstream ftemp("temp.bin",ios::out | ios::binary);
		finout->seekg(0);
		for(int i=0;i<loc;i++)
		{
			char c=finout->get();
			ftemp.put(c);
		}
		bool res=finout->fail();
		pureRead();
		res=finout->fail();
		char c=finout->get();
		while(!finout->fail())
		{
				ftemp.put(c);
				c=finout->get();
		}
		finout->clear();
		ftemp.close();
		finout->close();
		remove(fileName.c_str());
		rename("temp.bin",fileName.c_str());
		finout->open(fileName.c_str(),ios::in | ios::out | ios::binary);
		finout->seekg(loc);
		return 0;
	}

	int deleteRationalRec(){
		int loc = finout->tellg();
		ReadHeader();
		finout->seekg(loc);
		recordSize = deletePack();
		deleteUpdate();
		headerLink = loc;
		recordSize = headerPack();
		finout->seekg(0);
		pureWrite();
		return 0;
	}

	int addHeader(){
		finout->seekg(0);
		headerLink = 0;
		recordSize = headerPack();
		pureWrite();
		finout->seekg(0, ios::end);
		InitAddress = finout->tellg();
		return 0;
	}

	int ReadHeader(){
		finout->seekg(0);
		pureRead();
		headerUnPack();
		InitAddress = finout->tellg();
		return 0;
	}

	virtual Record* getInstance(string FileName)=0;

#define MAXN 1000

	int initialReading(int indexes[])
	{
		int nIndexes=0;
		int loc=InitAddress;
		finout->seekg(loc);
		while(!read())
		{
			indexes[nIndexes++]=loc;
			loc=finout->tellg();
		}
		indexes[nIndexes++]=loc;
		finout->clear();
		return nIndexes;
	}

	int readRec(int address){
		finout->seekg(address);
		read();
		return finout->tellg();
	}

	void blockMerge(Record* otherFile,int first,int second,int secondEnd)
	{
		Record* secondRec=this->getInstance("");
		secondRec->finout=finout;
		int firstEnd=second;
		int afterFirst=readRec(first);
		int afterSecond=second==secondEnd?second:secondRec->readRec(second);
		while(first<firstEnd || second<secondEnd){
			bool writeFirst=true;
			if(first==firstEnd)writeFirst=false;
			else if(second<secondEnd){
				void* key=secondRec->getKey();
				int res=cmp(key);
				delete key;
				if(res>0)writeFirst=false;
			}
			if(writeFirst){
				memcpy(otherFile->buffer,buffer,bufferSize);
				otherFile->recordSize=recordSize;
				first=afterFirst;
				if(first<firstEnd)
					afterFirst=readRec(first);
			}else{
				memcpy(otherFile->buffer,secondRec->buffer,bufferSize);
				otherFile->recordSize=secondRec->recordSize;
				second=afterSecond;
				if(second<secondEnd)
					afterSecond=secondRec->readRec(second);
			}
			
			int res=otherFile->pureWrite();
			if(res){
				cout<<"Error in writing in BlockMerge"<<endl;
				throw 1;
			}
		}

		delete secondRec;
	}

	int merge(Record* second,int indexes[],int nIndexes)
	{
		int i;
		second->finout->clear();
		second->finout->seekp(InitAddress);
		for(i=0;i<nIndexes-1;i+=2){
			blockMerge(second,indexes[i],indexes[i+1],indexes[i+2]);
			indexes[i/2]=indexes[i];
		}
		if(i>nIndexes-1)
		{
			cout<<"Error in merge"<<endl;
			throw 1;
		}
		indexes[i/2]=indexes[i];
		return nIndexes/2+1;
	}

	void sort(){
		string tempFileName="tempsort.bin";
		fstream f(tempFileName.c_str(),ios::out);
		f.close();
		Record* ftemp=getInstance(tempFileName);
		if(ftemp->finout->fail())
		{
			cout<<"Temp File could not be opened."<<endl;
			throw 1;
		}
		int nIndexes ,indexes[MAXN];
		nIndexes=initialReading(indexes);
		if(nIndexes<=2){
			delete ftemp;
			remove("tempsort.bin");
			return;
		}
		Record* first=this;
		while(nIndexes>2)
		{
			if(nIndexes%2==0)
				indexes[nIndexes]=indexes[nIndexes-1],nIndexes++;
			nIndexes=first->merge(ftemp,indexes,nIndexes);
			swap(first,ftemp);
		}
		if(first->fileName==fileName)
		{
			delete ftemp;
			remove("tempsort.bin");
		}else
		{
			delete first;
			finout->close();
			remove(fileName.c_str());
			rename(tempFileName.c_str(),fileName.c_str());
			finout->open(fileName.c_str(),ios::in | ios::out | ios::binary);
			finout->seekg(InitAddress);
		}
	}

	Record(int size,string fileName="test.bin"):FileRecord(fileName)
	{
		bufferSize=size;
		buffer=new char[size];
		recordSize=0;
	}

	~Record()
	{
		delete[] buffer;
	}

};

class FixSize:public Record
{
public:
	FixSize(int size,string FileName):Record(size,fileName)
	{
	}

	virtual int getSize()
	{
		return bufferSize;
	}

	virtual int pureRead()
	{
		finout->read(buffer,bufferSize);
		recordSize=bufferSize;
		return finout->fail()?1:0;
	}
	virtual int pureWrite()
	{
		recordSize=bufferSize;
		finout->write(buffer,recordSize);
		return finout->fail()?1:0;
	}

};



class DelimRecord:public Record
{
public:
	char delimiter;
	
	DelimRecord(int maxSize, string FileName, char delim = '|') :Record(maxSize, FileName)
	{
		delimiter=delim;
	}
	virtual int getSize()
	{
		return recordSize+1;
	}
	virtual int pureRead()
	{
		finout->get(buffer,bufferSize,delimiter);
		recordSize=strlen(buffer);
		char c=finout->get();
		if(c!=delimiter && !finout->eof())
			finout->putback(c);
		return finout->fail()?1:0;
	}
	virtual int pureWrite()
	{
		if(recordSize==0)
		{
			cout<<"No Packing"<<endl;
			throw 1;
		}
		finout->write(buffer,recordSize);
		finout->put(delimiter);
		return finout->fail()?1:0;
	}
};

class VariableRecord:public Record
{
public:
	VariableRecord(int maxSize,string FileName):Record(maxSize,FileName)
	{
	}
	virtual int getSize()
	{
		return recordSize+sizeof(recordSize);
	}
	virtual int pureRead()
	{
		finout->read((char*)&recordSize,sizeof(recordSize));
		finout->read(buffer,recordSize);
		return finout->fail()?1:0;
	}
	virtual int pureWrite()
	{
		if(recordSize==0)
		{
			cout<<"No packing"<<endl;
			throw 1;
		}
		bool res=finout->fail();
		finout->write((char*)&recordSize,sizeof(recordSize));
		res=finout->fail();
		finout->write(buffer,recordSize);
		res=finout->fail();
		return res?1:0;
	}
};



class RecordManager
{
public:
	virtual int attach(char* buffer,char* source,int size,int maxSize)=0;
	virtual int detach(char* buffer,char* source,int maxSize)=0;
};

class FixField:public RecordManager
{
public:
	int attach(char* buffer,char* source,int size,int maxSize)
	{
		memcpy(buffer,source,maxSize);
		return maxSize;
	}
	int detach(char* buffer,char* source,int maxSize)
	{
		memcpy(source,buffer,maxSize);
		return maxSize;
	}
};




class DelimField:public RecordManager
{
public:
	char delimiter;
	DelimField(char delim='&')
	{
		delimiter=delim;
	}
	int attach(char* buffer,char* source,int size,int maxSize)
	{
		memcpy(buffer,source,size);
		buffer[size]=delimiter;
		return size+1;
	}
	int detach(char* buffer,char* source,int maxSize)
	{
		int t=0;
		while(t<maxSize && buffer[t]!=delimiter)
		{	source[t]=buffer[t];
			t++;
		}
		if(buffer[t]!=delimiter)
			throw 1;
		return t+1;
	}
};

class VarLenField:public RecordManager
{
public:
	int attach(char* buffer,char* source,int size,int maxSize)
	{
		memcpy(buffer,(char*)&size,sizeof(size));
		buffer+=sizeof(size);
		memcpy(buffer,source,size);
		return size+sizeof(size);
	}
	int detach(char* buffer,char* source,int maxSize)
	{
		int size;
		memcpy((char*)&size,buffer,sizeof(size));
		if(size>maxSize)
			throw 1;
		buffer+=sizeof(size);
		memcpy(source,buffer,size);
		return size+sizeof(size);
	}
};









