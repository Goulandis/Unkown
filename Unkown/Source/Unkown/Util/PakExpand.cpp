#include "PakExpand.h"
#include "Debug\DebugFunLib.h"

UPakExpand::UPakExpand()
{
    
}

bool UPakExpand::Mount(FString PakFilePath)
{
    //UDebugFunLib::DebugLog(FEnumSet::Log, TEXT("Start to mount pak file and PakFilePath is ") + PakFilePath);
    DEBUGLOG(FEnumSet::Log, PakFilePath);
    FPlatformFileManager& PlatformFileManager = FPlatformFileManager::Get();
    HandlePlatform = &PlatformFileManager.GetPlatformFile();
    if (!PakPlatformFile)
    {
        PakPlatformFile = MakeShareable<FPakPlatformFile>(new FPakPlatformFile());
    }
    PakPlatformFile->Initialize(HandlePlatform, TEXT(""));
    PlatformFileManager.SetPlatformFile(*PakPlatformFile.Get());
    if (HandlePlatform->FileExists(*PakFilePath))
    {
        TSharedPtr<FPakFile> PakFile = MakeShareable<FPakFile>(new FPakFile(PakPlatformFile.Get(), *PakFilePath, false));
        if (!PakFile)
        {
            UDebugFunLib::DebugLog(FEnumSet::Error, TEXT("Failed to construct pak file that path is ") + PakFilePath);
            return false;
        }
        UDebugFunLib::DebugLog(FEnumSet::Log, TEXT("Successfully constructed FPakFile that name is ") + PakFile->GetFilename());
        TArray<FString> ExistPakFiles;
        PakPlatformFile->GetMountedPakFilenames(ExistPakFiles);
        if (ExistPakFiles.Find(PakFile->GetFilename()) >= 0)
        {
            UDebugFunLib::DebugLog(FEnumSet::Warning, PakFile->GetFilename() + TEXT(" is exists"));
            return false;
        }
        FString MountPoint = PakFile->GetMountPoint();
        int32 Pos = MountPoint.Find(TEXT("Content/"));
        FString NewMountPoint = FPaths::ProjectDir() + MountPoint.RightChop(Pos);
        PakFile->SetMountPoint(*NewMountPoint);
        if (PakPlatformFile->Mount(*PakFilePath, 1, *NewMountPoint))
        {
            FString AssestRef = TEXT("Blueprint'/Game/Pak/") + PakFile->GetFilename();          
            //TCHAR* AssestRef = "Blueprint'/Game/Pak/" + PakFile->GetFilename();
            UClass* uclass = StaticLoadClass(AActor::StaticClass(), NULL, *AssestRef);
            GetWorld()->SpawnActor(uclass);
            UDebugFunLib::DebugLog(FEnumSet::Log, TEXT("Successfully loaded assest that assest reference is ") + AssestRef);
            return true;
        }
    }
    UDebugFunLib::DebugLog(FEnumSet::Error, TEXT("Failed to mount pak file"));
    return false;
}
