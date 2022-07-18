// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#pragma once

#include "Engine/DeveloperSettings.h"
#include "Templates/SubclassOf.h"
#include "FlowSettings.generated.h"

class UFlowNode;

/**
 *
 */
UCLASS(Config = Game, defaultconfig, meta = (DisplayName = "Flow"))
class FLOW_API UFlowSettings final : public UDeveloperSettings
{
	GENERATED_UCLASS_BODY()

	static UFlowSettings* Get() { return CastChecked<UFlowSettings>(StaticClass()->GetDefaultObject()); }

	// Set if to False, if you don't want to create client-side Flow Graphs
	// And you don't access to the Flow Component registry on clients
	UPROPERTY(Config, EditAnywhere, Category = "Networking")
	bool bCreateFlowSubsystemOnClients;

	// How many nodes of given class should be preloaded with the Flow Asset instance?
	UPROPERTY(Config, EditAnywhere, Category = "Preload")
	TMap<TSubclassOf<UFlowNode>, int32> DefaultPreloadDepth;
	
	UPROPERTY(Config, EditAnywhere, Category = "SaveSystem")
	bool bWarnAboutMissingIdentityTags;

	/**
	 * If set, Only Properties with the corresponding Meta-Tag set will treated as inputs or outputs.
	 * Only works for c++
	 */
	UPROPERTY(Config, EditAnywhere, Category="Properties")
	bool bUseCustomMetaTagsForInputOutputs = false;
	
	UPROPERTY(Config, EditAnywhere, Category="Properties", meta=(DisplayName="Input Meta Tag", EditCondition="bUseCustomMetaTagsForInputOutputs"))
	FName PropertyInputMetaTag = FName("FlowInput");

	UPROPERTY(Config, EditAnywhere, Category="Properties", meta=(DisplayName="Output Meta Tag", EditCondition="bUseCustomMetaTagsForInputOutputs"))
	FName PropertyOutputMetaTag = FName("FlowOutput");
};
