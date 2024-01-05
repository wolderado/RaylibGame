//
// Created by Salih on 20-Dec-23.
//

#include "BulletManager.h"

BulletManager* BulletManager::instance = nullptr;

void BulletManager::UpdateAndRender(float deltaTime) {


    //cout << "Active Bullets: " << activeBullets.size() << endl;

    World::GetInstance()->DEBUG_BulletCount = activeBullets.size();

    //Loop through all active bullets
    for (int i = activeBullets.size()-1; i > 0; i--) {

        //Render
        renderer->RenderBillboard(0,0,activeBullets[i].position,1.0f,0,PALETTE_YELLOW1);


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



        auto[isHit, hitObject] = World::GetInstance()->CheckBulletCollision(activeBullets[i].position,activeBullets[i].sourceTeam);
        if(isHit) {
            BulletHit(i, hitObject);
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
    newBullet.lifeTime = 0.0f;
    newBullet.maxLifeTime = 2.0f;

    if(sourceTeam == TEAM_PLAYER)
        newBullet.maxLifeTime = 4.0f;

    newBullet.spriteIndex = spriteIndex;
    newBullet.speed = 100.0f + GetRandomValue(0,50);
    newBullet.damage = 4.0f;
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

void BulletManager::BulletHit(int index, GameObject*  target) {

    target->Hurt(activeBullets[index].damage);

    if(activeBullets[index].sourceTeam == TEAM_PLAYER)
        target->ShowHealthBar = true;

    ParticleManager::GetInstance()->CreateHitParticle(activeBullets[index].position);
    //activeBullets[index].lifeTime = activeBullets[index].maxLifeTime + 1;
}

void BulletManager::EraseBulletAt(int i) {

    activeBullets[i].isActive = false;
    inactiveBullets.push(activeBullets[i]);
    activeBullets.erase(activeBullets.begin() + i);
}
