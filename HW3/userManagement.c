#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <netdb.h> 
#include <arpa/inet.h>
#include <regex.h>
#define SIZE 50
#define NUM_USER 50
typedef struct UserManagement
{
	char username[SIZE];
	char password[SIZE];
	int status;
	char homepage[SIZE];
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
		fscanf(fptr1,"%s %s %d %s\n",user[numberUser].username,user[numberUser].password,&user[numberUser].status,user[numberUser].homepage);
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
// check function
bool isValidIpAddress(char *ipAddress)
{
    struct sockaddr_in sa;
    int result = inet_pton(AF_INET, ipAddress, &(sa.sin_addr));
    return result != 0;
}
bool isValidDomain(char *domain)
{
   regex_t regex;
    int reti;
    reti = regcomp(&regex, "^(([a-zA-Z]{1})|([a-zA-Z]{1}[a-zA-Z]{1})|([a-zA-Z]{1}[0-9]{1})|([0-9]{1}[a-zA-Z]{1})|([a-zA-Z0-9][-_\\.a-zA-Z0-9]{1,61}[a-zA-Z0-9]))\\.([a-zA-Z]{2,13}|[a-zA-Z0-9-]{2,30}\\.[a-zA-Z]{2,3})$", REG_EXTENDED);
        if (reti) {
            printf("Could not compile regex\n");
            return false;
        }else{
            reti = regexec(&regex, domain, 0, NULL, 0);
            if (!reti) {
               return true;
            }
            else return false;
        }
        

   
}
//function
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
			fwrite(" ", 1, 1, fptr);

			fwrite(tmp.homepage, 1, strlen(tmp.homepage), fptr);
			
			if (head->next != NULL)
			{
				fwrite("\n", 1, 1, fptr);
			}
			head = head->next;
		}
		fclose(fptr);
	}
}
void registerAcc(){
	char username[SIZE];
	char password[SIZE];
	char homepage[SIZE];
	int checkUsername = 0;
	char charSet[] = "	 ";
	printf("Enter username:");
	scanf("%[^\n]%*c",username);
	if(strlen(username) == 0 || strpbrk (username, charSet)!= NULL){
		printf("Invalid username. Please register again!!!\n");
	}else{
		node *p;
		UM tmp;
		for(p=root; p!= NULL; p=p->next){
				tmp = p->user;
				if (strcmp(username,tmp.username)==0){
					checkUsername = 1;
					break;
				}
		}	
		if(checkUsername == 1){
				printf("Account existed \n");
		}else{
			printf("Enter homepage:");
			scanf("%[^\n]%*c",homepage);
			if ( isValidIpAddress(homepage) == false && isValidDomain(homepage) == false){
				printf("Invalid homepage(IP or domain). Please register again!!!\n");
			}
			else{
				printf("Enter password:");
				scanf("%[^\n]%*c",password);
				if(password == NULL ){
					printf("Invalid password. Please register again!!!\n");
				}else{
					UM registerUser;
					strcpy(registerUser.username, username);
					strcpy(registerUser.password, password);
					strcpy(registerUser.homepage, homepage);
					registerUser.status = 2;
					registerUser.timeErrorActCode = 0;
					registerUser.timeErrorPassword = 0;
					insertAtHead(registerUser);
					numberUser ++;
					writeFile();
					printf("Successful registration. Activation required.\n");
					

				}
			}
			
		}
		
	
	}
}

