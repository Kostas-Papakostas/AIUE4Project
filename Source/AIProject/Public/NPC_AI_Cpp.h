// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "NPC_Cpp.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BlackboardData.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Hearing.h"
#include "Perception/AISense_Damage.h"
#include "AIController.h"
#include "NPC_AI_Cpp.generated.h"

/**
 * 
 */
UCLASS()
class AIPROJECT_API ANPC_AI_Cpp : public AAIController
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI Modules", meta = (AllowPrivateAccess = "true"))
	class USoundBase* soundEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI Modules", meta = (AllowPrivateAccess = "true"))
	class UBehaviorTree* GeneralTree;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI Modules", meta = (AllowPrivateAccess = "true"))
	class UBehaviorTree* Combat_Tree;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI Modules", meta = (AllowPrivateAccess = "true"))
	class UBlackboardData* BB_Tree;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI Modules", meta = (AllowPrivateAccess = "true"))
	class UBlackboardComponent* BBC_Tree;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI Modules", meta = (AllowPrivateAccess = "true"))
		class UAISenseConfig_Sight* ai_sight;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI Modules", meta = (AllowPrivateAccess = "true"))
		class UAISenseConfig_Hearing* ai_ears;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI Modules", meta = (AllowPrivateAccess = "true"))
		class UAISenseConfig_Damage* ai_damage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI Pawn", meta = (AllowPrivateAccess = "true"))
		class ANPC_Cpp* npc_Pawn;

public:
	ANPC_AI_Cpp();

	virtual void BeginPlay() override;

	virtual void Possess(APawn* Pawn) override;

	virtual void Tick(float DeltaSeconds) override;

	virtual FRotator GetControlRotation() const override;

	UFUNCTION()
		void OnPawnDetected(const TArray<AActor*> &DetectedPawns);

	UFUNCTION()
		void OnPawnSensed(AActor *actor, FAIStimulus stimulusAI);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI Modules")
	class UAIPerceptionComponent* AIPerception;

	UFUNCTION(BlueprintNativeEvent, Category = "DamageEvent")
		void DamageReceivedEvent(int damage);
	void DamageReceivedEvent_Implementation(int damage);

	UFUNCTION(BlueprintNativeEvent, Category = "StunEvents")
		void Stunned();
	void Stunned_Implementation();
	
	UFUNCTION(BlueprintNativeEvent, Category = "StunEvents")
		void Restored();
	void Restored_Implementation();
	
	UFUNCTION(BlueprintNativeEvent, Category = "SmartLinkEvents")
		void SmartLinkJump(FVector GoThere);
	void SmartLinkJump_Implementation(FVector GoThere);
	
};