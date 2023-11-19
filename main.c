#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
int getAmountOfClients(FILE *fp) {
    int count = 0;
    char *line = NULL;
    size_t line_length = 0;

    if (fp == NULL) {
        printf("Fail to read customer.txt");
        return -1; 
    }

    while (getline(&line, &line_length, fp) != -1) {
        count++;
    }
     
    return count;
}
void print(FILE *file, int **matrix, int rows, int cols) {
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                fprintf(file, "%d ", matrix[i][j]);
            }
            fprintf(file, "\n");
        }
    }

void init(int ***array, int rows, int cols) {
        *array = malloc(rows * sizeof(int *));
        for (int i = 0; i < rows; i++) {
            (*array)[i] = malloc(cols * sizeof(int));
            for (int j = 0; j < cols; j++) {
                (*array)[i][j] = 0;
            }
        }
    }
int **subtractMatrixes(int **matrix1,int **matrix2, int **resultmatrix, int totalClients, int totalResources){
    for (int i = 0; i < totalClients; i++) {
            for (int j = 0; j < totalResources; j++) {
                resultmatrix[i][j] = matrix1[i][j] - matrix2[i][j];
            }
        }
        return resultmatrix;

}

int **sumMatrixes(int **matrix1,int **matrix2, int **resultmatrix, int totalClients, int totalResources){
    for (int i = 0; i < totalClients; i++) {
            for (int j = 0; j < totalResources; j++) {
                resultmatrix[i][j] = matrix1[i][j] + matrix2[i][j];
            }
        }
        return resultmatrix;

}

void processRequest(FILE *fp, int totalResources, int totalClients, int **clientMaxResources,FILE *result, int **allocation) {
    if (fp == NULL){
        printf("Fail to read commands.txt");
        return;
    }
    char *line = NULL;
    size_t line_length = 0;
    int client;
    char command[2];
    int *resourceArray;
   
    while(getline(&line, &line_length, fp) != -1){
        char *token = strtok(line, " ");
        strcpy(command,token);
        token = strtok(NULL, " ");
        client = atoi(token);
        token = strtok(NULL, " ");
        resourceArray = malloc(totalResources * sizeof(int));

        for (int i = 0; i < totalResources; i++) {
            resourceArray[i] = atoi(token);
            token = strtok(NULL, " ");
        }
        
        int flag = 1;
        if (strcmp(command, "RQ") == 0) {
            for(int i = 0; i < totalResources; i++){
                if(resourceArray[i] > clientMaxResources[client][i]){
                    fprintf(result,"The customer %d request ", client);
                    
                    for(int i = 0; i < totalResources; i++){
                    fprintf(result, "%d ", resourceArray[i]);
                    }

                    fprintf(result, "was denied because exceed its maximum need\n");
                    flag = 0;
                    break;
                }
            }
            if (flag){
                fprintf(result, "Allocate to customer %d the resources ", client);

                for(int i = 0; i < totalResources; i++){
                    fprintf(result, "%d ", resourceArray[i]);
                    allocation[client][i] += resourceArray[i];
                }

                

                fprintf(result,"\n");

                for(int i = 0; i < totalResources; i++){
                    clientMaxResources[client][i] -= resourceArray[i];
                }
               
            }
        } else if (strcmp(command, "RL") == 0) {
            int flag = 1;
            for(int i = 0; i < totalResources; i++){
                if(resourceArray[i] > clientMaxResources[client][i]){
                    flag = 0;
                    break;
                }
            }
            if (flag){
                fprintf(result,"Release from customer %d the resources ", client);

                for(int i = 0; i < totalResources; i++){
                    fprintf(result,"%d ", resourceArray[i]);
                }

                fprintf(result,"\n");

                for(int i = 0; i < totalResources; i++){
                    clientMaxResources[client][i] += resourceArray[i];
                    allocation[client][i] -= resourceArray[i];
                }
               
            }
               
        } else {
            return;
        }

}
}

int main(int argc, char *argv[]) {
    FILE *customers = fopen("customer.txt", "r");
    FILE *test = fopen("commands.txt", "r");
    FILE *result = fopen("result.txt", "w");

    int totalClients = getAmountOfClients(customers);
    int totalRequests = getAmountOfClients(test);

    // printf("%d\n",totalRequests);

    fclose(test);
    int work[argc - 1];
    int **cliente;
    int **alocation;
    int **max;
    
    for(int i = 0 ; i < argc - 1; i++){
        work[i] = atoi(argv[i + 1]);
    }

    init(&max, totalClients, argc - 1);
    init(&alocation, totalClients, argc - 1);
    init(&cliente, totalClients, argc - 1);

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

    processRequest(commands, argc - 1, totalClients, cliente,result,alocation);
    sumMatrixes(alocation,cliente,max,totalClients,argc - 1);

    fprintf(result,"Max:\n");
    print(result, max, totalClients, argc - 1);
    fprintf(result,"Alocation:\n");
    print(result, alocation, totalClients, argc - 1);
    fprintf(result,"Need:\n");
    print(result, cliente, totalClients, argc - 1);

    fclose(customers);  
    fclose(commands);
    fclose(result);
  
   

    return 0;
}
