# AIUE4Project</br> 
Just the source code, project files on google drive <a>https://drive.google.com/file/d/12HPOAIDQnu-1tOEU5CWAlQHHJrGjq-lj/view?usp=sharing</a>

The project uses both blueprints and C++ code, the blueprints parts cannot be displayed here so made them accessible through the link above.

**Note: Since it is not a long term project, the player and npc are completely different classes and they don't share a common mother class for characters**

# Player character related code:</br>

Character Initializations:</br>

```
.
.
.
  WukongMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WukongSkeletalMesh"));
	WukongMesh->SetupAttachment(GetCapsuleComponent());
	WukongMesh->SetAnimInstanceClass(UWukongAnimEventGraph_Cpp::StaticClass());

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 420.f;//need to be changed
	GetCharacterMovement()->AirControl = 0.05f;
	GetCharacterMovement()->BrakingDecelerationFalling = 50.f;
	GetCharacterMovement()->AirControlBoostMultiplier = 1.5f;
	GetCharacterMovement()->GravityScale = 2.f;
	GetCharacterMovement()->MaxCustomMovementSpeed = 900.f;
	
	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetCapsuleComponent());
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	CameraBoom->ProbeChannel = ECollisionChannel::ECC_WorldStatic;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
.
.
.
```
</br>

Computing slope angle and using the value in the corresponding blendspace of the character. This function is being called in the tick function.
```
float AWukongCharracter_Cpp::ComputeSlopeAngle()
{
	//BEING USED IN THE BLENDSPACE IN ORDER TO CHENGE CHARRACTER'S SLOPE ANGLE
	//i.e. upstairs downstairs
	FRotator look = UKismetMathLibrary::FindLookAtRotation(previousLocation, this->GetActorLocation());
	previousLocation = previousLocation;
	
	float radiansAtan = UKismetMathLibrary::DegreesToRadians((float)atan(look.Pitch) * 180 / PI);
	return (radiansAtan*10.f);
}
```

Attack collision notifications. This part picks a montage of a simple hit(I use combo style mechanics and I need 4 montages of animations).
```
void AWukongCharracter_Cpp::SimpleHit() {//THIS TRIGGERS THE HIT
	if (!isEvading && !Stunned && !UltiActive && !KnockedBack) {
		if (IsAttacking) {
			SaveAttack = true;
		}
		else {
			IsAttacking = true;
			UAnimInstance* animMontage = WukongMesh->GetAnimInstance();
			pickAMontage(AttackCount, animMontage);
		}
	}
}

/*PICKS THE ANIM MONTAGE TO PLAY*/
inline void AWukongCharracter_Cpp::pickAMontage(int attaCount_p, UAnimInstance* animMontage_p) {
	switch (AttackCount) {
	case 0:
		AttackCount = 1;
		if (hitAnimation0 != NULL) {
			animMontage_p->Montage_Play(hitAnimation0, 1.f);
			animMontage_p->OnPlayMontageNotifyBegin.AddDynamic(this, &AWukongCharracter_Cpp::OnNotifyBegin);
		}
	case 1:
		AttackCount = 2;
		if (hitAnimation1 != NULL) {
			animMontage_p->Montage_Play(hitAnimation1, 1.f);
			animMontage_p->OnPlayMontageNotifyBegin.AddDynamic(this, &AWukongCharracter_Cpp::OnNotifyBegin);
		}
	case 2:
		AttackCount = 0;
		if (hitAnimation2 != NULL) {
			animMontage_p->Montage_Play(hitAnimation2, 1.f);
			animMontage_p->OnPlayMontageNotifyBegin.AddDynamic(this, &AWukongCharracter_Cpp::OnNotifyBegin);
		}
	case 3:
		AttackCount = 4;
		if (hitAnimation3 != NULL) {
			animMontage_p->Montage_Play(hitAnimation3, 1.f);
			animMontage_p->OnPlayMontageNotifyBegin.AddDynamic(this, &AWukongCharracter_Cpp::OnNotifyBegin);
		}

	}
}
```

Hit side detection received, animation blueprint reads the the correct side and plays one of the correct hit animations(Front, Back, Left, Right). Most of the flags are being read from the anim blueprint and it's written completely in blueprint.

```
void AWukongCharracter_Cpp::HitSideEvent(FVector HitVector_p)
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


	if (UKismetMathLibrary::InRange_FloatFloat(UKismetMathLibrary::Dot_VectorVector(HitVector, DirectionVector), -1, -.7, true, true)) {
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
```

# NPC Player class:

Behavior tree and animation blueprint are blueprint pure. Movement are blueprint tasks and not in C++.

Lightning ability functions:

```
/*Throw strikes on player and applies damage*/
void ANPC_Cpp::LightningStrikeOnPlayer_Implementation()
{
	FVector vector_Fusion;
	FVector* make_Array = new FVector[4];
	USkeletalMeshComponent* temp_Sk = Cast<AWukongCharracter_Cpp>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))->getSkeletalMesh();// skeletal variable

	UCapsuleComponent* temp_Cap = Cast<AWukongCharracter_Cpp>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))->GetCapsuleComponent();// capsule variable

	vector_Fusion.Set(temp_Cap->GetComponentLocation().X, temp_Cap->GetComponentLocation().Y, temp_Sk->GetComponentLocation().Z);

	/*4 strikes so 4 random points in 3d space*/
	make_Array[0] = vector_Fusion;
	make_Array[1] = UKismetMathLibrary::RandomUnitVector()*UKismetMathLibrary::RandomFloatInRange(150.f, 500.f) + vector_Fusion;
	make_Array[2] = UKismetMathLibrary::RandomUnitVector()*UKismetMathLibrary::RandomFloatInRange(150.f, 500.f) + vector_Fusion;
	make_Array[3] = UKismetMathLibrary::RandomUnitVector()*UKismetMathLibrary::RandomFloatInRange(150.f, 500.f) + vector_Fusion;

	/*emitter spawning*/
	/*on each of the 4 points*/
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
```
