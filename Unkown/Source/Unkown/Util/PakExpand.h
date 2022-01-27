// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "IPlatformFilePak.h"
#include "GenericPlatform/GenericPlatformFile.h"
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PakExpand.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMountAffter, FString,Type);

UCLASS(Blueprintable,BlueprintType)
class UNKOWN_API UPakExpand : public UObject
{
	GENERATED_BODY()
private:
	FPakPlatformFile* HandlePakPlatform;
	IPlatformFile* HandleOriginPlatform;
	FPlatformFileManager* PlatformFileManager;
	uint8* EncryptKey;
	TArray<uint8> KeyBase64Ary;
public:
	TMap<FString, FPakFile*> MapPakFile;
	UPROPERTY(BlueprintAssignable,Category="PakExpand")
	FMountAffter DelegateMountAffter;
private:
	~UPakExpand();
	FPakPlatformFile* GetHandlePakPlatform();
	IPlatformFile* GetHandleOriginPlatform();
	FPlatformFileManager* GetPlatformFileManager();
	bool IsClassReference(const FString ClassRef);
	void GetEncryptKey(uint8* Key);
public:
	UPakExpand();
	bool Mount(const FString PakFilePath);
	UFUNCTION(BlueprintCallable, Category = "PakExpand")
	bool Mount(const FString PakFilePath, const FString CryptoJsonPath);
	UFUNCTION(BlueprintCallable, Category = "PakExpand")
	TArray<FString> GetAllPakFromDir(const FString Dir,bool& Result);
	void UnEncrypt(const FString CryptoJsonPath);
	UFUNCTION(BlueprintCallable, Category = "PakExpand")
	FString GetPakFileName(const FString PakFilePath);
	UFUNCTION(BlueprintCallable, Category = "PakExpand")
	FString GetAssestRefDir(FString MountPoint);
	UFUNCTION(BlueprintCallable, Category = "PakExpand")
	bool Regular(const FString Str, const FString Ref);
	UFUNCTION(BlueprintCallable, Category = "PakExpand")
	AActor* SpawnActorFromPak(FString ClassRef, FTransform Transform, bool& Result);
	UFUNCTION(BlueprintCallable, Category = "PakExpand")
	UObject* NewObjectFromPak(FString ClassRef, bool& Result);
	UFUNCTION(BlueprintCallable, Category = "PakExpand")
	void ReadFile(FString FilePath);
};
