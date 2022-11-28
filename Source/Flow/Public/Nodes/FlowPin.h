// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#pragma once

#include "FlowPin.generated.h"

USTRUCT()
struct FLOW_API FFlowPin
{
	GENERATED_BODY()

	// A logical name, used during execution of pin
	UPROPERTY(EditDefaultsOnly, Category = "FlowPin")
	FName PinName;

	// An optional Display Name, you can use it to override PinName without the need to update graph connections
	UPROPERTY(EditDefaultsOnly, Category = "FlowPin")
	FText PinFriendlyName;

	UPROPERTY(EditDefaultsOnly, Category = "FlowPin")
	FString PinToolTip;

	static inline FName AnyPinName = TEXT("AnyPinName");
	
	FFlowPin()
		: PinName(NAME_None)
	{
	}

	FFlowPin(const FName& InPinName)
		: PinName(InPinName)
	{
	}

	FFlowPin(const FString& InPinName)
		: PinName(*InPinName)
	{
	}

	FFlowPin(const FText& InPinName)
		: PinName(*InPinName.ToString())
	{
	}

	FFlowPin(const TCHAR* InPinName)
		: PinName(FName(InPinName))
	{
	}

	FFlowPin(const uint8& InPinName)
		: PinName(FName(*FString::FromInt(InPinName)))
	{
	}

	FFlowPin(const int32& InPinName)
		: PinName(FName(*FString::FromInt(InPinName)))
	{
	}

	FFlowPin(const FStringView InPinName, const FText& InPinFriendlyName)
		: PinName(InPinName)
		  , PinFriendlyName(InPinFriendlyName)
	{
	}

	FFlowPin(const FStringView InPinName, const FString& InPinTooltip)
		: PinName(InPinName)
		  , PinToolTip(InPinTooltip)
	
	{
	}

	FFlowPin(const FStringView InPinName, const FText& InPinFriendlyName, const FString& InPinTooltip)
		: PinName(InPinName)
		, PinFriendlyName(InPinFriendlyName)
		  , PinToolTip(InPinTooltip)
	{
	}

	FORCEINLINE bool IsValid() const
	{
		return !PinName.IsNone();
	}

	FORCEINLINE bool operator==(const FFlowPin& Other) const
	{
		return PinName == Other.PinName;
	}

	FORCEINLINE bool operator!=(const FFlowPin& Other) const
	{
		return PinName != Other.PinName;
	}

	FORCEINLINE bool operator==(const FName& Other) const
	{
		return PinName == Other;
	}

	FORCEINLINE bool operator!=(const FName& Other) const
	{
		return PinName != Other;
	}

	friend uint32 GetTypeHash(const FFlowPin& FlowPin)
	{
		return GetTypeHash(FlowPin.PinName);
	}
};

USTRUCT()
struct FLOW_API FFlowInputOutputPin
{
	GENERATED_USTRUCT_BODY()

	FProperty* InputProperty;
	FProperty* OutputProperty;

	UPROPERTY()
	FName InputPinName;

	UPROPERTY()
	FName OutputPinName;

	UPROPERTY()
	FGuid InputNodeGuid;

	UPROPERTY()
	FGuid OutputNodeGuid;

	FORCEINLINE bool IsValid() const
	{
		return !OutputPinName.IsNone() && !InputPinName.IsNone() && InputNodeGuid.IsValid() && OutputNodeGuid.IsValid();
	}

	FFlowInputOutputPin()
	{
		InputPinName = OutputPinName = NAME_None;
		InputNodeGuid = OutputNodeGuid = FGuid();
		InputProperty = OutputProperty = nullptr;
	}

	FFlowInputOutputPin(const FName& InPinName, const FName& OutPinName, const FGuid& InNodeGuid, const FGuid& OutNodeGuid)
	{
		InputPinName = InPinName;
		OutputPinName = OutPinName;
		InputNodeGuid = InNodeGuid;
		OutputNodeGuid = OutNodeGuid;
		InputProperty = OutputProperty = nullptr;
	}

	FFlowInputOutputPin(FProperty* NewInputProperty, FProperty* NewOutputProperty)
	{
		InputPinName = OutputPinName = NAME_None;
		InputNodeGuid = OutputNodeGuid = FGuid();
		InputProperty = NewInputProperty;
		OutputProperty = NewOutputProperty;
	}
};

USTRUCT()
struct FLOW_API FFlowPropertyPin
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	FName Name;

	UPROPERTY()
	FString Tooltip;

	UPROPERTY()
	FName Category;

	UPROPERTY()
	FName SubCategory;

	UPROPERTY()
	UObject* SubCategoryObject = nullptr;

	UPROPERTY()
	bool bIsWeakPointer;
};

USTRUCT()
struct FLOW_API FFlowPinHandle
{
	GENERATED_BODY()

	// Update SFlowPinHandleBase code if this property name would be ever changed
	UPROPERTY()
	FName PinName;

	FFlowPinHandle()
		: PinName(NAME_None)
	{
	}
};

USTRUCT(BlueprintType)
struct FLOW_API FFlowInputPinHandle : public FFlowPinHandle
{
	GENERATED_BODY()

	FFlowInputPinHandle()
	{
	}
};

USTRUCT(BlueprintType)
struct FLOW_API FFlowOutputPinHandle : public FFlowPinHandle
{
	GENERATED_BODY()

	FFlowOutputPinHandle()
	{
	}
};

// Processing Flow Nodes creates map of connected pins
USTRUCT()
struct FLOW_API FConnectedPin
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	FGuid NodeGuid;

	UPROPERTY()
	FName PinName;

	UPROPERTY()
	FFlowInputOutputPin PinProperty;

	FConnectedPin()
		: NodeGuid(FGuid())
		  , PinName(NAME_None)
	{
	}

	FConnectedPin(const FGuid InNodeId, const FName& InPinName)
		: NodeGuid(InNodeId)
		  , PinName(InPinName)
	{
	}

	FConnectedPin(const FGuid InNodeId, const FName& InPinName, const FFlowInputOutputPin& InputOutputPin)
		: NodeGuid(InNodeId)
		  , PinName(InPinName)
		  , PinProperty(InputOutputPin)
	{
	}

	FORCEINLINE bool operator==(const FConnectedPin& Other) const
	{
		return NodeGuid == Other.NodeGuid && PinName == Other.PinName;
	}

	FORCEINLINE bool operator!=(const FConnectedPin& Other) const
	{
		return NodeGuid != Other.NodeGuid || PinName != Other.PinName;
	}

	friend uint32 GetTypeHash(const FConnectedPin& ConnectedPin)
	{
		return GetTypeHash(ConnectedPin.NodeGuid) + GetTypeHash(ConnectedPin.PinName);
	}
};

UENUM(BlueprintType)
enum class EFlowPinActivationType : uint8
{
	Default,
	Forced,
	PassThrough
};

// Every time pin is activated, we record it and display this data while user hovers mouse over pin
#if !UE_BUILD_SHIPPING
struct FLOW_API FPinRecord
{
	double Time;
	FString HumanReadableTime;
	EFlowPinActivationType ActivationType;

	static FString NoActivations;
	static FString PinActivations;
	static FString ForcedActivation;
	static FString PassThroughActivation;

	FPinRecord();
	FPinRecord(const double InTime, const EFlowPinActivationType InActivationType);

private:
	FORCEINLINE static FString DoubleDigit(const int32 Number);
};
#endif
