// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/UW_LobbyLayout.h"
#include "SViewCharacterSettings.h"
#include "GameFramework/Character.h"
#include "Components/Button.h"
#include "Components/EditableText.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Kismet/GameplayStatics.h"

void UUW_LobbyLayout::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    // 플레이어에 빙의된 캐릭터 스켈레탈 메시 컴포넌트 얻어오기
    ACharacter* Character = Cast<ACharacter>(GetOwningPlayerPawn());
    CurrentSkeletalMeshComponent = Character->GetMesh();

    // 바인드 작업
    BlackTeamButton.Get()->OnClicked.AddDynamic(this, &ThisClass::OnBlackTeamButtonClicked);
    WhiteTeamButton.Get()->OnClicked.AddDynamic(this, &ThisClass::OnWhiteTeamButtonClicked);
    SubmitButton.Get()->OnClicked.AddDynamic(this, &ThisClass::OnSubmitButtonClicked);

    // 커스텀 모듈 작업 가져오기
    const USViewCharacterSettings* CharacterSettings = GetDefault<USViewCharacterSettings>();
    if (true == IsValid(CharacterSettings))
    {
        // 마테리얼 정보 경로 가져오기
        for (auto Path : CharacterSettings->PlayerCharacterMeshMaterialPaths)
        {
            // 비동기 로딩 요청
            StreamableHandles.Emplace(UAssetManager::GetStreamableManager().RequestAsyncLoad
            (
                Path,
                FStreamableDelegate::CreateLambda([this, Path]() -> void
                    {
                        // 해당 마테리얼 속성으로 마테리얼 배열 채우기 작업
                        TSoftObjectPtr<UMaterial> LoadedMaterialInstanceAsset(Path);
                        LoadedMaterialInstanceAssets.Emplace(LoadedMaterialInstanceAsset);
                    })
            ));
        }
    }

}

void UUW_LobbyLayout::NativeDestruct()
{
    // 핸들 정리
    for (auto Handle : StreamableHandles)
    {
        if (true == Handle.IsValid()) Handle->ReleaseHandle();
    }

    Super::NativeDestruct();
}

void UUW_LobbyLayout::OnBlackTeamButtonClicked()
{
    // 블랙 마티리얼 변경
    CurrentSkeletalMeshComponent->SetMaterial(0, LoadedMaterialInstanceAssets[0].Get());

    SelectedTeam = 1;
}

void UUW_LobbyLayout::OnWhiteTeamButtonClicked()
{
    // 화이트 마티리얼 변경
    CurrentSkeletalMeshComponent->SetMaterial(0, LoadedMaterialInstanceAssets[1].Get());

    SelectedTeam = 2;
}

void UUW_LobbyLayout::OnSubmitButtonClicked()
{
    FString PlayerName = EditPlayerName->GetText().ToString();
    // 이름의 길이가 0 이하 10 이상일 경우 무시
    if (0 >= PlayerName.Len() || 10 <= PlayerName.Len()) return;

    // JSON을 활용
    // saved에 PlayerInfo.text로 저장
    const FString SavedDirectoryPath = FPaths::Combine(FPlatformMisc::ProjectDir(), TEXT("Saved"));
    const FString SavedFileName(TEXT("PlayerInfo.txt"));
    // 파일 경로 만들기
    FString AbsoluteFilePath = FPaths::Combine(*SavedDirectoryPath, *SavedFileName);
    FPaths::MakeStandardFilename(AbsoluteFilePath);

    // JSON Object 생성
    TSharedRef<FJsonObject> PlayerInfoJsonObject = MakeShared<FJsonObject>();
    PlayerInfoJsonObject->SetStringField("playername", PlayerName);
    PlayerInfoJsonObject->SetNumberField("team", SelectedTeam);

    FString PlayerInfoJsonString;
    TSharedRef<TJsonWriter<TCHAR>> JsonWriterArchive = TJsonWriterFactory<TCHAR>::Create(&PlayerInfoJsonString);
    // 아카이브를 통해 시리얼라이즈(오브젝트 -> 바이트) 작업
    if (true == FJsonSerializer::Serialize(PlayerInfoJsonObject, JsonWriterArchive))
    {
        FFileHelper::SaveStringToFile(PlayerInfoJsonString, *AbsoluteFilePath);
    }

    // 타이틀 레벨에서 작업했던 오픈레벨 파싱 작업
    UGameplayStatics::OpenLevel(GetWorld(), TEXT("Loading"), true, FString(TEXT("NextLevel=Example")));
}
