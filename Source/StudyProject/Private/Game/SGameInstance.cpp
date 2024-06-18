// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/SGameInstance.h"
//#include "Kismet/KismetSystemLibrary.h"
//#include "SUnrealObjectClass.h"
#include "Example/SPigeon.h"
#include "Example/SEagle.h"
#include "JsonObjectConverter.h"
#include "UObject/SavePackage.h"

USGameInstance::USGameInstance()
{
	// CDO를 초기화할 때 생성자에서 처리

	// 매개변수: 카테고리명, 로그 주의 경고 표시, 문구
	UE_LOG(LogTemp, Log, TEXT("USGameInstance::USGameInstance() has been called."));

	// CDO에 Name 속성 저장
	Name = TEXT("USGameInstance Class Default Object");
}

//UE_DISABLE_OPTIMIZATION		// 5.3버전 여기서부터 최적화 하지마!
PRAGMA_DISABLE_OPTIMIZATION		// 5.1버전 여기서부터 최적화 하지마!
void USGameInstance::Init()
{
	UE_LOG(LogTemp, Log, TEXT("Init() has been called."));

	// 바로 상위 클래스의 Init 함수 호출
	Super::Init();

	// this, GetWorld() 둘 다 가능
	//UKismetSystemLibrary::PrintString(this, TEXT("PrintString: Init() has been called."));

#pragma region check,ensure
	// ===============================================
	// ===============================================
	//// check, ensure
	//{
	//	UWorld* world = GetWorld();

	//	if (true == IsValid(world))
	//	{
	//		// IsValid로 확인하여 실행하는 작업을 할 수 있음
	//		// 그러나 만일 world 개체가 유효하지 않을 때 아무런 로직이 돌지 않아 무엇이 잘못된 것인지 알 수 없을 수도 있음
	//		// world 개체가 없다는 건 문제이므로 이럴 때는 checkf() 매크로 함수나 ensureMsgf() 매크로 함수를 활요하는 것이 좋음
	//		UKismetSystemLibrary::PrintString(world, TEXT("Init() has been called."));
	//	}

	//	// check 혹은 ensure 함수를 사용
	//	// 함수 체크를 할 때 true면 문제 없는 것, false면 에디터를 중단 종료 시키고 크러시를 발생
	//	// 단 ensure는 f5를 다시 눌렀을 때 에디터 계속 실행
	//	//check(false);
	//	//ensure(false);
	//	ensure(true == IsValid(world));

	//	// checkf, ensureMsgf의 경우 뒤에 인자를 더 붙여줘야함 즉 추가 메시지를 더 붙여준다고 생각하면 됨
	//	checkf(true == IsValid(world), TEXT("Invalid World."));
	//}
	// ===============================================
	// ===============================================
#pragma endregion

#pragma region CDO
	// ===============================================
	// ===============================================
	//// CDO 개념 알기
	//{
	//	// 런타임 중 클래스 정보
	//	UClass* RunTimeClassInfo = GetClass();
	//	// 컴파일 과정 중 클래스 정보
	//	UClass* CompileTimeClassInfo = StaticClass();

	//	checkf(CompileTimeClassInfo == RunTimeClassInfo, TEXT("CompileTimeClassInfo != RunTimeClassInfo"));

	//	UE_LOG(LogTemp, Log, TEXT("CompileTimeClassInfo->GetName(): %s"), *CompileTimeClassInfo->GetName());
	//	UE_LOG(LogTemp, Log, TEXT("RunTimeClassInfo->GetName(): %s"), *RunTimeClassInfo->GetName());

	//	//UE_LOG(LogTemp, Log, TEXT("Class Name: %s"), *RunTimeClassInfo->GetName());

	//	Name = TEXT("USGameInstance Object");

	//	UE_LOG(LogTemp, Log, TEXT("USGameInstance::Name %s"), *(RunTimeClassInfo->GetDefaultObject<USGameInstance>()->Name));
	//	UE_LOG(LogTemp, Log, TEXT("USGameInstance::Name: %s"), *Name);

	//	//UE_LOG(LogTemp, Log, TEXT("GetClass Name: %s"), *(GetClass()->GetDefaultObject<USGameInstance>()->Name));
	//	//UE_LOG(LogTemp, Log, TEXT("Name: %s"), *Name);
	//}
#pragma endregion

#pragma region Reflection
	// ===============================================
	// ===============================================
	//// 리플랙션
	//{
	//	// 언리얼에서는 new 연산자가 아닌 NewObject 템플릿 함수 API를 사용
	//	USUnrealObjectClass* USObject1 = NewObject<USUnrealObjectClass>();

	//	// USUnrealObjectClass에서 정의한 Name을 Getter
	//	UE_LOG(LogTemp, Warning, TEXT("USObject1's Name: %s"), *USObject1->GetName());

	//	// Unreal Property System을 이용해 USUnrealObjectClass의 Name을 Getter
	//	FProperty* NameProperty = USUnrealObjectClass::StaticClass()->FindPropertyByName(TEXT("Name"));
	//	FString CompiletimeUSObjectName;
	//	if (nullptr != NameProperty)
	//	{
	//		// 프로퍼티 시스템을 이용하여 USUnrealObjectClass의 개체에 있는 NameProperty(Name 프로퍼티)값을 CompiletimeUSObjectName에 복사 출력?
	//		NameProperty->GetValue_InContainer(USObject1, &CompiletimeUSObjectName);
	//		UE_LOG(LogTemp, Warning, TEXT("CompiletimeUSObjectName: %s"), *CompiletimeUSObjectName);
	//	}

	//	// USUnrealObjectClass의 HelloUnreal함수 호출 
	//	USObject1->HelloUnreal();

	//	// USObject1의 런타임 클래스를 통해 HelloUnreal 이름의 함수가 있는지 파악
	//	UFunction* HelloUnrealFunction = USObject1->GetClass()->FindFunctionByName("HelloUnreal");
	//	if (nullptr != HelloUnrealFunction)
	//	{
	//		// 해당 이름의 함수가 존재한다면 해당 함수의 이벤트를 발생시켜라!
	//		// 뒤에 nullptr은 뭐지???? => 인자가 없기 때문에 nullptr
	//		USObject1->ProcessEvent(HelloUnrealFunction, nullptr);
	//	}
	//}
	// ===============================================
	// ===============================================
#pragma endregion

#pragma region Interface
	// ===============================================
	// ===============================================
	//// 인터페이스 이용
	//{
	//	USPigeon* Pigeon1 = NewObject<USPigeon>();
	//	// nullptr과 isvalid의 차이에 대해 알아 볼 것
	//	if (true == IsValid(Pigeon1)) Pigeon1->Fly();

	//	USEagle* Eagle1 = NewObject<USEagle>();
	//	if (true == IsValid(Eagle1)) Eagle1->Fly();

	//	// 다형성을 위한 배열
	//	TArray<ISFlyable*> Birds;
	//	Birds.Reserve(10);

	//	// 런타임 중 클래스 정보를 가져와서 ImplementsInterface로 USFlyable이 구현 되어 있는지
	//	if (true == Pigeon1->GetClass()->ImplementsInterface(USFlyable::StaticClass()))
	//	{
	//		// 형변환 후 배열에 추가
	//		ISFlyable* Bird1 = Cast<ISFlyable>(Pigeon1);
	//		Birds.Emplace(Bird1);
	//	}

	//	if (true == Eagle1->GetClass()->ImplementsInterface(USFlyable::StaticClass()))
	//	{
	//		ISFlyable* Bird2 = Cast<ISFlyable>(Eagle1);
	//		Birds.Emplace(Bird2);
	//	}

	//	// 다형성의 예시
	//	for (ISFlyable* Bird : Birds)
	//	{
	//		Bird->Fly();
	//	}
	//}
	// ===============================================
	// ===============================================
#pragma endregion

#pragma region Serialization1
	// ===============================================
	// ===============================================
	//// 시리얼라이제이션1ver(직접 구조체 생성으로)
	//{
	//	USPigeon* Pigeon76 = NewObject<USPigeon>();
	//	Pigeon76->SetPigeonName(TEXT("Pigeon76"));
	//	Pigeon76->SetPigeonID(76);
	//	UE_LOG(LogTemp, Log, TEXT("[Pigeon76] Name: %s, ID: %d"), *Pigeon76->GetPigeonName(), Pigeon76->GetPigeonID());

	//	// ProjectDir() => 프로젝트 파일 경로
	//	// Combine함수를 사용하면 프로젝트 파일 경로에 TEXT를 붙여라, 즉 프로젝트 파일 경로 뒤에 Saved를 붙여라
	//	// 프로젝트 파일에 있는 Saved 폴더 경로를 문자열에 담은 것
	//	const FString SaveDirevtoryPath = FPaths::Combine(FPlatformMisc::ProjectDir(), TEXT("Saved"));
	//	UE_LOG(LogTemp, Log, TEXT("SaveDirevtoryPath: %s"), *SaveDirevtoryPath);
	//	
	//	const FString SavedFileName(TEXT("SerializedPigeon76Data.bin"));
	//	FString AbsoluteFilePath = FPaths::Combine(*SaveDirevtoryPath, *SavedFileName);
	//	UE_LOG(LogTemp, Log, TEXT("RelativeFilePath: %s"), *AbsoluteFilePath);
	//	// MakeStandardFilename과정을 거쳐야 소스코드에서도 사용이 가능한 포맷이 되는 것?
	//	// 어떤 플랫폼을 써도 표준화 된 파일 경로로 사용 가능
	//	FPaths::MakeStandardFilename(AbsoluteFilePath);
	//	UE_LOG(LogTemp, Log, TEXT("AbsoluteFilePath: %s"), *AbsoluteFilePath);

	//	// 파일 쓰기
	//	FPigeonData SerializedPigeon76Data(Pigeon76->GetPigeonName(), Pigeon76->GetPigeonID());
	//	// CreateFileWriter => 이것이 결국 아카이브
	//	// AbsoluteFilePath을 기준으로 아카이브를 생성
	//	FArchive* WriterArchive = IFileManager::Get().CreateFileWriter(*AbsoluteFilePath);
	//	// F가 붙은 것은 Struct를 지칭, struct는 nullptr 체크 밖에 못함
	//	// IsValid 지원X, IsValid는 UObject만 지원
	//	if (nullptr != WriterArchive)
	//	{
	//		*WriterArchive << SerializedPigeon76Data;
	//		WriterArchive->Close();
	//		delete WriterArchive;
	//		WriterArchive = nullptr;
	//	}

	//	// 파일 읽기
	//	FPigeonData DeserializedPigeon76Data;
	//	FArchive* ReaderArchive = IFileManager::Get().CreateFileReader(*AbsoluteFilePath);
	//	if (nullptr != ReaderArchive)
	//	{
	//		*ReaderArchive << DeserializedPigeon76Data;
	//		ReaderArchive->Close();
	//		delete ReaderArchive;
	//		ReaderArchive = nullptr;
	//	}

	//	USPigeon* ClonePigeon76 = NewObject<USPigeon>();
	//	ClonePigeon76->SetPigeonName(DeserializedPigeon76Data.Name);
	//	ClonePigeon76->SetPigeonID(DeserializedPigeon76Data.ID);
	//	UE_LOG(LogTemp, Log, TEXT("[ClonePigeon76] Name: %s, ID: %d"), *ClonePigeon76->GetPigeonName(), ClonePigeon76->GetPigeonID());
	//}
	// ===============================================
	// ===============================================
#pragma endregion

#pragma region Serialization2
	// ===============================================
	// ===============================================
	//// 시리얼라이제이션2ver(언리얼 제공 Serialize 함수로)
	//{
	//	USPigeon* Pigeon76 = NewObject<USPigeon>();
	//	Pigeon76->SetPigeonName(TEXT("Pigeon76"));
	//	Pigeon76->SetPigeonID(76);
	//	UE_LOG(LogTemp, Log, TEXT("[Pigeon76] Name: %s, ID: %d"), *Pigeon76->GetPigeonName(), Pigeon76->GetPigeonID());

	//	const FString SaveDirevtoryPath = FPaths::Combine(FPlatformMisc::ProjectDir(), TEXT("Saved"));
	//	const FString SavedFileName(TEXT("SerializedPigeon76Data.bin"));
	//	FString AbsoluteFilePath = FPaths::Combine(*SaveDirevtoryPath, *SavedFileName);
	//	FPaths::MakeStandardFilename(AbsoluteFilePath);

	//	// uint8 == 8비트 == 1바이트 == 1바이트 짜리 배열 즉 메모리 버퍼
	//	TArray<uint8> BufferForWriter;
	//	// BufferForWriter 버퍼를 통해 쓰기용 아카이브 생성
	//	FMemoryWriter MemoryWriterArchive(BufferForWriter);
	//	// Serialize 호출을 통해 Pigeon76의 정보가 MemoryWriterArchive에 쓰여짐
	//	Pigeon76->Serialize(MemoryWriterArchive);

	//	// 파일 쓰기
	//	TUniquePtr<FArchive> WriterArchive = TUniquePtr<FArchive>(IFileManager::Get().CreateFileWriter(*AbsoluteFilePath));
	//	if (nullptr != WriterArchive)
	//	{
	//		*WriterArchive << BufferForWriter;
	//		WriterArchive->Close();

	//		WriterArchive = nullptr;
	//	}

	//	// 파일 읽기
	//	TArray<uint8> BufferForReader;
	//	TUniquePtr<FArchive> ReaderArchive = TUniquePtr<FArchive>(IFileManager::Get().CreateFileReader(*AbsoluteFilePath));
	//	if (nullptr != ReaderArchive)
	//	{
	//		*ReaderArchive << BufferForReader;
	//		ReaderArchive->Close();

	//		ReaderArchive = nullptr;
	//	}

	//	// BufferForReader의 값을 읽어서 MemoryReaderArchive로 저장
	//	FMemoryReader MemoryReaderArchive(BufferForReader);
	//	USPigeon* ClonePigeon76 = NewObject<USPigeon>();
	//	ClonePigeon76->Serialize(MemoryReaderArchive);
	//	UE_LOG(LogTemp, Log, TEXT("[ClonePigeon76] Name: %s, ID:%d"), *ClonePigeon76->GetPigeonName(), ClonePigeon76->GetPigeonID());
	//}
	// ===============================================
	// ===============================================
#pragma endregion

#pragma region SerializationJson
	// ===============================================
	// ===============================================
	//// JSON 시리얼라이제이션
	//{
	//	USPigeon* Pigeon76 = NewObject<USPigeon>();
	//	Pigeon76->SetPigeonName(TEXT("Pigeon76"));
	//	Pigeon76->SetPigeonID(76);
	//	UE_LOG(LogTemp, Log, TEXT("[Pigeon76] Name: %s, ID: %d"), *Pigeon76->GetPigeonName(), Pigeon76->GetPigeonID());

	//	const FString SaveDirevtoryPath = FPaths::Combine(FPlatformMisc::ProjectDir(), TEXT("Saved"));
	//	const FString SavedFileName(TEXT("SerializedPigeon76Data.bin"));
	//	FString AbsoluteFilePath = FPaths::Combine(*SaveDirevtoryPath, *SavedFileName);
	//	FPaths::MakeStandardFilename(AbsoluteFilePath);

	//	// 언리얼 오브젝트를 제이슨 오브젝트로 변환
	//	// 빈 제이슨 오브젝트 생성
	//	TSharedRef<FJsonObject> Pigeon76JsonObject = MakeShared<FJsonObject>();
	//	// Pigeon76의 클래스 정보와 개체 정보를 넘겨서 해당 정보를 Pigeon76JsonObject로 변경(언리얼 -> 제이슨)
	//	FJsonObjectConverter::UStructToJsonObject(Pigeon76->GetClass(), Pigeon76, Pigeon76JsonObject);

	//	// 파일 쓰기
	//	FString WritedJsonString;
	//	TSharedRef<TJsonWriter<TCHAR>> JsonWriterArchive = TJsonWriterFactory<TCHAR>::Create(&WritedJsonString);
	//	if (true == FJsonSerializer::Serialize(Pigeon76JsonObject, JsonWriterArchive))
	//	{
	//		FFileHelper::SaveStringToFile(WritedJsonString, *AbsoluteFilePath);
	//	}

	//	// 파일 읽기
	//	FString ReadedJsonString;
	//	FFileHelper::LoadFileToString(ReadedJsonString, *AbsoluteFilePath);
	//	TSharedRef<TJsonReader<TCHAR>> JsonReaderArchive = TJsonReaderFactory<TCHAR>::Create(ReadedJsonString);
	//	
	//	USPigeon* ClonePigeon76 = NewObject<USPigeon>();
	//	TSharedPtr<FJsonObject> ClonePigeon76JsonObject = nullptr;
	//	if (true == FJsonSerializer::Deserialize(JsonReaderArchive, ClonePigeon76JsonObject))
	//	{
	//		// 제이슨 오브젝트를 언리얼 오브젝트로 변환
	//		// ClonePigeon76JsonObject의 레퍼런스를 넘겨서 해당 정보를 ClonePigeon76의 클래스와 ClonePigeon76 개체로 변경(제이슨 -> 언리얼)
	//		if (true == FJsonObjectConverter::JsonObjectToUStruct(ClonePigeon76JsonObject.ToSharedRef(), ClonePigeon76->GetClass(), ClonePigeon76))
	//		{
	//			UE_LOG(LogTemp, Log, TEXT("[ClonePigeon76] Name: %s, ID:%d"), *ClonePigeon76->GetPigeonName(), ClonePigeon76->GetPigeonID());
	//		}
	//	}
	//}
	// ===============================================
	// ===============================================
#pragma endregion

#pragma region Delegate
	// ===============================================
	// ===============================================
	//// 델리게이트
	//{
	//	SpawnedPigeon = NewObject<USPigeon>();
	//	SpawnedPigeon->SetPigeonName(TEXT("SpawnedPigeon"));
	//	SpawnedPigeon->SetPigeonID(7);

	//	// IsAlreadyBound 이미 바인드가 되어있는지 확인
	//	// OnPigeonFlying 델리게이트에 ThisClass::HandlePigeonFlying 함수가 바인드 되어있는가?
	//	// 라는 해당 조건을 확인, 즉 바인드 되어 있지 않을 경우
	//	if (false == SpawnedPigeon->OnPigeonFlying.IsAlreadyBound(this, &ThisClass::HandlePigeonFlying))
	//	{
	//		// SpawnedPigeon의 OnPigeonFlying 델리게이트에다가 ThisClass::HandlePigeonFlying 추가 작업
	//		SpawnedPigeon->OnPigeonFlying.AddDynamic(this, &ThisClass::HandlePigeonFlying);
	//	}
	//	SpawnedPigeon->Fly();
	//}
	// ===============================================
	// ===============================================
#pragma endregion

#pragma region FString
	// ===============================================
	// ===============================================
	//// FString
	//{
	//	// 한글로 적으면 문제가 발생할 수 있음
	//	// 이를 해결하기 위해서 
	//	// 파일 -> Save SGameInstance.cpp As.. -> Save 버튼 우측 역삼각형 클릭 -> Save with Encoding -> Encoding에 UTF-8 지정
	//	UE_LOG(LogTemp, Log, TEXT("헬로, 언리얼!"));

	//	// UTF-16 인코딩의 문자열을 생성하기 위한 언리얼 표준 문자 타입이 TCHAR
	//	// TCHAR 자료형 변수에 문자열을 저장하기 위해서는 TEXT() 매크로를 제공
	//	TCHAR TCHARArray[] = TEXT("Hello, Unreal!");

	//	// 문자열을 좀 더 자유롭게 조작하고 싶다면 TCHAR 배열 대신 FString 자료형을 사용하면 됨
	//	// TCHAR 배열을 Wrapping한 헬퍼 클래스가 FString
	//	FString String0 = TCHARArray;
	//	// String0와 String1은 같음
	//	FString String1 = FString(TEXT("Hello, Unreal!"));
	//	// FString에 Asterisk(*)를 붙혀줘야 TCHAR 배열이 반환됨
	//	// String1의 시작 주소로부터 값을 꺼내온다는 느낌
	//	UE_LOG(LogTemp, Log, TEXT("String1: %s"), *String1);

	//	// 포인터로도 받을 수 있음
	//	const TCHAR* TCHARPtr = *String1;
	//	TCHAR* TCHARDataPtr = String1.GetCharArray().GetData();

	//	// C 문자열 라이브러리에서 제공하는 문자열 처리 함수(strstr(), ...)를 제공하는 클래스 FCString
	//	// 다만 사용이 안전하다는건 보장받지 못함
	//	TCHAR TCHARArrayWithSize[100];
	//	FCString::Strcpy(TCHARArrayWithSize, String1.Len(), *String1);

	//	// ESearchCase::IgnoreCase => 대소문자를 신경쓰지 마라
	//	// String1의 컨테이너에 "unreal"이 있는가?
	//	if (true == String1.Contains(TEXT("unreal"), ESearchCase::IgnoreCase))
	//	{
	//		// String1에 "unreal"을 찾아서 시작 위치 인덱스를 반환
	//		int32 Index = String1.Find(TEXT("unreal"), ESearchCase::IgnoreCase);
	//		// Mid 함수 사용시 => "unreal" 문자열이 시작되는 곳에서부터 마지막까지 자름
	//		FString EndString = String1.Mid(Index);

	//		UE_LOG(LogTemp, Log, TEXT("Find unreal: %s"), *EndString);
	//	}

	//	int32 IntValue = 7;
	//	float FloatValue = 3.141592f;

	//	FString StringWithNumber = FString::Printf(TEXT("int32: %d and float: %f"), IntValue, FloatValue);
	//	UE_LOG(LogTemp, Log, TEXT("StringWithNumber: %s"), *StringWithNumber);

	//	// float 자료형의 표준은 상당히 복잡함 이를 정돈해서 문자열로 바꿔줌
	//	FString StringWithFloat = FString::SanitizeFloat(FloatValue);
	//	UE_LOG(LogTemp, Log, TEXT("StringWithFloat: %s"), *StringWithFloat);

	//	FString StringWithInt = FString::FromInt(IntValue);
	//	UE_LOG(LogTemp, Log, TEXT("StringWithInt: %s"), *StringWithInt);

	//	// FCString => C 문자열 라이브러리 함수(atoi(), strcpy(), ...)를 제공하는 클래스
	//	int32 IntValueFromString = FCString::Atoi(*StringWithInt);
	//	float FloatValueFromString = FCString::Atof(*StringWithFloat);
	//	StringWithNumber = FString::Printf(TEXT("int32: %d and float: %f"), IntValueFromString, FloatValueFromString);
	//	UE_LOG(LogTemp, Log, TEXT("숫자로부터 만든 문자열: %s"), *StringWithNumber);
	//}
	// ===============================================
	// ===============================================
#pragma endregion

#pragma region FName,FText
	// ===============================================
	// ===============================================
	//// FName, FText
	//// FName: 문자열 자체를 그대로 사용하면 연산량이 증가, 따라서 문자열로 관리하되 내부적으로 해시값으로 변환 관리하는 것이 FName, 대소문자 구분X
	//// FText: 다국어 지원을 위한 문자열 클래스, UI에서 주로 사용, 키-밸류처럼 키로써 작동, ex) "한국" : "Korea" 처럼 로컬리제이션
	//{
	//	FName KeyName1(TEXT("PELVIS"));
	//	FName KeyName2(TEXT("pelvis"));
	//	UE_LOG(LogTemp, Log, TEXT("Compare FName: %s"), KeyName1 == KeyName2 ? TEXT("Same") : TEXT("Different"));

	//	for (int i = 0; i < 10000; ++i)
	//	{
	//		FName SearchInNamePool = FName(TEXT("pelvis"));
	//		// FName은 결국 전역 Pool을 조사해보는 작업이 수반됨 
	//		// 따라서 static 키워드를 통해서 재조사가 이뤄지지 않게끔 함
	//		const static FName StaticOnlyOnce(TEXT("pelvis"));
	//	}
	//}
	// ===============================================
	// ===============================================
#pragma endregion

}
//UE_ENABLE_OPTIMIZATION		// 5.3버전 여기까지 최적화 하지마!
PRAGMA_ENABLE_OPTIMIZATION		// 5.1버전 여기까지 최적화 하지마!

void USGameInstance::Shutdown()
{
	UE_LOG(LogTemp, Log, TEXT("Shutdown() has been called."));

	//// OnPigeonFlying에 HandlePigeonFlying이 바인드 되어있는지 확인
	//if (true == SpawnedPigeon->OnPigeonFlying.IsAlreadyBound(this, &ThisClass::HandlePigeonFlying))
	//{
	//	// 바인드 되어 있다면 OnPigeonFlying에 HandlePigeonFlying 제거
	//	SpawnedPigeon->OnPigeonFlying.RemoveDynamic(this, &ThisClass::HandlePigeonFlying);
	//}

	Super::Shutdown();
}

void USGameInstance::HandlePigeonFlying(const FString& InName, const int32 InID)
{
	UE_LOG(LogTemp, Log, TEXT("[%d] %s is now flying."), InID, *InName);
}
