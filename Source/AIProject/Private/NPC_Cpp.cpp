// Fill out your copyright notice in the Description page of Project Settings.

#include "NPC_Cpp.h"
#include "UObject/ConstructorHelpers.h"
#include "math.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/ActorComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Math/Rotator.h"
#include "NPC_AI_Cpp.h"
#include "WukongCharracter_Cpp.h"
#include "KwangAnimEventGraph_Cpp.h"
#include "DestructibleComponent.h"
#include "GhostTrail_Cpp.h"
#include <functional>

#define RUN_ONCE(runcode) \
do\
{	\
		static bool coder=1;\
		if (!coder) {	\
				coder = 0; \
				runcode; \
		} \
} while (0)

#define RUN_ONCE_RESET() \
do\
{	\
		 \
} while (0)

// Sets default values
ANPC_Cpp::ANPC_Cpp()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	StLoop=true;
	StStart=true;
	UltiRdy=true;
	StrikesAreRdy=true;
	code_ran = false;

	KwangMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Char_Mesh"));
	KwangMesh->SetOnlyOwnerSee(true);
	KwangMesh->SetupAttachment(GetCapsuleComponent());
	KwangMesh->CastShadow = true;
	KwangMesh->bCastDynamicShadow = true;
	KwangMesh->SetAnimInstanceClass(UKwangAnimEventGraph_Cpp::StaticClass());
	
	P_Kwang_Primary_Trail_L = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Hand Effect Left"));
	P_Kwang_Primary_Trail_L->SetupAttachment(KwangMesh);

	sword_Attach = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Sword Attach Effect"));
	sword_Attach->SetupAttachment(KwangMesh);

	/*Collision object types initialization*/
	FCollisionObjectQueryParams(ECC_TO_BITFIELD(ECC_WorldStatic) |
		ECC_TO_BITFIELD(ECC_PhysicsBody) |
		ECC_TO_BITFIELD(ECC_Destructible) |
		ECC_TO_BITFIELD(ECC_Pawn));
	objects.Add(EObjectTypeQuery::ObjectTypeQuery1);
	objects.Add(EObjectTypeQuery::ObjectTypeQuery3);
	objects.Add(EObjectTypeQuery::ObjectTypeQuery4);
	objects.Add(EObjectTypeQuery::ObjectTypeQuery6);

	teleport = false;
	HP_Remaining = 4360;
	code_ran = false;
}


// Called when the game starts or when spawned
void ANPC_Cpp::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ANPC_Cpp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (UGameplayStatics::GetRealTimeSeconds(GetWorld()) >= StunRecoveryTimer) {
		StLoop = false;
		ANPC_AI_Cpp* controller = Cast<ANPC_AI_Cpp>(ThisClass::GetController());
		if (controller != nullptr) {
			controller->Restored_Implementation();
			Cast<AAIController>(ThisClass::GetController())->EnableInput(nullptr);
		}
	}

	if (UGameplayStatics::GetRealTimeSeconds(GetWorld()) >= ThrowSwordCooldown) {
		StrikesAreRdy = true;
	}

	if (UGameplayStatics::GetRealTimeSeconds(GetWorld()) >= UltCooldown) {
		UltiRdy = true;
	}

	if ((!UltiRdy) && (!StrikesAreRdy)) {
		P_Kwang_Primary_Trail_L->SetActive(false, false);
		P_Kwang_Primary_Trail_L->SetActive(true, false);
	}

	if (HP_Remaining <= 0) {
		KwangMesh->GetAnimInstance();
		Cast<UKwangAnimEventGraph_Cpp>(KwangMesh->GetAnimInstance())->Death=true;
		GetCharacterMovement()->DisableMovement();
	}

}

APatrolPath_Cpp* ANPC_Cpp::getPatrolPath() {
	return PatrolPath;
}

// Called to bind functionality to input
void ANPC_Cpp::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ANPC_Cpp::Destroyed() {
	Super::Destroyed();

	TArray<AActor*> players;
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), destroy_Emitter, ThisClass::GetActorLocation(), FRotator::ZeroRotator, { 10,10,10 });
	UKismetSystemLibrary::Delay(this, 5, FLatentActionInfo::FLatentActionInfo());
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AWukongCharracter_Cpp::StaticClass(), players);
	AController* temp_Controller = Cast<APawn>(players.operator[](0))->GetController();
	
	UKismetSystemLibrary::QuitGame(GetWorld(), Cast<APlayerController>(temp_Controller), EQuitPreference::Quit);
}

float ANPC_Cpp::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser){
	
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (teleport) {
		DamageReceived = DamageAmount / 2;
		HP_Remaining = HP_Remaining - (int)DamageReceived;
		ANPC_AI_Cpp* AIController = Cast<ANPC_AI_Cpp>(ThisClass::GetController());
		if (AIController != nullptr) {
			AIController->DamageReceivedEvent_Implementation(DamageAmount);
		}
	}
	else {
		HP_Remaining = HP_Remaining - (int)DamageAmount;
		ANPC_AI_Cpp* AIController = Cast<ANPC_AI_Cpp>(ThisClass::GetController());
		if (AIController != nullptr) {
			AIController->DamageReceivedEvent_Implementation(DamageAmount);
		}
	}

	return (float)HP_Remaining;

}

