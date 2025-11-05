#include "Aquarium.h"
#include "Core.h"
#include <cstdlib>

// --- REBOTE SIMPLE SIN GHOST (solo con X/Y y collisionRadius) ---
static inline void resolveBounce(std::shared_ptr<Creature> A,
                                 std::shared_ptr<Creature> B)
{
    float Ax = A->getX(), Ay = A->getY();
    float Bx = B->getX(), By = B->getY();
    float rA = A->getCollisionRadius();
    float rB = B->getCollisionRadius();
    float rS = rA + rB;

    float dx = Bx - Ax;
    float dy = By - Ay;
    float d2 = dx * dx + dy * dy;
    if (d2 >= rS * rS) return;

    float d = std::sqrt(std::max(d2, 1e-6f));
    float nx = dx / d;
    float ny = dy / d;

    float pen = rS - d;
    A->moveBy(-nx * 0.5f * pen, -ny * 0.5f * pen);
    B->moveBy( nx * 0.5f * pen,  ny * 0.5f * pen);

    if (auto pa = std::dynamic_pointer_cast<PlayerCreature>(A)) pa->setDirection(-nx, -ny);
    if (auto na = std::dynamic_pointer_cast<NPCreature>(A))     na->setDirection(-nx, -ny);
    if (auto pb = std::dynamic_pointer_cast<PlayerCreature>(B)) pb->setDirection(nx, ny);
    if (auto nb = std::dynamic_pointer_cast<NPCreature>(B))     nb->setDirection(nx, ny);
}






string AquariumCreatureTypeToString(AquariumCreatureType t){
    switch(t){
        case AquariumCreatureType::BiggerFish:
            return "BiggerFish";
        case AquariumCreatureType::NPCreature:
            return "BaseFish";
        case AquariumCreatureType::JellyFish:
            return "JellyFish";
        case AquariumCreatureType::PufferFish:
            return "PufferFish";
        case AquariumCreatureType::PowerUp: 
            return "PowerUp";
        default:
            return "UknownFish";
    }
}

// PlayerCreature Implementation
PlayerCreature::PlayerCreature(float x, float y, int speed, std::shared_ptr<GameSprite> sprite)
: Creature(x, y, speed, 10.0f, 1, sprite) {}


void PlayerCreature::setDirection(float dx, float dy) {
    m_dx = dx;
    m_dy = dy;
    normalize();
}

void PlayerCreature::move() {
    int effectiveSpeed = m_speed;
    if (hasSpeedBoost()) effectiveSpeed = m_speed * 2; // simple speed boost
    m_x += m_dx * effectiveSpeed;
    m_y += m_dy * effectiveSpeed;
    this->bounce();
}

void PlayerCreature::reduceDamageDebounce() {
    if (m_damage_debounce > 0) {
        --m_damage_debounce;
    }
    if (m_speedBoostTimer > 0) {
        --m_speedBoostTimer;
        if (m_speedBoostTimer == 0) {
            ofLogNotice() << "Speed boost expired" << std::endl;
        }
    }
}

void PlayerCreature::update() {
    this->reduceDamageDebounce();
    this->move();
}


void PlayerCreature::draw() const {
    
    ofLogVerbose() << "PlayerCreature at (" << m_x << ", " << m_y << ") with speed " << m_speed << std::endl;
    if (this->m_damage_debounce > 0) {
        ofSetColor(ofColor::red); // Flash red if in damage debounce
    }
    if (m_sprite) {
        m_sprite->draw(m_x, m_y);
    }
    ofSetColor(ofColor::white); // Reset color

}

void PlayerCreature::changeSpeed(int speed) {
    m_speed = speed;
}

void PlayerCreature::loseLife(int debounce) {
    if (m_damage_debounce <= 0) {
        if (m_lives > 0) this->m_lives -= 1;
        m_damage_debounce = debounce; // Set debounce frames
        ofLogNotice() << "Player lost a life! Lives remaining: " << m_lives << std::endl;
    }
    // If in debounce period, do nothing
    if (m_damage_debounce > 0) {
        ofLogVerbose() << "Player is in damage debounce period. Frames left: " << m_damage_debounce << std::endl;
    }
}

