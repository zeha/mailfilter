#pragma once

#ifndef _IXLIST_H_
#define _IXLIST_H_

enum iXList_DataType
{
    None = 0,
    Integer = 1,
    String = 2,
	Bool = 3
};

struct iXList_Storage
{
	int id;
	int displayid;
	/*const*/ char* name;
	void* data;
	iXList_DataType type;
	iXList_Storage* prev;
	iXList_Storage* next;
};
struct iXList_StorageRoot
{
	iXList_Storage* child;
};

class iXList
{
public:
	iXList(void);
	~iXList(void);

	// Add a value to the storage
	iXList_Storage* AddHeader(const char* name);
	iXList_Storage* AddValueChar(const char* name, const char* data);
	iXList_Storage* AddValueInt(const char* name, int data);
	iXList_Storage* AddValueBool(const char* name, bool data);
	iXList_Storage* AddValue(const char* name, void* data, iXList_DataType type);
	iXList_Storage* GetFirst();
	iXList_Storage* GetLast();
	// Get next storage entry
	iXList_Storage* GetNext(iXList_Storage* storage);
	iXList_Storage* GetNextNum(iXList_Storage* storage, int num);
	// Get previous storage entry
	iXList_Storage* GetPrev(iXList_Storage* storage);
	iXList_Storage* GetPrevNum(iXList_Storage* storage, int num);
private:
	iXList_StorageRoot* m_StorageRoot;
};

#endif _IXLIST_H_
