// Fill out your copyright notice in the Description page of Project Settings.

#include "PakExpandActor.h"
#include "../Debug/DebugFunLib.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"

// Sets default values
APakExpandActor::APakExpandActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//��ȡƽ̨�ļ����ӹ�����
	PlatformFileManager = &FPlatformFileManager::Get();
	//��ȡƽ̨I/O�ӿڣ����ڲ���ƽ̨�ļ�
	HandlePlatform = &PlatformFileManager->GetPlatformFile();
}

APakExpandActor::~APakExpandActor()
{
	PlatformFileManager = nullptr;
	HandlePlatform = nullptr;
	PakPlatformFile = nullptr;
}

// Called when the game starts or when spawned
void APakExpandActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APakExpandActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool APakExpandActor::Mount(FString PakFilePath)
{
    DEBUGLOG(FEnumSet::DebugLogType::Log, (TEXT("Start to mount pak file and PakFilePath is ") + PakFilePath));
    //�ж��ļ�ʱ����pak�ļ�
    if (!PakFilePath.EndsWith(".pak"))
    {
        DEBUGLOG(FEnumSet::DebugLogType::Warning, (PakFilePath + TEXT(" file is not pak file")));
        return false;
    }

    if (!PakPlatformFile)
    {
        //����Pakƽ̨�ļ�ϵͳ
        PakPlatformFile = new FPakPlatformFile();
    }
    //ʹ��ƽ̨�ӿڳ�ʼ��Pak�ļ�ƽ̨��װ��
    PakPlatformFile->Initialize(HandlePlatform, TEXT(""));
    //��PakPlatformFile���õ���㣬�����ļ�ʱ���Ȳ���PakPlatformFile�ڵ��ļ�
    PlatformFileManager->SetPlatformFile(*PakPlatformFile);
    //�ж��ļ��Ƿ����
    if (!HandlePlatform->FileExists(*PakFilePath))
    {
        DEBUGLOG(FEnumSet::DebugLogType::Error, (TEXT("The file isn't exists that path is ") + PakFilePath));
        PlatformFileManager->SetPlatformFile(*HandlePlatform);
        return false;
    }
    //����pak����
    TSharedPtr<FPakFile> PakFile = MakeShareable<FPakFile>(new FPakFile(PakPlatformFile, *PakFilePath, false));
    if (!PakFile)
    {
        DEBUGLOG(FEnumSet::DebugLogType::Error, (TEXT("Failed to construct pak file that path is ") + PakFilePath));
        PlatformFileManager->SetPlatformFile(*HandlePlatform);
        return false;
    }
    //��ȡpak�ļ����޺�׺�ļ���
    FString PakName = GetPakFileName(PakFilePath);
    DEBUGLOG(FEnumSet::DebugLogType::Log, (TEXT("Successfully constructed FPakFile that name is ") + PakName));
    TArray<FString> ExistPakFiles;
    //��ѯ�ѹ��ص�����pak������
    PakPlatformFile->GetMountedPakFilenames(ExistPakFiles);
    //�жϵ�ǰpak���Ƿ��ѹ���
    if (ExistPakFiles.Find(PakFilePath) >= 0)
    {
        DEBUGLOG(FEnumSet::DebugLogType::Warning, (PakName + TEXT(" is mounted")));
        PlatformFileManager->SetPlatformFile(*HandlePlatform);
        return false;
    }
    int32 Pos = PakFile->GetMountPoint().Find("Content/");
    FString MountPoint = PakFile->GetMountPoint().RightChop(Pos);
    DEBUGLOG(FEnumSet::DebugLogType::Log, (TEXT("PakFile mount point at ") + MountPoint));
    MountPoint = FPaths::ProjectDir() + MountPoint;
    DEBUGLOG(FEnumSet::DebugLogType::Log, (TEXT("New mount point at ") + MountPoint));
    //MountPoint = FPaths::ProjectDir() + TEXT("Content/DLCs/");
    PakFile->SetMountPoint(*MountPoint);
    //ʹ��pak����¼�Ĺ��ص����pak��
    if (!PakPlatformFile->Mount(*PakFilePath, 0, *PakFile->GetMountPoint()))
    {
        DEBUGLOG(FEnumSet::DebugLogType::Error, (TEXT("Failed to mount pak file at ") + PakFile->GetMountPoint() + TEXT(" mount point that path is ") + PakFilePath));
        PlatformFileManager->SetPlatformFile(*HandlePlatform);
        return false;
    }
    DEBUGLOG(FEnumSet::DebugLogType::Log, (TEXT("Successfully mount pak file at ") + PakFile->GetMountPoint() + TEXT(" mount point")));

    TArray<FString> FileList;
    PakFile->FindFilesAtPath(FileList, *PakFile->GetMountPoint(), true, false, true);
    DEBUGLOG(FEnumSet::DebugLogType::Log, (TEXT("FileList Num=") + FString::FromInt(FileList.Num())));
    //TArray<FSoftObjectPath> AllReourcePath;
    for (FString File : FileList)
    {
        DEBUGLOG(FEnumSet::DebugLogType::Log, (TEXT("FileList FileName=") + File));
        /*if (File.Contains(TEXT(".uasset")))
        {*/
        /*File.ReplaceInline(*FPaths::ProjectContentDir(), TEXT("/Game/"));
        DEBUGLOG(FEnumSet::DebugLogType::Log, (TEXT("FileReplaceInline=") + File));
        FString FileName = FPaths::GetBaseFilename(File);
        DEBUGLOG(FEnumSet::DebugLogType::Log, (TEXT("FileName=") + FileName));
        File.ReplaceInline(TEXT("uasset"), *FileName);
        ObjectPaths.AddUnique(File);*/

        FString Filename, FileExtn;
        int32 LastSlashIndex;
        File.FindLastChar(*TEXT("/"), LastSlashIndex);
        FString FileOnly = File.RightChop(LastSlashIndex + 1);
        FileOnly.Split(TEXT("."), &Filename, &FileExtn);

        if (FileExtn == TEXT("uasset"))
        {
            File = FileOnly.Replace(TEXT("uasset"), *Filename);
            File = TEXT("/Engine/") + File;
            ObjectPaths.AddUnique(FSoftObjectPath(File));
            //��FSoftObjectPathֱ��ת��ΪTSoftObjectPtr<UObject>������
            ObjectPtrs.AddUnique(TSoftObjectPtr<UObject>(ObjectPaths[ObjectPaths.Num() - 1]));
        }
        //}
    }

    //FStreamableManager& AssetLoader = UAssetManager::GetStreamableManager();
    //AssetLoader.RequestAsyncLoad(ObjectPaths, FStreamableDelegate::CreateUObject(this, &APakExpandActor::OnFinishLoadResource));
    //������Դ
    //FString AssestRef = TEXT("Blueprint'/Game/") + GetAssestRefDir(PakFile->GetMountPoint()) + TEXT("/") + PakName + TEXT(".") + PakName + TEXT("_C'");
    FString AssestRef = TEXT("Blueprint'/Game/") + GetAssestRefDir(PakFile->GetMountPoint()) + TEXT("/PakActor.PakActor_C'");
    UClass* uclass = StaticLoadClass(AActor::StaticClass(), NULL, *AssestRef);
    if (!uclass)
    {
        DEBUGLOG(FEnumSet::DebugLogType::Error, (TEXT("Failed to load class that reference is ") + AssestRef));
        PlatformFileManager->SetPlatformFile(*HandlePlatform);
    }
    GetWorld()->SpawnActor(uclass);
    PlatformFileManager->SetPlatformFile(*HandlePlatform);
    return true;
}

