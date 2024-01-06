//
// Created by Salih on 09-Dec-23.
//


Player* Player::instance = nullptr;

Player::Player() : GameObject(){

}


void Player::Init() {

    SetTeam(TEAM_PLAYER);

    Position = Vector3Zero();
    Rotation = QuaternionIdentity();

    playerCamera = { 0 };
    playerCamera.position = (Vector3){ 0.25f, 0.25f, 0.25f };
    playerCamera.target = (Vector3){ 0.0f, 0.0f, 1.0f };
    playerCamera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    playerCamera.fovy = defaultFOV;
    playerCamera.projection = CAMERA_PERSPECTIVE;

    hudCamera = { 0 };
    hudCamera.position = Vector3Zero();
    hudCamera.target = (Vector3){ 0.0f, 0.0f, 1.0f };
    hudCamera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    hudCamera.fovy = defaultFOV;
    hudCamera.projection = CAMERA_PERSPECTIVE;
    CollisionSize = 2.0f;

    IgnoresAllCollisions = false;
    CanCollide = true;

    SetHealth(STAT_HEALTH_PLAYER);

    AddTag("Player");


    std::cout << "Initialized Player" << "\n";
}

void Player::Update(float deltaTime) {

    ProcessInput(deltaTime);
    ProcessShoot(deltaTime);
    ProcessCamera(deltaTime);


    GameObject::Update(deltaTime);
}



void Player::ProcessInput(float deltaTime){

    ProcessRotation(deltaTime);
    ProcessThrust(deltaTime);

}


Vector3 EulerAnglesToDirectionVector(const Vector3& angles)
{
    float cosX = cos(angles.x);
    float sinX = sin(angles.x);
    float cosY = cos(angles.y);
    float sinY = sin(angles.y);
    float cosZ = cos(angles.z);
    float sinZ = sin(angles.z);

    Vector3 directionVector;
    directionVector.x = cosY * cosZ;
    directionVector.y = sinX * sinY * cosZ + cosX * sinZ;
    directionVector.z = -cosX * sinY * cosZ + sinX * sinZ;

    return directionVector;
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
    else{
        if(abs(smoothedInput.x) > 0.01f )
            smoothedInput.x = Lerp(smoothedInput.x, 0.0f, rotateSettleSpeed * deltaTime);
        else
            smoothedInput.x = 0;
    }

    if(abs(inputVec.y) > 0.1F)
    {
        smoothedInput.y += inputVec.y * rotateAccelerateSpeed * deltaTime;
    }
    else {
        if(abs(smoothedInput.y) > 0.01f )
            smoothedInput.y = Lerp(smoothedInput.y, 0.0f, rotateSettleSpeed * deltaTime);
        else
            smoothedInput.y = 0;
    }


    if(abs(inputVec.z) > 0.1F)
    {
        smoothedInput.z += inputVec.z * rotateAccelerateSpeed * deltaTime;
    }
    else {
        if(abs(smoothedInput.z) > 0.01f )
            smoothedInput.z = Lerp(smoothedInput.z, 0.0f, rotateSettleSpeed * deltaTime);
        else
            smoothedInput.z = 0;
    }


    smoothedInput = Vector3ClampValue(smoothedInput,-maxRotationSpeed,maxRotationSpeed);

    //Rotate camera methods
    CameraYaw(&playerCamera, smoothedInput.x, false);
    CameraPitch(&playerCamera, smoothedInput.y,false,false, true);
    CameraRoll(&playerCamera, smoothedInput.z );

/*    Rotation = QuaternionIdentity();*/

/*    Matrix lookMatrix = GetCameraViewMatrix(&playerCamera);
    Rotation = QuaternionFromMatrix(lookMatrix);*/

    //Rotation = Utility::LookAt(playerCamera.position,playerCamera.target);

    Vector3 target = GetCameraDirection();


/*    Rotation.x += -smoothedInput.y;
    Rotation.y += smoothedInput.x;
    Rotation.z += smoothedInput.z;*/
}

void Player::ProcessThrust(float deltaTime) {

    float accT = GetVelocityRatioToMaxValue();
    accT = accT * accT * accT;
    float acceleratePower = Lerp(thrustChangeSpeedMin, thrustChangeSpeedMax, accT);


    thrust = 0;
    if (IsKeyDown(KEY_LEFT_SHIFT)) {
        thrust += acceleratePower * deltaTime;

        if(trauma < thrustShakeTrauma)
            trauma = thrustShakeTrauma;
    }

    if (IsKeyDown(KEY_LEFT_CONTROL)) {
        thrust = 0;
        currentVelocity = Vector3Lerp(currentVelocity, Vector3Zero(), stopInertiaSpeed * deltaTime);
    }

    smoothThrust = Lerp(smoothThrust, thrust, deltaTime * 10.0f);

    Vector3 forward = GetCameraForward(&playerCamera);
    Vector3 velChange = Vector3Scale(forward, thrust);
    currentVelocity = Vector3Add(currentVelocity, velChange);
    currentVelocity = Vector3ClampValue(currentVelocity, -maxVelocity, maxVelocity);
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
/*    Vector3 toTarget = Vector3Subtract(playerCamera.target,playerCamera.position);
    toTarget = Vector3Normalize(toTarget);
    return toTarget;*/
    return GetCameraForward(&playerCamera);
}

Vector3 Player::GetSwayInput() {
    return (Vector3){smoothedInput.y,smoothedInput.x,smoothThrust};
}

