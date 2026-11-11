# Mastermind — site officiel

Branche `website` du dépôt [Mastermind](https://github.com/Azizfois2/Mastermind).
Elle contient uniquement le site vitrine publié par GitHub Pages :

**https://azizfois2.github.io/Mastermind/**

Le jeu lui-même — sources C, ressources Win32, `Makefile` — vit sur la branche
[`main`](https://github.com/Azizfois2/Mastermind/tree/main). Voir le
[README principal](https://github.com/Azizfois2/Mastermind#readme) pour les règles,
le calcul des indices, les jokers et le barème.

## Contenu

| Fichier | Rôle |
| --- | --- |
| `index.html` | Page unique, autonome : styles et scripts en ligne |
| `assets/og.png` | Carte de partage social 1200 × 630 |
| `assets/screenshot-*.png` | Captures réelles de la version 1.0.0 |
| `assets/favicon.svg` | Icône du site |
| `robots.txt`, `sitemap.xml` | Indexation |

Aucune dépendance externe : pas de police distante, pas de CDN, pas de framework.
La page reste lisible sans JavaScript ; seule la démo interactive en a besoin.

## Prévisualiser en local

Ouvrir `index.html` dans un navigateur suffit. Pour un rendu fidèle à GitHub Pages
(chemins relatifs, types MIME), servir le dossier :

```powershell
python -m http.server 8000
# puis http://localhost:8000/
```

## Déploiement

GitHub Pages publie cette branche à la racine (`/`). Chaque `push` sur `website`
déclenche une reconstruction automatique :

```powershell
git add .
git commit -m "site: mise à jour"
git push origin website
```

Suivre l'avancement :

```powershell
gh api repos/Azizfois2/Mastermind/pages/builds --jq '.[0] | {status, created_at}'
```

## Mettre à jour le téléchargement

Les liens pointent vers la dernière release :

```text
https://github.com/Azizfois2/Mastermind/releases/latest/download/Mastermind.exe
```

Après une nouvelle version, mettre à jour dans `index.html` :

1. le numéro affiché (`v1.0.0`) ;
2. l'empreinte SHA-256 dans le bloc `#sha` ;
3. les métadonnées `softwareVersion` du bloc JSON-LD ;
4. les captures dans `assets/` si l'interface a changé.

L'empreinte se calcule avec :

```powershell
Get-FileHash .\Mastermind.exe -Algorithm SHA256
```

## Licence

Le site et le jeu sont distribués sous **GNU General Public License v3.0**.
Voir [LICENSE](https://github.com/Azizfois2/Mastermind/blob/main/LICENSE).
