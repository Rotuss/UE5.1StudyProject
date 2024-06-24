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

    // �÷��̾ ���ǵ� ĳ���� ���̷�Ż �޽� ������Ʈ ������
    ACharacter* Character = Cast<ACharacter>(GetOwningPlayerPawn());
    CurrentSkeletalMeshComponent = Character->GetMesh();

    // ���ε� �۾�
    BlackTeamButton.Get()->OnClicked.AddDynamic(this, &ThisClass::OnBlackTeamButtonClicked);
    WhiteTeamButton.Get()->OnClicked.AddDynamic(this, &ThisClass::OnWhiteTeamButtonClicked);
    SubmitButton.Get()->OnClicked.AddDynamic(this, &ThisClass::OnSubmitButtonClicked);

    // Ŀ���� ��� �۾� ��������
    const USViewCharacterSettings* CharacterSettings = GetDefault<USViewCharacterSettings>();
    if (true == IsValid(CharacterSettings))
    {
        // ���׸��� ���� ��� ��������
        for (auto Path : CharacterSettings->PlayerCharacterMeshMaterialPaths)
        {
            // �񵿱� �ε� ��û
            StreamableHandles.Emplace(UAssetManager::GetStreamableManager().RequestAsyncLoad
            (
                Path,
                FStreamableDelegate::CreateLambda([this, Path]() -> void
                    {
                        // �ش� ���׸��� �Ӽ����� ���׸��� �迭 ä��� �۾�
                        TSoftObjectPtr<UMaterial> LoadedMaterialInstanceAsset(Path);
                        LoadedMaterialInstanceAssets.Emplace(LoadedMaterialInstanceAsset);
                    })
            ));
        }
    }

}

void UUW_LobbyLayout::NativeDestruct()
{
    // �ڵ� ����
    for (auto Handle : StreamableHandles)
    {
        if (true == Handle.IsValid()) Handle->ReleaseHandle();
    }

    Super::NativeDestruct();
}

void UUW_LobbyLayout::OnBlackTeamButtonClicked()
{
    // �� ��Ƽ���� ����
    CurrentSkeletalMeshComponent->SetMaterial(0, LoadedMaterialInstanceAssets[0].Get());

    SelectedTeam = 1;
}

void UUW_LobbyLayout::OnWhiteTeamButtonClicked()
{
    // ȭ��Ʈ ��Ƽ���� ����
    CurrentSkeletalMeshComponent->SetMaterial(0, LoadedMaterialInstanceAssets[1].Get());

    SelectedTeam = 2;
}

void UUW_LobbyLayout::OnSubmitButtonClicked()
{
    FString PlayerName = EditPlayerName->GetText().ToString();
    // �̸��� ���̰� 0 ���� 10 �̻��� ��� ����
    if (0 >= PlayerName.Len() || 10 <= PlayerName.Len()) return;

    // JSON�� Ȱ��
    // saved�� PlayerInfo.text�� ����
    const FString SavedDirectoryPath = FPaths::Combine(FPlatformMisc::ProjectDir(), TEXT("Saved"));
    const FString SavedFileName(TEXT("PlayerInfo.txt"));
    // ���� ��� �����
    FString AbsoluteFilePath = FPaths::Combine(*SavedDirectoryPath, *SavedFileName);
    FPaths::MakeStandardFilename(AbsoluteFilePath);

    // JSON Object ����
    TSharedRef<FJsonObject> PlayerInfoJsonObject = MakeShared<FJsonObject>();
    PlayerInfoJsonObject->SetStringField("playername", PlayerName);
    PlayerInfoJsonObject->SetNumberField("team", SelectedTeam);

    FString PlayerInfoJsonString;
    TSharedRef<TJsonWriter<TCHAR>> JsonWriterArchive = TJsonWriterFactory<TCHAR>::Create(&PlayerInfoJsonString);
    // ��ī�̺긦 ���� �ø��������(������Ʈ -> ����Ʈ) �۾�
    if (true == FJsonSerializer::Serialize(PlayerInfoJsonObject, JsonWriterArchive))
    {
        FFileHelper::SaveStringToFile(PlayerInfoJsonString, *AbsoluteFilePath);
    }

    // Ÿ��Ʋ �������� �۾��ߴ� ���·��� �Ľ� �۾�
    UGameplayStatics::OpenLevel(GetWorld(), TEXT("Loading"), true, FString(TEXT("NextLevel=Example")));
}
