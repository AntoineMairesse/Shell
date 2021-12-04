/*
  Projet minishell - Licence 3 Info - PSI 2021
 
  Nom : Mairesse
  Prénom : Antoine
  Num. étudiant : m21912663
  Groupe de projet : 10
  Date :
 
  Interface du minishell.
 
 */


#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>

#include "parser.h"
#include "processus.h"
#include "builtin.h"
#include <fcntl.h>

int main(int argc, char *argv[]) {
    char line[MAX_LINE_SIZE];
    char *cmdline[MAX_CMD_SIZE];
    process_t cmds[MAX_CMD_SIZE];

    while (1) {
        // (ré-)Initialiser les variables/structures
        for (int x = 0; x < MAX_CMD_SIZE; x++) {
            cmdline[x] = (char *) malloc(sizeof(char *) * MAX_LINE_SIZE);
        }
        // Affichage d'une invite de commande
        printf("Shell> ");

        // Lecture d'une ligne de commandes
        fgets(line, MAX_CMD_SIZE, stdin);
        trim(line);
        //printf("trim : '%s'\n", line);

        // "Nettoyage" de la ligne de commandes
        clean(line);
        //printf("clean : '%s'\n", line);

        // Découpage en "tokens"
        tokenize(line, cmdline);

        // Parsing de la ligne pour remplir les structures
        // de cmds.
        parse_cmd(cmdline, cmds);

        // Lancement des commandes dans l'ordre attendu,
        // avec les éventuelles redirections et conditionnements
        // d'exécution.
        launch_cmd(cmds);

        memset(line, 0, sizeof(line));
        memset(cmdline, 0, sizeof(cmdline));
        memset(cmds, 0, sizeof(cmds));
    }
    return -1; // Dead code
}
