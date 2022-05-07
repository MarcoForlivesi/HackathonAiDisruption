// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "Runtime/Online/HTTP/Public/Http.h"

#include "HttpServerModule.h"
#include "IHttpRouter.h"
#include "HttpRouteHandle.h"
#include "HttpServerRequest.h"
#include "HttpServerResponse.h"
#include "HttpPath.h"

#include "HumanResponse.h"

#include "AssistantMetaHumanGameModeBase.generated.h"
/**
 * 
 */
UCLASS()
class ASSISTANTMETAHUMAN_API AAssistantMetaHumanGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:
	AAssistantMetaHumanGameModeBase();

	virtual void StartPlay() override;

	UFUNCTION(BlueprintCallable)
	void SendMessage(FString Message);

	UFUNCTION(BlueprintCallable)
	bool IsIdle() const;
	UFUNCTION(BlueprintCallable)
	bool IsHappy() const;
	UFUNCTION(BlueprintCallable)
	bool IsAngry() const;
	UFUNCTION(BlueprintCallable)
	bool IsSad() const;
protected:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void OnHumanResponseComplete();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Endpoint;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FHumanResponse HumanResponse;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Port = 3384;
private:
	void OnMessageResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	static void BindRouters(const TSharedPtr<IHttpRouter>& HttpRouter);

	TUniquePtr<FHttpServerResponse> Response(const FHttpServerRequest& Request);

	FHttpModule* Http;

};
