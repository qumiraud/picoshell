/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   picoshell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qumiraud <qumiraud@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/24 13:26:33 by qumiraud          #+#    #+#             */
/*   Updated: 2025/07/25 10:38:57 by qumiraud         ###   ########.fr       */
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
				close(prevfd);
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
	// printf("status: %d\n", status); //Pour verifier avec les tests de la valeur de retour
	return (status);
}





int	picoshell(char **args[])
{
	int		i;
	int		status;
	int		pipefd[2];
	int		prevfd;
	pid_t	pid;

	i = 0;
	status = 0;
	prevfd = -1;
	//Verification qu'il y est au moins un argument
	if (args[i][0] == NULL)
		return (0);
	while (args[i])
	{
		//Création d'un nouveau pipe a chaque nouvelle commande
		if (pipe(pipefd) == -1)
		{
			return (1);
		}
		//Creation du processus enfant
		pid = fork();
		if (pid == -1)
			return (1);
		//Child
		else if (pid == 0)
		{
			//si il y a deja eu une commande
			if (prevfd != -1)
			{
				dup2(prevfd, STDIN_FILENO);
				close (prevfd);
			}
			//si il y a des commandes suivantes (pour ceux qui viendront après)
			if (args[i + 1])
			{
				close(pipefd[0]);
				dup2(pipefd[1], STDOUT_FILENO);
				close(pipefd[1]);
			}
			//sinon on ne redirige pas la sortie et on ferme bien les fd du pipe
			else
			{
				close(pipefd[0]);
				close(pipefd[1]);
			}
			//execution de la commande
			if (execvp(args[i][0], args[i]) == -1)
			{
				exit (1);
			}
		}
		//Parent
		else
		{
			//si prevfd a été utilisé on le ferme
			if (prevfd != -1)
				close(prevfd);
			//si il y a des commandes aprés, on attribue à prevfd la valeur du fd de pipefd[0]
			if (args[i + 1])
			{
				close(pipefd[1]);
				prevfd = pipefd[0];
			}
			//sinon on ferme tout
			else
			{
				close(pipefd[0]);
				close (pipefd[1]);
			}
			//on passe a la prochaine commande si il y en a une
			i++;
		}
	}
	//on attend le retour des enfants
	while(wait(&status) > 0)
		;
	//si probleme il y a eu on retourne 1
	if (status != 0)
		return (1);
	//sinon on retourne 0
	return (0);
}