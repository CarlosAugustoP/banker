#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int getAmountOfClients(FILE *fp) {
    int count = 0;
    char *line = NULL;
    size_t line_length = 0;

    if (fp == NULL) {
        perror("Error with file pointer");
        return -1; 
    }

    while (getline(&line, &line_length, fp) != -1) {
        count++;
    }
     

    return count;
}

void initMatrix(int **matrix, int row, int col) {
    int i, j;
    
    matrix = (int **)malloc(row * sizeof(int *));
    for (i = 0; i < row; i++)
        matrix[i] = (int *)malloc(col * sizeof(int));
    for (i = 0; i < row; i++)
        for (j = 0; j < col; j++)
            matrix[i][j] = 0;
}

//change later
void subtractRows(int i, int matrix1[][i], int need[][i], int rowToSubtract, int rowToSubtractFrom) {
    for (int j = 0; j < i; j++) {
        need[rowToSubtractFrom][j] = matrix1[rowToSubtractFrom][j] - matrix1[rowToSubtract][j];
    }
}

int inLine(const char *str, const char *substring, size_t substringSize) {
    size_t substringLength = strlen(substring);
    if (strncmp(str, substring, substringLength) == 0 && substringLength <= substringSize) {
        return 1;  
    } else {
        return 0;  
    }
}

int selectClient(const char *str) {
    const char *rqPosition = strstr(str, "RQ");
    const char *rlPosition = strstr(str, "RL");

    if (rqPosition != NULL || rlPosition != NULL) {
        const char *substring = (rqPosition != NULL) ? rqPosition + 2 : rlPosition + 2;

        int client;
        if (sscanf(substring, "%d", &client) == 1) {
            return client;
        } else {
            return -1;
        }
    }
}

int **getAllocationRequests(FILE *fp, int totalClients, int totalResources, int amountRequisitions, int clientMaxResources[totalClients][totalResources]) {
    
    int **allocationRequests = malloc(totalClients * sizeof(int *));//allocar memoria para o array de requisições
    for (int i = 0; i < totalClients; i++) {//preencher o array de requisições com base na quantia de clientes
        allocationRequests[i] = malloc(totalResources * sizeof(int));//agora, criar um array de recursos para cada cliente, com base na quantia dos tipos de recurso.
    }

    for (int i = 0; i < totalClients; i++) {
        for (int j = 0; j < totalResources; j++) {
            allocationRequests[i][j] = 0;
        }
    }

    printf("Matrix:\n");
    for (int i = 0; i < totalClients; i++) {
        for (int j = 0; j < totalResources; j++) {
            printf("%d ", allocationRequests[i][j]);
        }
        printf("\n");
    }

    char *line = NULL;
    size_t line_length = 0;

    if (fp == NULL) {
        perror("Error with file pointer");
        return NULL; 
    }

    for (int i = 0; i < amountRequisitions; i++) {
        if (getline(&line, &line_length, fp) != -1) { 
            if (strstr(line, "*") != NULL) {
                printf("exiting\n");
                    break; 
            }    
            char *token = strtok(line, " ");
            char *temp = token;
            printf("command: %s\n", token);
            if (token != NULL && (strcmp(token, "RQ") == 0 || strcmp(token, "RL") == 0)) {
                int client = atoi(strtok(NULL, " "));
                if (client >= 0 && client < totalClients) {
                    for (int j = 0; j < totalResources; j++) {
                        token = strtok(NULL, " ");
                        if (token != NULL) {
                            int variable = atoi(token); 
                            if (temp != NULL && strcmp(temp, "RQ") == 0) {
                                if (allocationRequests[client][j] > clientMaxResources[client][j]) {
                                    printf("The customer %d request ", client);
                                    
                                    for (int k = 0; k < totalResources; k++) {
                                        printf("%d ", allocationRequests[client][k]);
                                    }

                                    printf("was denied because exceed its maximum need\n");
                                } else {
                                    printf("Allocate to customer %d the resources ", client);
                                    
                                    for (int k = 0; k < totalResources; k++) {
                                        printf("%d ", allocationRequests[client][k]);
                                    }
                                    printf("\n");

                                    allocationRequests[client][j] = allocationRequests[client][j] + variable;                                
                                }
                            } else if (temp != NULL && strcmp(temp, "RL") == 0 ){
                                allocationRequests[client][j] = allocationRequests[client][j] - variable;
                            }
                        } else {
                            fprintf(stderr, "Error reading from file\n");
                            return NULL;
                        }
                    }
                } else {
                    fprintf(stderr, "Invalid client number\n");
                    return NULL;
                }
            } else {
                fprintf(stderr, "Invalid allocation request\n");
                return NULL;
            }
        } else {
            fprintf(stderr, "Error reading from file\n");
            return NULL;
        }
    }

    return allocationRequests;
}


int main(int argc, char *argv[]) {
    FILE *customers = fopen("customer.txt", "r");
    FILE *test = fopen("commands.txt", "r");
    int totalClients = getAmountOfClients(customers);
    int totalResources = getAmountOfClients(test);
    printf("%d\n",totalResources);
    fclose(test);
    int resourcesPerType[argc - 1];
    int cliente[totalClients][argc - 1]; 
    int need[totalClients][argc-1];
    int **allocation;

        allocation = malloc(totalClients * sizeof(int *));
    for (int i = 0; i < totalClients; i++) {
        allocation[i] = malloc((argc - 1) * sizeof(int));
    }

    for (int i = 0; i < totalClients; i++) {
        for (int j = 0; j < argc - 1; j++) {
            allocation[i][j] = 0;
        }
    }
    printf("Allocation Matrix:\n");
    for (int i = 0; i < totalClients; i++) {
        for (int j = 0; j < argc - 1; j++) {
            printf("%d ", allocation[i][j]);
        }
        printf("\n");
    }


   
    for (int i = 1; i < argc; i++) {
        resourcesPerType[i - 1] = atoi(argv[i]);
        printf("Total resources of type [%d] = %d\n", i, resourcesPerType[i - 1]);
    }

   
    if (customers == NULL) {
        perror("Fail to read customer.txt\n");
        return 1;
    }

    
    printf("Total clients: %d\n", totalClients);

    rewind(customers);

    for (int i = 0; i < totalClients; i++) {
        for (int j = 0; j < argc - 1; j++) {
            if (fscanf(customers, "%d,", &cliente[i][j]) != 1) {
                fprintf(stderr, "Error reading from file\n");
                fclose(customers);
                return 1;
            }
        }
    }

    

    for (int i = 0; i < totalClients; i++) {
      printf("Cliente %d:\n",i);  
        for (int j = 0; j < argc - 1; j++) {
            printf("%d ", cliente[i][j]);
        }
        printf("\n");
    }

   FILE *commands =  fopen ("commands.txt", "r");
   if (commands == NULL){
     printf("Fail to read commands.txt");
     return 1;
   }

   allocation = getAllocationRequests(commands, totalClients, argc - 1, totalResources, cliente);
   
   for (int i = 0; i < totalClients; i++) {
      printf("Requisicao: %d:\n",i);  
        for (int j = 0; j < argc - 1; j++) {
            printf("%d ", allocation[i][j]);
        }
        printf("\n");
    }

   fclose(customers);  
   fclose(commands);
   
  
   FILE *result = fopen("result.txt","w");

    return 0;
}
