// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PatrolPath_Cpp.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"
#include "Particles/ParticleSystemComponent.h"
//#include "GameFramework/Actor.h"
#include "Particles/ParticleSystem.h"
#include "NPC_Cpp.generated.h"

UCLASS()
class AIPROJECT_API ANPC_Cpp : public ACharacter
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Variables", meta = (AllowPrivateAccess = "true"))
	class APatrolPath_Cpp* PatrolPath;

	UPROPERTY(EditAnywhere, Category = "Mesh")
	class USkeletalMeshComponent* KwangMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Variables", meta = (AllowPrivateAccess = "true"))
	int HP_Remaining;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Variables", meta = (AllowPrivateAccess = "true"))
	bool teleport;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Variables", meta = (AllowPrivateAccess = "true"))
		float DamageReceived;

public:
	// Sets default values for this character's properties
	ANPC_Cpp();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Variables", meta = (AllowPrivateAccess = "true"))
		float TimeToWait;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Variables", meta = (AllowPrivateAccess = "true"))
		UParticleSystem* destroy_Emitter;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Variables", meta = (AllowPrivateAccess = "true"))
		UParticleSystemComponent* P_Kwang_Primary_Trail_L;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Destroyed() override;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
		AActor* DamageCauser) override;



public:	

	UFUNCTION()
		float getTimeToWait();

	UFUNCTION()
		APatrolPath_Cpp* getPatrolPath();

	UFUNCTION(BlueprintNativeEvent, Category = "StunEvents")
		void StartStunLoop();
	void StartStunLoop_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category = "GhostEvents")
		void GhostCount();
	void GhostCount_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category = "GhostEvents")
		void ManyGhosts();
	void ManyGhosts_Implementation();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	float StunRecoveryTimer;
	bool StLoop;
	bool StStart;
	bool UltiRdy;
	bool StrikesAreRdy;
	float UltCooldown;
	float ThrowSwordCooldown;
	
};
