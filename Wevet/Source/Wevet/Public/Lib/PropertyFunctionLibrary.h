// Copyright © 2018 wevet works All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PropertyFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class WEVET_API UPropertyFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable, Category = "PropertyFunctionLibrary|Variable")
	static void CopyAllObjectProperty(UObject* SourceObject, UObject* DestObject);

	UFUNCTION(BlueprintCallable, Category = "PropertyFunctionLibrary|Variable")
	static void CopyObjectProperties(UObject* SourceObject, UObject* DestObject, const TArray<FString>& PropertyNames);
};
