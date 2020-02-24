// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Components/TimelineComponent.h"
#include "spawnTrailTimeline.generated.h"

/**
 * 
 */
UCLASS()
class AIPROJECT_API UspawnTrailTimeline : public UActorComponent
{
	GENERATED_BODY()


public:
	UspawnTrailTimeline();
	
public:
	virtual void TickComponent(float DeltaTime,	enum ELevelTick TickType, struct FActorComponentTickFunction* ThisTickFunction) override;

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;
	
	virtual void Activate(bool bReset) override;

	virtual void Deactivate() override;

	virtual bool IsReadyForOwnerToAutoDestroy() const override;

	virtual bool IsPostLoadThreadSafe() const override;

};
