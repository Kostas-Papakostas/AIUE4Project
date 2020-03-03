
#include "NPC_AI_Cpp.h"
#include "AIController.h"
#include "NPC_Cpp.h"
#include "WukongCharracter_Cpp.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISenseConfig_Damage.h"
#include "Perception/AISense_Damage.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ConstructorHelpers.h"
#include "UObject/Object.h"

//Run once macro(pragma once could also work)
#define RUN_ONCE(runcode) \
do\
{	\
		static bool code_ran = 0; \
		if (!code_ran) {	\
				code_ran = 1; \
				runcode; \
		} \
} while (0)

ANPC_AI_Cpp::ANPC_AI_Cpp(){

	PrimaryActorTick.bCanEverTick = false;

	static ConstructorHelpers::FObjectFinder<USoundBase> soundEffect(TEXT("/Game/ParagonSunWukong/Audio/Cues/Wukong_Intro_Question.Wukong_Intro_Question"));
	AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception Component"));
	//SetPerceptionComponent(*CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception Component")));


	GeneralTree = CreateDefaultSubobject<UBehaviorTree>(TEXT("NPC_BT"));

	ai_sight = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
	ai_sight->SightRadius = 400.f;
	ai_sight->LoseSightRadius = 450.f;
	ai_sight->PeripheralVisionAngleDegrees = 180.f;
	ai_sight->DetectionByAffiliation.bDetectEnemies = true;
	ai_sight->DetectionByAffiliation.bDetectFriendlies = true;
	ai_sight->DetectionByAffiliation.bDetectNeutrals = true;

	ai_damage = CreateDefaultSubobject<UAISenseConfig_Damage>(TEXT("Damage Config"));

	ai_ears = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("Ears Config"));
	ai_ears->HearingRange = 3000.f;
	ai_ears->DetectionByAffiliation.bDetectEnemies = true;
	ai_ears->DetectionByAffiliation.bDetectNeutrals = true;
	ai_ears->DetectionByAffiliation.bDetectFriendlies = true;

	//AIPerception->SetDominantSense(*ai_sight->GetSenseImplementation());
	AIPerception->OnPerceptionUpdated.AddDynamic(this, &ANPC_AI_Cpp::OnPawnDetected);
	AIPerception->OnTargetPerceptionUpdated.AddDynamic(this, &ANPC_AI_Cpp::OnPawnSensed);
	AIPerception->ConfigureSense(*ai_sight);
	AIPerception->ConfigureSense(*ai_ears);
	AIPerception->ConfigureSense(*ai_damage);


	/*AIPerception->ConfigureSense(*ai_sight);
	AIPerception->SetDominantSense(ai_sight->GetSenseImplementation());*/

}

void ANPC_AI_Cpp::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);
}

void ANPC_AI_Cpp::Possess(APawn* Pawn) {
	Super::Possess(Pawn);
}

void ANPC_AI_Cpp::BeginPlay() {//BLACKBOARD VALUES INITIALIZED 
	Super::BeginPlay();

	UKismetSystemLibrary::Delay(this, 5.f, FLatentActionInfo::FLatentActionInfo());
	AAIController::RunBehaviorTree(GeneralTree);

	npc_Pawn = Cast<ANPC_Cpp>(AController::GetPawn());
	if (npc_Pawn) {
		AAIController::GetBlackboardComponent()->SetValueAsBool(UKismetSystemLibrary::MakeLiteralName("PathLooping"), npc_Pawn->getPatrolPath()->IsLooping);
		AAIController::GetBlackboardComponent()->SetValueAsBool(UKismetSystemLibrary::MakeLiteralName("CanSeePlayer"), false);
		AAIController::GetBlackboardComponent()->SetValueAsFloat(UKismetSystemLibrary::MakeLiteralName("Time"), npc_Pawn->getPatrolPath()->WaitTime);
		AAIController::GetBlackboardComponent()->SetValueAsBool(UKismetSystemLibrary::MakeLiteralName("IsInFightMode"), false);
		AAIController::GetBlackboardComponent()->SetValueAsBool(UKismetSystemLibrary::MakeLiteralName("IsRetreating"), false);
	}
}

