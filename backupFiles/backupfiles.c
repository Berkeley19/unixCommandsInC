#define _XOPEN_SOURCE
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
void dirAttributes(const char *d, const char *time);


//function which checks if modified date of file is greater than given date
//implements the long list format deatils of a specific file/directory
void longListFormat(const char *buffer, const char *name, const char *time){
	//initialise structures to gain information on files
	struct stat sbListing;
	stat(buffer, &sbListing);
	struct passwd *pwd;
	struct group *grp;
	//initialise two separate time structures
	struct tm timeInput = {0};
	struct tm currentFileTime = {0};
	char currentFile[100];
	//initialise future times to 0
	time_t inputResult = 0;
	time_t currentFileResult = 0;
	//get the modified time of this file
	strftime(currentFile, 100, "%Y-%m-%d %H:%M:%S",
		localtime(&(sbListing.st_mtime)));
	//place both times into their results
	strptime(time, "%Y-%m-%d %H:%M:%S", &timeInput);
	strptime(currentFile, "%Y-%m-%d %H:%M:%S", &currentFileTime);
	//check if the conversion can occur
	//if not then the format of the date is wrong
	//if it can then the correct date format was given
	if((mktime(&timeInput) == (time_t)-1) || 
			(mktime(&currentFileTime) == (time_t)-1)){
		puts("WRONG DATE FORMAT ENTERED, PLEASE RE-TRY");
 		exit(1);
	}else{
		//calculate if the currentfile date is larger than the input
		inputResult = mktime(&timeInput);
		currentFileResult = mktime(&currentFileTime);
		double seconds = difftime(currentFileResult, inputResult);
		if(seconds > 0){
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

			//using official description of stftime(), 
			//worked out the ls -l format for dates and time
			char date[20];
			strftime(date, 20, "%b %d %H:%M", localtime(&(sbListing.st_ctime)));
			printf(" %s", date);
			//print the name of currently searched file/directory
			printf(" %s", name);
			printf("\n");
		}
	}
}

//open and read entered directory
void dirAttributes(const char *d, const char *time){
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
			longListFormat(buffer, de->d_name, time);
			//recursively call function to repeat the process for this directory
			dirAttributes(buffer, time);
		}
		else{
			//print file details in ls -l format
			longListFormat(buffer, de->d_name, time);
		}

	}
	//close the files
	closedir(dp);
}

int main(int argc, char *argv[]){
	int c;
	char *tfile;
	char time[100];
	char str[100] = "Enter the filename or time format after -t, add wanted dir at the end";
	struct stat temp;
	//check for command line switches
	while((c = getopt(argc, argv, ":t:h:")) != -1){
		//for each case it will check for t and h no matter the order
		switch(c){
			case 't':
				//takes argument of -t to get date
				tfile = optarg;
				break;
			case 'h':
				//prints a helper for user
				puts(str);
				break;
			case '?':
				//if unrecognised 
				printf("Unrecognized option: -%c\n", optopt);
				break;
		}
	}
	//connect the given date format by -t to the stat structure
	stat(tfile, &temp);
	//if it is a file then get it's modification date
	if(S_ISREG(temp.st_mode) != 0){
		if(&temp.st_mtime == NULL){
			snprintf(time, 100, "%s", "1970-01-01 00:00:00");
		}else{
			strftime(time, 100, "%Y-%m-%d %H:%M:%S", localtime(&(temp.st_mtime)));
		}
	}else{
		//puts(tfile);
		snprintf(time, 100, "%s", tfile);
		//puts(time);
	}
	//puts(time)
	//call the heavy function
	dirAttributes(argv[argc-1], time);
	return 0;
}
