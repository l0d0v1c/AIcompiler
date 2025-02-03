#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_STRINGS 256

// Pointeur global sur le contenu source.
char *src;
// Indicateur : le token courant est-il une chaîne littérale ?
int tokenIsString = 0;
// Fichier de sortie pour l'assembleur généré.
FILE *out;

// Tableau pour stocker les chaînes de caractères utilisées dans les instructions 'γράφε'
char *strings[MAX_STRINGS];
int string_count = 0;

// Ajoute une chaîne dans le tableau et retourne l'indice (qui servira de numéro de label).
int add_string_literal(const char *s) {
    if (string_count >= MAX_STRINGS) {
        fprintf(stderr, "Erreur: trop de chaînes de caractères.\n");
        exit(1);
    }
    strings[string_count] = strdup(s);
    if (!strings[string_count]) {
        fprintf(stderr, "Erreur d'allocation mémoire pour la chaîne.\n");
        exit(1);
    }
    return string_count++;
}

// Fonction d'analyse lexicale : retourne le prochain token de la source.
char *nextToken() {
    static char token[256];
    int tokenIndex = 0;
    
    // Ignorer les espaces.
    while (*src && isspace((unsigned char)*src))
        src++;
    if (!*src)
        return NULL;
    
    // Si le caractère est un symbole parmi ( ) , ; +
    if (strchr("(),;+", *src) != NULL) {
        tokenIsString = 0;
        token[tokenIndex++] = *src;
        token[tokenIndex] = '\0';
        src++;
        return token;
    }
    
    // Si c'est le début d'une chaîne (délimitée par des guillemets).
    if (*src == '"') {
        tokenIsString = 1;
        src++;  // Ignorer le guillemet ouvrant.
        while (*src && *src != '"') {
            token[tokenIndex++] = *src;
            src++;
        }
        if (*src == '"')
            src++;  // Ignorer le guillemet fermant.
        token[tokenIndex] = '\0';
        return token;
    }
    
    // Sinon, lire un mot (identificateur, mot-clé, nombre, etc.)
    tokenIsString = 0;
    while (*src && !isspace((unsigned char)*src) && !strchr("(),;+", *src)) {
        token[tokenIndex++] = *src;
        src++;
    }
    token[tokenIndex] = '\0';
    return token;
}

// Fonction utilitaire qui vérifie que le token suivant correspond à la chaîne attendue.
void expect(const char *expected) {
    char *token = nextToken();
    if (!token || strcmp(token, expected) != 0) {
        fprintf(stderr, "Erreur: attendu '%s', trouvé '%s'\n", expected, token ? token : "NULL");
        exit(1);
    }
}

/*
 * Analyse et traduit une instruction d'affichage « γράφε ».
 * Syntaxe supportée :  
 *     γράφε( "chaine" );
 *
 * Pour chaque appel, on génère du code assembleur qui charge l'adresse
 * de la chaîne (placée dans la section __cstring) dans le registre x0 et appelle _puts.
 */
void parse_print_statement() {
    // On a déjà consommé le token "γράφε".
    expect("(");
    char *arg = nextToken();
    if (!arg || !tokenIsString) {
        fprintf(stderr, "Erreur: argument string attendu dans γράφε.\n");
        exit(1);
    }
    int label_index = add_string_literal(arg);
    expect(")");
    expect(";");
    // Génération d'un appel à _puts.
    // Les instructions suivantes chargent l'adresse de la chaîne L_str<label_index> dans x0.
    fprintf(out, "    adrp    x0, L_str%d@PAGE\n", label_index);
    fprintf(out, "    add     x0, x0, L_str%d@PAGEOFF\n", label_index);
    fprintf(out, "    bl      _puts\n");
}

/*
 * Analyse un bloc d'instructions.
 * Un bloc est délimité par les tokens "ἄρχισον" et "τελος".
 */
void parse_block() {
    while (1) {
        char *token = nextToken();
        if (!token) {
            fprintf(stderr, "Erreur: bloc non fermé, 'τελος' attendu.\n");
            exit(1);
        }
        if (strcmp(token, "τελος") == 0) {
            break;  // Fin du bloc.
        }
        if (strcmp(token, "γράφε") == 0) {
            parse_print_statement();
        } else if (strcmp(token, "ἄρχισον") == 0) {
            // Bloc imbriqué (on se contente d'appeler récursivement parse_block).
            parse_block();
        } else {
            fprintf(stderr, "Erreur: instruction inconnue '%s'.\n", token);
            exit(1);
        }
    }
}

/*
 * Analyse le programme complet.
 * Le programme doit commencer par "ἄρχισον".
 */
void parse_program() {
    char *token = nextToken();
    if (!token || strcmp(token, "ἄρχισον") != 0) {
        fprintf(stderr, "Erreur: le programme doit commencer par ἄρχισον.\n");
        exit(1);
    }
    parse_block();
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <fichier_source>\n", argv[0]);
        return 1;
    }
    // Ouvrir le fichier source écrit en grec ancien.
    FILE *in = fopen(argv[1], "r");
    if (!in) {
        perror("Erreur d'ouverture du fichier source");
        return 1;
    }
    fseek(in, 0, SEEK_END);
    long length = ftell(in);
    fseek(in, 0, SEEK_SET);
    char *srcOrig = malloc(length + 1);
    if (!srcOrig) {
        fprintf(stderr, "Erreur d'allocation mémoire.\n");
        return 1;
    }
    fread(srcOrig, 1, length, in);
    srcOrig[length] = '\0';
    fclose(in);
    src = srcOrig;
    
    // Ouvrir le fichier de sortie pour écrire le code assembleur.
    out = fopen("output.s", "w");
    if (!out) {
        perror("Erreur d'ouverture du fichier de sortie");
        free(srcOrig);
        return 1;
    }
    
    // Écrire l'en-tête de la section de code (section __text).
    fprintf(out, "    .section __TEXT,__text,regular,pure_instructions\n");
    fprintf(out, "    .globl _main\n");
    fprintf(out, "    .p2align 2\n");
    fprintf(out, "_main:\n");
    
    // Analyse du programme et génération du code pour chaque instruction.
    parse_program();
    
    // Terminer la fonction main : retourner 0 et effectuer un ret.
    fprintf(out, "    mov w0, 0\n");
    fprintf(out, "    ret\n\n");
    
    // Générer la section de données pour les chaînes littérales.
    fprintf(out, "    .section __TEXT,__cstring,cstring_literals\n");
    for (int i = 0; i < string_count; i++) {
        fprintf(out, "L_str%d:\n", i);
        fprintf(out, "    .asciz \"%s\"\n", strings[i]);
    }
    
    fclose(out);
    free(srcOrig);
    
    // Appeler Clang pour assembler et lier le fichier output.s en un exécutable pour Apple Silicon.
    int ret = system("clang -arch arm64 output.s -o output");
    if (ret != 0) {
        fprintf(stderr, "Erreur lors de la compilation de output.s\n");
        return 1;
    }
    printf("Compilation réussie. Exécutable généré : output\n");
    return 0;
}