// NPCreature Implementation
NPCreature::NPCreature(float x, float y, int speed, std::shared_ptr<GameSprite> sprite)
: Creature(x, y, speed, 22.f, 1, sprite) {
    if (speed < 3) speed = 3;
    if (speed > 6) speed = 6;
    m_speed = speed;

    m_dx = (rand() % 3 - 1); // -1, 0 o 1
    m_dy = (rand() % 3 - 1);
    if (m_dx == 0 && m_dy == 0) { m_dx = 1; m_dy = 0; } // evita quedar quieto
    normalize();

    m_creatureType = AquariumCreatureType::NPCreature;
}


void NPCreature::move() {
      m_x += m_dx * m_speed;
      m_y += m_dy * m_speed;

    // Opcional: voltea sprite según la dirección
    if (m_dx < 0)
        this->m_sprite->setFlipped(true);
    else
        this->m_sprite->setFlipped(false);

    bounce(); // ← usa tu nueva función de Core.cpp
}

void NPCreature::draw() const {

  

    if (m_creatureType == AquariumCreatureType::PowerUp) {
       
        float pulse = 0.8f + 0.4f * sin(ofGetElapsedTimef() * 6.0f);  
       
        ofSetColor(ofColor::yellow, 150 * pulse);
        ofDrawCircle(m_x, m_y, 45 * pulse);
        
        ofSetColor(ofColor::yellow, 255);
    } else {
        ofSetColor(ofColor::white);
    }
    
    if (m_sprite) {
        m_sprite->draw(m_x, m_y);
    }
    
    ofSetColor(ofColor::white); 
}

void NPCreature::setDirection(float dx, float dy) {
    m_dx = dx;
    m_dy = dy;
    normalize(); // usa tu método de Creature para mantener velocidad consistente
}


BiggerFish::BiggerFish(float x, float y, int speed, std::shared_ptr<GameSprite> sprite)
: NPCreature(x, y, speed, sprite) {
   if (speed < 2) speed = 2;
    if (speed > 5) speed = 5;  // más lento
    m_speed = speed;

    m_dx = (rand() % 3 - 1);
    m_dy = (rand() % 3 - 1);
    if (m_dx == 0 && m_dy == 0) { m_dx = -1; m_dy = 0; }
    normalize();

    setCollisionRadius(50); //  de 60 a 50
    m_value = 5;
    m_creatureType = AquariumCreatureType::BiggerFish;
}

void BiggerFish::move() {
    // Bigger fish might move slower or have different logic
    m_x += m_dx * (m_speed * 0.5); // Moves at half speed
    m_y += m_dy * (m_speed * 0.5);
    if(m_dx < 0 ){
        this->m_sprite->setFlipped(true);
    }else {
        this->m_sprite->setFlipped(false);
    }

    bounce();
}

void BiggerFish::draw() const {
    ofLogVerbose() << "BiggerFish at (" << m_x << ", " << m_y << ") with speed " << m_speed << std::endl;
    this->m_sprite->draw(this->m_x, this->m_y);
}

Jellyfish::Jellyfish(float x, float y, int speed, std::shared_ptr<GameSprite> sprite)
: NPCreature(x, y, speed, sprite) {
    setCollisionRadius(45.0f);
    m_value = 3;
    m_creatureType = AquariumCreatureType::JellyFish;
    m_amp = 30.0f;
    m_freq = 2.0f;
    m_phase = ofRandom(TWO_PI);
    m_drift = 0.5f;
    m_baseY = y;
}
void Jellyfish::draw() const {
    ofSetColor(255, 255, 255, 200); 
    if (m_sprite) {
        m_sprite->draw(m_x, m_y);
    }
    ofSetColor(255, 255, 255, 255); 
}
void Jellyfish::move() {
    // Current horizontal position, using m_drift for slower movement
    m_x += m_drift * (m_dx * m_speed);
    
    // Vertical bobbing motion calculated around the fixed m_baseY
    float time = ofGetElapsedTimef();
    m_y = m_baseY + m_amp * sin(m_freq * time + m_phase);
    
    // Optional: flip sprite based on horizontal direction
    if(m_dx < 0) {
        this->m_sprite->setFlipped(true);
    } else {
        this->m_sprite->setFlipped(false);
    }
    
    bounce();
}

