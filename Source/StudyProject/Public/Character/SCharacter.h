// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SCharacter.generated.h"

UCLASS()
class STUDYPROJECT_API ASCharacter : public ACharacter
{
	GENERATED_BODY()

	friend class UAN_CheckHit;

public:
	// Sets default values for this character's properties
	ASCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnMeleeAttackMontageEnded(UAnimMontage* InMontage, bool bInterruped);

	UFUNCTION()
	void OnCheckHit();

	UFUNCTION()
	void OnCheckAttackInput();

	// EndCombo()���� �̸� ����
	UFUNCTION()
	virtual void EndAttack(UAnimMontage* InMontage, bool bInterruped);
	
	UFUNCTION()
	void OnCharacterDeath();

	// BeginCombo()���� �̸� ����
	virtual void BeginAttack();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// ���� ������Ʈ�� �̵�
	/*void SetMaxHP(float InMaxHP) { MaxHP = InMaxHP; }

	void SetCurrentHP(float InCurrentHP) { CurrentHP = InCurrentHP; }

	float GetMaxHP() const { return MaxHP; }

	float GetCurrentHP() const { return CurrentHP; }

	bool IsDead() const { return bIsDead; }*/

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	virtual void SetWidget(class UStudyWidget* InStudyWidget) {}

	class USStatComponent* GetStatComponent() const { return StatComponent; }

	class ASWeaponActor* GetWeaponInstance() const { return WeaponInstance; }

public:
	// ��Ÿ�� �ܼ� ������ ����
	static int32 ShowAttackDebug;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ASTPSCharacter", meta = (AllowPrivateAccess))
	TObjectPtr<class USpringArmComponent> SpringArmComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ASTPSCharacter", meta = (AllowPrivateAccess))
	TObjectPtr<class UCameraComponent> CameraComponent;

	// TSubclassOf: ASWeaponActorŬ������ �������� �ڽ� Ŭ�������� ���� �����ϰ�
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	TSubclassOf<class ASWeaponActor> WeaponClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	TObjectPtr<class ASWeaponActor> WeaponInstance;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess))
	float MeleeAttackRange = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess))
	float MeleeAttackRadius = 20.0f;

	// ���� ������Ʈ�� �̵�
	/*UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess))
	float MaxHP = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess))
	float CurrentHP = 200.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess))
	uint8 bIsDead : 1;*/

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	TObjectPtr<class USStatComponent> StatComponent;

	FOnMontageEnded OnMeleeAttackMontageEndedDelegate;

	// ���Ǻ� ���� �̸��� �����ϱ� ������ �̸��� �޴� ������ ���
	FString AttackAnimMontageSectionName = FString(TEXT("Attack"));
	int32 MaxComboCount = 3;
	int32 CurrentComboCount = 0;

	// �ø�������� �ǰų� ���ø����̼� �� �ʿ� �����Ƿ� �׳� bool �ڷ��� ���
	// �ش� Ű�� ���;� �������� �Ѿ �� ����
	bool bIsAttackKeyPressed = false;

	bool bIsNowAttacking = false;
	
};
