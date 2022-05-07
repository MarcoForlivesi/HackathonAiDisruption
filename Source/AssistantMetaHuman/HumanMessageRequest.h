// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "HumanMessageRequest.generated.h"

USTRUCT(BlueprintType)
struct ASSISTANTMETAHUMAN_API FHumanMessageRequest
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tecma")
	FString Text;
};