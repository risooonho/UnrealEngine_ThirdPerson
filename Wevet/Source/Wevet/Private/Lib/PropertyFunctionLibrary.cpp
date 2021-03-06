// Copyright © 2018 wevet works All Rights Reserved.

#include "PropertyFunctionLibrary.h"

//UPropertyFunctionLibrary::UPropertyFunctionLibrary(const FObjectInitializer& ObjectInitializer)
//	: Super(ObjectInitializer)
//{
//}

void UPropertyFunctionLibrary::CopyAllObjectProperty(UObject* SourceObject, UObject* DestObject)
{
	const UClass* SourceClass = SourceObject->GetClass();
	const UClass* DestClass = DestObject->GetClass();

	if (DestObject->IsA(SourceClass))
	{
		for (const UProperty* SourceProperty : TFieldRange<const UProperty>(SourceClass))
		{
			UProperty* DestProperty = FindField<UProperty>(DestClass, *SourceProperty->GetNameCPP());
			if (!DestProperty)
			{
				continue;
			}

			const void* Source = SourceProperty->ContainerPtrToValuePtr<void>(SourceObject);
			void* Dest = DestProperty->ContainerPtrToValuePtr<void>(DestObject);
			SourceProperty->CopySingleValue(Dest, Source);
		}
	}
}

void UPropertyFunctionLibrary::CopyObjectProperties(UObject* SourceObject, UObject* DestObject, const TArray<FString>& PropertyNames)
{
	const UClass* SourceClass = SourceObject->GetClass();
	const UClass* DestClass = DestObject->GetClass();
	if (DestObject->IsA(SourceClass))
	{
		for (const FString& PropertyName : PropertyNames)
		{
			UProperty* SourceProperty = FindField<UProperty>(SourceClass, *PropertyName);
			if (!SourceProperty)
			{
				continue;
			}
			UProperty* DestProperty = FindField<UProperty>(DestClass, *PropertyName);
			if (!DestProperty)
			{
				continue;
			}
			const void* Source = SourceProperty->ContainerPtrToValuePtr<void>(SourceObject);
			void* Dest = DestProperty->ContainerPtrToValuePtr<void>(DestObject);
			SourceProperty->CopySingleValue(Dest, Source);
		}
	}
}