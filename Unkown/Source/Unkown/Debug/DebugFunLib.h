// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include <string>
#include "../Assets/EnumSet.h"
#include "Logging/LogMacros.h"
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DebugFunLib.generated.h"

extern std::string CFILE;
extern std::string CFUNC;
extern std::string::size_type IPOS;
extern std::string FILENAME;
extern FString FFILE;
extern FString FFUNC;
extern int LINE;

#ifndef HONG
#define DEBUGLOG(LogType,Context)\
CFILE = __FILE__;\
CFUNC = __func__;\
LINE = __LINE__;\
IPOS = CFILE.find_last_of("\\") + 1;\
FILENAME = CFILE.substr(IPOS, CFILE.length() - IPOS);\
FFILE = FString(FILENAME.c_str());\
FFUNC = FString(CFUNC.c_str());\
switch(LogType)\
{\
	case FEnumSet::Log:\
		UE_LOG(DebugLog, Log, TEXT("[%s : %s : %d] %s"), *FFILE, *FFUNC, LINE, *Context);\
		break;\
	case FEnumSet::Warning:\
		UE_LOG(DebugLog, Warning, TEXT("[%s : %s : %d] %s"), *FFILE, *FFUNC, LINE, *Context);\
		break;\
	case FEnumSet::Error:\
		UE_LOG(DebugLog, Error, TEXT("[%s : %s : %d] %s"), *FFILE, *FFUNC, LINE, *Context);\
		break;\
}
#endif // HONG

DECLARE_LOG_CATEGORY_EXTERN(DebugLog, Log, All);

UCLASS()
class UNKOWN_API UDebugFunLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable,Category="Debug")
	static void PrintDebugLog(FEnumSet::DebugLogType logType,FString context);
};


