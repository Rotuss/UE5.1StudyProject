// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotify/AN_MuzzleParticle.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Character/SViewCharacter.h"
#include "Item/SWeaponActor.h"

void UAN_MuzzleParticle::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (true == IsValid(MeshComp) && true == IsValid(ParticleSystem))
	{
		ASCharacter* AttackingCharacter = Cast<ASCharacter>(MeshComp->GetOwner());
		if (true == IsValid(AttackingCharacter) && nullptr != AttackingCharacter->GetWeaponInstance())
		{
			//FVector MuzzleLocation = AttackingCharacter->GetWeaponInstance()->GetMesh()->GetSocketLocation(MuzzleName);
			UGameplayStatics::SpawnEmitterAttached(ParticleSystem, AttackingCharacter->GetWeaponInstance()->GetMesh(), MuzzleName);
		}
	}
}
