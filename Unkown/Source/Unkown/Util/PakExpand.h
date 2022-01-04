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
UCLASS(Blueprintable,BlueprintType)
class UNKOWN_API UPakExpand : public UObject
{
	GENERATED_BODY()
private:
	FPakPlatformFile* PakPlatformFile;
	IPlatformFile* HandlePlatform;
	FPlatformFileManager* PlatformFileManager;
	TArray<FSoftObjectPath> ObjectPaths;
	TArray<TSoftObjectPtr<UObject>> ObjectPtrs;
private:
	~UPakExpand();
public:
	UPakExpand();

	UFUNCTION(BlueprintCallable,Category="PakExpand")
	bool Mount(FString PakFilePath);
	UFUNCTION(BlueprintCallable, Category = "PakExpand")
	FString GetPakFileName(FString PakFilePath);
	UFUNCTION(BlueprintCallable, Category = "PakExpand")
	FString GetAssestRefDir(FString MountPoint);
	void OnFinishLoadResource();
};