PufferFish::PufferFish(float x, float y, int speed, std::shared_ptr<GameSprite> sprite)
: NPCreature(x, y, speed, sprite) {
    setCollisionRadius(50.0f);
    m_value = 4;
    m_creatureType = AquariumCreatureType::PufferFish;
    m_dir = 1.0f;
    m_zigAmp = 20.0f;
    m_zigFreq = 1.0f;
    m_puff = false;
    m_puffTimer = 0;
    m_puffDuration = 60;
}
void PufferFish::draw() const {
    ofLogVerbose() << "PufferFish at (" << m_x << ", " << m_y << ") with speed " << m_speed << std::endl;

    

    if (m_sprite) {
        this->m_sprite->draw(this->m_x, this->m_y);
    }
}

void PufferFish::move() {
    // Horizontal patrol
    m_x += m_dir * m_speed;
    
    float time = ofGetElapsedTimef();
    m_y = m_y + m_zigAmp * sin(m_zigFreq * time);
    
    if (m_dir < 0) { 
        this->m_sprite->setFlipped(true);
    } else {
        this->m_sprite->setFlipped(false);
    }
    
    bounce();
    
    if (m_x <= getCollisionRadius()) {
        m_dir = 1.0f;
    } else if (m_x >= m_width - getCollisionRadius()) {
        m_dir = -1.0f; 
    }
}

// AquariumSpriteManager
AquariumSpriteManager::AquariumSpriteManager(){
    this->m_npc_fish = std::make_shared<GameSprite>("base-fish.png", 70,70);
    this->m_big_fish = std::make_shared<GameSprite>("bigger-fish.png", 120, 120);
    this->m_Jellyfish = std::make_shared<GameSprite>("jellyfish.png", 90, 90);
    this->m_PufferFish = std::make_shared<GameSprite>("pufferfish.png", 100, 100);
}

std::shared_ptr<GameSprite> AquariumSpriteManager::GetSprite(AquariumCreatureType t){
    switch(t){
        case AquariumCreatureType::BiggerFish:
            return std::make_shared<GameSprite>("bigger-fish.png", 120, 120);
            
        case AquariumCreatureType::NPCreature:
            return std::make_shared<GameSprite>("base-fish.png", 70, 70);
            
        case AquariumCreatureType::PowerUp:
            return std::make_shared<GameSprite>("1-up.png", 80, 80); 

        case AquariumCreatureType::JellyFish:
            return std::make_shared<GameSprite>("jellyfish.png", 90, 90);
            
        case AquariumCreatureType::PufferFish:
            return std::make_shared<GameSprite>("pufferfish.png", 100, 100);   
            
        default:
            return nullptr;
    }
}


// Aquarium Implementation
Aquarium::Aquarium(int width, int height, std::shared_ptr<AquariumSpriteManager> spriteManager)
    : m_width(width), m_height(height) {
        m_sprite_manager =  spriteManager;
    }



void Aquarium::addCreature(std::shared_ptr<Creature> creature) {
    creature->setBounds(m_width - 20, m_height - 20);
    m_creatures.push_back(creature);
}

void Aquarium::addAquariumLevel(std::shared_ptr<AquariumLevel> level){
    if(level == nullptr){return;} // guard to not add noise
    this->m_aquariumlevels.push_back(level);
}

