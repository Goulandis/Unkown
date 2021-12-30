// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintPlatformLibrary.h"
#include "UnkPlatformGameInstanceBase.generated.h"

/**
 * 
 */
UCLASS()
class UNKOWN_API UUnkPlatformGameInstanceBase : public UPlatformGameInstance
{
	GENERATED_BODY()
protected:

	virtual void OnStart() override;

	UFUNCTION(BlueprintImplementableEvent)
	void UnkOnStart();
};
