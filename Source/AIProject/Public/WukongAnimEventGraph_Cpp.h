// TI BE COMPLETED

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
	bool Emote;
	bool UltiCall;
	bool Dead;
};
