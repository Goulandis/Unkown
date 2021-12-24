// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Assets/EnumSet.h"
#include "Logging/LogMacros.h"
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DebugFunLib.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(Debug, Log, All);

UCLASS()
class UNKOWN_API UDebugFunLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable,Category="Debug")
	static void DebugLog(FEnumSet::DebugLogType logType,FString context);
};
