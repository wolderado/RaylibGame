//
// Created by Salih on 20-Dec-23.
//

#include "BulletManager.h"

BulletManager* BulletManager::instance = nullptr;

void BulletManager::UpdateAndRender(float deltaTime) {


    //Loop through all active bullets
    for (int i = 0; i < activeBullets.size(); i++) {

        //Update
        Vector3 velocity = Vector3Scale(activeBullets[i].direction,activeBullets[i].speed * deltaTime);
        Vector3 newPos = Vector3Add(activeBullets[i].position,velocity);
        activeBullets[i].position = newPos;
        activeBullets[i].lifeTime += deltaTime;
        if(activeBullets[i].lifeTime > activeBullets[i].maxLifeTime)
        {
            activeBullets[i].isActive = false;
            inactiveBullets.push(activeBullets[i]);
            activeBullets.erase(activeBullets.begin() + i);
            continue;
        }

        //Render
        renderer->RenderBillboard(0,0,activeBullets[i].position,1.0f,0);
/*
        DrawCircle3D(activeBullets[i].position,0.1f,Vector3Zero(),0,PALETTE_YELLOW1);
*/

        shared_ptr<GameObject> bulletHit = World::GetInstance()->CheckBulletCollision(activeBullets[i].position);
        if(bulletHit != nullptr) {
            if (bulletHit->GetTeam() != activeBullets[i].sourceTeam) {
                BulletHit(i, bulletHit);
                break;
            }
        }
    }


    //cout << (*cam).position.x << endl;

}

void BulletManager::CreateBullet(Vector3 position,Vector3 direction,int spriteIndex,TEAM sourceTeam) {

    Bullet newBullet;
    if(inactiveBullets.size() > 0)
    {
        newBullet = inactiveBullets.top();
        inactiveBullets.pop();
    }else
        newBullet = Bullet();

    newBullet.isActive = true;
    newBullet.position = position;
    newBullet.direction = direction;
    newBullet.lifeTime = 0;
    newBullet.maxLifeTime = 2.0f;
    newBullet.spriteIndex = spriteIndex;
    newBullet.speed = 100.0f + GetRandomValue(0,50);
    newBullet.damage = 1.0f;
    newBullet.sourceTeam = sourceTeam;

    activeBullets.push_back(newBullet);



}


BulletManager::BulletManager() {


}


BulletManager* BulletManager::GetInstance() {
    if(instance == nullptr)
        instance = new BulletManager();

    return instance;
}

void BulletManager::Init() {

    renderer = Renderer::GetInstance();
}

void BulletManager::BulletHit(int bulletID, shared_ptr<GameObject> target) {

    target->Hurt(activeBullets[bulletID].damage);

    if(activeBullets[bulletID].sourceTeam == TEAM_PLAYER)
        target->ShowHealthBar = true;

    activeBullets[bulletID].isActive = false;
    inactiveBullets.push(activeBullets[bulletID]);
    activeBullets.erase(activeBullets.begin() + bulletID);

    ParticleManager::GetInstance()->CreateHitParticle(activeBullets[bulletID].position);
}
