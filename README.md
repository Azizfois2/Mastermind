# Mastermind — jeu de déduction en C / Win32

<p>
  <img alt="Langage" src="https://img.shields.io/badge/C-11-00599C?logo=c&logoColor=white">
  <img alt="Plateforme" src="https://img.shields.io/badge/Windows-Win32-0078D6?logo=windows&logoColor=white">
  <img alt="Licence" src="https://img.shields.io/badge/licence-GPL--3.0-blue">
</p>

Un Mastermind à **deux joueurs sur le même PC**, écrit en C avec l'API Win32. Le joueur 1 compose un code secret de cinq chiffres (ou en génère un au hasard), le joueur 2 doit le retrouver en vingt tentatives grâce aux indices « bien placés » et « mal placés ». La partie est entièrement graphique : dialogues Win32, bitmap de fond, icône, infobulles et menu.

## Fonctionnalités

- **Deux modes de jeu**
  - *Débutant* : les cinq chiffres du code sont tous différents.
  - *Expert* : les répétitions sont autorisées, ce qui rend la déduction plus difficile.
- **Code secret de 5 chiffres** (0–9), saisi par le joueur 1 ou généré aléatoirement.
- **20 tentatives** par partie, avec retour du nombre de chiffres bien placés et mal placés.
- **Jokers** débloqués lorsqu'il reste 10 tentatives ou moins :
  - *Joker 1* (coût : 3 tentatives) révèle le chiffre d'une position choisie.
  - *Joker 2* (coût : 5 tentatives) révèle un chiffre absent du code.
  - Un seul joker peut être utilisé par partie.
- **Score en XP** : `tentatives restantes × 100`.
- **Scores persistants** : score cumulé et meilleur score par pseudo, enregistrés dans `scores.txt` et `scores2.txt`.
- Interface française, menu Jeu/Aide et boîte « À propos ».

## Règles

| Élément | Valeur |
| --- | --- |
| Longueur du code | 5 chiffres |
| Chiffres autorisés | 0 à 9 |
| Tentatives | 20 |
| Doublons | Interdits en mode Débutant, autorisés en mode Expert |
| Jokers | Débloqués à partir de 10 tentatives restantes, un seul par partie |
| Score | 100 XP par tentative restante |

## Compilation

Le projet est prévu pour **Windows** et une chaîne **MinGW/GCC**.

### Code::Blocks

Ouvrir `Projet/Project1.cbp`, puis *Build and run*.

### Dev-C++

Ouvrir `Projet/Project1.dev`, ou utiliser `Projet/Makefile.win`.

### Ligne de commande (MinGW)

Depuis le dossier `Projet` :

```bat
windres -i ressources.rc -o ressources.o
gcc -O2 main.c ressources.o -o Mastermind.exe -mwindows -lcomctl32 -lgdi32
```

`-mwindows` évite l'ouverture d'une console ; `comctl32` et `gdi32` sont nécessaires pour les contrôles communs, les infobulles et l'affichage du bitmap.

## Utilisation

1. Lancer `Mastermind.exe`.
2. Choisir **Mode Débutant** ou **Mode Expert** dans le menu *Jeu*.
3. Le joueur 1 saisit un code de cinq chiffres (ou clique sur le bouton de génération aléatoire), puis valide.
4. Le joueur 2 propose un code à chaque tour. Une boîte de dialogue indique les chiffres bien placés et mal placés.
5. À la victoire, saisir son pseudo pour enregistrer son score.

## Structure du projet

| Fichier | Rôle |
| --- | --- |
| `Projet/main.c` | Logique du jeu, procédures de dialogue et gestion des scores |
| `Projet/ressources.h` | Constantes des ressources et dimensions du jeu |
| `Projet/ressources.rc` | Dialogues, menu, icône et bitmap |
| `Projet/master.bmp`, `master.ico` | Ressources graphiques |
| `Projet/Project1.cbp` | Projet Code::Blocks |
| `Projet/Project1.dev`, `Makefile.win` | Projet et makefile Dev-C++ |

## Remarques

- Le jeu est **Windows uniquement** : il repose sur les dialogues et les contrôles communs Win32.
- Les fichiers `scores.txt` et `scores2.txt` sont créés dans le **répertoire courant**. Lancer l'exécutable depuis son dossier pour retrouver ses scores.
- Les scores sont stockés en texte brut, sans protection ; ce projet est avant tout pédagogique.
- Le dépôt contient aussi `Projet.rar` (archive du projet) et des binaires de compilation (`Project1.exe`, `main.o`).

## Licence

Distribué sous **GNU General Public License v3.0**. Voir [`LICENSE`](LICENSE).

## Auteurs

Projet réalisé par **Abdelaziz Mohammad Aamer Al Harbi** et **Youssef Aziz**.
