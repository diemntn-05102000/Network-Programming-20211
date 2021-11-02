#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>


#define PORT 5500
#define MAXLINE 1024
#define SUCCESSFULL 1
#define ERROR 0

#define SIZE 50
#define NUM_USER 50
typedef struct UserManagement
{
	char username[SIZE];
	char password[SIZE];
	int status;
	int timeErrorActCode;
	int timeErrorPassword;
}UM;
struct node_UM
{
  UM user;
  struct node_UM *next;
 };
typedef struct node_UM node;
node *root,*cur;
int numberUser=0;
node *userLoged = NULL;
node *makeNewNode(UM element)
{
  node *new = (node*)malloc(sizeof(node));
  new->user=element;
  new -> next =NULL;
  return new;
}
void insertAtHead(UM element)
{
  node *new = makeNewNode(element);
   if( root == NULL)
    {
      root = new;
      cur = root;
    }else{
		new ->next = root;
		root = new;
		cur = root;
   }
}

node *importFromText(FILE *fptr1)
{
  UM user[NUM_USER];

	while(!feof(fptr1))
	  {
		fscanf(fptr1,"%s %s %d\n",user[numberUser].username,user[numberUser].password,&user[numberUser].status);
		user[numberUser].timeErrorActCode = 0;
		user[numberUser].timeErrorPassword = 0;
		insertAtHead(user[numberUser]);
		numberUser++;
	}
	
	return root;
}
void insertAfCur(UM element)
{
  node *new = makeNewNode(element);
 
  if( root == NULL)
    {
      root = new;
      cur = root;
    }else{
    new ->next = cur -> next;
    cur->next = new;
    cur = cur->next;  
  }
}
void displayNode(node *p)
{
  if(p==NULL)
    {
      printf("Loi con tro\n");
      return;
    }
  UM user= p->user;
  printf("%-25s  %-20s  %d\n",user.username, user.password, user.status);
}
void DisplayList()
{
  node *p;
  if(root == NULL) printf("NULL\n");
  for(p=root; p!= NULL; p=p->next)
    displayNode(p);
}
void freeList()
{
  node *to_free = root;
  while(to_free != NULL)
    {
      root = root->next;
      free(to_free);
      to_free = root;
    }
}
void writeFile()
{
	FILE *fptr = fopen("nguoidung.txt", "w");
	node *head = root;
	if (fptr==NULL)
	{
		printf("File open error\n");
	}else{
		
		while (head != NULL)
		{
			UM tmp= head->user;
			fwrite(tmp.username, 1, strlen(tmp.username), fptr);
			fwrite(" ", 1, 1, fptr);

			fwrite(tmp.password, 1, strlen(tmp.password), fptr);
			fwrite(" ", 1, 1, fptr);

			char convertTest[5];
			sprintf(convertTest, "%d", tmp.status);
			fwrite(convertTest, 1, strlen(convertTest), fptr);
			
			
			if (head->next != NULL)
			{
				fwrite("\n", 1, 1, fptr);
			}
			head = head->next;
		}
		fclose(fptr);
	}
}



//socket
int checkingStatus = 0;
char passwordDigit[MAXLINE];
char passwordAlpha[MAXLINE];

struct sockaddr_in servaddr, cliaddr;
int sockfd;
socklen_t len = sizeof(cliaddr);
int firstCheckInput = 0;

node *findAnAccount(char username[SIZE])
{
			node *p;
			UM tmp;
			int checkUsername = 0;
			for(p=root; p!= NULL; p=p->next){
				tmp = p->user;
				if (strcmp(username,tmp.username)==0){
					checkUsername = 1;
					break;
				}
			}	
			if(checkUsername == 0){
				return NULL;
			}else{
				return p;
			}
}


int encodePassword(char *password)
{

	int index1 = 0, index2 = 0;
	memset(passwordDigit, 0, sizeof(passwordDigit));
	memset(passwordAlpha, 0, sizeof(passwordAlpha));

	for (int i = 0; i < strlen(password); i++)
	{
		// Check digit case
		if (isdigit(password[i]))
			passwordDigit[index1++] = password[i];
		else if ((password[i] >= 'a' && password[i] <= 'z') || (password[i] >= 'A' && password[i] <= 'Z'))
			passwordAlpha[index2++] = password[i];
		else
		{
			// Wrong case
			return 0;
		}
	}
	passwordDigit[index1] = '\0';
	passwordAlpha[index2] = '\0';

	return 1;
}

int changePassword(char *newPassword)
{

	if (!encodePassword(newPassword))
	{
		return ERROR;
	}
	strcpy(userLoged->user.password, newPassword);
	writeFile();
	return SUCCESSFULL;
}

