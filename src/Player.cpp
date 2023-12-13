//
// Created by Salih on 09-Dec-23.
//


Player::Player(){

}


void Player::Init() {

    playerCamera = { 0 };
    playerCamera.position = (Vector3){ 0.25f, 0.25f, 0.25f };
    playerCamera.target = (Vector3){ 0.0f, 0.0f, 1.0f };
    playerCamera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    playerCamera.fovy = 60.0f;
    playerCamera.projection = CAMERA_PERSPECTIVE;

    std::cout << "Initialized Player" << "\n";
}

void Player::Update(float deltaTime) {

    ProcessInput(deltaTime);
}

Camera* Player::GetCamera() {
    return &playerCamera;
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
    CameraPitch(&playerCamera, smoothedInput.y,false,false,true);
    CameraRoll(&playerCamera, smoothedInput.z );
}

void Player::ProcessThrust(float deltaTime) {

    float accT = GetVelocityRatioToMaxValue();
    accT = accT * accT * accT;
    float acceleratePower = Lerp(thrustChangeSpeedMin, thrustChangeSpeedMax, accT);


    float move = 0;
    if(IsKeyDown(KEY_LEFT_SHIFT))
        move += acceleratePower;

    if(IsKeyDown(KEY_LEFT_CONTROL))
    {
        move = 0;
        currentVelocity = Vector3Lerp(currentVelocity, Vector3Zero(), stopInertiaSpeed * deltaTime);
    }

    Vector3 forward = GetCameraForward(&playerCamera);
    Vector3 velChange = Vector3Scale(forward,move * deltaTime);
    currentVelocity = Vector3Add(currentVelocity, velChange);
    currentVelocity = Vector3ClampValue(currentVelocity, -maxVelocity, maxVelocity);


    Position = Vector3Add(Position, currentVelocity);
    playerCamera.position = Position;

    playerCamera.position  = Position;
    playerCamera.target = Vector3Add(playerCamera.position, Vector3Normalize(forward));

    //playerCamera.target = Vector3Add(playerCamera.target, Position);
    //playerCamera.target = Vector3Scale(GetCameraForward(&playerCamera),1);
}

Vector3 Player::GetVelocityNormalized() {
    return Vector3Normalize(currentVelocity);
}

Vector3 Player::GetVelocity() {
    return currentVelocity;
}



float Player::GetVelocityRatioToMaxValue() {
    return Vector3Length(currentVelocity) / maxVelocity;
}
