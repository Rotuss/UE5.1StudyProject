// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/SCharacter.h"
#include "SNonViewCharacter.generated.h"

/**
 * 
 */
UCLASS()
class STUDYPROJECT_API ASNonViewCharacter : public ASCharacter
{
	GENERATED_BODY()
	
	friend class UBTTask_Attack;

public:
	ASNonViewCharacter();

public:
	virtual void BeginPlay() override;

protected:
	virtual void BeginAttack() override;

	virtual void EndAttack(UAnimMontage* InMontage, bool bInterruped) override;

protected:
	// BP_NPC의 FireAttackMontage에 AM_NPC_Fire 지정 필수
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess))
	TObjectPtr<UAnimMontage> FireAttackMontage;

};
