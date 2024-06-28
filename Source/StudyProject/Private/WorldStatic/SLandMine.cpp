// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldStatic/SLandMine.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
ASLandMine::ASLandMine()
    : bIsExploded(false)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BodyBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BodyBoxComponent"));
	SetRootComponent(BodyBoxComponent);

	BodyStaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyStaticMeshComponent"));
	BodyStaticMeshComponent->SetupAttachment(GetRootComponent());

    ParticleSystemComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleSystemComponent"));
    ParticleSystemComponent->SetupAttachment(GetRootComponent());
    ParticleSystemComponent->SetAutoActivate(false);

	// SetReplicates() �Լ��� ����ص� ������ ��Ȥ ��� ���� �ش� ������� ó��
	bReplicates = true;

}

// Called when the game starts or when spawned
void ASLandMine::BeginPlay()
{
	Super::BeginPlay();
	
	//UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("ASLandMine::BeginPlay() has been called.")));

    //// HasAuthority(): ��ȯ���� true�̸� �������� ���� ��, false�̸� ������ �ƴ� PC���� ���� ��
    //if (true == HasAuthority())
    //{
    //    UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("ASLandMine::BeginPlay() has been called in Server PC.")));
    //}
    //// �� �ش� else �κ��� ������ �ƴ� PC�� ���� ����
    //else
    //{
    //    // �� ��ǻ��(Owning)
    //    if (GetOwner() == UGameplayStatics::GetPlayerController(this, 0))
    //    {
    //        UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("ASLandMine::BeginPlay() has been called in OwningClient PC.")));
    //    }
    //    // �� ��ǻ�Ͱ� �ƴ� �ٸ� ��ǻ��(Other)
    //    else
    //    {
    //        UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("ASLandMine::BeginPlay() has been called in OtherClient PC.")));
    //    }
    //}

    // ���ε� ����
    if (false == OnActorBeginOverlap.IsAlreadyBound(this, &ThisClass::OnLandMineBeginOverlap))
    {
        OnActorBeginOverlap.AddDynamic(this, &ThisClass::OnLandMineBeginOverlap);
    }

}

void ASLandMine::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	//UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("ASLandMine::EndPlay() has been called.")));

    /*if (true == HasAuthority())
    {
        UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("ASLandMine::EndPlay() has been called in Server PC.")));
    }
    else
    {
        if (GetOwner() == UGameplayStatics::GetPlayerController(this, 0))
        {
            UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("ASLandMine::EndPlay() has been called in OwningClient PC.")));
        }
        else
        {
            UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("ASLandMine::EndPlay() has been called in OtherClient PC.")));
        }
    }*/

    // ���ε� ����
    if (true == OnActorBeginOverlap.IsAlreadyBound(this, &ThisClass::OnLandMineBeginOverlap))
    {
        OnActorBeginOverlap.RemoveDynamic(this, &ThisClass::OnLandMineBeginOverlap);
    }

}

// Called every frame
void ASLandMine::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASLandMine::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    // bIsExploded�� Replicated �� ����Ʈ�� �߰�
    // �� �ش� Actor�� Replicated�Ǹ� ���� �ǰ� ����� ��Ű�� �Ͱ� ���ٰ� ���� ��
    DOREPLIFETIME(ThisClass, bIsExploded);

}

void ASLandMine::OnLandMineBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
    // Ȯ�� �۾�
    /*if (true == HasAuthority())
    {
        UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("ASLandMine::OnLandMineBeginOverlap() has been called in Server PC.")));
    }
    else
    {
        if (GetOwner() == UGameplayStatics::GetPlayerController(this, 0))
        {
            UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("ASLandMine::OnLandMineBeginOverlap() has been called in OwningClient PC.")));
        }
        else
        {
            UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("ASLandMine::OnLandMineBeginOverlap() has been called in OtherClient PC.")));
        }
    }*/

    // �������� ȣ���� �ƴٸ� SpawnEffect_NetMulticast()
    // NetMulticast�� ���������� ȣ�� ����
    if (true == HasAuthority() && false == bIsExploded)
    {
        SpawnEffect_NetMulticast();
        bIsExploded = true;
    }
}

void ASLandMine::OnRep_IsExploded()
{
    if (true == bIsExploded) BodyStaticMeshComponent->SetMaterial(0, ExplodedMaterial);

}

void ASLandMine::SpawnEffect_NetMulticast_Implementation()
{
    // ��ƼŬ �۵��� ���������� ����� �ʿ䰡 ����(������Ʈ�� Ŀ�� ��뷮�� �������� ������ �߻��� �� ����)
    // ���� ������ �ƴ� PC���� �۵��ǰ� false == HasAuthority() ���� �ɱ�
    if(false == HasAuthority()) ParticleSystemComponent->Activate(true);

    if (true == IsValid(ExplodedMaterial))
    {
        BodyStaticMeshComponent->SetMaterial(0, ExplodedMaterial);

    }

}

