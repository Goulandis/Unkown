// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "IPlatformFilePak.h"
#include "GenericPlatform/GenericPlatformFile.h"
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PakExpand.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class UNKOWN_API UPakExpand : public UObject
{
	GENERATED_BODY()
private:
	TSharedPtr<FPakPlatformFile> PakPlatformFile;
	IPlatformFile* HandlePlatform;
public:
	UPakExpand();

	UFUNCTION(BlueprintCallable,Category="PakExpand")
	bool Mount(FString PakFilePath);
};
