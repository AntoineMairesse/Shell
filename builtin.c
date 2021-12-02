/*
  Projet minishell - Licence 3 Info - PSI 2021
 
  Nom : Mairesse
  Prénom : Antoine
  Num. étudiant : m21912663
  Groupe de projet : 10
  Date :
 
  Gestion des commandes internes du minishell (implémentation).
 
 */


#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <unistd.h>
#include <sys/fcntl.h>

#include "builtin.h"
#include "processus.h"

/*
 La chaîne passée représente-t-elle une commande
 interne ?
 
 cmd : chaîne de caractères.
 
 Retourne 1 si la chaîne passée désigne une
 commande interne (cd, exit, ...)
 */
int is_builtin(const char *cmd) {
    assert(cmd != NULL);
    char test[1024] = "type ";
    strcat(test, cmd);
    int saved_stdout = dup(STDOUT_FILENO);
    int saved_stderr = dup(STDERR_FILENO);

    int file_null = open("/dev/null", O_WRONLY);
    //Suppression de l'affichage de "Type ...."
    dup2(file_null, STDERR_FILENO);
    dup2(file_null, STDOUT_FILENO);
    close(file_null);

    int response = system(test);

    //Restauration de la sortie d'erreur & standard
    dup2(saved_stdout, STDOUT_FILENO);
    close(saved_stdout);
    dup2(saved_stderr, STDERR_FILENO);
    close(saved_stderr);

    if (response == 0) {
        return 1;
    } else {
        return 0;
    }
}

/*
  Exécute la commande interne
 
  proc : process_t dont le contenu a été initialisé
         au moment du parsing
 
  Retourne 0 si tout s'est bien passé, la valeur de
  retour de la commande ou -1 si la commande n'est
  pas reconnue.
 */

int builtin(process_t *proc) {
    assert(proc != NULL);

    return -1;
}

/*
  Change directory : change le répertoire de travail
  courant du minishell.

  path : le chemin vers lequel déplacer le CWD
  fderr : le descripteur de la sortie d'erreur pour
          affichage éventuel (erreur du cd)
  
  Retourne le code de retour de l'appel système.
 */

int cd(const char *path, int fderr) {
    assert(path != NULL);
    // Sauvegarde de la sortie d'erreur actuelle
    int saved_stderr = dup(STDERR_FILENO);

    //Redirection de la sortie d'erreur actuelle vers fderr
    dup2(fderr, STDERR_FILENO);
    close(fderr);

    //Changement du répertoire de travail courant du minishell
    int response = chdir(path);
    if(response == -1){
        fprintf(stderr, "Erreur cd %s\n", path);
    }
    //Restauration de la sortie d'erreur
    dup2(saved_stderr, STDERR_FILENO);
    close(saved_stderr);
    return response;
}

/*
  Ajout/modification d'une variable d'environnement.
 
  var : nom de la variable
  value : valeur à lui donner
  fderr : le descripteur de la sortie d'erreur pour
          affichage éventuel
 
  Retourne le code de retour de l'appel système.
 */

int export(const char *var, const char *value, int fderr) {
    assert(var != NULL);
    assert(value != NULL);

    // Sauvegarde de la sortie d'erreur actuelle
    int saved_stderr = dup(STDERR_FILENO);

    //Redirection de la sortie d'erreur actuelle vers fderr
    dup2(fderr, STDERR_FILENO);
    close(fderr);

    int response = setenv(var, value, 0);

    //Restauration de la sortie d'erreur
    dup2(saved_stderr, STDERR_FILENO);
    close(saved_stderr);

    return response;
}

/*
  Quitter le minishell
 
  ret : code de retour du minishell
  fdout : descripteur de la sortie standard pour
          l'affichage éventuel d'un message de sortie
 
  Retourne un code d'erreur en cas d'échec.
 */

int exit_shell(int ret, int fdout) {
    //Redirection de la sortie standard actuelle vers fderr
    dup2(fdout, STDOUT_FILENO);
    close(fdout);
    exit(ret);
    return EXIT_FAILURE;
}
