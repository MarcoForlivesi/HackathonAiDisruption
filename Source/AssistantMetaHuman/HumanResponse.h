// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "HumanResponse.generated.h"

USTRUCT(BlueprintType)
struct ASSISTANTMETAHUMAN_API FHumanResponse
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tecma")
	FString Message;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tecma")
	FString Emotion;
};