/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qumiraud <qumiraud@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/24 13:26:36 by qumiraud          #+#    #+#             */
/*   Updated: 2025/07/24 15:37:22 by qumiraud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//*	Picoshell:

//*	pipe,fork,dup2,execvp,close,wait,exit

//*	Ecrivez la fonction suivante :
//*	"int picoshell(char **cmds[]);"
//*	L'objectif de cet exercice est d'exécuter une pipeline. La fonction doit exécuter chaque commande du tableau "cmds"
//*	 et connecter la sortie de chaque commande avec l'entrée de la suivante, comme un SHELL.
//*	"cmds" est un tableau avec une terminaison null, et des commandes valides.
//*	Chaque commande peut être directement appelé dans execvp. La première chaîne de caractère de chaque commande est le nom de la commande
//*	 et peut être directement donné en premier argument de execvp.
//*	Si erreur il y a, la fonction doit retourner 1, il faut fermer tout les descripteurs de fichier,
//*	 et attendre les processus enfant avant de retourner 0.
//*	La fonction doit être en mesure de gérer une centaine de "|" même si il y a moins de 30 descripteurs de fichier disponible.
//*	Vous disposez d'un main à disposition afin de vous aider à tester votre fonction.

//*	Examples:
//*	./picoshell /bin/ls "|" /usr/bin/grep picoshell
//*	picoshell
//*	./picoshell echo 'squalala' "|" cat "|" sed 's/a/b/g'
//*	squblblb/

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

int	picoshell(char **args[]);

static int count_cmds(int argc, char **argv)
{
    int count = 1;
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "|") == 0)
            count++;
    }
    return count;
}

int main(int argc, char **argv)
{
    if (argc < 2)
        return (fprintf(stderr, "Usage: %s cmd1 [args] | cmd2 [args] ...\n", argv[0]), 1);

    int cmd_count = count_cmds(argc, argv);
    char ***cmds = calloc(cmd_count + 1, sizeof(char **));
    if (!cmds)
        return (perror("calloc"), 1);

    // Parsear argumentos y construir array de comandos
    int i = 1, j = 0;

    while (i < argc)
    {
        int len = 0;
        while (i + len < argc && strcmp(argv[i + len], "|") != 0)
            len++;

        cmds[j] = calloc(len + 1, sizeof(char *));
        if (!cmds[j])
            return (perror("calloc"), 1);

        for (int k = 0; k < len; k++)
            cmds[j][k] = argv[i + k];
        cmds[j][len] = NULL;

        i += len + 1;  // Saltar el "|"
        j++;
    }
    cmds[cmd_count] = NULL;

    int ret = picoshell(cmds);

    // Limpiar memoria
    for (int i = 0; cmds[i]; i++)
        free(cmds[i]);
    free(cmds);

    return ret;
}