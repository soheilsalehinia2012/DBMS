#include <iostream>
#include<stdio.h>
#include <stdlib.h>
using namespace std;
#include "Student.h"

Student* t;

void pause()
{
		cout<<"\n\n\n\t\t\t!!";
		system("pause");
}

void sort()
{
	t->sort();
	pause();
}

void del()
{
	if(!t->deleteRec())
	{
		cout<<"Succeeded."<<endl;
	}else
	{	
		cout<<"Failed"<<endl;
	}
	pause();
}

void add()
{
	cin>>t->ID>>t->name>>t->lastName>>t->grade;
	int res=t->add();
	if(!res)
	{
		cout<<"Succeeded."<<endl;
	}else
	{	
		cout<<"Failed"<<endl;
	}
	pause();
}


void insert()
{
	cin>>t->ID>>t->name>>t->lastName>>t->grade;
	int RRN;
	cout<<"Enter RRN Number:";
	cin>>RRN;
	int res=t->insert(RRN);	
	if(!res)
	{
		cout<<"Succeeded."<<endl;
	}else
	{	
		cout<<"Failed"<<endl;
	}
	pause();
}

void search()
{
	int key;
	cout<<"Enter the key Number:";
	cin>>key;
	int res=t->search(&key);
	if(res)
	{
		cout<<"Succeeded."<<endl;
	}else
	{	
	cout<<"Failed."<<endl;}
	pause();
}

void rationalDelete(){
	if (!t->deleteRationalRec())
	{
		cout << "Succeeded." << endl;
	}
	else
	{
		cout << "Failed" << endl;
	}
	pause();
}

bool find()
{
	int RRN;
	cout<<"Enter RRN Number:";
	cin>>RRN;
	t->find(RRN);
	bool res;
	if(!t->finout->fail())
	{
		cout<<"Succeeded."<<endl;
		res=true;
	}else
	{	t->finout->clear();
	cout<<"Failed"<<endl;
	res=false;}
	pause();
	return res;
}

bool write()
{
	cin>>t->ID>>t->name>>t->lastName>>t->grade;
	t->write();
	return true;
}

bool append()
{
	cin >> t->ID >> t->name >> t->lastName >> t->grade;
	t->append();
	return true;
}

int read()
{
	int cur=t->finout->tellg();
	t->finout->seekg(0,ios::end);
	int end=t->finout->tellg();
	t->finout->seekg(cur);
	if(cur==end)
	{
		cout<<"End of the file."<<endl;
		return 1;
	}
////////

	t->read();
	if (t->ID == -1){
		return 2;
	}
	cout<<"ID: "<<t->ID<<"\tName:"<<t->name<<"\tLastName:"<<t->lastName<<"\tGrade:"<<t->grade<<endl;
	return 0;
}

int main()
{
	t=new Student();
	t->finout->seekg(0, ios::end);
	int end = t->finout->tellg();
	if (end == 0){
		t->addHeader();
	}
	else
	{
		t->ReadHeader();
	}
	do
	{
		if(t->finout->fail())
			return 4;
		system("cls");
		cout<<"Pointers:"<<t->finout->tellg()<<"\t"<<t->finout->tellp()<<endl;
		cout<<"1-\tWrite a student.\n2-\tRead a student\n3-\tReset\n4-\tFind\n5-\tSearch\n6-\tInsert\n";
		cout<<"7-\tAdd\n8-\tDelete\n9-\tSort\n10-\tSequential Read\n11-\tAppend\n20-\tExit"<<endl;
		int x=0;
		cin>>x;
		switch(x)
		{
		case 1:
			if(!write())
			{
				cout<<"Error"<<endl;
				return 1;
			}
			break;
		case 2:
			if(t->finout->eof())
			{
				cout<<"End of File"<<endl;
			}
			else if(read() == 1)
			{
				cout<<"Error"<<endl;
			}
			pause();
			break;
		case 3:
			t->finout->clear();
			t->finout->seekg(t->InitAddress);
			break;
		case 4:
			find();
			break;
		case 5:
			search();
			break;
		case 6:
			insert();
			break;
		case 7:
			add();
			break;
		case 8:
			rationalDelete();
			break;
		case 9:
			sort();
			break;
		case 10:
			t->finout->seekg(t->InitAddress);
			while (read() != 1);
			pause();
			break;
		case 11:
			append();
			break;
		case 20:
			return 0;
			break;
		}
	}while(true);
	t->finout->close();
	return 0;
}