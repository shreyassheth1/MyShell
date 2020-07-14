#include<stdio.h> //standard input and output
#include<string>  //functions of string
#include <unistd.h> // declarations of unix constants and posix api and typedef
#include<ctype.h> //testing and mapping char
#include<vector>  //vector data structure
#include<string.h> 
#include<fstream> //file input and output
#include<iostream> // cpp input output
#include<stdlib.h>  
#include <fcntl.h>  //file control
#include  <sys/types.h>
#include <sys/wait.h>
using namespace std;
string historyDirectory;
string historytemp;
void gpwd(){
	//prints current working directory
	char buf[1000];
	if(getcwd(buf,sizeof(buf))!=NULL)
	//getcwd is a function written in unistd.h which gives current directory
		printf("%s",buf);
	else
		perror("Error in directory name");
}
void printHistory(){
	ifstream in (historyDirectory);
		int n =1;
		string s;
		if(in.is_open()){
			while(!in.eof()){
				getline(in,s);
				cout<<n<<". "<<s<<"\n";
				n++;
			}
			in.close();
		}
}
int ifDigit(char c){
	switch(c){
		case '1': return 1;
			break;
		case '2': return 1;
			break;
		case '3': return 1;
			break;
		case '4': return 1;
			break;
		case '5': return 1;
			break;
		case '6': return 1;
			break;
		case '7': return 1;
			break;
		case '8': return 1;
			break;
		case '9': return 1;
			break;
		case '-': return 1;
			break;
		case '+': return 1;
			break;
		case '0': return 1;
			break;
	}
	return -1;
}
void writeHistory(string cmd){
	//ifstream in (".psh_history");
	vector<string> v;
	int ch,n=0;
	string s;
	
	ifstream in (historyDirectory);
	if (in.is_open()){
		while (! in.eof() ){
			getline (in,s);
			//cout<<"\nRead: "<<s;
	
			v.push_back(s);
			n++;
		}

		in.close();
	}
	
	//printf("n is : %d",n);
	v.push_back(cmd);
	
	if(n>=15){//threshold
		//n is equal to threshold
		//cout<<"Printing";
		ofstream out(historytemp,std::fstream::out | std::fstream::app);

		if (out.is_open()){
			
			for(int i=1;i<n;i++){
			
				out<<v[i];
				out<<"\n";
		 	}	
		 	//Push Final Element
	 		out << v[n];
	 	
		 	out.close();
		}
		else{
			perror("\nFile not opened");
		}
	 	const char * his = historyDirectory.c_str();
	 	const char * temp = historytemp.c_str();
    		remove(his);
            	rename(temp,his);
	}
	else{
		//printf("\nAppending..");
		ofstream out(historyDirectory,std::fstream::out | std::fstream::app);

		if (out.is_open()){
			out << v[n];
			out << "\n";
			out.close();
		}
	}
	
}
int process(char arr[]){
	char ext[100];
	strcpy(ext,arr);
	if(strcmp(arr,"pwd")==0){
		gpwd();
		return 0;
	}
	if(strcmp(arr,"history")==0){
		printHistory();
		return 0;
	}
	if(strstr(arr,"=")!=NULL){
		// contains env variable
		if((strstr(arr,"= "))!=NULL||(strstr(arr," ="))!=NULL){
			char *start = strtok(arr," ");
			printf("%s\n command not found",start);
			return 0;
		}
		char *name = strtok(arr,"=");
		char *val = strtok(arr,"=");
		int ret = setenv(name,val,1);
		if(ret==-1)
			perror("error: ");
		return 0;
	}
	char *start =  strtok(arr," ");

	if(strcmp(start,"cd")==0){
		char * path=strtok(NULL," ");
		int ret=chdir(path);
		if(ret==-1)
			perror("error ");
		return 0;
	}
	if(strcmp(start,"echo")==0){
		char *name = strtok(NULL," ");
		if(name[0]=='$'){// if env variable
			name = name+1;
			char *val = getenv(name);
			printf("%s\n",val);
		}else{
			printf("\n");
			while(name!=NULL){
				printf("%s ",name);
				name = strtok(NULL," ");
			}
		}
		return 0;
	}
	if(strcmp(start,"history")==0){
		printHistory();
		return 0;
	}
	if(arr[0]=='!'){
		 //shebang detected;
		ifstream in (historyDirectory);
		vector<string> v;
		int ch,n=0;

		string s;
		if(in.is_open()){
			while(!in.eof()){
				getline(in,s);
				//cout<<s;
				v.push_back(s);
				n++;
			}
			in.close();
		}
		if(arr[1]=='!'){ // last command to be executed again
			string cmd = v[n-1];
			char next[100];
			strcpy(next,cmd.c_str());
			writeHistory(next);
			if(strcmp(next,"exit")==0)
				return -1;
			process(next); //recursive call to process to be executed again
			return 0;
		}
		int digit=ifDigit(arr[1]);
		if(digit==1){
			arr=arr+1;
			int num;
			sscanf(arr,"%d",&num);
			if((num>n)||(num+n<0)||num==0){
				printf("%d:Event not found \n",num);
				return 0;
			}
			if(num<0){
				char next[100];
				string cmd = v[n+num];
				strcpy(next,cmd.c_str());
				cout<<next;
				if(strcmp(next,"exit")==0)
					return -1;
				process(next);
				return 0;
			}
			if(num>0){
				char next[100];
				string cmd= v[num];
				strcpy(next,cmd.c_str());
				cout<<next;
				if(strcmp(next,"exit")==0)
					return -1;
				process(next);
				return 0;
			}
		}else{//string
			arr = arr+1;
			int len = strlen(arr);
			for(int j=n-1;j>0;j--){
				string cmd = v[j];
				char next[100];
				strcpy(next,cmd.c_str());
				int len1 = strlen(next);
				if(len1<len){
					//do nothing
				}else if(len1==len){
					if(strcmp(next,arr)==0){
						if(strcmp(next,"exit")==0)
							return -1;
						process(next);
						return 0;
					}
				}else{
					int sflag = 0;
					//check for substring
					for(int k=0;k<len;k++){
						if(next[k]!=arr[k]){
							sflag = 1;
							break;
						}
					}
					if(sflag==0){
						if(strcmp(next,"exit")==0)
							return -1;
						process(next);
						return 0;
					}
				}
			}
			printf("%s Event Not Found",arr);
					return 0; 
		}
	}
	//execute the command
	char *args[60];
	char **next = args;
	char * temp = strtok(ext," ");
	while(temp != NULL){
		*next++ = temp;
		//printf("%s\n", );
		temp = strtok(NULL," ");
	}
	*next = NULL;
	pid_t pid;
	int s;
	if((pid = fork())<0){
		printf("error in forking\n");
	}else if(pid==0){
		if(execvp(args[0],args)<0){
			perror("error in execution");
		}
		exit(0);
	}else{
		
		while(wait(&s)!=pid);
	}
	return 0;
}

