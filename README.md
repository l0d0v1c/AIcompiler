

# Langage Ἑλληνικός - Projet o3-mini-high

Ce projet présente **Ἑλληνικός**, un langage de programmation fictif et minimaliste inspiré du grec ancien. Ce code est généré par **o3-mini-high** et se trouve dans le fichier **assembler.c**. Ce compilateur minimaliste traduit directement un programme écrit en Ἑλληνικός en assembleur ARM64 pour Apple Silicon, sans passer par une traduction intermédiaire en C.

## Caractéristiques du Langage

Le langage **Ἑλληνικός** utilise quelques mots-clés essentiels :

- **ἄρχισον**  
  Démarre un programme ou un bloc de code.

- **τελος**  
  Termine un programme ou un bloc de code.

- **γράφε**  
  Affiche du texte (seulement la version avec une chaîne littérale est supportée dans cet exemple).

La structure générale d’un programme en Ἑλληνικός est la suivante :

    ἄρχισον
        ... instructions ...
    τελος

Les instructions se terminent par un point-virgule `;`.

## Exemple Hello World

Voici un exemple minimal de programme "Hello World" écrit en **Ἑλληνικός** :

    ἄρχισον
        γράφε("Hello World!");
    τελος

Ce programme démarre avec `ἄρχισον`, affiche le texte `"Hello World!"` via la commande `γράφε`, puis se termine avec `τελος`.

## Compilation et Génération de l'Exécutable

Le fichier **assembler.c** (généré par **o3-mini-high**) est un compilateur minimaliste écrit en C qui procède comme suit :

1. Il lit un fichier source écrit en **Ἑλληνικός**.
2. Il génère directement du code assembleur ARM64 (dans le fichier `output.s`).
3. Il invoque Clang avec l'option `-arch arm64` pour assembler et lier le code en un exécutable natif pour Apple Silicon.

### Étapes pour Compiler et Exécuter un Programme en Ἑλληνικός

1. **Compiler le compilateur (assembler.c) :**

   Sur votre Apple Silicon, compilez le fichier **assembler.c** avec la commande suivante :

       gcc -o assembler assembler.c

   ou avec Clang :

       clang -o assembler assembler.c

2. **Créer un fichier source en Ἑλληνικός :**

   Créez un fichier (par exemple `programme.txt`) contenant le code source. Pour tester avec Hello World, utilisez :

       ```
       ἄρχισον
           γράφε("Hello World!");
       τελος
       ```
`
3. **Exécuter le compilateur :**

   Lancez le compilateur en passant le fichier source en argument :

       ./assembler programme.txt

   Le compilateur générera un fichier `output.s` (assembleur ARM64) et appellera Clang pour produire un exécutable nommé `output`.

4. **Exécuter l'exécutable généré :**

   Lancez l’exécutable :

       ./output

   Vous devriez voir s'afficher :

       Hello World!

## Conclusion

Ce projet, généré par **o3-mini-high**, démontre comment créer un compilateur minimaliste pour un langage de programmation inspiré du grec ancien. Le compilateur traduit directement le code source en assembleur ARM64 pour Apple Silicon, sans passer par une traduction en C. Ce projet peut être étendu pour ajouter de nouvelles fonctionnalités et constructions de langage.
