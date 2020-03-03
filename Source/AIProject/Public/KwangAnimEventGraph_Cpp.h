
#pragma once
#include "Animation/AnimInstance.h"
#include "CoreMinimal.h"
#include "KwangAnimEventGraph_Cpp.generated.h"

 /**
 *
 */
UCLASS(transient, Blueprintable, hideCategories = AnimInstance, BlueprintType)
class AIPROJECT_API UKwangAnimEventGraph_Cpp :public UAnimInstance
{

	GENERATED_BODY()

public:
	UKwangAnimEventGraph_Cpp();

	UPROPERTY()
		bool Death;
	bool playerDead;
};
