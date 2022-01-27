// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GenericPlatform/GenericPlatformFile.h"
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PlatformFileExpand.generated.h"

UCLASS(BlueprintType, Blueprintable)
class UNKOWN_API UPlatformFileExpand : public UObject
{
	GENERATED_BODY()
private:
	IPlatformFile* PlatformFile;
	FPlatformFileManager* PlatformFileManager;
    class FVisitor : public IPlatformFile::FDirectoryVisitor
    {
    public:
        TArray<FString> FileList;
        FString Extension = TEXT("");
        bool Visit(const TCHAR* FilenameOrDirectory, bool bIsDirectory) override
        {
            if (bIsDirectory)
            {
                return false;
            }
            if (FPaths::GetExtension(FilenameOrDirectory,true) == Extension)
            {
                FileList.Add(FilenameOrDirectory);
                return true;
            }           
            return false;
        }
    };
public:
	UPlatformFileExpand();
	UFUNCTION(BlueprintCallable,Category="PlatformFileExpand")
	bool CreateDirectory(const FString Directory);
	UFUNCTION(BlueprintCallable, Category = "PlatformFileExpand")
	bool CopyDirectory(const FString DestinationDirectory, const FString SourceDirectory);
	UFUNCTION(BlueprintCallable, Category = "PlatformFileExpand")
	bool DeleteDirectory(const FString Directory);
	UFUNCTION(BlueprintCallable, Category = "PlatformFileExpand")
	bool GetAllFileFromDirectory(const FString Directory,const FString Extension, TArray<FString>& FileList);
    UFUNCTION(BlueprintCallable, Category = "PlatformFileExpand")
    void ReadFile(const FString FilePath);
    UFUNCTION(BlueprintCallable, Category = "PlatformFileExpand")
    void WriteFile(const FString FilePath, const FString CmdLine);
    static bool Filter(const FString& Item);

    template <typename... STR>
    void FuncTest(STR... Strs);
    UFUNCTION(BlueprintCallable, Category = "PlatformFileExpand")
    void GConfigSet(FString Section, FString Key, FString Value, FString Path);
    void KeyValuePrint(const TCHAR* Key, const TCHAR* Value);
};

template<typename ...STR>
inline void UPlatformFileExpand::FuncTest(STR... Strs)
{
    //const TCHAR* Str[] = { Strs... };
    int INT[] = { Strs... };
    for (int Item : INT)
    {
        UE_LOG(LogTemp, Warning, TEXT("%d"), Item);
    }
}