void Aquarium::update() {
    // 1) Mover todas las criaturas (cada una hace su propio bounce)
    for (auto& creature : m_creatures) {
        creature->move();

        // 2) Rebote contra los límites del acuario
        float x = creature->getX();
        float y = creature->getY();
        float r = creature->getCollisionRadius();

        if (x - r < 0) {
            x = r;
            creature->moveBy(1, 0);
        }
        if (x + r > m_width) {
            x = m_width - r;
            creature->moveBy(-1, 0);
        }
        if (y - r < 0) {
            y = r;
            creature->moveBy(0, 1);
        }
        if (y + r > m_height) {
            y = m_height - r;
            creature->moveBy(0, -1);
        }
    }

    // 3) Rebote entre NPCs (sin incluir el jugador)
    for (size_t i = 0; i < m_creatures.size(); ++i) {
        for (size_t j = i + 1; j < m_creatures.size(); ++j) {
            auto A = m_creatures[i];
            auto B = m_creatures[j];

            // Solo NPC vs NPC
            bool AisNPC = (std::dynamic_pointer_cast<NPCreature>(A) != nullptr);
            bool BisNPC = (std::dynamic_pointer_cast<NPCreature>(B) != nullptr);
            if (!(AisNPC && BisNPC)) continue;

            // Detectar colisión
            float dx = B->getX() - A->getX();
            float dy = B->getY() - A->getY();
            float r = A->getCollisionRadius() + B->getCollisionRadius();
            if (dx * dx + dy * dy <= r * r) {
                resolveBounce(A, B);
            }
        }
    }

    // 4) Spawn de PowerUp según intervalo configurado
    if (++m_powerUpTimer >= m_powerUpInterval) {
        SpawnCreature(AquariumCreatureType::PowerUp);
        m_powerUpTimer = 0;
        ofLogNotice() << "Power-up spawned!" << std::endl;

    }


    // 5) Revisar niveles y repoblar si hace falta
    this->Repopulate();
}


void Aquarium::draw() const {
    for (const auto& creature : m_creatures) {
        creature->draw();
    }
}


void Aquarium::removeCreature(std::shared_ptr<Creature> creature) {
    auto it = std::find(m_creatures.begin(), m_creatures.end(), creature);
    if (it != m_creatures.end()) {
        ofLogVerbose() << "removing creature " << endl;
        int selectLvl = this->currentLevel % this->m_aquariumlevels.size();
        auto npcCreature = std::static_pointer_cast<NPCreature>(creature);
        this->m_aquariumlevels.at(selectLvl)->ConsumePopulation(npcCreature->GetType(), npcCreature->getValue());
        m_creatures.erase(it);
    }
}

void Aquarium::clearCreatures() {
    m_creatures.clear();
}

std::shared_ptr<Creature> Aquarium::getCreatureAt(int index) {
    if (index < 0 || size_t(index) >= m_creatures.size()) {
        return nullptr;
    }
    return m_creatures[index];
}



void Aquarium::SpawnCreature(AquariumCreatureType type) {
    int x = rand() % this->getWidth();
    int y = rand() % this->getHeight();

    // Velocidad lenta :)
    int speed = 1 + rand() % 4;  // 1..4

    switch (type) {
        case AquariumCreatureType::NPCreature:
            this->addCreature(std::make_shared<NPCreature>(
                x, y, speed, this->m_sprite_manager->GetSprite(AquariumCreatureType::NPCreature)
            ));
            break;
            
        case AquariumCreatureType::BiggerFish:
            this->addCreature(std::make_shared<BiggerFish>(
                x, y, speed, this->m_sprite_manager->GetSprite(AquariumCreatureType::BiggerFish)
            ));
            break;

        case AquariumCreatureType::JellyFish:
            this->addCreature(std::make_shared<Jellyfish>(
                (float)x, (float)y, speed, this->m_sprite_manager->GetSprite(AquariumCreatureType::JellyFish) 
            ));
            break;

            case AquariumCreatureType::PufferFish:
            this->addCreature(std::make_shared<PufferFish>(
                    (float)x, (float)y, speed, this->m_sprite_manager->GetSprite(AquariumCreatureType::PufferFish) 
            ));
            break; 
    
        case AquariumCreatureType::PowerUp: {
            auto powerUp = std::make_shared<NPCreature>(
                x, y, 1, m_sprite_manager->GetSprite(AquariumCreatureType::PowerUp)
            );
            powerUp->SetType(AquariumCreatureType::PowerUp);
            addCreature(powerUp);
            ofLogNotice() << "Power-up spawned at (" << x << ", " << y << ")";
            break;
        }

        default:
            ofLogError() << "Unknown creature type to spawn!";
            break;
    }
}



