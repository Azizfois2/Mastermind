# Mastermind — jeu de déduction en C / Win32

<p>
  <img alt="Langage" src="https://img.shields.io/badge/C-11-00599C?logo=c&logoColor=white">
  <img alt="Plateforme" src="https://img.shields.io/badge/Windows-Win32-0078D6?logo=windows&logoColor=white">
  <img alt="Licence" src="https://img.shields.io/badge/licence-GPL--3.0-blue">
</p>

Un Mastermind à **deux joueurs sur le même PC**, écrit en C avec l'API Win32. Le joueur 1 compose un code secret de cinq chiffres (ou en génère un au hasard), le joueur 2 doit le retrouver en vingt tentatives grâce aux indices « bien placés » et « mal placés ». La partie est entièrement graphique : fenêtre principale, dialogues Win32, bitmap de fond, icône, infobulles et menu.

## Sommaire

- [Aperçu](#aperçu)
- [Déroulement d'une partie](#déroulement-dune-partie)
- [Calcul des indices](#calcul-des-indices)
- [Les jokers](#les-jokers)
- [Score et fichiers de scores](#score-et-fichiers-de-scores)
- [Interface](#interface)
- [Compilation](#compilation)
- [Structure du projet](#structure-du-projet)
- [Détails d'implémentation](#détails-dimplémentation)
- [Limites et particularités](#limites-et-particularités)
- [Licence](#licence)

## Aperçu

| Élément | Valeur |
| --- | --- |
| Joueurs | 2, en alternance sur le même poste |
| Longueur du code | 5 chiffres |
| Chiffres autorisés | 0 à 9 |
| Tentatives | 20 par partie |
| Modes | Débutant (chiffres distincts), Expert (doublons autorisés) |
| Indices | Chiffres bien placés et mal placés |
| Jokers | 2 types, débloqués à 10 tentatives restantes, un seul par partie |
| Score | 100 XP par tentative restante, 2 000 XP maximum |
| Persistance | `scores.txt` (cumul) et `scores2.txt` (historique des parties) |

## Déroulement d'une partie

1. **Choix du mode.** La fenêtre principale propose **Mode Débutant** et **Mode Expert** (également dans le menu *Jeu*). Le mode ne change qu'une chose : les chiffres du code et des essais peuvent-ils se répéter ?
2. **Saisie du code par le joueur 1.** Une boîte de dialogue « Joueur 1 » s'ouvre. Le champ accepte exactement 5 chiffres et les masque par défaut avec des `*` ; la case **Afficher le code** les rend visibles. On peut aussi cliquer sur **Code aléatoire**.
3. **Passage au joueur 2.** Après validation, le message « Le code saisi est bien enregistré ! Au tour du Joueur 2. » confirme la transition. La boîte de dialogue passe alors en mode devinette : titre « Joueur 2 », consigne « Essayer de deviner le code », compteur de tentatives visible, bouton **Vérifier** et boutons de jokers.
4. **Chaque essai.** Le joueur 2 saisit 5 chiffres et clique sur **Vérifier**. Une boîte de dialogue affiche le nombre de **bien placés** et de **mal placés**, puis le compteur diminue de 1.
5. **Fin de partie.**
   - Si les 5 chiffres sont bien placés : victoire, calcul du score, saisie facultative du pseudo et enregistrement.
   - Si le compteur atteint 0 : défaite, le code secret est révélé.
   - Le bouton **Abandonner** ferme la partie sans enregistrer de score.

Le dialogue se ferme à la fin de chaque partie ; on revient à la fenêtre principale pour choisir un mode et recommencer.

## Calcul des indices

Deux compteurs sont calculés à chaque essai :

- **Bien placés** : même chiffre à la même position.
- **Mal placés** : chiffre présent dans le code, mais à une autre position.

L'algorithme traite d'abord les correspondances exactes, puis apparie les chiffres restants **une seule fois chacun**. Un chiffre du code ne peut donc pas être compté deux fois, même en mode Expert où les doublons sont autorisés.

Exemple :

| Position | 1 | 2 | 3 | 4 | 5 |
| --- | --- | --- | --- | --- | --- |
| Code secret | 1 | 2 | 3 | 4 | 5 |
| Essai | 2 | 1 | 3 | 7 | 5 |
| Résultat | mal placé | mal placé | bien placé | absent | bien placé |

Résultat affiché : **2 bien placés, 2 mal placés**. Le `3` et le `5` sont déjà à la bonne position ; le `1` et le `2` sont présents mais inversés ; le `7` n'est pas dans le code.

## Les jokers

Les jokers ne sont pas disponibles au début. Ils apparaissent après un essai, lorsque le compteur est passé à **10 tentatives restantes ou moins**. Un seul joker peut être utilisé par partie : dès que l'un est employé, les deux boutons disparaissent.

| Joker | Coût | Saisie | Effet |
| --- | --- | --- | --- |
| **Joker 1** | 3 tentatives | Un chiffre de `0` à `4` dans le champ, qui sert d'**index de position** | Révèle le chiffre secret à cette position (0 = première, 4 = cinquième) |
| **Joker 2** | 5 tentatives | Aucune | Indique un chiffre de `1` à `9` **absent** du code |

Précisions issues du code :

- Le coût est déduit immédiatement du compteur, sans consommer d'essai supplémentaire.
- Un joker n'est proposé que si le joueur peut payer son coût : **Joker 1** exige plus de 3 tentatives restantes, **Joker 2** plus de 5. À 3 tentatives ou moins, aucun bouton n'apparaît.
- Pour le Joker 1, il faut saisir **un seul chiffre** (`0` à `4`) dans le champ, puis cliquer sur le bouton. Un essai complet de 5 chiffres est refusé.
- Le Joker 2 parcourt les chiffres `1` à `9` : il ne proposera jamais `0`, même si `0` est absent du code.
- Utiliser un joker réduit mécaniquement le score final, puisque celui-ci dépend des tentatives restantes.

## Score et fichiers de scores

### Formule

```text
score = tentatives restantes au moment de la victoire × 100
```

L'essai gagnant ne consomme pas de tentative : une victoire au premier essai rapporte donc `20 × 100 = 2 000 XP`.

| Situation | Tentatives restantes | Score |
| --- | --- | --- |
| Victoire au 1er essai | 20 | 2 000 XP |
| Victoire après 5 essais ratés | 15 | 1 500 XP |
| Joker 1 utilisé à 10 tentatives, puis victoire | 7 | 700 XP |
| Victoire au 20e essai | 1 | 100 XP |

### Enregistrement

À la victoire, une boîte de dialogue demande un pseudo (facultatif) :

- **Avec pseudo** : le score est ajouté au cumul du joueur dans `scores.txt`, la partie est ajoutée à `scores2.txt`, et la boîte de victoire affiche le score de la partie, le score cumulé et le meilleur score.
- **Sans pseudo** (annulation ou champ vide) : la victoire est affichée, mais aucun score n'est enregistré.

Les fichiers sont créés dans le **répertoire courant** de l'exécutable, au format texte `Nom - Score` :

`scores.txt` — un total cumulé par pseudo, réécrit entièrement à chaque sauvegarde :

```text
Aziz - 1500
Youssef - 900
```

`scores2.txt` — historique de toutes les parties gagnées, en ajout seul :

```text
Aziz - 1500
Aziz - 700
Youssef - 900
```

Le meilleur score est le maximum des lignes de `scores2.txt` pour le pseudo exact. La comparaison des noms est **sensible à la casse** : `Aziz` et `aziz` sont deux joueurs différents.

## Interface

- **Fenêtre principale** de 640 × 500, non redimensionnable, avec bitmap de fond, icône et menu.
- **Menu _Jeu_** : Mode Débutant, Mode Expert, Quitter.
- **Menu _Aide_** : À propos (règles, coûts des jokers, auteurs).
- **Infobulles** sur les deux boutons de mode pour rappeler la contrainte sur les doublons.
- **Dialogue Joueur 1** : champ limité à 5 caractères, masqué par défaut, case *Afficher le code*, bouton *Code aléatoire*.
- **Dialogue Joueur 2** : compteur « Tentatives restantes », bouton *Vérifier*, boutons *Joker 1* / *Joker 2* révélés au bon moment, bouton *Abandonner*.
- La génération aléatoire utilise `rand()` initialisé avec `srand(time(NULL))` : en mode Débutant elle tire des chiffres distincts, en mode Expert elle autorise les répétitions.

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

## Structure du projet

| Fichier | Rôle |
| --- | --- |
| `Projet/main.c` | Logique du jeu, procédures de dialogue et gestion des scores |
| `Projet/ressources.h` | Constantes des ressources et dimensions du jeu |
| `Projet/ressources.rc` | Dialogues, menu, icône et bitmap |
| `Projet/master.bmp`, `master.ico` | Ressources graphiques |
| `Projet/Project1.cbp` | Projet Code::Blocks |
| `Projet/Project1.dev`, `Makefile.win` | Projet et makefile Dev-C++ |

## Détails d'implémentation

| Fonction | Responsabilité |
| --- | --- |
| `WinMain` | Initialise `rand()`, charge le bitmap et l'icône, crée la fenêtre, les boutons de mode et les infobulles |
| `WndProc` | Fenêtre principale : sélection du mode, menu, À propos, quitter, peinture du fond |
| `InputDlgProc` | Dialogue central : saisie du code, phase de devinette, jokers, victoire et défaite |
| `chg` | Bascule de l'état « Joueur 1 » vers « Joueur 2 », réinitialise le compteur et crée les boutons |
| `ResetGame` | Remet à zéro le code, l'essai et les tableaux de correspondance |
| `good` / `bad` | Calculent les bien placés et les mal placés, avec appariement unique |
| `Generate` / `Generateunique` | Tirent un code aléatoire avec ou sans doublons |
| `jone` / `jtwo` | Fournissent la réponse des deux jokers |
| `scorefs` / `scorefs2` | Lisent et analysent `scores.txt` et `scores2.txt` |
| `scorefm` | Ajoute le score au cumul du joueur et réécrit `scores.txt` |
| `scoremax` | Retourne le meilleur score d'un joueur depuis `scores2.txt` |
| `NameDlgProc` | Saisie du pseudo à la victoire |

## Limites et particularités

- **Windows uniquement** : le jeu repose sur les dialogues et les contrôles communs Win32.
- **Français uniquement** : tous les textes de l'interface sont codés en dur.
- Les scores sont lus et écrits dans le **répertoire courant** ; lancer l'exécutable depuis son dossier pour retrouver ses parties.
- Les fichiers de scores sont en **texte brut**, sans verrou ni protection : ils sont éditables et un nom contenant ` - ` perturbe l'analyse.
- Le cumul est prévu pour **100 joueurs** par fichier (tableau `PS player[100]`) et le pseudo est tronqué à **31 caractères** dans la boîte de dialogue.
- Si le dossier n'est pas accessible en écriture, la sauvegarde peut échouer sans message ; les scores affichés ne sont alors pas garantis d'être conservés.
- Le mode Débutant interdit les doublons **dans le code comme dans les essais** ; le mode Expert les autorise.
- Le joueur 1 ne ressaisit pas son code : une faute de frappe n'est pas détectable avant la fin de la partie.
- Fermer le dialogue avec **Abandonner** termine la partie sans enregistrer de score.
- Le score dépend uniquement des tentatives restantes : le nombre d'essais réellement joués et l'usage d'un joker n'ont d'effet que par les tentatives qu'ils consomment.
- Aucun tableau des meilleurs scores n'est affiché en dehors de la boîte de victoire.

## Licence

Distribué sous **GNU General Public License v3.0**. Voir [`LICENSE`](LICENSE).

## Auteurs

Projet réalisé par **Abdelaziz Mohammad Aamer Al Harbi** et **Youssef Aziz**.