void activate(){
	char username[SIZE] = {};
	char password[SIZE];
	char actCode[SIZE];
	char charSet[] = "	 ";
	printf("Enter username:");
	scanf("%[^\n]%*c",username);
	if(strlen(username) == 0 || strpbrk (username, charSet)!= NULL){
		printf("Invalid username. Please enter again!!!\n");
	}else{

		printf("Enter password:");
		scanf("%[^\n]%*c",password);
		if(password == NULL || strpbrk (password, charSet)!= NULL ){
			printf("Invalid password\n");
		}else{
			node *p;
			UM tmp;
			int check =0;
			printf("Enter activate code:");
			scanf("%[^\n]%*c",actCode);
			for(p=root; p!= NULL; p=p->next){
				tmp = p->user;
				if (strcmp(username,tmp.username)==0 && strcmp(password,tmp.password)==0){
					check = 1;
					if(tmp.status == 0){
						printf("Account is blocked\n");
					}else{
						if(strcmp(actCode,"20184068")!=0){
							p->user.timeErrorActCode++;
							if(p->user.timeErrorActCode > 4){
								p->user.status = 0;
								writeFile();
								printf("Activation code is incorrect.\nAccount is blocked\n");
							}else{
								printf("Account is not activated\n");
							}
						}else{
						
								p->user.status = 1;
								writeFile();
								printf("Account is activated\n");

						}
					}
				break;
					
				}
			}
			if(check == 0){
				printf("Incorrect password or username\n");

			}
			

		}
	}
}
node * login(){
	char username[SIZE];
	char password[SIZE];
	char charSet[] = "	 ";
	int checkUsername = 0;

	printf("Enter username:");
	scanf("%[^\n]%*c",username);
	if(strlen(username) == 0 || strpbrk (username, charSet)!= NULL){
		printf("Invalid username. Please enter again!!!\n");
		return NULL;
	}else{
			node *p;
			UM tmp;
			for(p=root; p!= NULL; p=p->next){
				tmp = p->user;
				if (strcmp(username,tmp.username)==0){
					checkUsername = 1;
					break;
				}
			}	
			if(tmp.status == 0){
				printf("Account is blocked\n");
				return NULL;
			}else{
				if(checkUsername == 0){
				printf("Cannot find account\n");
				return NULL;
				}else{
					printf("Enter password:");
					scanf("%[^\n]%*c",password);
					if(password == NULL || strpbrk (password, charSet)!= NULL ){
						printf("Invalid password. Please enter again!!!\n");
						return NULL;
					}else{
						if (strcmp(password,tmp.password)==0){
							printf("Hello %s\n", p->user.username);
							cur = p;
							return p;
						}else{
							p->user.timeErrorPassword ++;
							if (p->user.timeErrorPassword > 3){
								p->user.status = 0;
								writeFile();
								printf("Password is incorrect. Account is blocked \n");
							}else{
								printf("Password is incorrect\n");

							}
							
							return NULL;
						}
							
					}

				}
			}
			
	}
}
void search(){
	char username[SIZE] = {};
	char charSet[] = "	 ";
	int checkUsername = 0;
	printf("Enter username:");
	scanf("%[^\n]%*c",username);
	if(strlen(username) == 0 || strpbrk (username, charSet)!= NULL){
		printf("Invalid username. Please enter again!!!\n");
	}else{
		node *p;
		UM tmp;
		for(p=root; p!= NULL; p=p->next){
				tmp = p->user;
				if (strcmp(username,tmp.username)==0){
					checkUsername = 1;
					break;
				}
		}	
		if(checkUsername == 0){
			printf("Cannot find account\n");
		}else{

			if(userLoged == NULL){
				printf("Not logged in. Please login!!!\n");
			}else{
				if(p->user.status == 0){
					printf("Account is blocked\n");
				}else{
					if(p->user.status == 1){
						printf("Account is active\n");
					}else printf("Account is idle\n");
				}
			}
		}

	}
}
void changePassword(){
	char password[SIZE] = {};
	char newPassword[SIZE];
	if(userLoged == NULL){
		printf("Account is not sign in . Please login!!!\n");
	}else{
		printf("Enter current password:");
		scanf("%[^\n]%*c",password);
		if(strlen(password) == 0 ){
			printf("Invalid password. Please enter again!!!\n");
		}else{
			if(strcmp(password,userLoged->user.password)==0){
				printf("Enter new password:");
				scanf("%[^\n]%*c",newPassword);
				if(strlen(newPassword) == 0 ){
					printf("Invalid password. Please enter again!!!\n");
				}else{
					strcpy(cur->user.password, newPassword);
					writeFile();
					printf("Password is changed\n");
				}
			}else{
				printf("Current password is incorrect. Please try again\n");
			}
		}

	}
}
void signOut(){
	char username[SIZE] = {};
	char charSet[] = "	 ";
	int checkUsername = 0;
	printf("Enter username:");
	scanf("%[^\n]%*c",username);
	if(strlen(username) == 0 || strpbrk (username, charSet)!= NULL){
		printf("Invalid username. Please enter again!!!\n");
	}else{
		node *p;
		UM tmp;
		for(p=root; p!= NULL; p=p->next){
				tmp = p->user;
				if (strcmp(username,tmp.username)==0){
					checkUsername = 1;
					break;
				}
		}	
		if(checkUsername == 0){
			printf("Cannot find account\n");
		}else{

			if(userLoged == NULL){
				printf("Account is not sign in\n");
			}else{
				printf("Goodbye %s\n", userLoged->user.username);
				userLoged = NULL;
			}
		}

	}
}

