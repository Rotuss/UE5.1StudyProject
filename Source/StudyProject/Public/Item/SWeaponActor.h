// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SWeaponActor.generated.h"

UCLASS()
class STUDYPROJECT_API ASWeaponActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASWeaponActor();

	USkeletalMeshComponent* GetMesh() const { return Mesh; }

	UAnimMontage* GetMeleeAttackMontage() const { return MeleeAttackMontage; }

	UAnimMontage* GetEquipAnimMontage() const { return EquipAnimMontage; }

	UAnimMontage* GetUnequipAnimMontage() const { return UnequipAnimMontage; }

	TSubclassOf<UAnimInstance> GetUnarmedCharacterAnimLayer() const { return UnarmedCharacterAnimLayer; }

	TSubclassOf<UAnimInstance> GetArmedCharacterAnimLayer() const { return ArmedCharacterAnimLayer; }

	float GetMaxRange() const { return MaxRange; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	TObjectPtr<USkeletalMeshComponent> Mesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess))
	TObjectPtr<UAnimMontage> MeleeAttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ASWeaponActor|AnimLayer", meta = (AllowPrivateAccess))
	TSubclassOf<UAnimInstance> UnarmedCharacterAnimLayer;

	// ���⿡ ���� �޶��� ��
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ASWeaponActor|AnimLayer", meta = (AllowPrivateAccess))
	TSubclassOf<UAnimInstance> ArmedCharacterAnimLayer;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	TObjectPtr<UAnimMontage> EquipAnimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	TObjectPtr<UAnimMontage> UnequipAnimMontage;

	// ���⿡ ���� ��ȿ ��Ÿ�, Units = cm => ������ ������ �� ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess, Units = cm))
	float MaxRange = 25000.0f;

};
