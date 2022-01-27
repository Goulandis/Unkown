#include "PakExpand.h"
#include "../Debug/DebugFunLib.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"
#include "Json.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"

UPakExpand::UPakExpand()
{
    GetPlatformFileManager();
    GetHandleOriginPlatform();
    GetHandlePakPlatform();
}

UPakExpand::~UPakExpand()
{
}

FPakPlatformFile* UPakExpand::GetHandlePakPlatform()
{
    if (!HandlePakPlatform)
    {
        //创建Pak平台文件系统
        HandlePakPlatform = new FPakPlatformFile();
        //使用平台接口初始化Pak文件平台包装器
        HandlePakPlatform->Initialize(GetHandleOriginPlatform(), TEXT(""));
    }
    return HandlePakPlatform;
}

IPlatformFile* UPakExpand::GetHandleOriginPlatform()
{
    if (!HandleOriginPlatform)
    {
        //获取平台I/O接口，用于操作平台文件
        HandleOriginPlatform = &GetPlatformFileManager()->GetPlatformFile();
    }
    return HandleOriginPlatform;
}

FPlatformFileManager* UPakExpand::GetPlatformFileManager()
{
    if (!PlatformFileManager)
    {
        //获取平台文件链接管理器
        PlatformFileManager = &FPlatformFileManager::Get();     
    }
    return PlatformFileManager;
}

bool UPakExpand::IsClassReference(const FString ClassRef)
{
    return Regular(ClassRef,TEXT("^Blueprint'/Game/.*_C'$"));
}

void UPakExpand::GetEncryptKey(uint8* Key)
{
    if (KeyBase64Ary.Num() >= 0)
    {
        FMemory::Memcpy(Key, KeyBase64Ary.GetData(), FAES::FAESKey::KeySize);
    }
    else
    {
        DEBUGLOG(FEnumSet::DebugLogType::Error, FString("The EncryptKey is null"));
    }
}

bool UPakExpand::Mount(const FString PakFilePath)
{
    DEBUGLOG(FEnumSet::DebugLogType::Log, (TEXT("Start to mount pak file and PakFilePath is ") + PakFilePath));
    //判断文件时候是pak文件
    if (!PakFilePath.EndsWith(".pak"))
    {
        DEBUGLOG(FEnumSet::DebugLogType::Warning, (PakFilePath + TEXT(" file is not pak file")));
        DelegateMountAffter.Broadcast(TEXT("FileIsNotPak"));
        return false;
    } 
    //将PakPlatformFile设置到最顶层，查找文件时优先查找PakPlatformFile内的文件
    GetPlatformFileManager()->SetPlatformFile(*GetHandlePakPlatform());
    //判断文件是否存在
    if (!GetHandleOriginPlatform()->FileExists(*PakFilePath))
    {
        DEBUGLOG(FEnumSet::DebugLogType::Error, (TEXT("The file isn't exists that path is ") + PakFilePath));   
        GetPlatformFileManager()->SetPlatformFile(*GetHandleOriginPlatform());
        DelegateMountAffter.Broadcast(TEXT("FileIsNotExists"));
        return false;
    }
    //创建pak对象
    TSharedPtr<FPakFile> PakFile = MakeShareable<FPakFile>(new FPakFile(GetHandlePakPlatform(), *PakFilePath, false));
    if (!PakFile)
    {
        DEBUGLOG(FEnumSet::DebugLogType::Error, (TEXT("Failed to construct pak file that path is ") + PakFilePath)); 
        GetPlatformFileManager()->SetPlatformFile(*GetHandleOriginPlatform());
        DelegateMountAffter.Broadcast(TEXT("PakIsNotValid"));
        return false;
    } 
    //获取pak文件的无后缀文件名
    FString PakName = GetPakFileName(PakFilePath);
    DEBUGLOG(FEnumSet::DebugLogType::Log, (TEXT("Successfully constructed FPakFile that name is ") + PakName));
    TArray<FString> ExistPakFiles;
    //查询已挂载的所有pak包名称
    GetHandlePakPlatform()->GetMountedPakFilenames(ExistPakFiles);
    //判断当前pak包是否已挂载
    if (ExistPakFiles.Find(PakFilePath) >= 0)
    {
        DEBUGLOG(FEnumSet::DebugLogType::Warning, (PakName + TEXT(" is mounted")));
        GetPlatformFileManager()->SetPlatformFile(*GetHandleOriginPlatform());
        DelegateMountAffter.Broadcast(TEXT("PakHasMount"));
        return false;
    }
    int32 Pos = PakFile->GetMountPoint().Find("Content/");
    FString MountPoint = PakFile->GetMountPoint().RightChop(Pos);    
    MountPoint = FPaths::ProjectDir() + MountPoint;
    PakFile->SetMountPoint(*MountPoint);
    //使用pak包记录的挂载点挂载pak包
    if (!GetHandlePakPlatform()->Mount(*PakFilePath, 0, *PakFile->GetMountPoint()))
    {
        DEBUGLOG(FEnumSet::DebugLogType::Error, (TEXT("Failed to mount pak file at ") + PakFile->GetMountPoint() + TEXT(" mount point that path is ") + PakFilePath));
        GetPlatformFileManager()->SetPlatformFile(*GetHandleOriginPlatform());
        DelegateMountAffter.Broadcast(TEXT("FailedToMount"));
        return false;
    }
    DEBUGLOG(FEnumSet::DebugLogType::Log, (TEXT("Successfully mount pak file at ") + PakFile->GetMountPoint() + TEXT(" mount point")));

    MapPakFile.Add(PakFilePath, PakFile.Get());
    DelegateMountAffter.Broadcast(TEXT("SuccessfullyMount"));
    GetPlatformFileManager()->SetPlatformFile(*GetHandleOriginPlatform());
    return true;
}

