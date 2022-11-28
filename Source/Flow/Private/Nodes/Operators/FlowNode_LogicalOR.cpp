// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#include "Nodes/Operators/FlowNode_LogicalOR.h"

static FName ResetName = TEXT("Reset");

UFlowNode_LogicalOR::UFlowNode_LogicalOR(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, bEnabled(true)
	, ExecutionLimit(1)
	, ExecutionCount(0)
{
#if WITH_EDITOR
	Category = TEXT("Operators");
	NodeStyle = EFlowNodeStyle::Logic;
#endif

	SetNumberedInputPins(0, 1);
	InputPins.Add(FFlowPin(TEXT("Enable"), TEXT("Enabling resets Execution Count")));
	InputPins.Add(FFlowPin(TEXT("Disable"), TEXT("Disabling resets Execution Count")));
}

void UFlowNode_LogicalOR::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UFlowNode_LogicalOR, bOnce))
	{
		if (bOnce)
		{
			InputPins.EmplaceAt(0, ResetName);
			OutputPins.Emplace(ResetName);
		}
		else
		{
			InputPins.Remove(ResetName);
			OutputPins.Remove(ResetName);
		}

		OnReconstructionRequested.ExecuteIfBound();
	}
}

void UFlowNode_LogicalOR::ExecuteInput(const FName& PinName)
{
	if (PinName == TEXT("Enable"))
	{
		if (!bEnabled)
		{
			ResetCounter();
			bEnabled = true;
		}
		return;
	}

	if (PinName == TEXT("Disable"))
	{
		if (bEnabled)
		{
			bEnabled = false;
			Finish();
		}
		return;
	}

	if (bEnabled && PinName.ToString().IsNumeric())
	{
		ExecutionCount++;
		if (ExecutionLimit > 0 && ExecutionCount == ExecutionLimit)
		{
			bEnabled = false;
		}

		TriggerFirstOutput(true);
	}
}

void UFlowNode_LogicalOR::Cleanup()
{
	ResetCounter();
}

void UFlowNode_LogicalOR::ResetCounter()
{
	ExecutionCount = 0;
}
