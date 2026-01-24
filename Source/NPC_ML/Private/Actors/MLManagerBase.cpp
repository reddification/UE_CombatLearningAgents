#include "LearningAgentsManager.h"
#include "Subsystems/MLSubsystem.h"


// Sets default values
AMLManagerBase::AMLManagerBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	PrimaryActorTick.TickInterval = 0.1;
	
#if WITH_EDITORONLY_DATA
	bIsSpatiallyLoaded = false;
#endif
	
	LearningAgentsManager = CreateDefaultSubobject<ULearningAgentsManager>(TEXT("LearningAgentsManager"));
}

// Called when the game starts or when spawned
void AMLManagerBase::BeginPlay()
{
	Super::BeginPlay();
	auto MLS = GetWorld()->GetSubsystem<UMLSubsystem>();
	MLS->RegisterManager(this, BehaviorTag);
}

void AMLManagerBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (auto WorldLocal = GetWorld())
		if (auto MLS = WorldLocal->GetSubsystem<UMLSubsystem>())
			MLS->UnregisterManager(this, BehaviorTag);
	
	Super::EndPlay(EndPlayReason);
}

void AMLManagerBase::SetManagerActive(bool bNewActive)
{
	bActive = bNewActive;
	SetActorTickEnabled(bActive);
}
