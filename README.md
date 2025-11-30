# ECOSYSTEME
homework of GAP
# ÉCOSYSTÈME — Simulateur 2D d'écosystème

Simulation 2D simple d'un écosystème composé de plantes, herbivores et carnivores. Les entités bougent, mangent, se reproduisent, vieillissent et meurent. Rendu via SDL3.

---

## Fonctionnalités
- 3 types d'entités : PLANT, HERBIVORE, CARNIVORE
- Comportements basiques : recherche de nourriture, fuite aux prédateurs, rester dans les limites
- Vieillissement, consommation d'énergie, reproduction
- Rendu visuel et barre d'énergie pour chaque entité
- Configuration facile des constantes de simulation

---

## Arborescence (extrait)
- include/Core/Entity.h        — définition de Entity, méthodes de steering
- src/Core/Entity.cpp         — logique d'entité (move, eat, reproduce, render)
- src/Core/GameEngine.*       — moteur : initialisation SDL, boucle principale, gestion d'entités
- include/Structs.h           — Vector2D, Color, Food, types
- src/main.cpp                — point d'entrée (initialisation et Run)
- CMakeLists.txt              — configuration de build (si présent)

---

## Prérequis (Windows)
- C++17/C++20 toolchain (Visual Studio 2019/2022 ou mingw)
- CMake
- SDL3 (bibliothèques + headers)
  - Recommandé : installer via vcpkg ou msys2
- (Optionnel) vcpkg pour faciliter l'installation des dépendances

Exemple vcpkg:
- git clone https://github.com/microsoft/vcpkg.git
- cd vcpkg && .\bootstrap-vcpkg.bat
- .\vcpkg.exe install sdl3 sdl3-image

---

## Build (exemples)

Avec CMake + Visual Studio (via vcpkg):
```bash
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release
```

Avec CMake + Ninja (MinGW/MSYS2, SDL3 disponible):
```bash
mkdir build && cd build
cmake .. -G Ninja
ninja
```

Exécuter :
- build\Release\Ecosystem.exe (Visual Studio)
- ./Ecosystem.exe (Ninja / Make)

---

## Contrôles (par défaut)
- Espace : pause / reprise
- R : reset simulation
- F : ajouter nourriture
- Flèches : ajuster la vitesse de simulation
- Échap : quitter

(Contrôles affichés aussi dans la sortie console au démarrage)

---

## Architecture & composants clés

- Entity
  - Champs : position, mVelocity, mEnergy, mAge, size, color, EntityType
  - Méthodes publiques importantes :
    - Update(float dt) : pipeline per-frame (ConsumeEnergy, Age, Move, CheckVitality)
    - Move(float dt) : applique mVelocity et logiques de steering
    - SeekFood(const std::vector<Food>&), AvoidPredators(const std::vector<Entity>&), StayInBounds(float w, float h)
    - ApplyForce(Vector2D)
    - Reproduce(), Eat(float)
  - Notes : certaines méthodes de steering doivent être combinées et appliquées via ApplyForce ; implémentations existantes peuvent être incomplètes.

- Vector2D / Structs.h
  - Types utilitaires : opérations vectorielles (addition, normalisation, longueur). Vérifier la présence de Normalize(), Length() pour éviter duplication.

- GameEngine
  - Initialisation SDL, boucle principale, gestion des entités, rendu, input.
  - Méthodes attendues : Initialize(), Run(), Shutdown(), gestion des événements/inputs.

---

## Ce qui ne marche pas / problèmes connus
- Fonctions incomplètes ou ellipsées dans src/Core/Entity.cpp :
  - SeekFood : implémentation incomplète dans la version actuelle (variables non initialisées dans le résumé).
  - Move / Update : contrôles early-return manquants pour entités mortes/plantes ; combinaison des forces de steering non appliquée proprement.
- main.cpp contient 3 lignes invalides :
  - Ecosystem::Core::GameEngine::HandleInput;
  - Ecosystem::Core::Ecosystem::SpawnFood;
  - Ecosystem::Core::Ecosystem::RemoveDeadEntities;
  Ces expressions ne sont pas des appels valides — doivent être supprimées ou remplacées par des appels via une instance (ex. engine.HandleInput()) si les méthodes existent.
- Gestion des vitesses : pas de clamp explicite (max speed) — risque d'instabilité.
- Constantes magiques (MAX_STEERING_FORCE, SEEK_RADIUS, MARGIN...) dispersées — difficile à tuner.
- Performances : recherche naïve O(n) pour voisins/food — OK pour petit nombre d'entités, mais prévoir grille ou quadtree pour grande échelle.
- Tests unitaires absents (Vector2D, comportements).

---

## Recommandations & tâches à prioriser
1. Corriger main.cpp (supprimer lignes invalides) — déjà identifié.
2. Finaliser SeekFood : calcul distance, target le plus proche, renvoyer steering normalisé * max_force.
3. Implémenter ApplyForce si absent et clamp de vitesse (MAX_SPEED).
4. Refaire Move/Update :
   - Early-return si !IsAlive
   - Pour plantes : ne pas bouger
   - Combiner forces : steer = w_seek*Seek + w_avoid*Avoid + w_bounds*Bounds
   - ApplyForce(steer) puis intégrer position += mVelocity * dt * speedFactor
5. Centraliser constantes dans Config.h pour tuning rapide.
6. Ajouter utilitaires Vector2D (Length, Normalize, Clamp) et tests unitaires.
7. Améliorations optionnelles : partition spatiale, UI overlay, paramètres runtime, sauvegarde / load d'état.

---

## Suggestions d'améliorations (fonctionnelles)
- Ajout de comportements supplémentaires : regroupement (flocking), apprentissage simple, différenciation individuelle.
- Visualisation : tracer vecteurs de steering, afficher id/age/energy pour debug.
- Paramétrage via fichier JSON / GUI pour changer fréquemment les constantes.
- Ajout d'un mode "profiling" pour mesurer temps CPU par étape.

---

## Contribution
- Fork → feature branch → Pull Request
- Ouvrir issue pour bug/feature avant implémenter si majeur
- Tests unitaires ajoutés en parallèle des fixes

---

## Dépannage rapide
- Erreur de link SDL3 : vérifier que CMake trouve SDL3 (chemin vcpkg/toolchain ou variables SDL3_DIR)
- Compile errors liées à méthodes incomplètes : rechercher "…" ou blocs non terminés dans Entity.cpp et main.cpp
- Comportement étrange (vitesse/oscillations) : ajouter clamp MAX_SPEED et diminuer MAX_STEERING_FORCE

Laquelle de ces actions souhaites-tu que j'applique maintenant ?
