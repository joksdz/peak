#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>

#define FILESLIM  1024
#define FILENAMELIM  1024

int main(){

const char *path = "/usr/share/applications/" ;
DIR* d = opendir(path);
	struct dirent *dir;
	if(d==NULL){
		    fprintf(stderr, "Error opening dir: %s\n",path);
        return 69;
	}
 	 char files[FILESLIM][FILENAMELIM];
         u_int16_t index=0;

	while ((dir = readdir(d)) != NULL) {

		if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0 || strstr(dir->d_name ,".desktop") == NULL) {
            continue;
        }

		if(strcmp(strstr(dir->d_name ,".desktop" ),".desktop")==0) {
			strcpy(files[index],dir->d_name);
		index++;
		}

	}
	closedir(d);



	for(int i =0; i < index; i++){
printf("%s \n",files[i]);
	}
	return 0 ;
}
