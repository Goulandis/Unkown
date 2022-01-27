// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PrefetchExpand.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class UNKOWN_API UPrefetchExpand : public UObject
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable,Category="PrefetchExpand")
	void Prefetch(const FString DataFilePath);
};
