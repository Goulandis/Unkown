// Fill out your copyright notice in the Description page of Project Settings.
#include "DebugFunLib.h"
#include <string>

DEFINE_LOG_CATEGORY(Debug); 

void UDebugFunLib::DebugLog(FEnumSet::DebugLogType logType, FString context)
{  
	std::string File = __FILE__;
	std::string Func = __func__;
	int Line = __LINE__;
	std::string::size_type iPos = File.find_last_of("\\") + 1;
	std::string FileName = File.substr(iPos, File.length() - iPos);
	FString FFile = FString(FileName.c_str());
	FString FFunc = FString(Func.c_str());
	UE_LOG(Debug, Error, TEXT("FILE:%s"), *FFile);
	UE_LOG(Debug, Error, TEXT("FUNC:%s"), *FFunc);
	UE_LOG(Debug, Error, TEXT("LINE:%d"), Line);
	switch (logType)
	{
	case FEnumSet::Log:
		UE_LOG(Debug, Log, TEXT("[%s:%s:%d]%s"), *FFile, *FFunc, Line, *context);
		break;
	case FEnumSet::Warning:
		UE_LOG(Debug, Warning, TEXT("[%s:%s:%d]%s"), *FFile, *FFunc, Line, *context);
		break;
	case FEnumSet::Error:
		UE_LOG(Debug, Error, TEXT("[%s:%s:%d]%s"), *FFile, *FFunc, Line, *context);
		break;
	}  
}