void ANPC_Cpp::StartStunLoop_Implementation() {
	StLoop = true;
	StStart = false;
	Cast<ANPC_AI_Cpp>(ThisClass::GetController())->Stunned_Implementation();
}

void ANPC_Cpp::GhostCount_Implementation() {
	GetWorld()->SpawnActor<AGhostTrail_Cpp>(GetClass(), KwangMesh->GetComponentToWorld());
}

void ANPC_Cpp::ManyGhosts_Implementation() {
	FRotator GhostRotation = UKismetMathLibrary::FindLookAtRotation(KwangMesh->GetComponentToWorld().GetLocation(),
		KwangMesh->GetRightVector());
	FTransform GhostTransform = KwangMesh->GetComponentToWorld();
	GhostTransform.SetRotation(GhostRotation.Quaternion());
	GetWorld()->SpawnActor<AGhostTrail_Cpp>(GetClass(), GhostTransform);

	FRotator secondGhostRotation = UKismetMathLibrary::FindLookAtRotation(KwangMesh->GetComponentToWorld().GetLocation(), KwangMesh->GetRightVector()*-1);
	GhostTransform.SetRotation(secondGhostRotation.Quaternion());
	GetWorld()->SpawnActor<AGhostTrail_Cpp>(GetClass(), GhostTransform);
}

void ANPC_Cpp::LightningStrikeOnPlayer_BPCallable()
{
	if (StrikesAreRdy) {
		StrikesAreRdy = false;
	}
}

void ANPC_Cpp::LightningStrikeOnPlayer_Implementation()
{
	FVector vector_Fusion;
	FVector* make_Array = new FVector[4];
	USkeletalMeshComponent* temp_Sk = Cast<AWukongCharracter_Cpp>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))->getSkeletalMesh();// ->GetComponentLocation();

	UCapsuleComponent* temp_Cap = Cast<AWukongCharracter_Cpp>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))->GetCapsuleComponent();// ->GetForwardVector();

	vector_Fusion.Set(temp_Cap->GetComponentLocation().X, temp_Cap->GetComponentLocation().Y, temp_Sk->GetComponentLocation().Z);


	make_Array[0] = vector_Fusion;
	make_Array[1] = UKismetMathLibrary::RandomUnitVector()*UKismetMathLibrary::RandomFloatInRange(150.f, 500.f) + vector_Fusion;
	make_Array[2] = UKismetMathLibrary::RandomUnitVector()*UKismetMathLibrary::RandomFloatInRange(150.f, 500.f) + vector_Fusion;
	make_Array[3] = UKismetMathLibrary::RandomUnitVector()*UKismetMathLibrary::RandomFloatInRange(150.f, 500.f) + vector_Fusion;


	

	/*emitter spawning*/
	for (int i = 0; i < 4; i++) {
		switch (i) {
		case 1 :
			UKismetSystemLibrary::Delay(GetWorld(), 1, FLatentActionInfo::FLatentActionInfo());
		case 2 :
			UKismetSystemLibrary::Delay(GetWorld(), 1.5, FLatentActionInfo::FLatentActionInfo());
		case 3 :
			UKismetSystemLibrary::Delay(GetWorld(), 0.8, FLatentActionInfo::FLatentActionInfo());
		}

		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), lightnings_Emitter, make_Array[i], FRotator::ZeroRotator, FVector::OneVector, true, EPSCPoolMethod::None);

		/*sphere creation after each spawn*/
		sphereTraces(vector_Fusion, temp_Cap->GetForwardVector(), 350.0, objects);
		UKismetSystemLibrary::Delay(GetWorld(), 0.4, FLatentActionInfo::FLatentActionInfo());
		
	}
}

