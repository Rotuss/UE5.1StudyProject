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
	
public:
	ASNonViewCharacter();

public:
	virtual void BeginPlay() override;

};
