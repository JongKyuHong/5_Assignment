## 5번 과제하기

### 1. 시작 위치 설정하기

```c++
FVector StartLocation(0.0f, 50.0f, 0.0f);
SetActorLocation(StartLocation);
```

- 벡터이므로 x, y, z를 뜻한다.
- f는 float 리터럴을 나타내는 C++의 접미사이다.
- C++에서 소수점 리터럴은 기본적으로 double타입이고 f를 붙이면 float로 변환
- FVector의 경우 float를 사용하기 때문에 타입 불일치가 생길 수 있다. 그래서 f를붙여줌
- StartLocation은 임의로 변수이름을 지은것이고 SetActorLocation()은 액터의 월드 좌표를 설정한다.

### 2. 로그 출력하기

```c++
GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, FString::Printf(TEXT("위치 출력 >>> X : %f, Y : %f, Z : %f"), StartLocation.X, StartLocation.Y, StartLocation.Z));
```

- AddOnScreenDebugMessage를 통해서 인게임내 로그출력을 한다.
- 첫번째 인자 : 메시지 식별 키
  - -1일시 : 매번 새로운 줄에 메시지 추가
  - 양수일시 : 같은 Key의 이전 메시지를 덮어씌운다.
- 두번째 인자 : 표시 시간
- 세번째 인자 : 색상
- 네번째 인자 : 디버그 메시지(출력 메시지)
- 다섯번째 인자 : 최신 메시지 위치(기본값 true : 새 메시지가 위쪽에 표시)
- 여섯번째 인자 : 텍스트 크기
- FString과 Printf를 통해서 TEXT에 변수를 찍어 줄 수 있다.

### 3. Turn, Move함수 만들기

- MyActor.h에서 MyActor클래스의 private에 Turn, Move, EventPrint함수를 선언해 놓는다.
- 총 이동거리, 이벤트 발생 횟수, Move함수 호출 수, Turn함수 호출 수, 이동 종료 변수도 선언

```c++
class ASSIGNMENT5_API AMyActor : public AActor
{
...
private:
	void Move();
	void Turn();
    void EventPrint();

    int MoveCount;
    int TurnCount;
    int EventCount;
    float TotalDistance;
    bool bCompleteEvent;
};
```

#### Move함수 만들기

```c++
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
	float Distance = FMath::Sqrt(dx*dx + dy*dy);

	// 총 이동거리 누적
	TotalDistance += Distance;

	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Orange,
		FString::Printf(TEXT("%d번 이동, 현재위치 >>> X : %f, Y : %f, Z : %f"),
			MoveCount+1, NewLocation.X, NewLocation.Y, NewLocation.Z));
}
```

- GetActorForwardVector를 구하고, 이를 랜덤 이동값과 곱하고 기존 액터 위치와 더한값을 새로운 위치로 저장한다.
- Z축은 고정하고 평면에서만 위치가 변할 수 있게 Z값은 계속 유지하려고 했다.
- 로그를 찍을때 헤더에서 private으로 설정했던 MoveCount를 찍어주고 새로운 좌표의 X,Y,Z를 출력했다.

#### Turn함수 만들기

```c++
void AMyActor::Turn() {
	// 랜덤 회전 값
	float RandomRotateValue = FMath::FRandRange(-180.0f, 180.0f);

	FRotator NewRotation = GetActorRotation();

	// 좌우회전에 랜덤값 추가
	NewRotation.Yaw += RandomRotateValue;

	SetActorRotation(NewRotation);

	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Magenta, FString::Printf(TEXT("%d번 회전 >> %f도"), TurnCount+1, RandomRotateValue));
}
```

- GetActorRotation으로 액터의 회전 값을 구하고 랜덤 값을 Yaw에 더해주고 저장

### 4. Tick() 수정하기

- Tick()

```c++
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
```

- DeltaTime은 이전 프레임과 현재 프레임 사이의 시간 차
  - 60FPS환경에서는 1/60초 즉 0.0166초
  - Accumulator를 0.5f로 지정해서 0.5초마다 이동이 가능하게 하였다.
- 이동시 50퍼센트 확률로 Move() 혹은 Turn()이 호출된다.
  - 각각 MoveCount, TurnCount로 카운팅된다.
- 또, 이동시 50퍼센트 확률로 랜덤 이벤트가 발생할 수 있다. (메시지만 출력)
- MoveCount+TurnCount가 10이되면 이동을 멈추고 통계를 로그로 띄우고 끝나게 된다.

### EventPrint() 함수 수정

```c++
void AMyActor::EventPrint(const FString& EventType) {
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red,
		FString::Printf(TEXT("% s 시 랜덤 이벤트 발생!!!!!!!"), *EventType));
}
```

- Move()냐 Turn()이냐에 따라 다르게 로그를 출력시켜주었다.
