// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "STorch.generated.h"

UCLASS()
class STUDYPROJECT_API ASTorch : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASTorch();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	// TObjectPtr: 언리얼에서 안전하게 쓸 수 있게 권장하는 포인터, 포인터 타입이라 블프에서도 수정 가능
	// Visivle-: 에디터에서 수정 불가, Edit-: 에디터에서 수정 가능
	// 콜리전용
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ASTorch", meta = (AllowPrivateAccess))
	TObjectPtr<class UBoxComponent> BoxComponent;

	// 횃불 메시용
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ASTorch", meta = (AllowPrivateAccess))
	TObjectPtr<class UStaticMeshComponent> BodyStaticMeshComponent;

	// 점광원용
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ASTorch", meta = (AllowPrivateAccess))
	TObjectPtr<class UPointLightComponent> PointLightComponent;
	
	// 불에 대한 파티클용
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ASTorch", meta = (AllowPrivateAccess))
	TObjectPtr<class UParticleSystemComponent> ParticleSystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ASTorch", meta = (AllowPrivateAccess))
	TObjectPtr<class URotatingMovementComponent> RotatingMovementComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ASTorch", meta = (AllowPrivateAccess))
	int32 ID;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ASTorch", meta = (AllowPrivateAccess))
	float RotationSpeed;

};
