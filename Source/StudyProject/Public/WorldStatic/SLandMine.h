// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SLandMine.generated.h"

UCLASS()
class STUDYPROJECT_API ASLandMine : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASLandMine();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnLandMineBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

	UFUNCTION(NetMulticast, Unreliable)
	void SpawnEffect_NetMulticast();

	UFUNCTION()
	void OnRep_IsExploded();

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ASLandMine", meta = (AllowPrivateAccess))
	TObjectPtr<class UBoxComponent> BodyBoxComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ASLandMine", meta = (AllowPrivateAccess))
	TObjectPtr<UStaticMeshComponent> BodyStaticMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ASLandMine", meta = (AllowPrivateAccess))
	TObjectPtr<UParticleSystemComponent> ParticleSystemComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ASLandMine", meta = (AllowPrivateAccess))
	TObjectPtr<class UMaterial> ExplodedMaterial;

	// ReplicatedUsing = 함수명 => 함수명과 똑같은 이름의 함수를 꼭 생성해줘야만 함(안하면 컴파일 에러)
	// Net Cull Distance 범위 밖에 있다가 안으로 들어올 때 Replicated가 작동
	// 작동하며 복제가 이루어 질 때 해당 함수명의 함수도 호출 됨
	UPROPERTY(ReplicatedUsing = OnRep_IsExploded, VisibleAnywhere, BlueprintReadOnly, Category = "ASLandMine", meta = (AllowPrivateAccess))
	uint8 bIsExploded : 1;

};
