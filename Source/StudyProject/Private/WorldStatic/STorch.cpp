// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldStatic/STorch.h"
#include "Components/BoxComponent.h"
#include "Components/PointLightComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "GameFramework/RotatingMovementComponent.h"

// Sets default values
ASTorch::ASTorch()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	// 틱 함수를 호출하게 할 것이냐
	PrimaryActorTick.bCanEverTick = false;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	SetRootComponent(BoxComponent);

	BodyStaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyStaticMeshComponent"));
	BodyStaticMeshComponent->SetupAttachment(GetRootComponent());
	BodyStaticMeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, -30.0f));
	// 경로 정보는 플레이 중 변경되지 않으므로 static 키워드 사용
	// ConstructorHelpers::FObjectFinder: 템플릿 타입에 맞춰 해당 오브젝트를 찾아주는 프로그램
	// 하드코딩의 경우 큰 팀작업으로 아트팀이 존재한다면 경로가 바뀌는 경우가 잦음, 따라서 블프에서 직접 조정하게 작업하는 것이 좋음
	// 블프에서 에셋을 지정하게 되면 해당 오브젝트의 경로가 바뀌어도 'Fix Up Redirectors'를 통해 자동으로 변경되서 안전하고 편리
	/*static ConstructorHelpers::FObjectFinder<UStaticMesh> BodyStaticMesh(TEXT("/Script/Engine.StaticMesh'/Game/StarterContent/Architecture/Pillar_50x500.Pillar_50x500'"));
	if (true == BodyStaticMesh.Succeeded()) BodyStaticMeshComponent->SetStaticMesh(BodyStaticMesh.Object);*/

	PointLightComponent = CreateDefaultSubobject<UPointLightComponent>(TEXT("PointLightComponent"));
	PointLightComponent->SetupAttachment(GetRootComponent());
	PointLightComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 500.0f));

	ParticleSystemComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleSystemComponent"));
	ParticleSystemComponent->SetupAttachment(GetRootComponent());
	ParticleSystemComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 500.0f));
	/*static ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleTemplate(TEXT("/Script/Engine.ParticleSystem'/Game/StarterContent/Particles/P_Fire.P_Fire'"));
	if (true == ParticleTemplate.Succeeded()) ParticleSystemComponent->SetTemplate(ParticleTemplate.Object);*/

	RotatingMovementComponent = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("RotatingMovementComponent"));

}

// Called when the game starts or when spawned
void ASTorch::BeginPlay()
{
	Super::BeginPlay();
	
	RotationSpeed = 300.0f;
	RotatingMovementComponent->RotationRate = FRotator(0.0f, RotationSpeed, 0.0f);

}

// Called every frame
void ASTorch::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 액터의 월드 로테이션 값을 추가적으로 연산
	//AddActorWorldRotation(FRotator(0.0f, RotationSpeed * DeltaTime, 0.0f));

}