FRotator ANPC_AI_Cpp::GetControlRotation() const {//RETURNS CONTROLLER ROTATION
	if (GetPawn() == nullptr) {
		return FRotator(0.f, 0.f, 0.f);
	}
	return FRotator(0.f, GetPawn()->GetActorRotation().Yaw, 0.f);
}

void ANPC_AI_Cpp::OnPawnSensed(AActor *actor, FAIStimulus stimulusAI) {
	//EVENT CALLED WHEN A PAWN SENSED
	if (Cast<AWukongCharracter_Cpp>(actor)!=nullptr) {//checks for a successsful dynamic_cast
		
		UAIPerceptionSystem::GetSenseClassForStimulus(GetWorld(), stimulusAI);

		bool sensed = stimulusAI.WasSuccessfullySensed();

		//Value chenge for each of the 2 blackboards
		AAIController::GetBlackboardComponent()->SetValueAsBool(UKismetSystemLibrary::MakeLiteralName("CanSeePlayer"), sensed);

		AAIController::GetBlackboardComponent()->SetValueAsBool(
			UKismetSystemLibrary::MakeLiteralName("OnSight"), sensed);
	}
}

/*******IS BEING USED TO DETECT NOISE FROM PLEYER*******/
void ANPC_AI_Cpp::OnPawnDetected(const TArray<AActor*> &DetectedPawns) {
	FActorPerceptionBlueprintInfo info;
	for (AActor* actor : DetectedPawns) {
		AIPerception->GetActorsPerception(actor, info);
		for (FAIStimulus lastSense : info.LastSensedStimuli) {
			FVector stLocation = lastSense.StimulusLocation;//stores player's noise location
			FVector rcvLocation = lastSense.ReceiverLocation;//location to play sound
			
			if (lastSense.Tag.ToString().Equals("Noise",ESearchCase::IgnoreCase )) {
				AAIController::GetBlackboardComponent()->SetValueAsBool(UKismetSystemLibrary::MakeLiteralName("IsInvestigating"), true);
				AAIController::GetBlackboardComponent()->SetValueAsVector(UKismetSystemLibrary::MakeLiteralName("TargetLocation"), stLocation);
				UGameplayStatics::PlaySoundAtLocation( UObject::GetWorld(), soundEffect, rcvLocation, FRotator(0.f, 0.f, 0.f), 1, 1, 0, nullptr, nullptr, nullptr);
			}
			else {
				AAIController::GetBlackboardComponent()->SetValueAsBool(UKismetSystemLibrary::MakeLiteralName("IsInvestigating"), false);
			}
		}
	}
}

/*EVENT CALLED DELEGATED WHEN DAMAGE RECEIVED*/
void ANPC_AI_Cpp::DamageReceivedEvent_Implementation(int damage) {
	if (damage > 20) {
		RunBehaviorTree(Combat_Tree);
		UseBlackboard(BB_Tree, BBC_Tree);
		RUN_ONCE(BBC_Tree->SetValueAsBool(UKismetSystemLibrary::MakeLiteralName("StrikesAreRdy"), true));
	}
}

/*CHANGE STUN STATEMENT*/
void ANPC_AI_Cpp::Stunned_Implementation() {
	AAIController::GetBlackboardComponent()->SetValueAsBool(UKismetSystemLibrary::MakeLiteralName("GotStunned"), true);
	if (BBC_Tree != NULL) {
		BBC_Tree->SetValueAsBool(UKismetSystemLibrary::MakeLiteralName("Stunned"), true);
	}
}

/*CHANGE STUN STATEMENT*/
void ANPC_AI_Cpp::Restored_Implementation(){
	AAIController::GetBlackboardComponent()->SetValueAsBool(UKismetSystemLibrary::MakeLiteralName("GotStunned"), false);
	if (BBC_Tree != NULL) {
		BBC_Tree->SetValueAsBool(UKismetSystemLibrary::MakeLiteralName("Stunned"), false);
	}
}

/*SMART LINK ACTIVATION*/
void ANPC_AI_Cpp::SmartLinkJump_Implementation(FVector GoThere) {
	AAIController::GetBlackboardComponent()->SetValueAsVector(UKismetSystemLibrary::MakeLiteralName("TargetLocation"), GoThere);
}