void domainToIp(char *domain){
    struct hostent *he = gethostbyname(domain);
    struct in_addr **addr_list;
    int i = 0;
    if(isValidDomain(domain) == true){

        if (he == NULL)
        {
            printf("Not found information\n");
        }else{
            addr_list = (struct in_addr **)he->h_addr_list;

            printf("Official IP: %s\n", inet_ntoa(*(struct in_addr *)he->h_name));
            printf("Alias IP:\n");
            for (i = 0; addr_list[i] != NULL; i++)
            {
                printf("%s\n", inet_ntoa(*addr_list[i]));
            }
        }
    }else printf("Wrong parameter\n");
    
}
void ipToDomain(char *ipInput){
    typedef struct hostent DATA;
    typedef struct in_addr ADD;
    char **names;
    DATA *hostname;
    ADD ip;
    inet_aton(ipInput, &ip);
    hostname = gethostbyaddr(&ip, sizeof(ip), AF_INET);

    if (!isValidIpAddress(ipInput))
    {
        printf("Wrong parameter\n");
        return;
    }else{
         if (hostname){
            printf("Official name: %s\n", hostname->h_name);
            printf("Alias name:\n");
            names = hostname -> h_aliases;
            while(*names) {
                printf(" %s", *names);
                names++;
            }
        }
        else{
            printf("Not found information\n");
        }
    }
    
}
void homepageWithIp(){
	if(userLoged == NULL){
		printf("Please login!!!\n");
	}else{
		if(isValidIpAddress(userLoged->user.homepage) == true){
			printf("IP address: %s\n", userLoged->user.homepage);
		}else{
			domainToIp(userLoged->user.homepage);
		}
	}
   
    
}
void homepageWithDomain(){
	if(userLoged == NULL){
		printf("Please login!!!\n");
	}else{
		if(isValidDomain(userLoged->user.homepage) == true){
			printf("Domain: %s\n", userLoged->user.homepage);
		}else{
			ipToDomain(userLoged->user.homepage);
		}
	}
   
    
}

int main()
{

	FILE *fptr;
	int n=0;
	fptr=fopen("nguoidung.txt","r");
	if (fptr==NULL)
	{
		printf("File open error\n");
		return 0;
	}else{
		importFromText(fptr);
	}
	do
	{
		n =0;
		printf("USER MANAGEMENT PROGRAM\n");
		printf("-----------------------------------\n");
		printf("1. Register\n");
		printf("2. Activate\n");
		printf("3. Sign in \n");
		printf("4. Search\n");
		printf("5. Change password\n");
		printf("6. Sign out\n");
		printf("7. Homepage with domain name\n");
		printf("8. Homepage with IP address\n");
		printf("Your choice (1-8, other to quit):");
		scanf("%d%*c",&n);
		switch(n)
		{
			case 1:
				registerAcc();
				break;
			case 2:
				activate();
				break;
		
			case 3:
		
				userLoged = login();
				break;
			
			case 4:
				search();
			
				break;
			
			case 5:
				changePassword();
				break;
			case 6:
				signOut();
				break;
			case 7:
				homepageWithDomain();
				break;
			case 8:
				homepageWithIp();
				break;
			default:
            	break;
		}
	} while (n <= 8 && n >= 1);
	fclose(fptr);
	freeList();
	return 1;
	}
