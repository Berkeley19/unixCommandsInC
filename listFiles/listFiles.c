#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <grp.h>
#include <pwd.h>
#include <unistd.h>
#include <time.h>
//set size of future path 
#define BUFFER_SIZE (256)

//declaration of dirAttributes
void dirAttributes(const char *d);


//functions to implement the long list format deatils of a specific file/directory
void longListFormat(const char *buffer, const char *name){
	//initialise structures to gain 
	struct stat sbListing;
	stat(buffer, &sbListing);
	struct passwd *pwd;
	struct group *grp;
	//for permissions part, used code from STACKOVERFLOW 
	printf( (S_ISDIR(sbListing.st_mode)) ? "d" : "-");
    	printf( (sbListing.st_mode & S_IRUSR) ? "r" : "-");
    	printf( (sbListing.st_mode & S_IWUSR) ? "w" : "-");
    	printf( (sbListing.st_mode & S_IXUSR) ? "x" : "-");
    	printf( (sbListing.st_mode & S_IRGRP) ? "r" : "-");
   	printf( (sbListing.st_mode & S_IWGRP) ? "w" : "-");
    	printf( (sbListing.st_mode & S_IXGRP) ? "x" : "-");
    	printf( (sbListing.st_mode & S_IROTH) ? "r" : "-");
    	printf( (sbListing.st_mode & S_IWOTH) ? "w" : "-");
    	printf( (sbListing.st_mode & S_IXOTH) ? "x" : "-");
	//prints number of links of that file/directory
	printf(" %d", sbListing.st_nlink);
	//checks for potential username, if none then just uses numeric value
	if((pwd = getpwuid(sbListing.st_uid)) != NULL){
		printf(" %s", pwd->pw_name);
	}
	else{
		printf(" %d", sbListing.st_uid);
	}
	//checks for potential group, if none then the numerical version
	if ((grp = getgrgid(sbListing.st_gid)) != NULL){
    		printf(" %s", grp->gr_name);
	}
	else{
    		printf(" %d", sbListing.st_gid);
	}
	//the size of the file/directory
	//different spacing for larger numbers
	if(sbListing.st_size > 9999){
		printf(" %*d", 8, sbListing.st_size);
	}else{
		printf(" %*d", 4, sbListing.st_size);
	}

	//using official description of stftime(), worked out the ls -l format for dates and time
	char date[10];
	strftime(date, 20, "%b %d %H:%M", localtime(&(sbListing.st_ctime)));
	printf(" %s", date);
	//print the name of currently searched file/directory
	printf(" %s", name);
	printf("\n");
}

//open and read entered directory
void dirAttributes(const char *d){
	//opened directory
	DIR *dp = opendir(d);
	//this structure contains certain details of a file, most importanly the name
	struct dirent *de;
	//if directory can't be found, stop program
	if(dp == NULL){
		puts("Error! No available directory.");
		exit(1);
	}
	//each file/sub-directorires can be read if valid
	while((de=readdir(dp)) != NULL){
		//array of characters to hold new path
		char buffer[BUFFER_SIZE];
		//stat contains all the necessary attributes to a file
		struct stat sb;
		snprintf(buffer, BUFFER_SIZE, "%s/%s",d, de->d_name);
		stat(buffer, &sb);
		//if it is a directory continue, else print file name
		if(S_ISREG(sb.st_mode) == 0){
			//remove the unnecessary files
			if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0){
                		continue;
			}
			//call to list the ls -l format of this directory
			longListFormat(buffer, de->d_name);
			//recursively call function to repeat the process for this directory
			dirAttributes(buffer);
		}
		else{
			//print file details in ls -l format
			longListFormat(buffer, de->d_name);
		}

	}
	//close the files
	closedir(dp);
}

int main(void){
	dirAttributes(".");
	return 0;
}
