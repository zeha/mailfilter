#include "StdAfx.h"
#include "mf_c.h"

//: m_StorageRoot(NULL)
CMF_C::CMF_C(void)
{
	m_StorageRoot.child = NULL;
}

CMF_C::~CMF_C(void)
{
}

MF_C_Storage* CMF_C::AddHeader(char* name)
{	return this->AddValue(name,NULL,None);	}
MF_C_Storage* CMF_C::AddValueInt(char* name, int data)
{	return this->AddValue(name,(void*)data,Integer);	}
MF_C_Storage* CMF_C::AddValueBool(char* name, bool data)
{	return this->AddValue(name,(void*)data,Bool);	}
MF_C_Storage* CMF_C::AddValueChar(char* name, char* data)
{	return this->AddValue(name,data,String);	}

// Add a value to the storage
MF_C_Storage* CMF_C::AddValue(char* name, void* data, MF_C_DataType type)
{
	MF_C_Storage* storage;

	storage = (MF_C_Storage*)malloc(sizeof(MF_C_Storage));
	storage->name = name;
	storage->data = data;
	storage->type = type;
	storage->prev = NULL;
	storage->next = NULL;

	if (m_StorageRoot.child == NULL)
	{
		storage->id = 0;
		storage->displayid = 0;
		// abspeichern
		m_StorageRoot.child = storage;
	} else {
		storage->prev = this->GetLast(m_StorageRoot.child);
		storage->id = storage->prev->id + 1;
		storage->displayid = storage->id;
		// abspeichern
		storage->prev->next = storage;
	}

	return NULL;
}

MF_C_Storage* CMF_C::GetFirst()
{
	return m_StorageRoot.child;
}

MF_C_Storage* CMF_C::GetLast(MF_C_Storage* storage)
{
	MF_C_Storage* myStorage = storage;
	while(myStorage->next != NULL)
	{
//		if (myStorage->next != NULL)
			myStorage = myStorage->next;
//		else
//			break;
	}
	return myStorage;
}

// Get next storage entry
MF_C_Storage* CMF_C::GetNext(MF_C_Storage* storage)
{
	return storage->next;
}

MF_C_Storage* CMF_C::GetNextNum(MF_C_Storage* storage, int num)
{
	MF_C_Storage* myStorage = storage;
	int i = 0;
	for (i = 0; i<=num; i++)
	{
		if (myStorage->next != NULL)
			myStorage = myStorage->next;
		else
			break;
	}
	return myStorage;
}

// Get previous storage entry
MF_C_Storage* CMF_C::GetPrev(MF_C_Storage* storage)
{
	return storage->prev;
}

MF_C_Storage* CMF_C::GetPrevNum(MF_C_Storage* storage, int num)
{
	MF_C_Storage* myStorage = storage;
	int i = 0;
	for (i = num; i>0; i--)
	{
		if (myStorage->prev != NULL)
			myStorage = myStorage->prev;
		else
			break;
	}
	return myStorage;
}
