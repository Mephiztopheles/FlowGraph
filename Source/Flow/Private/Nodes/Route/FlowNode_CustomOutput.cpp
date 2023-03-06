// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#include "Nodes/Route/FlowNode_CustomOutput.h"

#include "FlowAsset.h"
#include "Nodes/Route/FlowNode_SubGraph.h"

UFlowNode_CustomOutput::UFlowNode_CustomOutput(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
#if WITH_EDITOR
	Category = TEXT("Route");
	NodeStyle = EFlowNodeStyle::InOut;
#endif

	OutputPins.Empty();
	AllowedSignalModes = {EFlowSignalMode::Enabled, EFlowSignalMode::Disabled};
}

void UFlowNode_CustomOutput::ExecuteInput(const FName& PinName)
{
	if (!EventName.IsNone() && GetFlowAsset()->GetCustomOutputs().Contains(EventName) && GetFlowAsset()->GetNodeOwningThisAssetInstance())
	{
		GetFlowAsset()->TriggerCustomOutput(EventName);
	}
}

#if WITH_EDITOR
FString UFlowNode_CustomOutput::GetNodeDescription() const
{
	return EventName.ToString();
}

EDataValidationResult UFlowNode_CustomOutput::ValidateNode()
{
	if (EventName.IsNone())
	{
		ValidationLog.Error<UFlowNode>(TEXT("Event Name is empty!"), this);
		return EDataValidationResult::Invalid;
	}

	return EDataValidationResult::Valid;
}
#endif