FString APakExpandActor::GetPakFileName(FString PakFilePath)
{
    FString SubLeft;
    FString SubRight;
    SubRight = PakFilePath;
    if (SubRight.Contains("/"))
    {
        SubRight = SubRight.Replace(TEXT("/"), TEXT("\\"));
    }
    while (SubRight.Contains(TEXT("\\")))
    {
        SubRight.Split(TEXT("\\"), &SubLeft, &SubRight);
    }
    if (SubRight.Contains("."))
    {
        SubRight.Split(TEXT("."), &SubLeft, &SubRight);
    }
    else
    {
        DEBUGLOG(FEnumSet::DebugLogType::Error, (SubRight + TEXT(" is not pak file")));
    }
    return SubLeft;
}

FString APakExpandActor::GetAssestRefDir(FString MountPoint)
{
    if (MountPoint.Contains("\\"))
    {
        MountPoint = MountPoint.Replace(TEXT("\\"), TEXT("/"));
    }
    FString SubLeft;
    FString SubRight = MountPoint;
    while (SubRight.Contains(TEXT("/")))
    {
        SubRight.Split(TEXT("/"), &SubLeft, &SubRight);
    }
    return SubLeft;
}

UClass* APakExpandActor::LoadClassWithRef(FString ClassRef)
{
    UClass* uclass = StaticLoadClass(AActor::StaticClass(), NULL, *ClassRef);
    if (uclass)
    {
        DEBUGLOG(FEnumSet::DebugLogType::Error, (TEXT("Successfully load class ")+ ClassRef));
        GetWorld()->SpawnActor(uclass);
        return uclass;
    }
    DEBUGLOG(FEnumSet::DebugLogType::Error, (TEXT("Failed to load class ") + ClassRef));
    return nullptr;
}

void APakExpandActor::OnFinishLoadResource()
{
}

