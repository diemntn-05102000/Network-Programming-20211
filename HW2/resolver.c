#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <netdb.h> 
#include <arpa/inet.h>
#include <regex.h>
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
int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Wrong number of parameters. Please enter: ./resolver parameter1 parameter2\n");
        exit(1);
    }

    int param1 = atoi(argv[1]);
    char param2[50];
    strcpy(param2, argv[2]);
    if(param1 == 1){
        ipToDomain(param2);
    }else if (param1 == 2)
    {
        domainToIp(param2);
    }else printf("Wrong parameter 1\n");
    
    return 0;
}