// Fill out your copyright notice in the Description page of Project Settings.
#include "PulseSphere_Cpp.h"
#include "Engine/StaticMeshActor.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "WukongAnimEventGraph_Cpp.h"
#include "WukongCharracter_Cpp.h"


APulseSphere_Cpp::APulseSphere_Cpp() {


	pulseSphereMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PulseSphere"));
	pulseSphereMesh->bVisible = false;

	KwangShield = CreateDefaultSubobject<USphereComponent>(TEXT("Shield"));
	WukongOverlaps = CreateDefaultSubobject<USphereComponent>(TEXT("PushPulse"));
	
}

void APulseSphere_Cpp::BeginPlay() {
	Super::BeginPlay();
	KwangShield->OnComponentBeginOverlap.AddDynamic(this, &APulseSphere_Cpp::OnBeginOverlap);
	KwangShield->OnComponentEndOverlap.AddDynamic(this, &APulseSphere_Cpp::OnOverlapEnd);
	WukongOverlaps->OnComponentBeginOverlap.AddDynamic(this, &APulseSphere_Cpp::OnWukongBeginOverlap);
}

void APulseSphere_Cpp::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (UKismetSystemLibrary::GetDisplayName(OtherActor).Equals("NPC_Cpp_BP")) {
		NPCOverlaps = true;
	}
}

void APulseSphere_Cpp::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	if (UKismetSystemLibrary::GetDisplayName(OtherActor).Equals("NPC_Cpp_BP")) {
		NPCOverlaps = false;
	}
}

void APulseSphere_Cpp::OnWukongBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) 
{
	if (UKismetSystemLibrary::GetDisplayName(OtherActor).Equals("WukongCharracter_Cpp_BP")&&NPCOverlaps) 
	{
		AWukongCharracter_Cpp* tempChar = Cast<AWukongCharracter_Cpp>(OtherActor);
		{
			pulseSphereMesh->ToggleVisibility();

			if (tempChar != nullptr) {
				FRotator rotateToPlayer = UKismetMathLibrary::FindLookAtRotation(OtherActor->GetActorLocation(), tempChar->GetCapsuleComponent()->GetComponentLocation());

				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), pulseEmitter, OtherActor->GetActorLocation(), rotateToPlayer, FVector::OneVector);
			}
			UKismetSystemLibrary::Delay(this, 1.f, FLatentActionInfo::FLatentActionInfo());
			pulseSphereMesh->ToggleVisibility();
		}

		{
			tempChar->GetCharacterMovement()->AirControl = 1.f;
			Cast<UWukongAnimEventGraph_Cpp>(tempChar->GetMesh()->GetAnimInstance())->KnockedBack = true;
			tempChar->LaunchCharacter(UKismetMathLibrary::GetForwardVector({ 0.f,tempChar->GetControlRotation().Yaw, 0.f })*-2100, true, false);
			tempChar->GetCharacterMovement()->AirControl = 0.05;
			UGameplayStatics::ApplyDamage(tempChar, 10.f, NULL, this, NULL);
		}
	}
}