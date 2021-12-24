// Fill out your copyright notice in the Description page of Project Settings.


#include "DebugFunLib.h"


DEFINE_LOG_CATEGORY(Debug); 

void UDebugFunLib::DebugLog(FEnumSet::DebugLogType logType, FString context)
{   
	switch (logType)
	{
	case FEnumSet::Log:
		UE_LOG(Debug, Log, TEXT("[%s:%s:%s]%s"), __FILE__, __func__, __LINE__, *context);
		break;
	case FEnumSet::Warning:
		UE_LOG(Debug, Warning, TEXT("[%s:%s:%s]%s"), __FILE__, __func__, __LINE__, *context);
		break;
	case FEnumSet::Error:
		UE_LOG(Debug, Error, TEXT("[%s:%s:%s]%s"), __FILE__, __func__, __LINE__, *context);
		break;
	}  
}