void Player::ShakeCamera(float amount) {
    trauma += amount;
    trauma = fmin(trauma,1.0f);
}

void Player::ProcessCamera(float deltaTime) {

    float shakeAmount = trauma * trauma;
    float offsetX = GetRandomValue(-100, 100) * shakeMaxMoveAmount * shakeAmount * deltaTime;
    float offsetY = GetRandomValue(-100, 100) * shakeMaxMoveAmount * shakeAmount * deltaTime;
    float offsetZ = GetRandomValue(-100, 100) * shakeMaxMoveAmount * shakeAmount * deltaTime;

    shakeOffset = (Vector3){offsetX,offsetY,offsetZ};

    if(trauma > 0)
        trauma -= traumaDecay * deltaTime;


    //FOV changes
    float newFOV = defaultFOV;

    if(thrust > 0)
        newFOV += thrustFOVChange;

    if(shotThisFrame)
        newFOV += shootFOVChange;

    playerCamera.fovy = Lerp(playerCamera.fovy,newFOV,deltaTime * fovChangeSpeed);
    hudCamera.fovy = playerCamera.fovy;
}

void Player::OnCollision(GameObject *otherObject, Vector3 collisionTotalVelocity) {
    GameObject::OnCollision(otherObject, collisionTotalVelocity);

    float collisionForce = Vector3Length(collisionTotalVelocity);
    float shakeForce = collisionForce / maxVelocity;
    ShakeCamera(shakeForce);
}

void Player::ProcessShoot(float deltaTime) {

    shotThisFrame = false;

    if(IsKeyDown(KEY_SPACE)) {

        shootTimer += deltaTime;
        if (shootTimer < shootCooldown)
            return;

        //Shoot
        shootTimer = 0;
        if (trauma < shootShakeTrauma) {
            trauma = shootShakeTrauma;
            shakeOffset.z += 0.5f;
        }
        shootCanonIndex++;
        if (shootCanonIndex > 1)
            shootCanonIndex = 0;

        //Adjust shoot position to match with guns
        Vector3 shootPosition = Vector3Zero();
        Vector3 rotationBasedShootPosition = Vector3Zero();
        Vector3 right = GetCameraRight(&playerCamera);
        Vector3 forward = GetCameraForward(&playerCamera);
        Vector3 up = GetCameraUp(&playerCamera);

        shootPosition = Vector3Add(shootPosition, Vector3Scale(forward, 2.3f));
        shootPosition = Vector3Add(shootPosition, Vector3Scale(up, -0.2f));

        rotationBasedShootPosition = (Vector3){0,-0.2f,2.3f};

        if (shootCanonIndex == 0) {
            rotationBasedShootPosition.x = 1.48f;
            shootPosition = Vector3Add(shootPosition, Vector3Scale(right, 1.48f));
        }else {
            rotationBasedShootPosition.x = -1.48f;
            shootPosition = Vector3Add(shootPosition, Vector3Scale(right, -1.48f));
        }

        Vector3 shootDirection = GetCameraForward(&playerCamera);
        shootDirection = Vector3Add(shootDirection, Vector3Scale(GetVelocityNormalized(), 0.5f * Vector3Length(GetVelocity())));
        //shootDirection = Vector3Normalize(shootDirection);

        Vector3 globalShootPosition = Vector3Add(Position, shootPosition);
        BulletManager::GetInstance()->CreateBullet(globalShootPosition, shootDirection, 0, TEAM_PLAYER);

        //Push back
        //currentVelocity = Vector3Add(currentVelocity, Vector3Scale(shootDirection, -shootBackwardsPush));

        //Particle
        //cout << shootPosition.x << " " << shootPosition.y << " " << shootPosition.z << endl;
        ParticleManager::GetInstance()->CreateShootMuzzle(shootPosition,instance);

        shotThisFrame = true;
        OnShoot.Invoke(shootCanonIndex);
    }
}


void Player::LateUpdate(float deltaTime) {
    GameObject::LateUpdate(deltaTime);

    //Update camera
    camPosition = Vector3Add(Position,shakeOffset);
    playerCamera.position = camPosition;
    playerCamera.target = Vector3Add(playerCamera.target, Vector3Normalize(GetCameraForward(&playerCamera)));
    hudCamera.position = shakeOffset;
    hudCamera.target = Vector3Add(hudCamera.position , (Vector3){ 0.0f, 0.0f, 1.0f });
}

Player *Player::GetInstance() {
    if(instance == nullptr)
        instance = new Player();

    return instance;
}

void Player::AddScrap(int amount) {
    totalScrap += amount;

    Vector3 particlePos = Vector3Add(GetCollectPosition(),Vector3Scale(GetCameraForward(&playerCamera),5.0f));
    particlePos = Vector3Add(particlePos,Vector3Scale(GetCameraUp(&playerCamera),1.0f));
    ParticleManager::GetInstance()->CreateCollectFX(particlePos, GetCameraForward(&playerCamera),PALETTE_PURPLE2);

    OnScrapGain.Invoke(amount);
}

Vector3 Player::GetCollectPosition() {

    return Vector3Add(Position,Vector3Scale(GetCameraUp(&playerCamera),-3.0f));
}

void Player::Hurt(float damage) {

    damage = damage * STAT_HEALTH_PLAYER_DAMAGE_REDUCTION;


    if(trauma < 0.4f)
        ShakeCamera(0.2f);

    GameObject::Hurt(damage);


}

