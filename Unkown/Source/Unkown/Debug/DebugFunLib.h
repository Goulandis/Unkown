// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include <string>
#include "Assets/EnumSet.h"
#include "Logging/LogMacros.h"
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DebugFunLib.generated.h"
#ifndef HONG
#define DEBUGLOG(logType,context)\
std::string File = __FILE__;\
std::string Func = __func__;\
int Line = __LINE__;\
FString FFile = FString(File.c_str());\
FString FFunc = FString(Func.c_str());\
switch(logType)\
{\
	case FEnumSet::Log:\
		UE_LOG(Debug, Log, TEXT("[%s:%s:%d]%s"), *FFile, *FFunc, Line, *context);\
		break;\
	case FEnumSet::Warning:\
		UE_LOG(Debug, Warning, TEXT("[%s:%s:%d]%s"), *FFile, *FFunc, Line, *context);\
		break;\
	case FEnumSet::Error:\
		UE_LOG(Debug, Error, TEXT("[%s:%s:%d]%s"), *FFile, *FFunc, Line, *context);\
		break;\
}
#endif // HONG

DECLARE_LOG_CATEGORY_EXTERN(Debug, Log, All);

UCLASS()
class UNKOWN_API UDebugFunLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable,Category="Debug")
	static void DebugLog(FEnumSet::DebugLogType logType,FString context);
};


