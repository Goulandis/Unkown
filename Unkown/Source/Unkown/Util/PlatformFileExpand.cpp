// Fill out your copyright notice in the Description page of Project Settings.


#include "PlatformFileExpand.h"
#include "../Debug/DebugFunLib.h"
#include "HAL/IPlatformFileCachedWrapper.h"
#include "HAL/IPlatformFileOpenLogWrapper.h"
#include "IPlatformFileSandboxWrapper.h"
#include "Templates/UniquePtr.h"

bool FilterGlobal(const FString& Item)
{
    return Item.Contains(TEXT("Sandbox"));
}

UPlatformFileExpand::UPlatformFileExpand()
{
    PlatformFileManager = &FPlatformFileManager::Get();
    PlatformFile = &PlatformFileManager->GetPlatformFile();
}

bool UPlatformFileExpand::CreateDirectory(const FString Derictory)
{
    bool Result = false;
    if (Derictory.Contains(TEXT("/")) || Derictory.Contains(TEXT("\\")))
    {
        Result = PlatformFile->CreateDirectoryTree(*Derictory);
    }
    else
    {
        Result = PlatformFile->CreateDirectory(*Derictory);
    }
    return Result;
}

bool UPlatformFileExpand::CopyDirectory(const FString DestinationDirectory, const FString SourceDirectory)
{
    return PlatformFile->CopyDirectoryTree(*DestinationDirectory, *SourceDirectory, true);
}

bool UPlatformFileExpand::DeleteDirectory(const FString Directory)
{
    return PlatformFile->DeleteDirectoryRecursively(*Directory);
}

bool UPlatformFileExpand::GetAllFileFromDirectory(const FString Directory, const FString Extension, TArray<FString>& FileList)
{
    TSharedPtr<FVisitor> Visitor = MakeShareable(new FVisitor);
    Visitor->Extension = Extension;
    if (PlatformFile->IterateDirectoryRecursively(*Directory, *Visitor))
    {
        if (Visitor->FileList.Num() <= 0)
        {
            DEBUGLOG(FEnumSet::DebugLogType::Error, (TEXT("Couldn't find ") + Extension + TEXT(" file from ") + Directory));
            return false;
        }
        FileList = Visitor->FileList;
        return true;
    }
    DEBUGLOG(FEnumSet::DebugLogType::Error, (TEXT("Failed to iterate directory:") + Directory));
    return false;
}

void UPlatformFileExpand::ReadFile(const FString FilePath)
{
    TArray<FString> Result;
    FFileHelper::LoadFileToStringArray(Result, *FilePath); 
    TFunction<bool(const FString&)> Fun = UPlatformFileExpand::Filter;
    //FFileHelper::LoadFileToStringArrayWithPredicate(Result, *FilePath, [](const FString& Item) {return Item.Contains(TEXT("Sandbox")); });
    FFileHelper::LoadFileToStringArrayWithPredicate(Result, *FilePath, Fun);
    for (FString item : Result)
    {
        DEBUGLOG(FEnumSet::DebugLogType::Warning, (TEXT("Result:") + item));
    }   
}

void UPlatformFileExpand::WriteFile(const FString FilePath, const FString CmdLine)
{
    FFileHelper::SaveStringToFile(CmdLine, *FilePath);
}

bool UPlatformFileExpand::Filter(const FString& Item)
{  
    return Item.Contains(TEXT("Sandbox"));
}

void UPlatformFileExpand::GConfigSet(FString Section, FString Key, FString Value, FString Path)
{
    GConfig->SaveCurrentStateForBootstrap(*Path);
}

void UPlatformFileExpand::KeyValuePrint(const TCHAR* Key, const TCHAR* Value)
{
    DEBUGLOG(FEnumSet::Warning, (Key + FString("=") + Value));
}