int startprocess(char arr[]){
	int fInput=0,fOut=0,fError=0,append=0;
	char sys_in,sys_out,sys_err;
	char cpy[100];
	char *inputFile,*outputFile,*errorFile;
	strcpy(cpy,arr);

	if(strstr(arr,"<")!=NULL){
		//input redirection
		fInput = 1;
		char *next = strtok(cpy," ");
		while(next!=NULL){
			//till cmd;
			if(next[0]=='<'){
				int i= 1;
				while(next[i]==' '){
					i++;
				}
				char *pr = next+1+i;
				//cout<<pr;
				inputFile = strtok(pr," ");
				//cout<<"\ninput file is :"<<inputFile;
				break;
			}
			next = strtok(NULL," ");
		}
		strcpy(cpy,arr);//for again doing it
	}
	if (strstr(arr,">")!=NULL){
		//output redirection
		fOut =1;
		char *next = strtok(cpy," ");
		while(next!=NULL){
			//output redirection
			if(next[0]=='>'){
				int i=1;
	     		while(next[i]==' '){
	     			i++;
	     		}
	     		char *pr=next+i+1;
		     	outputFile = strtok(pr," ");
		     	cout<<outputFile;
				break;
			}
			next = strtok(NULL," ");
		}
		strcpy(cpy,arr);
	}
	if(strstr(arr,"2>")!=NULL){
		fError=1;
		char * next = strtok(cpy," ");
		while(next!=NULL){
			if(next[0]=='2'&&next[1]=='>'){
				int i=2;
	     		while(next[i]==' '){
	     			i++;
	     		}
	     		char *pr=next+1+i;
	     		errorFile = strtok(pr," ");
	     		cout<<errorFile;
	     		break;
			}
			next = strtok(NULL," ");
		}
		strcpy(cpy,arr);
	}
	char cmd[100];
	char *tok = strtok(cpy," ");
	while(tok!=NULL){
		if((strstr(tok,">")==NULL)&&(strstr(tok,"<")==NULL)&&(strstr(tok,"2>")==NULL)){
			strcat(cmd,tok);
			strcat(cmd," ");
		}
		tok = strtok(NULL," ");
	}
	//cout<<"cmd"<<cmd;
	int fdIn,fdOut,fdError;
	if(fInput==1){
		fdIn = open(inputFile,O_RDONLY);
		if(fdIn<0){
			perror("cant open file");
		}else{
			sys_in = dup(0);
			close(0);
			dup2(fdIn,0);
		}
	}
	if(fOut){
		// if(append==1)
		// 	fdOut = open(outputFile, O_WRONLY | O_APPEND | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
		// else
		fdOut = open(outputFile, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
		sys_out=dup(1);
		close(1);
		dup2(fdOut,1);
	}
	if(fError==1){
		fdError = open(errorFile, O_WRONLY | O_APPEND | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
		sys_err=dup(2);
		close(2);
		dup2(fdError,2);
	}
	int ret = process(cmd);

	if(ret==-1){
		printf("breaking\n");
		return -1;
	}
	//cout<<arr;
	if(fInput==1){
		close(fdIn);
		dup2(sys_in,0);
	}
	if(fOut==1){
		close(fdOut);
		dup2(sys_out,1);
	}
	if(fError==1){
		close(fdError);
		dup2(sys_err,2);
	}
	return 0;
}
int main(int argc, char const *argv[])
{
	char arr[50];
	char *p;
	char historyDir[1000];
	if(getcwd(historyDir,sizeof(historyDir))!=NULL){
		historytemp = historyDirectory + ".temp" ;
		historyDirectory = historyDirectory + ".psh_history";
	}
	while(true){
		cout<<"\npsh";
		gpwd(); // to print current working directory
		printf(":) ");
		//cout<<"here";
		fgets(arr,sizeof(arr),stdin); // input string into array from file stdin
		char arr1[50];
		memset(arr1,0,sizeof(arr1));
		int i=0,j=0;
		for(i=0;isspace(arr[i]);++i);//remove leading spaces means first spaces;
		//cout<<i;
		if(strcmp(arr+i,"")==0){
			//NULL CMD
			continue;//execute while again
		}
		for( j=0;arr[i]!='\n';i++,j++){ //changes command into string
			arr1[j]=arr[i];
		}
		string cmd(arr1,arr1+j);
	
		//cout<<cmd;
		// strchr checks first occurence of '\n' and replaces it with '\0'
		 
		//if(strstr(arr,"!")==NULL)//strstr(haystack,needle)
		writeHistory(cmd); //write into history if not null
		if(cmd=="exit"){
			printf("bye with love\n");
			exit(0);
		}
		if(strstr(arr1,"|")!=NULL){
			char cpy[50],cpy1[50];
			strcpy(cpy,arr1);
			strcpy(cpy1,arr1);

			int pipeCount = 0;
			int len = strlen(arr1);
			int v;
			for(v=0;v<len;v++){
				if(arr1[v]=='|'){
					pipeCount = 1+ pipeCount;
				}
			}
			pipeCount++;
			char *saved;
			char *next = strtok_r(arr1,"|",&saved);
			pid_t pid;
			int in,fd[2],u;
			int cur_in = dup(0);
			int cur_out = dup(1);
			in = cur_in;

			for(u=0;u<pipeCount-1;u++){
				char *args[60];
				char **ext = args;
				char copy[100];
				strcpy(copy,next);

				char * temp = strtok(next," ");
				while (temp!=NULL){
					//printf("%s\n",temp);
					*ext++ = temp;
					temp = strtok(NULL," ");
					//printf("%s\n",temp);
				}
				*ext = NULL;
				pipe(fd);
				if(fork()==0){
					dup2(fd[1],1);
					execvp(args[0],args);
				}
				dup2(fd[0],0);
				close(fd[1]);
				next=strtok_r(NULL,"|",&saved);
			}
			//printf("here\n");
			char *args[60];
			char **ext = args;
			char copy[100];
			strcpy(copy,next);
			char *temp = strtok(next," ");
			while(temp != NULL){
				*ext++ = temp;
				printf("%s\n",temp);
				temp = strtok(NULL," ");
			}
			*ext = NULL;
			int s;
			wait(NULL);
			if( (pid = fork())==0){
				execvp(args[0],args);
				//cout<<"here11";
			}
			else{
				//cout<<"here";
				while(wait(&s)!=pid);
				//return 0;
			}
			close(fd[0]);
			close(fd[1]);
			dup2(cur_in,0);
			dup2(cur_out,1);
		}
		else{
			int ret=startprocess(arr1);
			if(ret==-1)
			break;
		}
	}
	return 0;
}