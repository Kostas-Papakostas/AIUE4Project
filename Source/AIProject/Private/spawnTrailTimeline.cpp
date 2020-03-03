
#include "spawnTrailTimeline.h"
#include "Components/TimelineComponent.h"

UspawnTrailTimeline::UspawnTrailTimeline() {


}


void UspawnTrailTimeline::TickComponent(float DeltaTime, enum ELevelTick TickType, struct FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UspawnTrailTimeline::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void UspawnTrailTimeline::Activate(bool bReset)
{
	Super::Activate(bReset);
}

void UspawnTrailTimeline::Deactivate() {
	Super::Deactivate();

}

bool UspawnTrailTimeline::IsReadyForOwnerToAutoDestroy() const
{
	Super::IsReadyForOwnerToAutoDestroy();

	return false;
}

bool UspawnTrailTimeline::IsPostLoadThreadSafe() const
{
	Super::IsPostLoadThreadSafe();

	return false;
}
