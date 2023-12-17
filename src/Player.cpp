//
// Created by Salih on 09-Dec-23.
//


Player::Player(){

}


void Player::Init() {

    SetTeam(TEAM_PLAYER);

    playerCamera = { 0 };
    playerCamera.position = (Vector3){ 0.25f, 0.25f, 0.25f };
    playerCamera.target = (Vector3){ 0.0f, 0.0f, 1.0f };
    playerCamera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    playerCamera.fovy = 60.0f;
    playerCamera.projection = CAMERA_PERSPECTIVE;

    hudCamera = { 0 };
    hudCamera.position = Vector3Zero();
    hudCamera.target = (Vector3){ 0.0f, 0.0f, 1.0f };
    hudCamera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    hudCamera.fovy = 60.0f;
    hudCamera.projection = CAMERA_PERSPECTIVE;


    std::cout << "Initialized Player" << "\n";
}

void Player::Update(float deltaTime) {

    ProcessInput(deltaTime);
    GameObject::Update(deltaTime);
}



void Player::ProcessInput(float deltaTime){

    ProcessRotation(deltaTime);
    ProcessThrust(deltaTime);
}



void Player::ProcessRotation(float deltaTime) {
    Vector3 inputVec = {0,0,0};

    if(IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))
        inputVec.x += 1;
    if(IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))
        inputVec.x -= 1;
    if(IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))
        inputVec.y += 1;
    if(IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))
        inputVec.y -= 1;

    if(IsKeyDown(KEY_E))
        inputVec.z += 1;
    if(IsKeyDown(KEY_Q))
        inputVec.z -= 1;


    inputVec = Vector3Normalize(inputVec);




    //Smooth input
    if(abs(inputVec.x) > 0.1F)
    {
        smoothedInput.x += inputVec.x * rotateAccelerateSpeed * deltaTime;
    }
    else
    {
        smoothedInput.x = Lerp(smoothedInput.x, 0, rotateSettleSpeed * deltaTime);
    }

    if(abs(inputVec.y) > 0.1F)
    {
        smoothedInput.y += inputVec.y * rotateAccelerateSpeed * deltaTime;
    }
    else
        smoothedInput.y = Lerp(smoothedInput.y, 0, rotateSettleSpeed * deltaTime);

    if(abs(inputVec.z) > 0.1F)
    {
        smoothedInput.z += inputVec.z * rotateAccelerateSpeed * deltaTime;
    }
    else
        smoothedInput.z = Lerp(smoothedInput.z, 0, rotateSettleSpeed * deltaTime);


    smoothedInput.x = Clamp(smoothedInput.x, -maxRotationSpeed, maxRotationSpeed);
    smoothedInput.y = Clamp(smoothedInput.y, -maxRotationSpeed, maxRotationSpeed);
    smoothedInput.z = Clamp(smoothedInput.z, -maxRotationSpeed, maxRotationSpeed);

    //Rotate camera methods
    CameraYaw(&playerCamera, smoothedInput.x, false);
    CameraPitch(&playerCamera, smoothedInput.y,false,false, true);
    CameraRoll(&playerCamera, smoothedInput.z );


/*
    Vector3 toTarget = Vector3Subtract(playerCamera.target,playerCamera.position);
    toTarget = Vector3Normalize(toTarget);


    Rotation.x = asinf(-toTarget.y);
    Rotation.y = atan2f(toTarget.x, toTarget.z);
    Rotation.z = 0.0f;  // Assuming roll is 0

    float yaw = Rotation.y;
    float pitch = Rotation.x;

    Vector3 direction = {
            cosf(pitch) * sinf(yaw),
            -sinf(pitch),
            cosf(pitch) * cosf(yaw)
    };

    DrawLine3D(playerCamera.target, Vector3Add(playerCamera.target,toTarget), PURPLE);

    DrawSphereWires(Vector3Add(playerCamera.target,toTarget), 0.1f, 16, 16, PURPLE);

    DrawSphereWires(Vector3Add(playerCamera.target,direction), 0.2f, 16, 16, RED);

    //DrawSphereWires(playerCamera.target, 0.1f, 16, 16, RED);*/
}

void Player::ProcessThrust(float deltaTime) {

    float accT = GetVelocityRatioToMaxValue();
    accT = accT * accT * accT;
    float acceleratePower = Lerp(thrustChangeSpeedMin, thrustChangeSpeedMax, accT);


    thrust = 0;
    if (IsKeyDown(KEY_LEFT_SHIFT))
        thrust += acceleratePower * deltaTime;

    if (IsKeyDown(KEY_LEFT_CONTROL)) {
        thrust = 0;
        currentVelocity = Vector3Lerp(currentVelocity, Vector3Zero(), stopInertiaSpeed * deltaTime);
    }

    smoothThrust = Lerp(smoothThrust, thrust, deltaTime * 5.0f);

    Vector3 forward = GetCameraForward(&playerCamera);
    Vector3 velChange = Vector3Scale(forward, thrust);
    currentVelocity = Vector3Add(currentVelocity, velChange);
    currentVelocity = Vector3ClampValue(currentVelocity, -maxVelocity, maxVelocity);



    playerCamera.position = Position;
    playerCamera.target = Vector3Add(playerCamera.position, Vector3Normalize(forward));

    //playerCamera.target = Vector3Add(playerCamera.target, Position);
    //playerCamera.target = Vector3Scale(GetCameraForward(&playerCamera),1);
}


float Player::GetVelocityRatioToMaxValue() {
    return Vector3Length(currentVelocity) / maxVelocity;
}

Camera* Player::GetCamera() {
    return &playerCamera;
}

Camera *Player::GetHUDCamera() {
    return &hudCamera;
}

Vector3 Player::GetCameraDirection() {
    Vector3 toTarget = Vector3Subtract(playerCamera.target,playerCamera.position);
    toTarget = Vector3Normalize(toTarget);
    return toTarget;
}

Vector3 Player::GetSwayInput() {
    return (Vector3){smoothedInput.y,smoothedInput.x,smoothThrust};
}
