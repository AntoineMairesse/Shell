/*
  Projet minishell - Licence 3 Info - PSI 2021
 
  Nom : Mairesse
  Prénom : Antoine
  Num. étudiant : m21912663
  Groupe de projet : 10
 
  Parsing de la ligne de commandes utilisateur (implémentation).
 
 */

#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include "parser.h"
#include "processus.h"
#include "builtin.h"

/*
  Suppression des espaces en début et fin de chaîne.
      Ex : "   chaîne   " => "chaîne"
 
  str : chaîne à modifier
 
  Retourne 0 ou un code d'erreur.
 */
int trim(char *str) {
    assert(str != NULL);
    int j = 0, i = 0;
    while (isspace(str[j])) {
        j++;
    }

    while (str[i + j] != '\0') {
        str[i] = str[i + j];
        i++;
    }
    str[i] = '\0';

    int index;
    i = 0;
    /* Find last index of non-white space character */
    while (str[i] != '\0') {
        if (!isspace(str[i])) {
            index = i;
        }
        i++;
    }

    /* Mark next character to last non-white space character as NULL */
    str[index + 1] = '\0';
    return 0;
}

/*
  Suppression des doublons d'espaces.
      Ex : "cmd1   -arg  ;   cmd2  <  input"
        => "cmd1 -arg ; cmd2 < input"
 
  str: chaîne à modifier
 
  Retourne 0 ou un code d'erreur.
 */
int clean(char *str) {
    assert(str != NULL);
    int i = 0, j = 0;
    while (str[i]) {
        if (!isspace(str[i])) {
            str[j] = str[i];
            j++;
            i++;
        } else {
            str[j] = str[i];
            j++;
            i++;
            while (isspace(str[i])) {
                i++;
            }
        }
    }
    str[j] = '\0';
    return 0;
}

/*
  Découpage de la chaîne en ses éléments.
    Ex : "ls -l | grep ^a ; cat"
      => {"ls", "-l", "|", "grep", "^a", ";", "cat", NULL}
  str : chaîne à découper (peut être modifiée)
  tokens : tableau des pointeurs sur les éléments de la
           chaîne. Terminé par NULL.
 
  Retourne 0 ou un code d'erreur.
 */
int tokenize(char *str, char *tokens[]) {
    assert(str != NULL);
    assert(tokens != NULL);
    int j = 0;
    const char *separators = " ";
    char *strToken = strtok(str, separators);
    while (strToken != NULL) {
        strcat(tokens[j], strToken);
        j++;
        // On demande le token suivant.
        strToken = strtok(NULL, separators);
    }
    tokens[j] = NULL;
    return 0;
}

/*
  S'agit-il d'un mot réservé du shell ?
  Les mots réservés sont par exemple :
    ";", "&", "<", "2>", "||", ...
 
  tok : la chaîne à tester
 
  Retourne 1 s'il s'agit d'un mot réservé, 0 sinon.
 */
int is_reserved(const char *tok) {
    assert(tok != NULL);
    char *reserved[23] = {";", "&", "||", "&&", "|", "<", ">>", ">", "0>", "1>", "2>", "0>>", "1>>", "2>>", ">&0",
                          ">&1", ">&2", "0>&1", "0>&2", "1>&0", "1>&2", "2>&0", "2>&1"};
    for (int x = 0; x < 23; x++) {
        if (strcmp(tok, reserved[x]) == 0) {
            return 1;
        }
    }
    return 0;
}

int is_background(const char *tok) {
    assert(tok != NULL);
    char *background[2] = {"&", "|"};
    for (int x = 0; x < 2; x++) {
        if (strcmp(tok, background[x]) == 0) {
            return 1;
        }
    }
    return 0;
}

int is_redirect(const char *tok) {
    assert(tok != NULL);
    char *background[18] = {"<", ">>", ">", "0>", "1>", "2>", "0>>", "1>>", "2>>", ">&0", ">&1", ">&2", "0>&1", "0>&2",
                            "1>&0", "1>&2", "2>&0", "2>&1"};
    for (int x = 0; x < 18; x++) {
        if (strcmp(tok, background[x]) == 0) {
            return 1;
        }
    }
    return 0;
}

/*
  Remplit le tableau de commandes en fonction du contenu
  de tokens.
    Ex : {"ls", "-l", "|", "grep", "^a", NULL} =>
         {{path = "ls",
           argv = {"ls", "-l", NULL},
           bg = 1,
           ...},
          {path = "grep",
           argv = {"grep", "^a", NULL},
           bg = 0,
           ...}}
  tokens : le tableau des éléments de la ligne de
           commandes (peut être modifié)
  commands : le tableau dans lequel sont stockés les
             différentes structures représentant le
             commandes.
  Retourne 0 ou un code d'erreur.
 */

