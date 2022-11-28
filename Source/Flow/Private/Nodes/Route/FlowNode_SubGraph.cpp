// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#include "Nodes/Route/FlowNode_SubGraph.h"
#include "CoreUObject.h"

#include "FlowAsset.h"
#include "FlowSubsystem.h"

FFlowPin UFlowNode_SubGraph::StartPin(TEXT("Start"));
FFlowPin UFlowNode_SubGraph::FinishPin(TEXT("Finish"));

UFlowNode_SubGraph::UFlowNode_SubGraph(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, bCanInstanceIdenticalAsset(false)
{
#if WITH_EDITOR
	Category = TEXT("Route");
	NodeStyle = EFlowNodeStyle::SubGraph;
#endif

	InputPins = {StartPin};
	OutputPins = {FinishPin};
}

bool UFlowNode_SubGraph::CanBeAssetInstanced() const
{
	return !Asset.IsNull() && (bCanInstanceIdenticalAsset || Asset->GetPathName() != GetFlowAsset()->GetTemplateAsset()->GetPathName());
}

void UFlowNode_SubGraph::SetProperties(TArray<FFlowInputOutputPin> Pins)
{
	PropertiesToSet.Empty();

	for (FFlowInputOutputPin Pin : Pins)
	{
		UFlowNode* ConnectedParentNode = GetFlowAsset()->GetNode(Pin.OutputNodeGuid);
		const FProperty* OutputProperty = ConnectedParentNode->FindOutputPropertyByPinName(Pin.OutputPinName);
		uint8* OutputVariableHolder = ConnectedParentNode->GetVariableContainer();
		const void* OutputValuePtr = OutputProperty->ContainerPtrToValuePtr<const void>(OutputVariableHolder);
		FString Value;
		OutputProperty->ExportText_Direct(Value, OutputValuePtr, OutputValuePtr, ConnectedParentNode->GetVariableHolder(), PPF_None);
		PropertiesToSet.Add(Pin.InputProperty, Value);
	}
}

const TArray<FFlowPropertyPin> UFlowNode_SubGraph::GetInputProperties()
{
	return InputPropertyPins;
}

const TArray<FFlowPropertyPin> UFlowNode_SubGraph::GetOutputProperties()
{
	return OutputPropertyPins;
}

void UFlowNode_SubGraph::PreloadContent()
{
	if (CanBeAssetInstanced() && GetFlowSubsystem())
	{
		GetFlowSubsystem()->CreateSubFlow(this, FString(), true);
	}
}

void UFlowNode_SubGraph::FlushContent()
{
	if (CanBeAssetInstanced() && GetFlowSubsystem())
	{
		GetFlowSubsystem()->RemoveSubFlow(this, EFlowFinishPolicy::Abort);
	}
}

void UFlowNode_SubGraph::ExecuteInput(const FName& PinName)
{
	if (CanBeAssetInstanced() == false)
	{
		if (Asset.IsNull())
		{
			LogError(TEXT("Missing Flow Asset"));
		}
		else
		{
			LogError(FString::Printf(TEXT("Asset %s cannot be instance, probably is the same as the asset owning this SubGraph node."), *Asset->GetPathName()));
		}

		Finish();
		return;
	}

	if (PinName == TEXT("Start"))
	{
		if (GetFlowSubsystem())
		{
			GetFlowSubsystem()->CreateSubFlow(this, PropertiesToSet);
		}
	}
	else if (!PinName.IsNone())
	{
		GetFlowAsset()->TriggerCustomEvent(this, PinName);
	}
}

void UFlowNode_SubGraph::Cleanup()
{
	if (CanBeAssetInstanced() && GetFlowSubsystem())
	{
		GetFlowSubsystem()->RemoveSubFlow(this, EFlowFinishPolicy::Keep);
	}
}

void UFlowNode_SubGraph::ForceFinishNode()
{
	TriggerFirstOutput(true);
}

void UFlowNode_SubGraph::OnLoad_Implementation()
{
	if (!SavedAssetInstanceName.IsEmpty() && !Asset.IsNull())
	{
		GetFlowSubsystem()->LoadSubFlow(this, SavedAssetInstanceName);
		SavedAssetInstanceName = FString();
	}
}

#if WITH_EDITOR
FString UFlowNode_SubGraph::GetNodeDescription() const
{
	return Asset.IsNull() ? FString() : Asset.ToSoftObjectPath().GetAssetName();
}

UObject* UFlowNode_SubGraph::GetAssetToEdit()
{
	return Asset.IsNull() ? nullptr : LoadAsset<UObject>(Asset);
}

TArray<FName> UFlowNode_SubGraph::GetContextInputs()
{
	TArray<FName> EventNames;

	if (!Asset.IsNull())
	{
		Asset.LoadSynchronous();
		for (const FName& PinName : Asset.Get()->GetCustomInputs())
		{
			if (!PinName.IsNone())
			{
				EventNames.Emplace(PinName);
			}
		}
	}

	return EventNames;
}

TArray<FName> UFlowNode_SubGraph::GetContextOutputs()
{
	TArray<FName> EventNames;

	if (!Asset.IsNull())
	{
		Asset.LoadSynchronous();
		for (const FName& PinName : Asset.Get()->GetCustomOutputs())
		{
			if (!PinName.IsNone())
			{
				EventNames.Emplace(PinName);
			}
		}
	}

	return EventNames;
}

UObject* UFlowNode_SubGraph::GetVariableHolder()
{
	if (Asset.IsNull())
	{
		return Super::GetVariableHolder();
	}

	const UFlowAsset* FlowAsset = Asset.LoadSynchronous();

	if (!FlowAsset)
	{
		return nullptr;
	}

	return const_cast<UScriptStruct*>(FlowAsset->Properties.GetScriptStruct());
}

void UFlowNode_SubGraph::PostLoad()
{
	Super::PostLoad();

	SubscribeToAssetChanges();
}

void UFlowNode_SubGraph::PreEditChange(FProperty* PropertyAboutToChange)
{
	Super::PreEditChange(PropertyAboutToChange);

	if (PropertyAboutToChange->GetFName() == GET_MEMBER_NAME_CHECKED(UFlowNode_SubGraph, Asset))
	{
		if (Asset)
		{
			Asset->OnSubGraphReconstructionRequested.Unbind();
		}
	}
}

void UFlowNode_SubGraph::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.Property && PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UFlowNode_SubGraph, Asset))
	{
		GatherProperties();
		OnReconstructionRequested.ExecuteIfBound();
		SubscribeToAssetChanges();
	}
}

void UFlowNode_SubGraph::SubscribeToAssetChanges()
{
	if (Asset)
	{
		TWeakObjectPtr<UFlowNode_SubGraph> SelfWeakPtr(this);
		Asset->OnSubGraphReconstructionRequested.BindLambda([SelfWeakPtr]()
		{
			if (SelfWeakPtr.IsValid())
			{
				SelfWeakPtr->GatherProperties();
				SelfWeakPtr->OnReconstructionRequested.ExecuteIfBound();
			}
		});
	}
}

void UFlowNode_SubGraph::GatherProperties()
{
	InputPropertyPins = Super::GetInputProperties();
	OutputPropertyPins = Super::GetOutputProperties();
}
#endif
