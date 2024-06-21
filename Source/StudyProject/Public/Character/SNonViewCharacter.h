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

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void SetWidget(UStudyWidget* InStudyWidget) override;

protected:
	virtual void BeginAttack() override;

	virtual void EndAttack(UAnimMontage* InMontage, bool bInterruped) override;

protected:
	// BP_NPCÀÇ FireAttackMontage¿¡ AM_NPC_Fire ÁöÁ¤ ÇÊ¼ö
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess))
	TObjectPtr<UAnimMontage> FireAttackMontage;

	// À§Á¬ ÄÄÆ÷³ÍÆ® ºÎÂø
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	TObjectPtr<class USWidgetComponent> WidgetComponent;

};