// repopulation will be called from the levl class
// it will compose into aquarium so eating eats frm the pool of NPCs in the lvl class
// once lvl criteria met, we move to new lvl through inner signal asking for new lvl
// which will mean incrementing the buffer and pointing to a new lvl index
void Aquarium::Repopulate() {
    ofLogVerbose("entering phase repopulation");
    // lets make the levels circular
    int selectedLevelIdx = this->currentLevel % this->m_aquariumlevels.size();
    ofLogVerbose() << "the current index: " << selectedLevelIdx << endl;
    std::shared_ptr<AquariumLevel> level = this->m_aquariumlevels.at(selectedLevelIdx);


    if(level->isCompleted()){
        level->levelReset();
        this->currentLevel += 1;
        selectedLevelIdx = this->currentLevel % this->m_aquariumlevels.size();
        ofLogNotice()<<"new level reached : " << selectedLevelIdx << std::endl;
        level = this->m_aquariumlevels.at(selectedLevelIdx);
        this->clearCreatures();
    }

    
    // now lets find how many to respawn if needed 
    std::vector<AquariumCreatureType> toRespawn = level->Repopulate();
    ofLogVerbose() << "amount to repopulate : " << toRespawn.size() << endl;
    if(toRespawn.size() <= 0 ){return;} // there is nothing for me to do here
    for(AquariumCreatureType newCreatureType : toRespawn){
        this->SpawnCreature(newCreatureType);
    }
}


// Aquarium collision detection
std::shared_ptr<GameEvent>
DetectAquariumCollisions(std::shared_ptr<Aquarium> aquarium,
                         std::shared_ptr<PlayerCreature> player) {
    if (!aquarium || !player) return nullptr;

    for (int i = 0; i < aquarium->getCreatureCount(); ++i) {
        auto npc = aquarium->getCreatureAt(i);
        if (!npc || npc.get() == player.get()) continue;

        float rP = player->getCollisionRadius();
        float rN = npc->getCollisionRadius();
        float dx = player->getX() - npc->getX();
        float dy = player->getY() - npc->getY();
        if (dx*dx + dy*dy <= (rP + rN)*(rP + rN)) {
            return std::make_shared<GameEvent>(GameEventType::COLLISION, player, npc);
        }
    }
    return nullptr;
}

  

//  Imlementation of the AquariumScene

void AquariumGameScene::Update(){
if (!updateControl.tick()) {
    return;
}
// no hay reset(); tick() maneja el conteo


    this->m_player->update();
    this->m_aquarium->update();
    
    // detectar colisiones
    auto event = DetectAquariumCollisions(m_aquarium, m_player);
    if (event && event->isCollisionEvent() && event->creatureA && event->creatureB) {
        auto A = event->creatureA; // player
        auto B = event->creatureB; // npc

        // First: check if B is a power-up
        if (auto npcB = std::dynamic_pointer_cast<NPCreature>(B)) {
            if (npcB->GetType() == AquariumCreatureType::PowerUp) {
                ofLogNotice() << "Player collected a power-up! Applying speed boost." << std::endl;

                if (m_powerUpsound.isLoaded()) {
                    m_powerUpsound.play();
                } else {
                    ofLogError("Sound") << "Power-up sound not loaded!" << std::endl;
                }

                m_player->applySpeedBoost();
                m_aquarium->removeCreature(B);
                return;
            }
        }

        if (m_player->getPower() < B->getValue()) {
            // Rebote real
            resolveBounce(A, B);
            m_player->loseLife(3 * 60);
            if (m_player->getLives() <= 0) {
                m_lastEvent = std::make_shared<GameEvent>(GameEventType::GAME_OVER, m_player, nullptr);
                return;
            }
        } else {
            // Come
            m_aquarium->removeCreature(B);
            m_player->addToScore(1, B->getValue());

            if (biteSound.isLoaded()) {
                biteSound.play();
            } else {
                ofLogError("Sound") << "Bite sound not loaded!" << std::endl;
            }
            
            const int kPowerEvery = 15;
            if (m_player->getScore() > 0 && m_player->getScore() % kPowerEvery == 0) {
                m_player->increasePower(1);
                ofLogNotice() << "Player power increased to " << m_player->getPower() << "!";
            }
        }
}
}