bool UPakExpand::Mount(const FString PakFilePath, const FString CryptoJsonPath)
{
    if (!CryptoJsonPath.IsEmpty())
    {
        UnEncrypt(CryptoJsonPath);
        FCoreDelegates::GetPakEncryptionKeyDelegate().BindUObject(this, &UPakExpand::GetEncryptKey);
        DEBUGLOG(FEnumSet::DebugLogType::Log, FString("PakEncryptionKeyDelegate has bind UPakExpand::UnEncrypt"));
    }  
    return Mount(PakFilePath);  
}

TArray<FString> UPakExpand::GetAllPakFromDir(const FString Dir, bool& Result)
{
    FString PakDir(FPaths::ProjectContentDir() + Dir + TEXT("/"));
    IFileManager& FileManager = IFileManager::Get();
    TArray<FString> ResultList;
    if (FileManager.DirectoryExists(*PakDir))
    {
        TArray<FString> PakList;      
        FileManager.FindFiles(PakList, *PakDir,TEXT("*.pak"));
        for (int i = 0; i < PakList.Num(); i++)
        {
            FString PakFilePath(PakDir + PakList[i]);
            DEBUGLOG(FEnumSet::DebugLogType::Log, (TEXT("PakName:") + PakFilePath));
            ResultList.Add(PakFilePath);
        }
        Result = true;
    }
    if (ResultList.Num() <= 0)
    {
        Result = false;
    }
    return ResultList;
}

void UPakExpand::UnEncrypt(const FString CryptoJsonPath)
{
    if (!FPaths::FileExists(CryptoJsonPath))
    {
        DEBUGLOG(FEnumSet::DebugLogType::Log, (CryptoJsonPath + TEXT(" file is not exists")));
        return;
    }
    FString JsonStr;
    FFileHelper::LoadFileToString(JsonStr, *CryptoJsonPath);
    TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
    TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(JsonStr);
    FString KeyStr;
    if (FJsonSerializer::Deserialize(JsonReader, JsonObject))
    {
        TSharedPtr<FJsonObject> EncryptionKey = JsonObject->GetObjectField(TEXT("EncryptionKey"));
        KeyStr = EncryptionKey->GetStringField(TEXT("Key"));
        DEBUGLOG(FEnumSet::DebugLogType::Log, (TEXT("Successfully read crypto key:") + KeyStr));
        FBase64::Decode(KeyStr, KeyBase64Ary);
        //EncryptKey = (uint8*)malloc(FAES::FAESKey::KeySize);
        //FMemory::Memcpy(EncryptKey, KeyBase64Ary.GetData(), FAES::FAESKey::KeySize);
    }   
}

FString UPakExpand::GetPakFileName(const FString PakFilePath)
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

FString UPakExpand::GetAssestRefDir(FString MountPoint)
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

bool UPakExpand::Regular(const FString Str, const FString Reg)
{
    FRegexPattern Pattern(Reg);
    FRegexMatcher RegMatcher(Pattern, Str);
    RegMatcher.SetLimits(0, Str.Len());
    return RegMatcher.FindNext();
}

AActor* UPakExpand::SpawnActorFromPak(FString ClassRef, FTransform Transform, bool& Result)
{
    if (!IsClassReference(ClassRef))
    {
        DEBUGLOG(FEnumSet::DebugLogType::Error, (ClassRef + TEXT(" class reference is incorrent")));
        Result = false;
        return nullptr;
    }
    GetPlatformFileManager()->SetPlatformFile(*GetHandlePakPlatform());
    UClass* uclass = StaticLoadClass(AActor::StaticClass(), NULL, *ClassRef);
    if (!uclass)
    {
        DEBUGLOG(FEnumSet::DebugLogType::Error, (TEXT("Failed to load class that reference is ") + ClassRef));
        Result = false;
        GetPlatformFileManager()->SetPlatformFile(*GetHandleOriginPlatform());
        return nullptr;
    }
    AActor* Actor = GetWorld()->SpawnActor(uclass,&Transform);
    if (!Actor)
    {
        DEBUGLOG(FEnumSet::DebugLogType::Error, (TEXT("Failed to create actor with ") + ClassRef));
        Result = false;
        GetPlatformFileManager()->SetPlatformFile(*GetHandleOriginPlatform());
        return nullptr;
    }   
    Result = true;
    GetPlatformFileManager()->SetPlatformFile(*GetHandleOriginPlatform());
    return Actor;
}

UObject* UPakExpand::NewObjectFromPak(FString ClassRef, bool& Result)
{
    return nullptr;
}

void UPakExpand::ReadFile(FString FilePath)
{

}
