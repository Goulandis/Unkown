// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "EnumSet.generated.h"

/**
 * 
 */
UCLASS()
class UNKOWN_API UEnumSet : public UObject
{
	GENERATED_BODY()
	
};

UENUM(BlueprintType)
namespace FEnumSet
{
	enum DebugLogType
	{
		Log,
		Warning,
		Error
	};	
}