#include "stdafx.h"

#include <stdio.h>
#include <string>
#include <vector>
#include <string.h>
#include <algorithm>


#include <cstddef>
#include "../MailFilter/Config/MFConfig-Filter.h++"
#include "../MailFilter/Config/MFConfig-defines.h"

extern std::vector<MailFilter_Configuration::Filter> MFC_FilterList;

extern int ExecuteRead(const char* szInFile, int inType);
extern int ExecuteWrite(const char* szOutFile, int outType);

