// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_MuzzleParticle.generated.h"

/**
 * 
 */
UCLASS()
class STUDYPROJECT_API UAN_MuzzleParticle : public UAnimNotify
{
	GENERATED_BODY()
	
private:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference);

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	TObjectPtr<class UParticleSystem> ParticleSystem;

	FName MuzzleName = TEXT("MuzzleFlash");

};
