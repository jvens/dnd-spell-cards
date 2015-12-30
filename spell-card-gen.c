#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <sys/wait.h>

int main(int argc, char** argv)
{
	DIR *in_dir, *out_dir;
	struct dirent *file;
	FILE *fp;
	char buffer[1024];
	char *title;
	char file_name[1024];
	char* fn;
	int in_length, out_length;
	char out_name[1024];
	char* on;

	char *ptr;
	pid_t pid;

	int count = 0;

	char *params[10];
	int pcount = 0;
	memset(&params, 0, sizeof(params));
	params[pcount++] = "pandoc";
	params[pcount++] = "-s";
	params[pcount++] = "--template=spell-card.tex";
	params[pcount++] = "--latex-engine=xelatex";
	params[pcount++] = "-o";

	if(argc < 3)
	{
		printf("usage: %s <in directory> <out directory>\n",argv[0]);
		return 1;
	}

	in_dir = opendir(argv[1]);
	if(in_dir == NULL)
	{
		printf("Invalid input directory name\n");
		return -1;
	}
	
	in_length = strlen(argv[1]);
	strcpy(file_name, argv[1]);
	if(file_name[in_length-1] != '/')
		file_name[in_length++] = '/';

	fn = file_name + in_length;

	out_length = strlen(argv[2]);
	strcpy(out_name, argv[2]);
	if(out_name[out_length-1] != '/')
		out_name[out_length++] = '/';

	on = out_name + out_length;

	while(file = readdir(in_dir))
	{
		if(strstr(file->d_name, ".markdown") == NULL && strstr(file->d_name, ".md") == NULL)
			continue;
		strcpy(fn, file->d_name);
		//printf("%s\t", file_name); fflush(stdout);
		fp = fopen(file_name, "r");
		title = NULL;
		while(fgets(buffer, 1024, fp))
		{
			if(strstr(buffer, "title:") != NULL)
			{
				strtok(buffer, "\"");
				title = strtok(NULL, "\"");
				break;
			}
		}
		fclose(fp);

		if(title == NULL)
		{	
			printf("Failed to parse file %s\n", file_name);
			continue;
		}
		for(ptr=title; *ptr; ptr++)
		{
			*ptr = *ptr == ' ' ? '-' : *ptr;
			*ptr = *ptr == '\'' ? '-' : *ptr;
			*ptr = *ptr == '/' ? '-' : *ptr;
		}
		strcpy(on, title);
		strcat(on, ".pdf");
		//printf("output: %s\n", out_name);
		params[pcount] = out_name;
		params[pcount+1] = file_name;
		if(pid = fork() == 0)
		{
			execvp(params[0], params);
			perror("execvp");
			return -1;
		}
		else
		{
			count ++;

			int status;
			//printf("Creating spell card %s\n", title);

			if(count >= 8)
			{
				wait(&status);
				count --;
			}
			/*waitpid(pid, &status, 0);
			if(status == 0)
				printf(": OK\n");
			else
				printf(": FAILED\n");
			return 0;*/
		}
	}

	(void) closedir(in_dir);

	return 0;
}
