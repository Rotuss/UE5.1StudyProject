// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotify/AN_CheckHit.h"
#include "Character/SViewCharacter.h"

void UAN_CheckHit::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (true == IsValid(MeshComp))
	{
		ASCharacter* AttackingCharacter = Cast<ASCharacter>(MeshComp->GetOwner());
		if (true == IsValid(AttackingCharacter))
		{
			AttackingCharacter->OnCheckHit();
		}
	}
}
