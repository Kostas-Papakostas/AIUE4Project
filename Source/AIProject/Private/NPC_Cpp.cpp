// Fill out your copyright notice in the Description page of Project Settings.

#include "NPC_Cpp.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/ActorComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SceneComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Math/Rotator.h"
#include "NPC_AI_Cpp.h"
#include "WukongCharracter_Cpp.h"
#include "KwangAnimEventGraph_Cpp.h"
#include "GhostTrail_Cpp.h"

#define RUN_ONCE(runcode) \
do\
{	\
		static bool code_ran = 0; \
		if (!code_ran) {	\
				code_ran = 1; \
				runcode; \
		} \
} while (0)

// Sets default values
ANPC_Cpp::ANPC_Cpp()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	teleport = false;
	HP_Remaining = 4360;

}

float ANPC_Cpp::getTimeToWait() {
	return TimeToWait;
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