int parse_cmd(char *tokens[], process_t *commands) {
    assert(tokens != NULL);
    assert(commands != NULL);
    int i = 0, j = 0, y = 0;
    init_process(commands);
    while (tokens[i] != NULL) {
        if (is_reserved(tokens[i])) {
            commands[j].argv[y] = NULL;
            if (is_redirect(tokens[i])) {
                if (strcmp(tokens[i], "0>") == 0 && tokens[i + 1] != NULL) {
                    commands[j].stdin = open(tokens[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0777);
                } else if ((strcmp(tokens[i], "1>") == 0 || strcmp(tokens[i], ">") == 0) && tokens[i + 1] != NULL) {
                    commands[j].stdout = open(tokens[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0777);
                } else if (strcmp(tokens[i], "2>") == 0 && tokens[i + 1] != NULL) {
                    commands[j].stderr = open(tokens[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0777);
                } else if ((strcmp(tokens[i], ">>") == 0 || strcmp(tokens[i], "1>>") == 0) && tokens[i + 1] != NULL) {
                    commands[j].stdout = open(tokens[i + 1], O_WRONLY | O_CREAT | O_APPEND, 0777);
                } else if (strcmp(tokens[i], "<") == 0 && tokens[i + 1] != NULL) {
                    commands[j].stdin = open(tokens[i + 1], O_RDONLY, 0777);
                } else if (strcmp(tokens[i], "0>>") == 0 && tokens[i + 1] != NULL) {
                    commands[j].stdin = open(tokens[i + 1], O_WRONLY | O_CREAT | O_APPEND, 0777);
                } else if (strcmp(tokens[i], "2>>") == 0 && tokens[i + 1] != NULL) {
                    commands[j].stderr = open(tokens[i + 1], O_WRONLY | O_CREAT | O_APPEND, 0777);
                } else if (strcmp(tokens[i], ">&0") == 0 || strcmp(tokens[i], "1>&0") == 0) {
                    printf("test1");
                    commands[j].stdin = commands[j].stdout;
                    printf("test2");
                } else if (strcmp(tokens[i], ">&2") == 0 || strcmp(tokens[i], "1>&2") == 0) {
                    commands[j].stderr = commands[j].stdout;
                } else if (strcmp(tokens[i], "0>&1") == 0) {
                    commands[j].stdout = commands[j].stdin;
                } else if (strcmp(tokens[i], "0>&2") == 0) {
                    commands[j].stderr = commands[j].stdin;
                } else if (strcmp(tokens[i], "2>&0") == 0) {
                    commands[j].stdin = commands[j].stderr;
                } else if (strcmp(tokens[i], "2>&1") == 0) {
                    commands[j].stdout = commands[j].stderr;
                }
                i++;
                //On enlève les tokens inutiles
                while (tokens[i] != NULL && tokens[i + 1] != NULL && !is_reserved(tokens[i + 1])) {
                    i++;
                }
            } else if (is_background(tokens[i])) {
                commands[j].bg = 1;
                if (strcmp(tokens[i], "|") == 0) {
                    commands[j].pipe = 1;
                }
            }
            else
                commands[j].bg = 0;


            if (tokens[i] != NULL && tokens[i + 1] != NULL) {
                j++;
                init_process(commands + j);
                commands[j].path = tokens[i + 1];
                y = 0;
                if (strcmp(tokens[i], "||") == 0 || strcmp(tokens[i], "&&") == 0) {
                    commands[j].exec_cmd = 0;
                    if (strcmp(tokens[i], "||") == 0) {
                        if(commands[j - 1].reverse == 0)
                            commands[j - 1].next_failure = commands + j;
                        else
                            commands[j - 1].next_success = commands + j;
                    } else if (strcmp(tokens[i], "&&") == 0) {
                        if(commands[j - 1].reverse == 0)
                            commands[j - 1].next_success = commands + j;
                        else
                            commands[j - 1].next_failure = commands + j;
                    }
                }
            }
        } else {
            if(strcmp(tokens[i], "!") == 0) {
                commands[j].reverse = 1;
            } else {
                commands[j].argv[y] = tokens[i];
                y++;
                commands[j].argv[y] = (char *) malloc(sizeof(char *));
            }
        }
        if (tokens[i] != NULL)
            i++;
    }
    commands[j].argv[y] = NULL;
    return 0;
}
