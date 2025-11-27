#include "Core/Entity.h"
#include <cmath>
#include <iostream>
#include <algorithm>

namespace Ecosystem {
namespace Core {
const float MAX_STEERING_FORCE = 0.5f; // Force maximale de direction
const float SEEK_RADIUS = 150.0f;      // Rayon dans lequel l'entité cherche sa nourriture

// 🏗 CONSTRUCTEUR PRINCIPAL
Entity::Entity(EntityType type, Vector2D pos, std::string entityName)
    : mType(type), position(pos), name(entityName), 
      mRandomGenerator(std::random_device{}())  // Initialisation du générateur aléatoire
{
    // 🔧 INITIALISATION SELON LE TYPE
    switch(mType) {
        case EntityType::HERBIVORE:
            mEnergy = 80.0f;
            mMaxEnergy = 150.0f;
            mMaxAge = 200;
            color = Color::Blue();
            size = 8.0f;
            break;
            
        case EntityType::CARNIVORE:
            mEnergy = 100.0f;
            mMaxEnergy = 200.0f;
            mMaxAge = 150;
            color = Color::Red();
            size = 12.0f;
            break;
            
        case EntityType::PLANT:
            mEnergy = 50.0f;
            mMaxEnergy = 100.0f;
            mMaxAge = 300;
            color = Color::Green();
            size = 6.0f;
            break;
    }
    
    mAge = 0;
    mIsAlive = true;
    mVelocity = GenerateRandomDirection();
    
    std::cout << "🌱 Entité créée: " << name << " à (" << position.x << ", " << position.y << ")" << std::endl;
}

// 🏗 CONSTRUCTEUR DE COPIE
Entity::Entity(const Entity& other)
    : mType(other.mType), position(other.position), name(other.name + "_copy"),
      mEnergy(other.mEnergy * 0.7f),  // Enfant a moins d'énergie
      mMaxEnergy(other.mMaxEnergy),
      mAge(0),  // Nouvelle entité, âge remis à 0
      mMaxAge(other.mMaxAge),
      mIsAlive(true),
      mVelocity(other.mVelocity),
      color(other.color),
      size(other.size * 0.8f),  // Enfant plus petit
      mRandomGenerator(std::random_device{}())
{
    std::cout << "👶 Copie d'entité créée: " << name << std::endl;
}

// 🗑 DESTRUCTEUR
Entity::~Entity() {
    std::cout << "💀 Entité détruite: " << name << " (Âge: " << mAge << ")" << std::endl;
}

// ⚙️ MISE À JOUR PRINCIPALE
void Entity::Update(float deltaTime) {
    if (!mIsAlive) return;
    
    // 🔄 PROCESSUS DE VIE
    ConsumeEnergy(deltaTime);
    Age(deltaTime);
    Move(deltaTime);
    CheckVitality();
}

// 🚶 MOUVEMENT
void Entity::Move(float deltaTime) {
    if (mType == EntityType::PLANT) return;  // Les plantes ne bougent pas
    
    // 🎲 Comportement aléatoire occasionnel
    std::uniform_real_distribution<float> chance(0.0f, 1.0f);
    if (chance(mRandomGenerator) < 0.02f) {
        mVelocity = GenerateRandomDirection();
    }
    
    // 📐 Application du mouvement
    position = position + mVelocity * deltaTime * 20.0f;
    
    // 🔄 Consommation d'énergie due au mouvement
    mEnergy -= mVelocity.Distance(Vector2D(0, 0)) * deltaTime * 0.1f;
}

// 🍽 MANGER
void Entity::Eat(float energy) {
    mEnergy += energy;
    if (mEnergy > mMaxEnergy) {
        mEnergy = mMaxEnergy;
    }
    std::cout << "🍽 " << name << " mange et gagne " << energy << " énergie" << std::endl;
}

// 🔄 CONSOMMATION D'ÉNERGIE
void Entity::ConsumeEnergy(float deltaTime) {
    float baseConsumption = 0.0f;
    
    switch(mType) {
        case EntityType::HERBIVORE:
            baseConsumption = 1.5f;
            break;
        case EntityType::CARNIVORE:
            baseConsumption = 2.0f;
            break;
        case EntityType::PLANT:
            baseConsumption = -0.5f;  // Les plantes génèrent de l'énergie !
            break;
    }
    
    mEnergy -= baseConsumption * deltaTime;
}

// 🎂 VIEILLISSEMENT
void Entity::Age(float deltaTime) {
    mAge += static_cast<int>(deltaTime * 10.0f);  // Accéléré pour la simulation
}

// ❤️ VÉRIFICATION DE LA SANTÉ
void Entity::CheckVitality() {
    if (mEnergy <= 0.0f || mAge >= mMaxAge) {
        mIsAlive = false;
        std::cout << "💀 " << name << " meurt - ";
        if (mEnergy <= 0) std::cout << "Faim";
        else std::cout << "Vieillesse";
        std::cout << std::endl;
    }
}

// 👶 REPRODUCTION
bool Entity::CanReproduce() const {
    return mIsAlive && mEnergy > mMaxEnergy * 0.8f && mAge > 20;
}

std::unique_ptr<Entity> Entity::Reproduce() {
    if (!CanReproduce()) return nullptr;
    
    // 🎲 Chance de reproduction
    std::uniform_real_distribution<float> chance(0.0f, 1.0f);
    if (chance(mRandomGenerator) < 0.3f) {
        mEnergy *= 0.6f;  // Coût énergétique de la reproduction
        return std::make_unique<Entity>(*this);  // Utilise le constructeur de copie
    }
    
    return nullptr;
}

// 🎲 GÉNÉRATION DE DIRECTION ALÉATOIRE
Vector2D Entity::GenerateRandomDirection() {
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
    return Vector2D(dist(mRandomGenerator), dist(mRandomGenerator));
}

// 🎨 CALCUL DE LA COULEUR BASÉE SUR L'ÉTAT
Color Entity::CalculateColorBasedOnState() const {
    float energyRatio = GetEnergyPercentage();
    
    Color baseColor = color;
    
    // 🔴 Rouge si faible énergie
    if (energyRatio < 0.3f) {
        baseColor.r = 255;
        baseColor.g = static_cast<uint8_t>(baseColor.g * energyRatio);
        baseColor.b = static_cast<uint8_t>(baseColor.b * energyRatio);
    }
    
    return baseColor;
}

Vector2D Entity::SeekFood(const std::vector<Food>& foodSources) const {
    if (foodSources.empty() || mType == EntityType::CARNIVORE) {
        // Seuls les herbivores cherchent cette nourriture, et si la liste est vide.
        return Vector2D(0, 0);
    }

    // On utilise la distance au carré pour les comparaisons afin d'éviter la fonction std::sqrt()
    float closestDistSq = SEEK_RADIUS * SEEK_RADIUS; 
    Vector2D targetPosition = Vector2D(0, 0);
    bool foundTarget = false;

    // 1. Trouver la source de nourriture la plus proche dans le rayon de recherche
    for (const auto& food : foodSources) {
        // Vecteur de l'entité à la nourriture (position cible - position actuelle)
        Vector2D toFood;
        float distSq; // Assurez-vous que Vector2D a la méthode LengthSq()

        if (distSq < closestDistSq) {
            closestDistSq = distSq;
            targetPosition = food.position;
            foundTarget = true;
        }
    }

    if (!foundTarget) {
        return Vector2D(0, 0); // Aucune nourriture trouvée à portée
    }

    // 2. Calculer la force de "Seek" (Recherche)
    
    // Calculer le vecteur pointant vers la cible
    Vector2D desiredDirection;

    // Le vecteur désiré est la direction normalisée (longueur 1) multipliée par la force maximale
    Vector2D desiredVelocity; 

    // Retourne la force de direction à appliquer dans Entity::Move/ApplyForce
    return desiredVelocity;
}
// ...existing code...
Vector2D Entity::AvoidPredators(const std::vector<Entity>& predators) const {
    // Les plantes ne fuient pas; les carnivores ne fuient pas non plus
    if (mType == EntityType::PLANT || mType == EntityType::CARNIVORE) {
        return Vector2D(0.0f, 0.0f);
    }

    const float AVOID_RADIUS = 120.0f;
    const float AVOID_RADIUS_SQ = AVOID_RADIUS * AVOID_RADIUS;

    Vector2D steer{0.0f, 0.0f};
    float totalWeight = 0.0f;

    for (const auto& pred : predators) {
        // Seuls les prédateurs vivants et de type CARNIVORE sont considérés
        if (!pred.mIsAlive || pred.mType != EntityType::CARNIVORE) continue;

        float dx = position.x - pred.position.x;
        float dy = position.y - pred.position.y;
        float distSq = dx*dx + dy*dy;

        if (distSq > 0.0f && distSq < AVOID_RADIUS_SQ) {
            // Plus le prédateur est proche, plus la force de fuite est grande.
            float weight = 1.0f / std::sqrt(distSq); // inverse de la distance
            steer.x += dx * weight;
            steer.y += dy * weight;
            totalWeight += weight;
        }
    }

    if (totalWeight <= 0.0f) {
        return Vector2D(0.0f, 0.0f); // Aucun prédateur menaçant à portée
    }

    // Moyenne pondérée
    steer.x /= totalWeight;
    steer.y /= totalWeight;

    // Normaliser et appliquer force maximale de direction
    float len = std::sqrt(steer.x*steer.x + steer.y*steer.y);
    if (len > 1e-6f) {
        steer.x = (steer.x / len) * MAX_STEERING_FORCE;
        steer.y = (steer.y / len) * MAX_STEERING_FORCE;
    } else {
        steer = Vector2D(0.0f, 0.0f);
    }

    // Retourner la force de fuite (steering) à appliquer par Move/ApplyForce
    return steer;
}
// ...existing code...
// ...existing code...
Vector2D Entity::StayInBounds(float worldWidth, float worldHeight) const {
    // Marge à partir des bords où l'entité commence à corriger sa trajectoire
    const float MARGIN = 40.0f;
    Vector2D steer{0.0f, 0.0f};

    // Calculer une poussée pour ramener l'entité dans la zone sûre
    if (position.x < MARGIN) {
        steer.x = (MARGIN - position.x); // pousse vers la droite
    } else if (position.x > worldWidth - MARGIN) {
        steer.x = (worldWidth - MARGIN - position.x); // pousse vers la gauche (négatif)
    }

    if (position.y < MARGIN) {
        steer.y = (MARGIN - position.y); // pousse vers le bas
    } else if (position.y > worldHeight - MARGIN) {
        steer.y = (worldHeight - MARGIN - position.y); // pousse vers le haut (négatif)
    }

    // Si aucune correction n'est nécessaire
    if (std::abs(steer.x) < 1e-6f && std::abs(steer.y) < 1e-6f) {
        return Vector2D(0.0f, 0.0f);
    }

    // Normaliser et appliquer la force maximale de direction
    float len = std::sqrt(steer.x * steer.x + steer.y * steer.y);
    if (len > 1e-6f) {
        steer.x = (steer.x / len) * MAX_STEERING_FORCE;
        steer.y = (steer.y / len) * MAX_STEERING_FORCE;
    }

    return steer;
}
// ...existing code...


// 🎨 RENDU GRAPHIQUE
void Entity::Render(SDL_Renderer* renderer) const {
    if (!mIsAlive) return;
    
    Color renderColor = CalculateColorBasedOnState();
    
    SDL_FRect rect = {
        position.x - size / 2.0f,
        position.y - size / 2.0f,
        size,
        size
    };
    
    SDL_SetRenderDrawColor(renderer, renderColor.r, renderColor.g, renderColor.b, renderColor.a);
    SDL_RenderFillRect(renderer, &rect);
    
    // 🔵 Indicateur d'énergie (barre de vie)
    if (mType != EntityType::PLANT) {
        float energyBarWidth = size * GetEnergyPercentage();
        SDL_FRect energyBar = {
            position.x - size / 2.0f,
            position.y - size / 2.0f - 3.0f,
            energyBarWidth,
            2.0f
        };
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        SDL_RenderFillRect(renderer, &energyBar);
    }
}

} // namespace Core
} // namespace Ecosystem