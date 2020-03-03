

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "animationInstance.generated.h"

/**
 * 
 */
UCLASS()
class AIPROJECT_API UanimationInstance : public UAnimInstance
{
	GENERATED_BODY()
	
	DECLARE_DELEGATE_OneParam(OnPlayMontageNotifyBegin, AActor*)
	
	


};