void ANPC_Cpp::sphereTraces(FVector start, FVector end, float radius, const TArray<TEnumAsByte<EObjectTypeQuery>>& objectsToInteract)
{
	TArray<FHitResult> hitResults;
	TArray<AActor*> ignore;
	UKismetSystemLibrary::SphereTraceMultiForObjects(GetWorld(), start, end, radius, objectsToInteract, false, ignore, EDrawDebugTrace::None, hitResults, true);

	for (FHitResult temp : hitResults) {

		FVector impactPulse = temp.ImpactPoint*FVector( 0,0,UKismetMathLibrary::RandomFloatInRange(100, 180) );
		if (temp.Component->IsSimulatingPhysics()) {
			Cast<UStaticMeshComponent>(temp.Component)->AddImpulseAtLocation(impactPulse, temp.Location, NAME_None);
			Cast<UDestructibleComponent>(temp.Component)->AddImpulseAtLocation(impactPulse, temp.Location, NAME_None);  /*WORKS IF THE PLUGIN DEPENDENCY ADDED*/

		}

		if (UKismetSystemLibrary::GetDisplayName(Cast<UObject>(temp.Actor)) == "WukongCharracter_Cpp") {
			
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),lightnings_Emitter, temp.Location, FRotator(0, 0, 0), FVector(1, 1, 1), true, EPSCPoolMethod::None);
			goto if_Jump;
		}
		else {
		if_Jump:	
			if (!Cast<AWukongCharracter_Cpp>(temp.Actor)->GetCharacterMovement()->IsFalling() &&
				!Cast<AWukongCharracter_Cpp>(temp.Actor)->isEvading) {

				//RUN_ONCE(sendDamage(temp.GetActor(), 125.f, this)); //Maybe function instead of Macros
				__Run_Once(&ANPC_Cpp::sendDamage,temp.GetActor(),125.f,this);
			}
		}
	}//RUN_ONCE_RESET(); //Maybe function instead of Macros
	__Reset_Once();
}

inline void ANPC_Cpp::__Run_Once(void (ANPC_Cpp::*func)(AActor *, float, AActor*) , AActor* ac, float num, AActor* otherac)
{
	if (!code_ran) {
		//func( ac, num, otherac);
		sendDamage(ac, num, otherac);
		code_ran = true;
	}
}

void ANPC_Cpp::sendDamage(AActor* damagedActor_p, float baseDamage_p, AActor* damageCauser_p) {
	UGameplayStatics::ApplyDamage(damagedActor_p, baseDamage_p, nullptr, damageCauser_p, NULL);
	Cast<AWukongCharracter_Cpp>(damagedActor_p)->Stunned = true;
	Cast<AWukongCharracter_Cpp>(damagedActor_p)->StunTimer = UGameplayStatics::GetRealTimeSeconds(GetWorld()) + 1.5;
	Cast<AWukongCharracter_Cpp>(damagedActor_p)->Recovered = false;

}

void ANPC_Cpp::HitSideEvent(FVector HitVector_p)
{
	FVector HitVector = HitVector_p;
	FVector DirectionVector;
	FVector capsuleLocation = GetCapsuleComponent()->GetComponentLocation();
	FRotator capsuleRotation = GetCapsuleComponent()->GetComponentRotation();

	DirectionVector = capsuleLocation - (UKismetMathLibrary::GetRightVector(capsuleRotation) + capsuleLocation);
	float sqrtForNormalization = sqrtf(powf(HitVector.X, 2) + powf(HitVector.Y, 2) + powf(HitVector.Z, 2));

	/*NORMALIZED HITVECTOR*/
	HitVector = FVector(HitVector.X / sqrtForNormalization, HitVector.Y / sqrtForNormalization, HitVector.Z / sqrtForNormalization);

	/*NORMALIZED DIRECTIONVECTOR*/
	DirectionVector = FVector(DirectionVector.X / sqrtForNormalization, DirectionVector.Y / sqrtForNormalization, DirectionVector.Z / sqrtForNormalization);

	
	if (UKismetMathLibrary::InRange_FloatFloat(UKismetMathLibrary::Dot_VectorVector(HitVector, DirectionVector) , -1, -.7, true, true)) {
		Front = 1.0;
		UKismetSystemLibrary::Delay(GetWorld(), .7, FLatentActionInfo::FLatentActionInfo());
		Front = 0;
	}
	else if (UKismetMathLibrary::InRange_FloatFloat(UKismetMathLibrary::Dot_VectorVector(HitVector, DirectionVector), .7, 1, true, true)) {
		Back = 1;
		UKismetSystemLibrary::Delay(GetWorld(), .7, FLatentActionInfo::FLatentActionInfo());
		Back = 0;
	}
	else if (UKismetMathLibrary::InRange_FloatFloat(UKismetMathLibrary::Dot_VectorVector(HitVector, DirectionVector), -.7, .7, true, true)) {

		if (UKismetMathLibrary::Cross_VectorVector(HitVector, DirectionVector).Z<0) {
			Right = 1;
			UKismetSystemLibrary::Delay(GetWorld(), .7, FLatentActionInfo::FLatentActionInfo());
			Right = 0;
		}
		else if (UKismetMathLibrary::Cross_VectorVector(HitVector, DirectionVector).Z>0) {
			Left = 1;
			UKismetSystemLibrary::Delay(GetWorld(), .7, FLatentActionInfo::FLatentActionInfo());
			Left = 0;
		}
	}
}

void ANPC_Cpp::playerIsDead()
{
	Cast<UKwangAnimEventGraph_Cpp>(KwangMesh->GetAnimInstance())->playerDead = true;
	enemyDead = true;
	GetCharacterMovement()->StopMovementImmediately();
	this->DisableInput(nullptr);
}