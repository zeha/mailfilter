#include "ix.h"
#include "iXList.h"
#include <malloc.h>
#include <string.h>
#include <stdlib.h>

//: m_StorageRoot(NULL)
iXList::iXList(void)
{
	m_StorageRoot = new iXList_StorageRoot;
	m_StorageRoot->child = NULL;
}

iXList::~iXList(void)
{
	if (m_StorageRoot == NULL)
		return;
		
	if (m_StorageRoot->child == NULL)
	{	delete(m_StorageRoot);
		return;
		}
		
	iXList_Storage* att = this->GetLast();
	iXList_Storage* attold;
	while (att != NULL)
	{
		attold = att;
		if (att->type == String)
			if (att->data != NULL)
				free(att->data);
		
		if (att->name != NULL)
			free(att->name);
		
		att = this->GetPrev(att);
		delete(attold);
	}
	
	delete(m_StorageRoot);
}

iXList_Storage* iXList::AddHeader(const char* name)
{	return this->AddValue(name,NULL,None);	}
iXList_Storage* iXList::AddValueInt(const char* name, int data)
{	return this->AddValue(name,(void*)data,Integer);	}
iXList_Storage* iXList::AddValueBool(const char* name, bool data)
{	return this->AddValue(name,(void*)data,Bool);	}
iXList_Storage* iXList::AddValueChar(const char* name, const char* data)
{	return this->AddValue(name,strdup(data),String);	}

// Add a value to the storage
iXList_Storage* iXList::AddValue(const char* name, void* data, iXList_DataType type)
{
	iXList_Storage* storage;

	storage = (iXList_Storage*)malloc(sizeof(iXList_Storage));
	storage->name = strdup(name);
	storage->data = data;
	storage->type = type;
	storage->prev = NULL;
	storage->next = NULL;

	if (m_StorageRoot->child == NULL)
	{
		storage->id = 0;
		storage->displayid = 0;
		// abspeichern
		m_StorageRoot->child = storage;
	} else {
		storage->prev = this->GetLast();
		storage->id = storage->prev->id + 1;
		storage->displayid = storage->id;
		// abspeichern
		storage->prev->next = storage;
	}

	return NULL;
}

iXList_Storage* iXList::GetFirst()
{
	if (m_StorageRoot)
		return m_StorageRoot->child;
		else
		return NULL;
}

iXList_Storage* iXList::GetLast()
{
	if (!m_StorageRoot)
		return NULL;
		
	if (m_StorageRoot->child == NULL)
		return NULL;
		
	iXList_Storage* myStorage = m_StorageRoot->child;
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
iXList_Storage* iXList::GetNext(iXList_Storage* storage)
{
	if (storage == NULL)
		return NULL;

	return storage->next;
}

iXList_Storage* iXList::GetNextNum(iXList_Storage* storage, int num)
{
	if (storage == NULL)
		return NULL;

	iXList_Storage* myStorage = storage;
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
iXList_Storage* iXList::GetPrev(iXList_Storage* storage)
{
	if (storage == NULL)
		return NULL;

	return storage->prev;
}

iXList_Storage* iXList::GetPrevNum(iXList_Storage* storage, int num)
{
	if (storage == NULL)
		return NULL;

	iXList_Storage* myStorage = storage;
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
