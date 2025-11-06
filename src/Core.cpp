#include "Core.h"


// Creature Inherited Base Behavior
void Creature::setBounds(int w, int h) { m_width = w; m_height = h; }
void Creature::normalize() {
    float length = std::sqrt(m_dx * m_dx + m_dy * m_dy);
    if (length != 0) {
        m_dx /= length;
        m_dy /= length;
    }
}

void Creature::moveBy(float dx, float dy) {
    // mueve y mantiene dentro de los bounds definidos con setBounds
  m_x = ofClamp(m_x + dx, 0.f, m_width);
  m_y = ofClamp(m_y + dy, 0.f, m_height);
}

void Creature::bounce() {
   bool hit = false;

    // Rebote en los bordes horizontales
    if (m_x < 0.f) {
        m_x = 0.f;
        m_dx = std::abs(m_dx); // Cambia dirección a la derecha
        hit = true;
    }
    else if (m_x > m_width) {
        m_x = m_width;
        m_dx = -std::abs(m_dx); // Cambia dirección a la izquierda
        hit = true;
    }

    // Rebote en los bordes verticales
    if (m_y < 0.f) {
        m_y = 0.f;
        m_dy = std::abs(m_dy); // Cambia dirección hacia abajo
        hit = true;
    }
    else if (m_y > m_height) {
        m_y = m_height;
        m_dy = -std::abs(m_dy); // Cambia dirección hacia arriba
        hit = true;
    }

    // Normaliza dirección si hubo rebote
    if (hit) {
        float len = std::sqrt(m_dx * m_dx + m_dy * m_dy);
        if (len > 0.0001f) {
            m_dx /= len;
            m_dy /= len;
        }
    }
}
    
    



void GameEvent::print() const {
        
        switch (type) {
            case GameEventType::NONE:
                ofLogVerbose() << "No event." << std::endl;
                break;
            case GameEventType::COLLISION:
                ofLogVerbose() << "Collision event between creatures at (" 
                << creatureA->getX() << ", " << creatureA->getY() << ") and ("
                << creatureB->getX() << ", " << creatureB->getY() << ")." << std::endl;
                break;
            case GameEventType::CREATURE_ADDED:
                ofLogVerbose() << "Creature added at (" 
                << creatureA->getX() << ", " << creatureA->getY() << ")." << std::endl;
                break;
            case GameEventType::CREATURE_REMOVED:
                ofLogVerbose() << "Creature removed at (" 
                << creatureA->getX() << ", " << creatureA->getY() << ")." << std::endl;
                break;
            case GameEventType::GAME_OVER:
                ofLogVerbose() << "Game Over event." << std::endl;
                break;
            case GameEventType::NEW_LEVEL:
                ofLogVerbose() << "New Game level" << std::endl;
            default:
                ofLogVerbose() << "Unknown event type." << std::endl;
                break;
        }
};

// collision detection between two creatures
bool checkCollision(std::shared_ptr<Creature> a, std::shared_ptr<Creature> b) {
    if (!a || !b) return false;

       float dx = a->getX() - b->getX();
    float dy = a->getY() - b->getY();
    float combinedRadius = a->getCollisionRadius() + b->getCollisionRadius();

    // distancia^2 <= (r1+r2)^2  →  hay colisión
    return (dx*dx + dy*dy) <= (combinedRadius * combinedRadius);
  }


string GameSceneKindToString(GameSceneKind t){
    switch(t)
    {
        case GameSceneKind::GAME_INTRO: return "GAME_INTRO";
        case GameSceneKind::AQUARIUM_GAME: return "AQUARIUM_GAME";
        case GameSceneKind::GAME_OVER: return "GAME_OVER";
    };
};

std::shared_ptr<GameScene> GameSceneManager::GetScene(string name){
    if(!this->HasScenes()){return nullptr;}
    for(std::shared_ptr<GameScene> scene : this->m_scenes){
        if(scene->GetName() == name){
            return scene;
        }
    }
    return nullptr;
}

void GameSceneManager::Transition(string name){
    if(!this->HasScenes()){return;} // no need to do anything if nothing inside
    std::shared_ptr<GameScene> newScene = this->GetScene(name);
    if(newScene == nullptr){return;} // i dont have the scene so time to leave
    if(newScene->GetName() == this->m_active_scene->GetName()){return;} // another do nothing since active scene is already pulled
    this->m_active_scene = newScene; // now we keep it since this is a valid transition
    return;
}

void GameSceneManager::AddScene(std::shared_ptr<GameScene> newScene){
    if(this->GetScene(newScene->GetName()) != nullptr){
        return; // this scene already exist and shouldnt be added again
    }
    this->m_scenes.push_back(newScene);
    if(m_active_scene == nullptr){
        this->m_active_scene = newScene; // need to place in active scene as its the only one in existance right now
    }
    return;
}

std::shared_ptr<GameScene> GameSceneManager::GetActiveScene(){
    return this->m_active_scene;
}

string GameSceneManager::GetActiveSceneName(){
    if(this->m_active_scene == nullptr){return "";} // something to handle missing activate scenes
    return this->m_active_scene->GetName();
}

void GameSceneManager::UpdateActiveScene(){
    if(!this->HasScenes()){return;} // make sure we have a scene before we try to paint
    this->m_active_scene->Update();

}

void GameSceneManager::DrawActiveScene(){
    if(!this->HasScenes()){return;} // make sure we have something before Drawing it
    this->m_active_scene->Draw();
}


void GameIntroScene::Update(){

}

void GameIntroScene::Draw(){
    this->m_banner->draw(500,350);
}

void GameOverScene::Update(){

}

void GameOverScene::Draw(){
    ofBackgroundGradient(ofColor::red, ofColor::black);
    this->m_banner->draw(500,350);

}