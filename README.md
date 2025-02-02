# Langage Ἑλληνικός

Ce projet présente **Ἑλληνικός**, un langage de programmation fictif et minimaliste inspiré du grec ancien. L’objectif est de proposer une syntaxe originale et ludique en utilisant des mots grecs antiques, tout en démontrant comment construire un compilateur (ou transpileur) qui traduit ce langage en code C.

## Caractéristiques du Langage

Le langage **Ἑλληνικός** utilise quelques mots-clés essentiels :

- **ἄρχισον**  
  Démarre un programme ou un bloc de code.

- **τελος**  
  Termine un programme ou un bloc de code.

- **μεταβλητὴ**  
  Permet de déclarer une variable (équivalent de `int` en C).

- **γράφε**  
  Affiche du texte ou une valeur (équivalent de `printf` en C).


La structure générale d’un programme est la suivante :

```plaintext
ἄρχισον
    ... instructions ...
τελος

Les instructions se terminent par un point-virgule ;.

Exemple Hello World

Voici un exemple minimal de programme “Hello World” écrit en Ἑλληνικός :

ἄρχισον
    γράφε("Hello World!");
τελος

Ce programme démarre avec ἄρχισον, affiche le texte "Hello World!" grâce à la commande γράφε, puis se termine avec τελος.

Compilation du Langage

Un compilateur (ou transpileur) en C est fourni dans ce projet. Ce compilateur réalise les opérations suivantes :
	1.	Il lit un fichier source écrit en Ἑλληνικός.
	2.	Il génère un fichier source C (output.c) équivalent.
	3.	Il appelle gcc pour compiler ce fichier C et produire un exécutable.

Étapes pour Compiler et Exécuter un Programme en Ἑλληνικός
	1.	Compiler le compilateur :
Compilez le fichier source du compilateur (par exemple compiler.c) avec gcc :

gcc -o compiler compiler.c


	2.	Créer un fichier source en Ἑλληνικός :
Créez un fichier (par exemple programme.txt) contenant le code source. Pour tester avec le Hello World, écrivez :

ἄρχισον
    γράφε("Hello World!");
τελος


	3.	Exécuter le compilateur :
Lancez le compilateur en lui passant le fichier source en argument :

./compiler programme.txt

Le compilateur générera un fichier output.c puis appellera gcc pour créer l’exécutable output.

	4.	Exécuter le programme généré :
Exécutez l’exécutable obtenu :

./output

Vous devriez alors voir s’afficher :

Hello World!



Conclusion

Ce projet est une démonstration ludique de la création d’un langage de programmation inspiré du grec ancien et de la réalisation d’un compilateur minimaliste qui traduit ce langage en C. Vous pouvez étendre ce système pour ajouter d’autres constructions (boucles, conditions, expressions complexes, etc.) et ainsi explorer la conception de langages et de compilateurs de manière créative.

