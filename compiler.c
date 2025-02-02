#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Pointeur de parcours sur le contenu source
char *src;
// Indicateur : le token courant est-il une chaîne littérale ?
int tokenIsString = 0;
// Fichier de sortie pour le code C généré
FILE *out;

/* ===================== Fonctions Lexicales ===================== */

/*
 * Lit le prochain token dans la chaîne source.
 * Un token peut être :
 *  - Un symbole (caractères dans "(),=;+"),
 *  - Une chaîne délimitée par des guillemets,
 *  - Un mot (identificateur ou nombre).
 */
char *nextToken() {
    static char token[256];
    int tokenIndex = 0;

    // Ignorer les espaces
    while (*src && isspace((unsigned char)*src))
        src++;
    if (!*src)
        return NULL;

    // Si le caractère est un symbole
    if (strchr("(),=;+", *src) != NULL) {
        tokenIsString = 0;
        token[tokenIndex++] = *src;
        token[tokenIndex] = '\0';
        src++;
        return token;
    }

    // Si c'est le début d'une chaîne (entre guillemets)
    if (*src == '"') {
        tokenIsString = 1;
        src++;  // Ignore le guillemet ouvrant
        while (*src && *src != '"') {
            token[tokenIndex++] = *src;
            src++;
        }
        if (*src == '"')
            src++;  // Ignore le guillemet fermant
        token[tokenIndex] = '\0';
        return token;
    }

    // Sinon, lire un mot (nombre, identificateur, etc.)
    tokenIsString = 0;
    while (*src && !isspace((unsigned char)*src) && !strchr("(),=;+", *src)) {
        token[tokenIndex++] = *src;
        src++;
    }
    token[tokenIndex] = '\0';
    return token;
}

/*
 * Vérifie que le prochain token est bien celui attendu.
 * Sinon, affiche une erreur et quitte.
 */
void expect(const char *expected) {
    char *token = nextToken();
    if (!token || strcmp(token, expected) != 0) {
        fprintf(stderr, "Erreur: attendu '%s', trouvé '%s'\n", expected, token ? token : "NULL");
        exit(1);
    }
}

/* ===================== Analyse Syntaxique et Génération ===================== */

/*
 * Pour gérer la commande γράφε.
 * La syntaxe supportée est :
 *    γράφε( <argument> [ , <expression> ] );
 *
 * Si le premier argument est une chaîne, on génère :
 *    printf("%s", "texte");
 * ou, en cas d'argument supplémentaire, par exemple :
 *    printf("%s%d", "texte", expression);
 */
void parse_print_statement() {
    expect("(");
    char *arg = nextToken();
    if (!arg) {
        fprintf(stderr, "Erreur: argument attendu dans γράφε.\n");
        exit(1);
    }
    if (tokenIsString) {
        // Argument chaîne littérale
        char strLiteral[256];
        strcpy(strLiteral, arg);
        char *next = nextToken();
        if (next && strcmp(next, ",") == 0) {
            char *exprArg = nextToken();
            expect(")");
            expect(";");
            // Génère printf avec format "%s%d"
            fprintf(out, "    printf(\"%%s%%d\", \"%s\", %s);\n", strLiteral, exprArg);
        } else {
            if (!next || strcmp(next, ")") != 0) {
                fprintf(stderr, "Erreur: attendu ')' dans γράφε.\n");
                exit(1);
            }
            expect(";");
            fprintf(out, "    printf(\"%s\");\n", strLiteral);
        }
    } else {
        // Si ce n'est pas une chaîne, on suppose qu'il s'agit d'une expression (nombre ou variable)
        char exprArg[256];
        strcpy(exprArg, arg);
        expect(")");
        expect(";");
        fprintf(out, "    printf(\"%%d\", %s);\n", exprArg);
    }
}

/*
 * Analyse un bloc d'instructions.
 * Un bloc est délimité par ἄρχισον ... τελος.
 */
void parse_block() {
    while (1) {
        char *token = nextToken();
        if (!token) {
            fprintf(stderr, "Erreur: bloc non fermé, attendu τελος.\n");
            exit(1);
        }
        if (strcmp(token, "τελος") == 0) {
            break;
        }
        if (strcmp(token, "μεταβλητὴ") == 0) {
            // Déclaration de variable : μεταβλητὴ ident = expression;
            char *varName = nextToken();
            expect("=");
            char *expr = nextToken();
            expect(";");
            fprintf(out, "    int %s = %s;\n", varName, expr);
        } else if (strcmp(token, "γράφε") == 0) {
            parse_print_statement();
        } else if (strcmp(token, "ἄρχισον") == 0) {
            // Bloc imbriqué
            fprintf(out, "    {\n");
            parse_block();
            fprintf(out, "    }\n");
        } else {
            // Sinon, on considère qu'il s'agit d'une affectation : ident = expression;
            char *varName = token;
            expect("=");
            char *expr = nextToken();
            expect(";");
            fprintf(out, "    %s = %s;\n", varName, expr);
        }
    }
}

/*
 * Analyse le programme complet.
 * Le programme doit débuter par ἄρχισον et se terminer par τελος.
 * On génère le code C correspondant.
 */
void parse_program() {
    char *token = nextToken();
    if (!token || strcmp(token, "ἄρχισον") != 0) {
        fprintf(stderr, "Erreur: le programme doit commencer par ἄρχισον.\n");
        exit(1);
    }
    // Écrire l'en-tête du fichier C généré
    fprintf(out, "#include <stdio.h>\n\n");
    fprintf(out, "int main() {\n");
    parse_block();
    fprintf(out, "    return 0;\n");
    fprintf(out, "}\n");
}

/* ===================== Fonction Principale ===================== */

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <fichier_source>\n", argv[0]);
        return 1;
    }
    // Ouvrir le fichier source écrit en grec ancien
    FILE *in = fopen(argv[1], "r");
    if (!in) {
        perror("Erreur d'ouverture du fichier");
        return 1;
    }
    // Charger le contenu du fichier en mémoire
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

    // Ouvrir (ou créer) le fichier de sortie pour le code C généré
    out = fopen("output.c", "w");
    if (!out) {
        perror("Erreur d'ouverture du fichier de sortie");
        free(srcOrig);
        return 1;
    }
    // Analyse du programme grec et génération du code C
    parse_program();
    fclose(out);
    free(srcOrig);

    // Appeler gcc pour compiler le fichier output.c et générer l'exécutable "output"
    int ret = system("gcc output.c -o output");
    if (ret != 0) {
        fprintf(stderr, "Erreur de compilation de output.c\n");
        return 1;
    }
    printf("Compilation réussie. Exécutable généré : output\n");
    return 0;
}