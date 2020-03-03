#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "WukongCharracter_Cpp.generated.h"

UCLASS(config = Game)
class AIPROJECT_API AWukongCharracter_Cpp : public ACharacter
{
	GENERATED_BODY()

		/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

private:
	UPROPERTY(EditAnywhere, Category="Mesh")
	class USkeletalMeshComponent* WukongMesh;

public:
	// Sets default values for this character's properties
	AWukongCharracter_Cpp();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseLookUpRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Staff)
		UCapsuleComponent* StaffTipA;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Staff)
		UCapsuleComponent* ExtendC;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CharParticle)
		UParticleSystem* p_Jump_Cloud;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CharParticle)
		UParticleSystem* p_Wukong_Poke_Hit;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CharParticle)
		UParticleSystem* p_Wukong_Death;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Staff)
		UParticleSystem* p_Wukong_Staff_Impact;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Staff)
		UParticleSystemComponent* p_Wukong_Trail;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Staff)
		UParticleSystemComponent* staff_Particles;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sounds)
		USoundBase* s_Wukong_Distraction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sounds)
		USoundBase* s_Wukong_Hit;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ClonePoint)
		USphereComponent* sphere;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ClonePoint)
		USphereComponent* sphere1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ClonePoint)
		USphereComponent* sphere2; 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ClonePoint)
		USphereComponent* sphere3; 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ClonePoint)
		USphereComponent* sphere4; 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ClonePoint)
		USphereComponent* sphere5; 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ClonePoint)
		USphereComponent* sphere6;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ClonePoint)
		USphereComponent* sphere7;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	class UAnimMontage* hitPoke;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	class UAnimMontage* hitAnimation0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	class UAnimMontage* hitAnimation1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	class UAnimMontage* hitAnimation2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	class UAnimMontage* hitAnimation3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	class UAnimMontage* hitSlam;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actions")
	TArray<TEnumAsByte<EObjectTypeQuery>> objects;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimationHelpers")
		FVector previousLocation;

public:
	UFUNCTION()
		void OnNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload);

	UFUNCTION()
		void OnQslamNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload);

	UFUNCTION()
		void OnNotifyBeginPoke(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload);

	UFUNCTION()
		void OnNotifyEndPoke(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload);

	UFUNCTION()
		void HitSideEvent(FVector HitVector_p);

	UFUNCTION()
		void DamageReceivedEvent(float Damage_p);

	UFUNCTION()
		USkeletalMeshComponent* getSkeletalMesh() { return WukongMesh; };

protected:

	void MoveForward(float Value);

	void MoveRight(float Value);

	void Jump_Function();

	virtual void Landed(const FHitResult &Hit) override;

	void StopJumping_Function();

	void EvadeAction();

	float ComputeSlopeAngle();

	void UltimateAction();

	void PokeHit();

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
		AActor* DamageCauser) override;

	void TurnAtRate(float Rate);

	void LookUpAtRate(float Rate);

	void SimpleHit();

	void UltiCast();

	void EmoteAction();
	
	inline void pickAMontage(int attackCount_p, UAnimInstance* animMontage_p);

	void NoiseDistraction();

public:
	UFUNCTION()
		void DestroyChar();

	UFUNCTION()
		void ResetCombo();

	UFUNCTION()
	void ComboAttackSave();

	UFUNCTION()
	void PokeHitEvent(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void SimpleHitEvent(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/*DEFAULT UE4 STARTER FUNCTIONS*/
	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Resets HMD orientation in VR. */
	void OnResetVR();

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);
	/*DEFAULT UE4 STARTER FUNCTIONS*/

	void Q_SlamEvent();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

public:	

	inline float ComputeCooldown(float Cooldown) { return (UGameplayStatics::GetRealTimeSeconds(GetWorld()) + Cooldown); }

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	TArray<AActor*> IgnoreActors;
	//TArray<USphereComponent*> spheresArray;
	USphereComponent* spheresList[8] = { sphere, sphere1, sphere2, sphere3, sphere4, sphere5, sphere6, sphere7 };//Being used to get clone spawn locations
	TArray<USphereComponent*> spheresArray;

	bool isEvading;
	bool Stunned;
	bool Recovered;
	bool UltiActive;
	bool UltiCallingAndCasting;
	bool DoubleJump;
	bool KnockedBack;
	bool SaveAttack;
	bool IsAttacking;
	bool OnHit;
	bool PokeRdy;
	bool RightClickHit;
	bool FlipFlopEvade; //STARTS AS FALSE, CAUSE THERE IS NOW EVADE
	bool FlipFlopEmote;
	bool UltiRdy;
	bool Qslam;
	float radius;
	float slopeAngle = 0.f;
	float slamCooldown = 0.f;
	float StunTimer=0.f;
	float UltiCooldown = 0.f;
	float PokeCooldownTimer=0.f;
	float UltiTimer;
	float Front, Back, Right, Left;
	int JumpCounter = 0.f;
	int AttackCount = 0.f;
	int HPRemaining = 2350;
};
