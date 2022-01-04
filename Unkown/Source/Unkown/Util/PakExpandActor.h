// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "IPlatformFilePak.h"
#include "GenericPlatform/GenericPlatformFile.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PakExpandActor.generated.h"

UCLASS(Blueprintable, BlueprintType)
class UNKOWN_API APakExpandActor : public AActor
{
	GENERATED_BODY()
private:
	FPakPlatformFile* PakPlatformFile;
	IPlatformFile* HandlePlatform;
	FPlatformFileManager* PlatformFileManager;
	TArray<FSoftObjectPath> ObjectPaths;
	TArray<TSoftObjectPtr<UObject>> ObjectPtrs;
public:	
	// Sets default values for this actor's properties
	APakExpandActor();
	~APakExpandActor();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "PakExpand")
	bool Mount(FString PakFilePath);
	UFUNCTION(BlueprintCallable, Category = "PakExpand")
	FString GetPakFileName(FString PakFilePath);
	UFUNCTION(BlueprintCallable, Category = "PakExpand")
	FString GetAssestRefDir(FString MountPoint);
	UFUNCTION(BlueprintCallable, Category = "PakExpand")
	UClass* LoadClassWithRef(FString ClassRef);
	void OnFinishLoadResource();
};
