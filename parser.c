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

    while(str[i+j] != '\0'){
        str[i] = str[i+j];
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
    while (str[i])
    {
        if (str[i] != ' '){
            str[j] = str[i];
            j++;
            i++;
        } else {
            str[j] = str[i];
            j++;
            i++;
            while(str[i] == ' '){
                i++;
            }
        }
    }
    str[j] = '\0';
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
    const char * separators = " ";
    char * strToken = strtok(str, separators);
    while(strToken != NULL){
        strcat(tokens[j], strToken);
        j++;
        // On demande le token suivant.
        strToken = strtok (NULL, separators);
    }

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
    char *test[13] = {";", "&", "<", "2>", "||", "&&", "!", ">", ">>", "2>>", ">&2", "2>&1", "|"};
    for(int x = 0; x < 13; x++){
        if(strcmp(tok,test[x]) == 0){
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


}
