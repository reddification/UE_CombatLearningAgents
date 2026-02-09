#include "Actors/TrainingEpisodePCG.h"

#include "PCGComponent.h"
#include "PCGGraph.h"
#include "Components/BoxComponent.h"

ATrainingEpisodePCG::ATrainingEpisodePCG()
{
	PrimaryActorTick.bCanEverTick = false;
	
	PCGComponent = CreateDefaultSubobject<UPCGComponent>(TEXT("PCG Component"));
	PCGComponent->GenerationTrigger = EPCGComponentGenerationTrigger::GenerateOnDemand;
	
	BoundsComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Bounds volume"));
	BoundsComponent->SetCollisionProfileName(FName("NoCollision"));
	BoundsComponent->SetCanEverAffectNavigation(false);
	SetRootComponent(BoundsComponent);
	
#if WITH_EDITOR
	bIsSpatiallyLoaded = false;
#endif
}

void ATrainingEpisodePCG::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	PCGComponent->OnPCGGraphCleanedDelegate.AddUObject(this, &ATrainingEpisodePCG::OnPCGCleanupCompleted);
	PCGComponent->OnPCGGraphGeneratedDelegate.AddUObject(this, &ATrainingEpisodePCG::OnPCGGenerateCompleted);
}

void ATrainingEpisodePCG::SetPCG(UPCGGraph* NewPcgGraph, bool bGenerateImmediately)
{
	PCGComponent->Cleanup();
	if (!PCGComponent->IsCleaningUp())
	{
		PCGComponent->SetGraph(NewPcgGraph);
		NewGraphSetEvent.Broadcast();
		if (bGenerateImmediately)
			PCGComponent->Generate();
	}
	else
	{
		bPendingGraphChange = true;
		PendingGraphReplacement = NewPcgGraph;
		bGenerateGraphWhenChanged = bGenerateImmediately;
	}
}

bool ATrainingEpisodePCG::Generate()
{
	if (PCGComponent->GetGraph())
	{
		PCGComponent->Generate();
		return PCGComponent->IsGenerating();
	}
	
	return false;
}

bool ATrainingEpisodePCG::Cleanup()
{
	PCGComponent->Cleanup();
	return PCGComponent->IsCleaningUp();
}

bool ATrainingEpisodePCG::CleanupAndDestroy()
{
	bPendingDestroy = true;
	PCGComponent->Cleanup();
	bool bStartedCleanup = PCGComponent->IsCleaningUp();
	if (!bStartedCleanup)
		Destroy();
	
	return bStartedCleanup;
}

bool ATrainingEpisodePCG::IsReady() const
{
	return !PCGComponent->IsCleaningUp();
}

void ATrainingEpisodePCG::SetSeed(int32 NewSeed, const FName& EpisodeSeedParameterName)
{
	PCGComponent->Seed = NewSeed;
	auto PCGGraph = PCGComponent->GetGraphInstance();
	if (!ensure(PCGGraph))
		return;
	
	PCGGraph->SetGraphParameter(EpisodeSeedParameterName, NewSeed);
}

void ATrainingEpisodePCG::SetParameters(const FPcgParametersContainer& Parameters)
{
	auto PCGGraph = PCGComponent->GetGraphInstance();
	if (!ensure(PCGGraph))
		return;
	
	for (const auto& FloatParameter : Parameters.FloatParameters)
	{
		auto ParameterSetResult = PCGGraph->SetGraphParameter(FloatParameter.Key, FloatParameter.Value);
		ensure(ParameterSetResult == EPropertyBagResult::Success);
	}
}

void ATrainingEpisodePCG::SetExtents(const FVector& Extents)
{
	BoundsComponent->SetBoxExtent(Extents);
}

void ATrainingEpisodePCG::OnPCGCleanupCompleted(UPCGComponent* PcgComponent)
{
	CleanupCompletedEvent.Broadcast();
	
	if (bPendingDestroy)
	{
		Destroy();
	}
	else if (bPendingGraphChange)
	{
		PCGComponent->SetGraph(PendingGraphReplacement);
		PendingGraphReplacement = nullptr;
		bPendingGraphChange = false;
		
		if (bGenerateGraphWhenChanged)
			PCGComponent->Generate();
	}
}

void ATrainingEpisodePCG::OnPCGGenerateCompleted(UPCGComponent* PcgComponent)
{
	GenerationCompletedEvent.Broadcast();
}

#if WITH_EDITOR

void ATrainingEpisodePCG::StopGenerationInProgress()
{
	PCGComponent->StopGenerationInProgress();
}

#endif