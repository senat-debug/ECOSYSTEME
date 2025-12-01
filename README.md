YOUBISSI NGATCHEUSSI DAVID 25P921
# ÉCOSYSTÈME — Simulateur 2D d'écosystème

Simulation 2D simple et pédagogique d'un écosystème composé de plantes, herbivores et carnivores. Chaque entité possède énergie, âge, comportements (recherche de nourriture, fuite, reproduction) et est rendue avec SDL3.

---

## Aperçu rapide
- Langage : C++ (C++17 recommandé)  
- Rendu : SDL3  
- Structure : moteur (GameEngine), monde (Ecosystem), entités (Entity), utilitaires (Structs/Vector2D, Color, Food)

Capture d'intention : observer l'émergence de comportements (naissances, morts, interaction prédateur/proie) et pouvoir ajuster les paramètres de simulation.

---

## Fonctionnalités principales
- 3 types d'entités : PLANT, HERBIVORE, CARNIVORE  
- Cycle de vie : énergie, consommation, vieillissement, mort  
- Reproduction simple (constructeur de copie)  
- Comportements de steering : SeekFood, AvoidPredators, StayInBounds  
- Rendu visuel des entités + barre d'énergie  
- Gestion simple de la nourriture (SpawnFood) et statistiques

---

## Arborescence clé
- include/Core/Entity.h       — définition des entités et comportements  
- src/Core/Entity.cpp         — logique d'entité (mouvement, vie, rendu)  
- include/Core/Ecosystem.h    — gestionnaire du monde (entités, nourriture)  
- src/Core/Ecosystem.cpp      — implémentation du monde  
- include/Core/GameEngine.h   — boucle principale, events, rendu  
- src/Core/GameEngine.cpp     — implémentation du moteur  
- include/Structs.h           — Vector2D, Color, Food  
- src/main.cpp                — point d'entrée (initialisation + Run)  
- assets/                     — futurs assets (textures, sons)

---

## Prérequis (Windows)
- Visual Studio 2019/2022 ou g++/clang++ compatible C++17  
- CMake (optionnel)  
- SDL3 (headers + libs) — installer via vcpkg, msys2 ou manuel

Exemple avec vcpkg :
1. bootstrap-vcpkg.bat  
2. vcpkg install sdl3 sdl3-image

---

## Compilation & exécution (exemples)
Avec CMake + vcpkg toolchain (PowerShell / cmd) :
```bash
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build . --config Release
# Exécuter
.\Release\Ecosystem.exe
```

Ou g++ (ligne de commande, SDL3 doit être trouvable) :
```bash
g++ -std=c++17 -Iinclude -o Ecosystem src/*.cpp src/Core/*.cpp src/Graphics/*.cpp -lSDL3
./Ecosystem.exe
```

---

## Commandes & contrôles
- Espace : pause / reprise  
- R : réinitialiser la simulation  
- F : ajouter de la nourriture  
- Flèches haut/bas : accélérer / ralentir la simulation  
- Échap : quitter
---

## Contribution
1. Fork → nouvelle branche feature/bugfix → pull request.  
2. Ouvrir issue décrivant bug/feature si majeur.  
3. Follow coding style : mPrefix pour membres privés, namespaces Ecosystem::Core, headers guards / pragma once.  
4. Inclure tests ou démonstration visuelle pour
