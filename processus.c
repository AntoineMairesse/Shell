/*
  Projet minishell - Licence 3 Info - PSI 2021
 
  Nom : Mairesse
  Prénom : Antoine
  Num. étudiant : m21912663
  Groupe de projet : 10
  Date :
 
  Gestion des processus (implémentation).
 
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <unistd.h>
#include <sys/wait.h>

#include "processus.h"
#include "builtin.h"

#ifndef NDEBUG

int check_zero(void *ptr, size_t size) {
    int result = 0;
    for (size_t i = 0; i < size; ++i) {
        result += *((char *) ptr++);
    }
    return result;
}

#endif

/*
  Initialiser une structure process_t avec les
  valeurs par défaut.
 
  proc : un pointeur sur la structure à initialiser
 
  Retourne 0 ou un code d'erreur.
 */
int init_process(process_t *proc) {
    assert(proc != NULL);
    assert(check_zero(proc, sizeof(*proc)) == 0);
    proc->argv = (char **) malloc(sizeof(char **));
    proc->path = (char *) malloc(sizeof(char) * 1024);
    proc->bg = 0;
    proc->pipe = 0;
    proc->stdin = STDIN_FILENO;
    proc->stderr = STDERR_FILENO;
    proc->stdout = STDOUT_FILENO;
    proc->pid = getpid();
}

/*
  Remplacer les noms de variables d'environnement
  par leurs valeurs.
 
  proc : un pointeur sur la structure à modifier.
 
  Retourne 0 ou un code d'erreur.
 */
int set_env(process_t *proc) {
    assert(proc != NULL);

}

/*
  Lancer la commande en fonction des attributs de
  la structure et initialiser les champs manquants.
 
  On crée un nouveau processus, on détourne
  éventuellement les entrée/sorties.
  On conserve le PID dans la structure du processus
  appelant (le minishell).
  On attend si la commande est lancée en "avant-plan"
  et on initialise le code de retour.
  On rend la main immédiatement sinon.
 
  proc : un pointeur sur la structure contenant les
         informations pour l'exécution de la commande.
 
  Retourne 0 ou un code d'erreur.
  
 */
int launch_cmd(process_t *proc) {
    assert(proc != NULL);
    int i = 0, j = 0;
    while (proc[i].argv != NULL) {
        if(strcmp(proc[i].argv[0], "cd") == 0){
            cd(proc[i].argv[1], proc[i].stderr);
        }
        else if (strcmp(proc[i].argv[0], "exit") == 0){
            exit_shell(0, proc[i].stdout);
        }
        else if (proc[i].pipe == 1) {
            int   p[2];
            pid_t pid;
            int   fd_in = 0;
            while(proc[i].argv != NULL){
                if(proc[i - 1].pipe == 0 && i > 0){
                    break;
                }
                pipe(p);
                pid = fork();
                if (pid == 0)
                {
                    dup2(fd_in, 0); //change the input according to the old one
                    if (proc[i + 1].argv != NULL)
                        dup2(p[1], 1);
                    close(p[0]);
                    execvp(proc[i].argv[0], proc[i].argv);
                    exit(EXIT_FAILURE);
                }
                else
                {
                    wait(NULL);
                    close(p[1]);
                    fd_in = p[0]; //save the input for the next command
                    i++;
                }
            }
        } else {
            builtin(proc + i);
        }
        i++;
    }

}
