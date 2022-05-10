// Copyright Epic Games, Inc. All Rights Reserved.


#include "AssistantMetaHumanGameModeBase.h"

#include "HttpServerModule.h"
#include "IHttpRouter.h"
#include "HttpRouteHandle.h"

#include "HttpModule.h"

#include "HttpServerRequest.h"
#include "HttpServerResponse.h"
#include "HttpPath.h"

#include "WebUtil.h"

#include "Runtime/Online/HTTP/Public/Http.h"
#include "JsonObjectConverter.h"
#include "Serialization/JsonReader.h"

#include "HumanMessageRequest.h"

using namespace UnrealHttpServer;

AAssistantMetaHumanGameModeBase::AAssistantMetaHumanGameModeBase()
{
	Http = &FHttpModule::Get();
}

void AAssistantMetaHumanGameModeBase::StartPlay()
{
	Super::StartPlay();

	auto HttpServerModule = &FHttpServerModule::Get();
	UE_LOG(LogTemp, Log, TEXT("Starting UnrealHttpServer Server..."));
	TSharedPtr<IHttpRouter> HttpRouter = HttpServerModule->GetHttpRouter(Port);

	const FHttpPath HttpPath(TEXT("/TestHttpServer"));
	const FHttpRequestHandler RequestHandler = [this]
	(const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete)
	{
		TSharedPtr<FJsonObject> RequestBody = FWebUtil::GetRequestJsonBody(Request);
		UE_LOG(LogTemp, Warning, TEXT("Response"));

		TSharedPtr<FJsonObject> Data;
		FString Message("Response");
		bool Success = true;
		int32 SUCCESS_CODE = 0;

		TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
		JsonObject->SetObjectField(TEXT("data"), Data);
		JsonObject->SetStringField(TEXT("message"), Message);
		JsonObject->SetBoolField(TEXT("success"), Success);
		JsonObject->SetNumberField(TEXT("code"), (double)SUCCESS_CODE);
		FString JsonString;
		TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonString);
		FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);
		TUniquePtr<FHttpServerResponse> Response = FHttpServerResponse::Create(JsonString, TEXT("application/json"));
		OnComplete(MoveTemp(Response));
		return true;
	};

	FHttpRouteHandle HttpRouteHandle = HttpRouter->BindRoute(HttpPath, EHttpServerRequestVerbs::VERB_GET, RequestHandler);

	BindRouters(HttpRouter);
	// Start Listeners
	HttpServerModule->StartAllListeners();
	if (GEngine != nullptr)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, TEXT("UnrealHttpServer Server Started"));
	}
}

void AAssistantMetaHumanGameModeBase::SendMessage(FString Message)
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
	Request->SetURL(Endpoint);
	Request->SetVerb("POST");
	Request->SetHeader(TEXT("User-Agent"), "X-UnrealEngine-Agent");
	Request->SetHeader("Content-Type", TEXT("application/x-www-form-urlencoded"));

	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);

	JsonObject->SetStringField("text", Message);

	FString RequestBody;
	TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&RequestBody);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	Request->SetContentAsString(RequestBody);

	//FStringFormatOrderedArguments Args;
	//Args.Add(FStringFormatArg(Token));
	//Request->SetHeader("Authorization", FString::Format(TEXT("Bearer {0}"), Args));

	Request->OnProcessRequestComplete().BindUObject(this, &AAssistantMetaHumanGameModeBase::OnMessageResponse);
	Request->ProcessRequest();
}

bool AAssistantMetaHumanGameModeBase::IsIdle() const
{
	return HumanResponse.Emotion == TEXT("Idle");
}

bool AAssistantMetaHumanGameModeBase::IsHappy() const
{
	return HumanResponse.Emotion == TEXT("Happy");
}

bool AAssistantMetaHumanGameModeBase::IsAngry() const
{
	return HumanResponse.Emotion == TEXT("Angry");
}

bool AAssistantMetaHumanGameModeBase::IsSad() const
{
	return HumanResponse.Emotion == TEXT("Sad");
}


void AAssistantMetaHumanGameModeBase::BindRouters(const TSharedPtr<IHttpRouter>& HttpRouter)
{
	//FWebUtil::BindRoute(HttpRouter, TEXT("/player/get_location"), EHttpServerRequestVerbs::VERB_GET, &AAssistantMetaHumanGameModeBase::Response);
}

void AAssistantMetaHumanGameModeBase::OnMessageResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	FString ResponseBody = Response->GetContentAsString();
	if (ResponseBody.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("Error Response"));
		return;
	}

	bool Success = FJsonObjectConverter::JsonObjectStringToUStruct(ResponseBody, &HumanResponse, 0, 0);

	/*TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	TSharedRef< TJsonReader<> > Reader = TJsonReaderFactory<>::Create(ResponseBody);
	if (FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		HumanResponse.Message = JsonObject->GetStringField(TEXT("Message"));
		HumanResponse.Emotion = JsonObject->GetStringField(TEXT("Emotion"));
	}*/

	OnHumanResponseComplete();
}

TUniquePtr<FHttpServerResponse> AAssistantMetaHumanGameModeBase::Response(const FHttpServerRequest& Request)
{
	return TUniquePtr<FHttpServerResponse>();
}