void AquariumGameScene::Draw() {
    this->m_player->draw();
    this->m_aquarium->draw();
    this->paintAquariumHUD();

}


void AquariumGameScene::paintAquariumHUD(){
    float panelWidth = ofGetWindowWidth() - 150;
    ofDrawBitmapString("Score: " + std::to_string(this->m_player->getScore()), panelWidth, 20);
    ofDrawBitmapString("Power: " + std::to_string(this->m_player->getPower()), panelWidth, 30);
    ofDrawBitmapString("Lives: " + std::to_string(this->m_player->getLives()), panelWidth, 40);
    
    // Show speed boost status
    if (this->m_player->hasSpeedBoost()) {
        ofSetColor(ofColor::yellow);
        ofDrawBitmapString("SPEED BOOST ACTIVE!", panelWidth, 50);
        ofSetColor(ofColor::white);
    }
    
    for (int i = 0; i < this->m_player->getLives(); ++i) {
        ofSetColor(ofColor::red);
        ofDrawCircle(panelWidth + i * 20, 65, 5);
    }
    ofSetColor(ofColor::white); // Reset color to white for other drawings
}

void AquariumLevel::populationReset(){
    for(auto node: this->m_levelPopulation){
        node->currentPopulation = 0; // need to reset the population to ensure they are made a new in the next level
    }
}

void AquariumLevel::ConsumePopulation(AquariumCreatureType creatureType, int power){
    for(std::shared_ptr<AquariumLevelPopulationNode> node: this->m_levelPopulation){
        ofLogVerbose() << "consuming from this level creatures" << endl;
        if(node->creatureType == creatureType){
            ofLogVerbose() << "-cosuming from type: " << AquariumCreatureTypeToString(node->creatureType) <<" , currPop: " << node->currentPopulation << endl;
            if(node->currentPopulation == 0){
                return;
            } 
            node->currentPopulation -= 1;
            ofLogVerbose() << "+cosuming from type: " << AquariumCreatureTypeToString(node->creatureType) <<" , currPop: " << node->currentPopulation << endl;
            this->m_level_score += power;
            return;
        }
    }
}

bool AquariumLevel::isCompleted(){
    return this->m_level_score >= this->m_targetScore;
}




std::vector<AquariumCreatureType> Level_0::Repopulate() {
    std::vector<AquariumCreatureType> toRepopulate;
    for(std::shared_ptr<AquariumLevelPopulationNode> node : this->m_levelPopulation){
        int delta = node->population - node->currentPopulation;
        ofLogVerbose() << "to Repopulate :  " << delta << endl;
        if(delta >0){
            for(int i = 0; i<delta; i++){
                toRepopulate.push_back(node->creatureType);
            }
            node->currentPopulation += delta;
        }
    }
    return toRepopulate;

}


std::vector<AquariumCreatureType> Level_1::Repopulate() {
    std::vector<AquariumCreatureType> toRepopulate;
    for(std::shared_ptr<AquariumLevelPopulationNode> node : this->m_levelPopulation){
        int delta = node->population - node->currentPopulation;
        if(delta >0){
            for(int i=0; i<delta; i++){
                toRepopulate.push_back(node->creatureType);
            }
            node->currentPopulation += delta;
        }
    }
    return toRepopulate;
}

std::vector<AquariumCreatureType> Level_2::Repopulate() {
    std::vector<AquariumCreatureType> toRepopulate;
    for(std::shared_ptr<AquariumLevelPopulationNode> node : this->m_levelPopulation){
        int delta = node->population - node->currentPopulation;
        if(delta >0){
            for(int i=0; i<delta; i++){
                toRepopulate.push_back(node->creatureType);
            }
            node->currentPopulation += delta;
        }
    }
    return toRepopulate;
}
