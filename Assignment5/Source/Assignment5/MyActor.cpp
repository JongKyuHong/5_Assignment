// Fill out your copyright notice in the Description page of Project Settings.


#include "MyActor.h"

// Sets default values
AMyActor::AMyActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MoveCount = 0;
	TurnCount = 0;
	EventCount = 0;
	TotalDistance = 0.0f;
	bCompleteEvent = false;
}

// Called when the game starts or when spawned
void AMyActor::BeginPlay()
{
	Super::BeginPlay();
	FVector StartLocation(0.0f, 50.0f, 0.0f);
	SetActorLocation(StartLocation);

	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, FString::Printf(TEXT("위치 출력 >>> X : %f, Y : %f, Z : %f"), StartLocation.X, StartLocation.Y, StartLocation.Z));
}

// Called every frame
void AMyActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 이미 10회 이동하면 더이상 이동 안함
	if (bCompleteEvent) {
		return;
	}

	static float TimeAccumulator = 0.0f;
	TimeAccumulator += DeltaTime;

	// 0.5초마다 실행
	if (TimeAccumulator >= 0.5f) {
		TimeAccumulator = 0.0f;
		if (MoveCount + TurnCount < 10) {
			// 50퍼센트 확률로 이동 or 회전 선택
			if (FMath::RandRange(0, 1)) {
				Move();
				MoveCount++;
				// 50퍼센트 확률로 이벤트 발생
				if (FMath::RandRange(0, 1)) {
					EventPrint(TEXT("이동"));
					EventCount++;
				}
			} else {
				Turn();
				TurnCount++;
				if (FMath::RandRange(0, 1)) {
					EventPrint(TEXT("회전"));
					EventCount++;
				}
			}
		} else {
			// 10회 완료
			bCompleteEvent = true;

			// 통계
			GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Cyan, FString::Printf(TEXT("이동끝")));
			GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Cyan, FString::Printf(TEXT("총 이동거리 : %f"), TotalDistance));
			GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Cyan, FString::Printf(TEXT("총 이벤트 발생 횟수 : %d"), EventCount));
		}
	}
}

void AMyActor::Move() {
	// 현재 바라보는 방향의 벡터
	FVector ForwardVector = GetActorForwardVector();

	// 랜덤 이동값
	float MoveDistance = FMath::FRandRange(500.0f, 1000.0f);

	// 새 위치 계산
	FVector PrevLocation = GetActorLocation();
	FVector NewLocation = PrevLocation + (ForwardVector * MoveDistance);

	// Z축 고정으로 평면 이동만
	NewLocation.Z = PrevLocation.Z;
	SetActorLocation(NewLocation);

	// 이동거리 계산
	float dx = NewLocation.X - PrevLocation.X;
	float dy = NewLocation.Y - PrevLocation.Y;
	float Distance = FMath::Sqrt(dx * dx + dy * dy);

	// 총 이동거리 누적
	TotalDistance += Distance;

	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Orange,
		FString::Printf(TEXT("%d번 이동, 현재위치 >>> X : %f, Y : %f, Z : %f"),
			MoveCount + 1, NewLocation.X, NewLocation.Y, NewLocation.Z));
}

void AMyActor::Turn() {
	// 랜덤 회전 값
	float RandomRotateValue = FMath::FRandRange(-180.0f, 180.0f);

	FRotator NewRotation = GetActorRotation();

	// 좌우회전에 랜덤값 추가
	NewRotation.Yaw += RandomRotateValue;

	SetActorRotation(NewRotation);

	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Magenta, FString::Printf(TEXT("%d번 회전 >> %f도"), TurnCount + 1, RandomRotateValue));
}

void AMyActor::EventPrint(const FString& EventType) {
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red,
		FString::Printf(TEXT("%s시 랜덤 이벤트 발생!!!!!!!"), *EventType));
}