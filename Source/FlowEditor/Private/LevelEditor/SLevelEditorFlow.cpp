// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#include "LevelEditor/SLevelEditorFlow.h"
#include "FlowAsset.h"
#include "FlowComponent.h"
#include "FlowWorldSettings.h"

#include "Graph/FlowGraphSettings.h"

#include "Editor.h"
#include "PropertyCustomizationHelpers.h"

#define LOCTEXT_NAMESPACE "SLevelEditorFlow"

void SLevelEditorFlow::Construct(const FArguments& InArgs)
{
	CreateFlowWidget();
	FEditorDelegates::OnMapOpened.AddRaw(this, &SLevelEditorFlow::OnMapOpened);
}

void SLevelEditorFlow::OnMapOpened(const FString& Filename, bool bAsTemplate)
{
	CreateFlowWidget();
}

void SLevelEditorFlow::CreateFlowWidget()
{
	if (const UFlowComponent* FlowComponent = FindFlowComponent(); FlowComponent && FlowComponent->RootFlow)
	{
		FlowAssetPath = FlowComponent->RootFlow->GetPathName();
	}
	else
	{
		FlowAssetPath = FString();
	}

	ChildSlot
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SObjectPropertyEntryBox)
					.AllowedClass(UFlowGraphSettings::Get()->WorldAssetClass)
					.DisplayThumbnail(false)
					.OnObjectChanged(this, &SLevelEditorFlow::OnFlowChanged)
					.ObjectPath(this, &SLevelEditorFlow::GetFlowAssetPath) // needs function to automatically refresh view upon data change
			]
	];
}

FString SLevelEditorFlow::GetFlowAssetPath() const
{
	return FlowAssetPath;
}

void SLevelEditorFlow::OnFlowChanged(const FAssetData& NewAsset)
{
	FlowAssetPath = NewAsset.GetSoftObjectPath().ToString();

	if (UFlowComponent* FlowComponent = FindFlowComponent())
	{
		if (UObject* NewObject = NewAsset.GetAsset())
		{
			FlowComponent->RootFlow = Cast<UFlowAsset>(NewObject);
		}
		else
		{
			FlowComponent->RootFlow = nullptr;
		}

		const bool bSuccess = FlowComponent->MarkPackageDirty();
		ensureMsgf(bSuccess, TEXT("World Settings couldn't be marked dirty while changing the assigned Flow Asset."));
	}
}

UFlowComponent* SLevelEditorFlow::FindFlowComponent() const
{
	if (const UWorld* World = GEditor->GetEditorWorldContext().World())
	{
		if (const AWorldSettings* WorldSettings = World->GetWorldSettings())
		{
			if (UActorComponent* FoundComponent = WorldSettings->GetComponentByClass(UFlowComponent::StaticClass()))
			{
				return Cast<UFlowComponent>(FoundComponent);
			}
		}
	}

	return nullptr;
}

#undef LOCTEXT_NAMESPACE
