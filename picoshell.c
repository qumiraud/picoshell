/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   picoshell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qumiraud <qumiraud@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/24 13:26:33 by qumiraud          #+#    #+#             */
/*   Updated: 2025/07/24 16:53:18 by qumiraud         ###   ########.fr       */
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

int	picoshell(char **args[])
{
	int		i;
	int		status;
	int		pipefd[2];
	int		prevfd;
	pid_t	pid;

	i = 0;
	prevfd = -1;
	status = 0;
	if (args[i][0] == NULL)
		return (0);
	while (args[i])
	{
		if (pipe(pipefd) == -1)
			return (1);
		pid = fork();
		if (pid == -1)
			return (1);
		else if (pid == 0)
		{
			if (prevfd != -1)
			{
				dup2(prevfd, STDIN_FILENO);
				close(prevfd);
			}
			if (args[i + 1])
			{
				close(pipefd[0]);
				dup2(pipefd[1], STDOUT_FILENO);
				close(pipefd[1]);
			}
			else
			{
				close(pipefd[0]);
				close(pipefd[1]);
			}
			if (execvp(args[i][0], args[i]) == -1)
			{
				exit (1);
			}
		}
		else
		{
			if (prevfd != -1)
			{
				close(prevfd);
			}
			if (args[i + 1])
			{
				close(pipefd[1]);
				prevfd = pipefd[0];
			}
			else
			{
				close(pipefd[0]);
				close(pipefd[1]);
			}
			i++;
		}
	}
	while (wait(&status) > 0)
		;
	if (status != 0)
		status = 1;
	printf("status: %d\n", status);
	return (status);
}