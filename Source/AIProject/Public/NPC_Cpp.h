// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PatrolPath_Cpp.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"
#include "Particles/ParticleSystemComponent.h"
//#include "GameFramework/Actor.h"
#include "Particles/ParticleSystem.h"
#include <functional>
#include "NPC_Cpp.generated.h"

UCLASS()
class AIPROJECT_API ANPC_Cpp : public ACharacter
{
	GENERATED_BODY()


public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Variables", meta = (AllowPrivateAccess = "true"))
	class APatrolPath_Cpp* PatrolPath;

	UPROPERTY(EditAnywhere, Category = "Mesh")
	class USkeletalMeshComponent* KwangMesh;


private:
	int HP_Remaining;
	bool teleport;
	float DamageReceived;

public:
	// Sets default values for this character's properties
	ANPC_Cpp();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emitter Variables", meta = (AllowPrivateAccess = "true"))
		UParticleSystem* destroy_Emitter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emitter Variables", meta = (AllowPrivateAccess = "true"))
		UParticleSystem* lightnings_Emitter;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Emitter Variables", meta = (AllowPrivateAccess = "true"))
		UParticleSystemComponent* P_Kwang_Primary_Trail_L;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Emitter Variables", meta = (AllowPrivateAccess = "true"))
		UParticleSystemComponent* sword_Attach;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "General Variables", meta = (AllowPrivateAccess = "true"))
	TArray<TEnumAsByte<EObjectTypeQuery>> objects;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Destroyed() override;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
		AActor* DamageCauser) override;



public:	

	UFUNCTION()
		USceneComponent* getRootComponent() { return RootComponent; }
	
	float getTeleport() { return teleport; };

	UFUNCTION()
		void playerIsDead();

	UFUNCTION()
		APatrolPath_Cpp* getPatrolPath();

	void sendDamage(AActor* damagedActor_p, float baseDamage_p, AActor* damageCauser_p);

	UFUNCTION()
		void HitSideEvent(FVector HitVector_p);

	UFUNCTION(BlueprintNativeEvent, Category = "StunEvents")
		void StartStunLoop();
	void StartStunLoop_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category = "GhostEvents")
		void GhostCount();
	void GhostCount_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category = "GhostEvents")
		void ManyGhosts();
	void ManyGhosts_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category = "StrikesEvents")
		void LightningStrikeOnPlayer();
	void LightningStrikeOnPlayer_Implementation();

	UFUNCTION(BlueprintCallable, Category = "StrikesEvents")
	void LightningStrikeOnPlayer_BPCallable();

	inline void __Run_Once(void (ANPC_Cpp::*func)(AActor *, float, AActor*), AActor* ac, float num, AActor* otherac);
	inline void __Reset_Once() { code_ran = false; };

	void sphereTraces(FVector start, FVector end, float radius, const TArray<TEnumAsByte<EObjectTypeQuery>> &objectsToInteract);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	bool StLoop;
	bool StStart;
	bool UltiRdy;
	bool StrikesAreRdy;
	bool code_ran;
	bool enemyDead;
	float UltCooldown;
	float ThrowSwordCooldown;
	float StunRecoveryTimer;
	float Front, Back, Right, Left;
};
