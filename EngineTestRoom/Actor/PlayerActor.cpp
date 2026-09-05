#include "PlayerActor.h"

#include <cmath>
#include <Component/SceneComponent.h>
#include <Component/CameraComponent.h>
#include <Render/Renderer3D.h>
#include <World/World.h>
#include <World/Level.h>
#include <algorithm>
#include <Input/Input.h>

PlayerActor::PlayerActor(const Vector3& location)
    : super(location)
{
    sceneComponent = AddComponent<USceneComponent>();
    SetRootComponent(sceneComponent);
    
    cameraComponent = AddComponent<UCameraComponent>();
    cameraComponent->SetAttachParent(sceneComponent);

    cameraComponent->SetRelativeLocation(Vector3(0.0f, 0.0f, 0.0f));
    cameraComponent->SetTarget(Vector3::Zero);
}

PlayerActor::~PlayerActor()
{
}

void PlayerActor::BeginPlay()
{
    AActor::BeginPlay();
    
    GetLevel()->GetWorld()->SetActiveCamera(cameraComponent);
    Input::Get().SetMouseCaptured(true);
    Look(0.0f, 0.0f);

}

void PlayerActor::Tick(float deltaTime)
{
    AActor::Tick(deltaTime);
    ProcessInput(deltaTime);
}

void PlayerActor::ProcessInput(float deltaTime)
{
    Input& input = Input::Get();

    /* Move */
    float forwardInput = 0.0f;
    float rightInput = 0.0f;
    float verticalInput = 0.0f;

    if (input.GetKey('W')) forwardInput += 1.0f;    
    if (input.GetKey('A')) rightInput -= 1.0f;
    if (input.GetKey('S')) forwardInput -= 1.0f;
    if (input.GetKey('D')) rightInput += 1.0f;
    if (input.GetKey(VK_SPACE)) verticalInput += 1.0f;
    if (input.GetKey(VK_CONTROL)) verticalInput -= 1.0f;

    Move(forwardInput, rightInput, verticalInput, deltaTime);

    /* Look */
    const Vector2& mouseDelta = input.GetMouseDelta();
    Look(mouseDelta.x, mouseDelta.y);
}

void PlayerActor::Move(float forwardInput, float rightInput, float verticalInput, float deltaTime)
{
    const float inputLength = std::sqrt(forwardInput * forwardInput + rightInput * rightInput +verticalInput * verticalInput);

    if (inputLength > 1.0f)
    {
        forwardInput /= inputLength;
        rightInput /= inputLength;
        verticalInput /= inputLength;
    }

    const float cosYaw = std::cos(yaw);
    const float sinYaw = std::sin(yaw);

    const Vector3 forward(cosYaw, 0.0f, sinYaw);
    const Vector3 right(sinYaw, 0.0f, -cosYaw);

    const Vector3 moveDirection(
        forward.x * forwardInput + right.x * rightInput,
        verticalInput,
        forward.z * forwardInput + right.z * rightInput
    );

    constexpr float moveSpeed = 10.0f;

    Vector3 location = GetActorLocation();

    location.x += moveDirection.x * moveSpeed * deltaTime;
    location.y += moveDirection.y * moveSpeed * deltaTime;
    location.z += moveDirection.z * moveSpeed * deltaTime;

    SetActorLocation(location);
}

void PlayerActor::Look(float deltaX, float deltaY)
{
    yaw -= deltaX * mouseSensitivity;

    /*
     * 화면 좌표의 y는 아래로 갈수록 커지므로
     * 마우스를 위로 움직였을 때 위를 보도록 뺀다.
     */
    pitch -= deltaY * mouseSensitivity;

    constexpr float pitchLimit = 1.5533f; // 약 89도
    pitch = std::clamp(pitch, -pitchLimit, pitchLimit);

    const float cosPitch = std::cos(pitch);

    const Vector3 forward(
        cosPitch * std::cos(yaw),
        std::sin(pitch),
        cosPitch * std::sin(yaw)
    );

    const Vector3 cameraLocation = cameraComponent->GetWorldLocation();
    cameraComponent->SetTarget(cameraLocation + forward);
}
 