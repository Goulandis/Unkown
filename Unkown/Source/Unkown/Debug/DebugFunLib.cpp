// Fill out your copyright notice in the Description page of Project Settings.
#include "DebugFunLib.h"
#include <string>

DEFINE_LOG_CATEGORY(DebugLog); 

std::string CFILE = "";
std::string CFUNC = "";
std::string FILENAME = "";
std::string::size_type IPOS = 0;
int LINE = 0;
FString FFILE = TEXT("");
FString FFUNC = TEXT("");

void UDebugFunLib::PrintDebugLog(FEnumSet::DebugLogType logType, FString context)
{  
	CFILE = __FILE__;
	CFUNC = __func__;
	LINE = __LINE__;
	IPOS = CFILE.find_last_of("\\") + 1;
	FILENAME = CFILE.substr(IPOS, CFILE.length() - IPOS);
	FFILE = FString(FILENAME.c_str());
	FFUNC = FString(CFUNC.c_str());
	switch (logType)
	{
	case FEnumSet::Log:
		UE_LOG(DebugLog, Log, TEXT("[%s:%s:%d]%s"), *FFILE, *FFUNC, LINE, *context);
		break;
	case FEnumSet::Warning:
		UE_LOG(DebugLog, Warning, TEXT("[%s:%s:%d]%s"), *FFILE, *FFUNC, LINE, *context);
		break;
	case FEnumSet::Error:
		UE_LOG(DebugLog, Error, TEXT("[%s:%s:%d]%s"), *FFILE, *FFUNC, LINE, *context);
		break;
	}  
}
