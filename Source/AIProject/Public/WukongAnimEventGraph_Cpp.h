// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Animation/AnimInstance.h"
#include "CoreMinimal.h"
#include "WukongAnimEventGraph_Cpp.generated.h"

/**
 * 
 */
UCLASS(transient, Blueprintable, hideCategories=AnimInstance,BlueprintType)
class AIPROJECT_API UWukongAnimEventGraph_Cpp:public UAnimInstance
{

	GENERATED_BODY()

public:
	UWukongAnimEventGraph_Cpp();

	UPROPERTY()
		bool KnockedBack;
};
