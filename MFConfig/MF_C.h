#pragma once

enum MF_C_DataType
{
    None = 0,
    Integer = 1,
    String = 2,
	Bool = 3
};

struct MF_C_Storage
{
	int id;
	int displayid;
	char* name;
	void* data;
	MF_C_DataType type;
	MF_C_Storage* prev;
	MF_C_Storage* next;
};
struct MF_C_StorageRoot
{
	MF_C_Storage* child;
};

class CMF_C
{
public:
	CMF_C(void);
	~CMF_C(void);

	// Add a value to the storage
	MF_C_Storage* AddHeader(char* name);
	MF_C_Storage* AddValueChar(char* name, char* data);
	MF_C_Storage* AddValueInt(char* name, int data);
	MF_C_Storage* AddValueBool(char* name, bool data);
	MF_C_Storage* AddValue(char* name, void* data, MF_C_DataType type);
	MF_C_Storage* GetFirst();
	MF_C_Storage* GetLast(MF_C_Storage* storage);
	// Get next storage entry
	MF_C_Storage* GetNext(MF_C_Storage* storage);
	MF_C_Storage* GetNextNum(MF_C_Storage* storage, int num);
	// Get previous storage entry
	MF_C_Storage* GetPrev(MF_C_Storage* storage);
	MF_C_Storage* GetPrevNum(MF_C_Storage* storage, int num);
private:
	MF_C_StorageRoot m_StorageRoot;
};
