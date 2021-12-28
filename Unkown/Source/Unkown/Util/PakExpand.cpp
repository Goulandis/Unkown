#include "PakExpand.h"
#include "Debug\DebugFunLib.h"

UPakExpand::UPakExpand()
{
    
}

bool UPakExpand::Mount(FString PakFilePath)
{
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
            UDebugFunLib::DebugLog(FEnumSet::Error, TEXT("Failed to read Pak file that path is ") + PakFilePath);
            return false;
        }
        TArray<FString> ExistPakFiles;
        PakPlatformFile->GetMountedPakFilenames(ExistPakFiles);
        if (ExistPakFiles.Find(PakFile->GetFilename()) >= 0)
        {
            UDebugFunLib::DebugLog(FEnumSet::Warning, PakFile->GetFilename() + TEXT(" is exists"));
            return false;
        }
        FString MountPoint = PakFile->GetMountPoint();
        int32 Pos = MountPoint.Find("Content/");
        FString NewMountPoint = FPaths::ProjectDir() + MountPoint.RightChop(Pos);
        PakFile->SetMountPoint(*NewMountPoint);
        if (PakPlatformFile->Mount(*PakFilePath, 1, *NewMountPoint))
        {
            FString AssestRef = "Blueprint'/Game/Pak/" + PakFile->GetFilename();
            //TCHAR* AssestRef = "Blueprint'/Game/Pak/" + PakFile->GetFilename();
            UClass* uclass = StaticLoadClass(AActor::StaticClass(), NULL, *AssestRef);
            GetWorld()->SpawnActor(uclass);
            return true;
        }
    }
    return false;
}