void actionAfterChangePassword(char *newPassword)
{
	int changePasswordStatus = changePassword(newPassword);

	// In case have special character => out => send error to client
	if (changePasswordStatus == ERROR)
	{
		sendto(sockfd, (const char *)"Fail", strlen("Fail"),
			   MSG_CONFIRM, (const struct sockaddr *)&cliaddr,
			   len);
	}
	else
	{

		// Send message
		sendto(sockfd, (const char *)"SuccessFull", strlen("SuccessFull"),
			   MSG_CONFIRM, (const struct sockaddr *)&cliaddr,
			   len);


		sendto(sockfd, (const char *)passwordDigit, strlen(passwordDigit),
			   MSG_CONFIRM, (const struct sockaddr *)&cliaddr,
			   len);
		sendto(sockfd, (const char *)passwordAlpha, strlen(passwordAlpha),
			   MSG_CONFIRM, (const struct sockaddr *)&cliaddr,
			   len);
	}
}
int loginAccount(char *userName, char *password)
{

	node *user = findAnAccount(userName);
	if (user == NULL)
	{
		return 5;
	}

	if (user->user.status == 3)
	{
		return 3;
	}

	if (user->user.timeErrorPassword == 3)
	{
		user->user.status = 3;
		writeFile();
		return 3;
	}

	if (strcmp(user->user.password, password) == 0)
	{
		if (user->user.status == 3)
		{
			return 3;
		}else{
			if (user->user.status == 2)
			{
				return 2;
			}else{
				if (user->user.status == 1)
				{
					user->user.timeErrorPassword = 0;
					userLoged = user;
					return 1;
				}else{
					return 0;
				}


			}
		}
	}
	user->user.timeErrorPassword++;
	return 4;//wrong password
}
char *convertValue(long status)
{
	switch (status)
	{
	case 1://active
		return "1";
		break;

	case 0://not ready
		return "0";
		break;

	case 3://blocked
		return "3";
		break;

	case 4://wrong password
		return "4";
		break;

	default:
		return "5";//not find
		break;
	}
}


int main(int argc, char *argv[])
{

	if (argc != 2)
	{
		printf("Invalid argument. The model using is: ./server {{PORT}}\n");
		exit(0);
	}
	int port = atoi(argv[1]);

	char username[MAXLINE];
	char password[MAXLINE];

	char *wellcome = "Connected!!! Please enter username and password..\n";
	char *insertPassword = "Insert password";
	

	FILE *fptr;
	fptr=fopen("nguoidung.txt","r");
	if (fptr==NULL)
	{
		printf("File open error\n");
		return 0;
	}else{
		importFromText(fptr);
	}
	
	printf("Server started and listen on PORT: %d\n", port);
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}

	memset(&servaddr, 0, sizeof(servaddr));
	memset(&cliaddr, 0, sizeof(cliaddr));

	// Filling server information
	servaddr.sin_family = AF_INET; // IPv4
	servaddr.sin_addr.s_addr = INADDR_ANY;
	servaddr.sin_port = htons(PORT);

	// Bind the socket with the server address
	if (bind(sockfd, (const struct sockaddr *)&servaddr,
			 sizeof(servaddr)) < 0)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	char firstHandle[MAXLINE];
	recvfrom(sockfd, (char *)firstHandle, MAXLINE,
			 MSG_WAITALL, (struct sockaddr *)&cliaddr,
			 &len);

	sendto(sockfd, (const char *)wellcome, strlen(wellcome),
		   MSG_CONFIRM, (const struct sockaddr *)&cliaddr,
		   len);

	while (1)
	{
		// Reset string receive
		memset(username, 0, sizeof(username));
		// Recv username
		recvfrom(sockfd, (char *)username, MAXLINE,
				 MSG_WAITALL, (struct sockaddr *)&cliaddr,
				 &len);

		if (strcmp(username, "") == 0)
		{
			printf("Turn off server...\n");
			exit(0);
		}

		sendto(sockfd, (const char *)insertPassword, strlen(insertPassword),
			   MSG_CONFIRM, (const struct sockaddr *)&cliaddr,
			   len);

		memset(password, 0, sizeof(password));

		recvfrom(sockfd, (char *)password, MAXLINE,
				 MSG_WAITALL, (struct sockaddr *)&cliaddr,
				 &len);

		int i_valueAfterLogin = loginAccount(username, password);
		// Check account after submit username and password
		char *test2 ;
		
		test2 = convertValue(i_valueAfterLogin);
		sendto(sockfd, (const char *)test2, strlen(test2),
			   MSG_CONFIRM, (const struct sockaddr *)&cliaddr,
			   len);

		char *activeStatus = "1";
		if (strcmp(test2, activeStatus) == 0)
		{
			char passwordWantChange[MAXLINE];
			memset(passwordWantChange, 0, sizeof(passwordWantChange));
			// 1st time
			recvfrom(sockfd, (char *)passwordWantChange, MAXLINE,
					 MSG_WAITALL, (struct sockaddr *)&cliaddr,
					 &len);

			if (strcmp(passwordWantChange, "") == 0)
			{
				exit(0);
			}
			while (strcmp(passwordWantChange, "bye") != 0)
			{
				if (strcmp(passwordWantChange, "") == 0)
				{
					
					exit(0);
				}
				actionAfterChangePassword(passwordWantChange);
				// Reset string and listen for next action
				memset(passwordWantChange, 0, sizeof(passwordWantChange));
				recvfrom(sockfd, (char *)passwordWantChange, MAXLINE,
						 MSG_WAITALL, (struct sockaddr *)&cliaddr,
						 &len);
			}

			if (strcmp(passwordWantChange, "bye") == 0)
			{
				userLoged = NULL;
				exit(0);
			}
		}
	}
	fclose(fptr);
	freeList();
	return 0;
}