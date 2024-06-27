// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldStatic/SLandMine.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
ASLandMine::ASLandMine()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BodyBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BodyBoxComponent"));
	SetRootComponent(BodyBoxComponent);

	BodyStaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyStaticMeshComponent"));
	BodyStaticMeshComponent->SetupAttachment(GetRootComponent());

	// SetReplicates() 함수를 사용해도 되지만 간혹 경고가 떠서 해당 방식으로 처리
	bReplicates = true;

}

// Called when the game starts or when spawned
void ASLandMine::BeginPlay()
{
	Super::BeginPlay();
	
	UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("ASLandMine::BeginPlay() has been called.")));

}

// Called every frame
void ASLandMine::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

