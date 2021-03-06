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
	//获取平台文件链接管理器
	PlatformFileManager = &FPlatformFileManager::Get();
	//获取平台I/O接口，用于操作平台文件
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
    //判断文件时候是pak文件
    if (!PakFilePath.EndsWith(".pak"))
    {
        DEBUGLOG(FEnumSet::DebugLogType::Warning, (PakFilePath + TEXT(" file is not pak file")));
        return false;
    }

    if (!PakPlatformFile)
    {
        //创建Pak平台文件系统
        PakPlatformFile = new FPakPlatformFile();
    }
    //使用平台接口初始化Pak文件平台包装器
    PakPlatformFile->Initialize(HandlePlatform, TEXT(""));
    //将PakPlatformFile设置到最顶层，查找文件时优先查找PakPlatformFile内的文件
    PlatformFileManager->SetPlatformFile(*PakPlatformFile);
    //判断文件是否存在
    if (!HandlePlatform->FileExists(*PakFilePath))
    {
        DEBUGLOG(FEnumSet::DebugLogType::Error, (TEXT("The file isn't exists that path is ") + PakFilePath));
        PlatformFileManager->SetPlatformFile(*HandlePlatform);
        return false;
    }
    //创建pak对象
    TSharedPtr<FPakFile> PakFile = MakeShareable<FPakFile>(new FPakFile(PakPlatformFile, *PakFilePath, false));
    if (!PakFile)
    {
        DEBUGLOG(FEnumSet::DebugLogType::Error, (TEXT("Failed to construct pak file that path is ") + PakFilePath));
        PlatformFileManager->SetPlatformFile(*HandlePlatform);
        return false;
    }
    //获取pak文件的无后缀文件名
    FString PakName = GetPakFileName(PakFilePath);
    DEBUGLOG(FEnumSet::DebugLogType::Log, (TEXT("Successfully constructed FPakFile that name is ") + PakName));
    TArray<FString> ExistPakFiles;
    //查询已挂载的所有pak包名称
    PakPlatformFile->GetMountedPakFilenames(ExistPakFiles);
    //判断当前pak包是否已挂载
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
    //使用pak包记录的挂载点挂载pak包
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
            //将FSoftObjectPath直接转换为TSoftObjectPtr<UObject>并储存
            ObjectPtrs.AddUnique(TSoftObjectPtr<UObject>(ObjectPaths[ObjectPaths.Num() - 1]));
        }
        //}
    }

    //FStreamableManager& AssetLoader = UAssetManager::GetStreamableManager();
    //AssetLoader.RequestAsyncLoad(ObjectPaths, FStreamableDelegate::CreateUObject(this, &APakExpandActor::OnFinishLoadResource));
    //加载资源
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

