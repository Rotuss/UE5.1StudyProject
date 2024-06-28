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

	// SetReplicates() 함수를 사용해도 되지만 간혹 경고가 떠서 해당 방식으로 처리
	bReplicates = true;

}

// Called when the game starts or when spawned
void ASLandMine::BeginPlay()
{
	Super::BeginPlay();
	
	//UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("ASLandMine::BeginPlay() has been called.")));

    //// HasAuthority(): 반환값이 true이면 서버에서 실행 중, false이면 서버가 아닌 PC에서 실행 중
    //if (true == HasAuthority())
    //{
    //    UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("ASLandMine::BeginPlay() has been called in Server PC.")));
    //}
    //// 즉 해당 else 부분은 서버가 아닌 PC일 때의 조건
    //else
    //{
    //    // 내 컴퓨터(Owning)
    //    if (GetOwner() == UGameplayStatics::GetPlayerController(this, 0))
    //    {
    //        UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("ASLandMine::BeginPlay() has been called in OwningClient PC.")));
    //    }
    //    // 내 컴퓨터가 아닌 다른 컴퓨터(Other)
    //    else
    //    {
    //        UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("ASLandMine::BeginPlay() has been called in OtherClient PC.")));
    //    }
    //}

    // 바인드 설정
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

    // 바인드 제거
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

    // bIsExploded가 Replicated 될 리스트에 추가
    // 즉 해당 Actor가 Replicated되면 같이 되게 등록을 시키는 것과 같다고 보면 됨
    DOREPLIFETIME(ThisClass, bIsExploded);

}

void ASLandMine::OnLandMineBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
    // 확인 작업
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

    // 서버에서 호출이 됐다면 SpawnEffect_NetMulticast()
    // NetMulticast는 서버에서만 호출 가능
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
    // 파티클 작동이 서버에서도 적용될 필요가 없음(프로젝트가 커서 사용량이 많아지면 과부하 발생할 수 있음)
    // 따라서 서버가 아닌 PC에서 작동되게 false == HasAuthority() 조건 걸기
    if(false == HasAuthority()) ParticleSystemComponent->Activate(true);

    if (true == IsValid(ExplodedMaterial))
    {
        BodyStaticMeshComponent->SetMaterial(0, ExplodedMaterial);

    }

}

