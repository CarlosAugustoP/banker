#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#define MAX_WORD_SIZE 8
#define ALOCCATION_WORD_SIZE 12
#define NEED_WORD_SIZE 5

int getAmountOfClients(FILE *fp) {
    int count = 0;
    char *line = NULL;
    size_t line_length = 0;

    while (getline(&line, &line_length, fp) != -1) {
        count++;
    }

    return count;
}
int countSpaces(char *line){
    int count = 0;
    for (int i = 0; i < strlen(line); i++){
        if (line[i] == ' '){
            count++;
        }
    }
    return count;
}

int countCommas(char *line){
    int count = 0;
    for (int i = 0; i < strlen(line); i++){
        if (line[i] == ','){
            count++;
        }
    }
    return count;
}

int countIntegers(char *str) {
    int count = 0;
    int inNumber = 0;
    while (*str) {
        if (isdigit(*str)) {
            if (!inNumber) {
                count++;
                inNumber = 1;
            }
        } else {
            inNumber = 0;
        }
        str++;
    }
    return count;
}


int countDots(char *line){
    int count = 0;
    for (int i = 0; i < strlen(line); i++){
        if (line[i] == '.'){
            count++;
        }
    }
    return count;
}

int countDigits(int number){
    int count = 0;
    while (number != 0) {
        number /= 10;
        count++;
    }
    return count;
}


int isCommandsProperlyFormated(FILE *fp, int totalResources) {
    int spacecounter = 0;
    int count = 0;
    char *line = NULL;
    size_t line_length = 0;


    while (getline(&line, &line_length, fp) != -1) {
        /*
        printf("%d\n",countIntegers(line));
        printf("%d\n",countSpaces(line));
        printf("%d\n",totalResources);
        */
        if (countIntegers(line) == 0 && strcmp(line, "*\n") != 0){
            return 0;
        }//deteccao de linhas vazias
        if (countIntegers(line) != countSpaces(line) && strcmp(line, "*\n") != 0){
            return 0;
        }//deteccao de chars no meio dos numeros

        if (countIntegers(line)-1 != totalResources && strcmp(line, "*\n") != 0) {
            return 2;
        }

        if (countDots(line) >= 1){ //verifica se temos um double sem levar a uma enxaqueca
            return 0;
        }
        //printf("number of spaces in line:%d\n",countSpaces(line));
        
        if (countSpaces(line)!= countIntegers(line) && strcmp(line, "*\n") != 0) {//this probably needs to be changed!!!!!!
            return 0;
        }
        count++;

        if (strcmp(line, "*\n") == 0) {
            continue;
        }

        char *token = strtok(line, " ");
        if (strcmp(token, "RQ") != 0 && strcmp(token, "RL") != 0) {
            //printf("cai aqui, linha 98\n");
            return 0;
        }

        token = strtok(NULL, " ");
        if (token == NULL) {
            //printf("cai aqui, linha 104\n");
            return 0;
        }

        int client = atoi(token);

        if(atoi(token) == 0 && strcmp(token, "0") != 0){
            //printf("cai aqui, linha 111\n");
            return 0;
        }

        if (client < 0) {
            return 0;
        }

        int *resourceArray = malloc(totalResources * sizeof(int));//bug probably here, not getting total resources correctly
        for (int i = 0; i < totalResources; i++) { 
            token = strtok(NULL, " ");
            if (countCommas(token)!=0){
                //printf("cai aqui, linha 123\n");
                return 0;
            }
            
            if (token == NULL) {
                //printf("cai aqui, linha 123\n");
                //printf("cai aqui");
                return 0;
            }
            
            // Check if the token contains alphabetic characters
            for (int j = 0; j < strlen(token); j++) {
                if (isalpha(token[j])) {
                    
                    return 0;
                }
            }
            /* Teste para descobrir se é numero negativo.*/
            int var = atoi(token);
            if (var<0) {
                return 0;
            }
            resourceArray[i] = var;
        }
        
    }

    free(line);
    return 1;
}



int isCustomersProperlyFormated(FILE *fp, int totalResources) {
    char *line = NULL;
    size_t line_length = 0;

    while (getline(&line, &line_length, fp) != -1) {
        int commasCount = countCommas(line);
        int numbersCount = countIntegers(line); 
        int spacesCount = countSpaces(line);
        // printf("number of commas in line:%d\n",commasCount);
        // printf("number of numbers in line:%d\n",numbersCount);
        if (atoi(line)<0){
            free(line);
            return 0;
        }
        if (commasCount != numbersCount - 1) {
            free(line);
            return 0;
        }
        if (spacesCount != 0) {
            free(line);
            return 0;
        }
        if (commasCount != totalResources - 1) {
            free(line);
            return 2;
        }
    }

    free(line);

    return 1;
}

int verifyCommandLineCommands(FILE *fp, int totalResources){
    char *line = NULL;
    size_t line_length = 0;
    int spaces = 0;

    while (getline(&line, &line_length, fp) != -1) {
        spaces = countSpaces(line);
        if (spaces != totalResources - 1 && strcmp(line, "*\n") != 0){
            return 0;
        }
    }return 1;
}

int bankersTest(int totalClients, int totalResources, int *available, int **clientMaxResources, int **allocation, int **need) {

    int *safeSeq = malloc(totalClients * sizeof(int));
    int *finished = malloc(totalClients * sizeof(int));
    int *work = malloc(totalResources * sizeof(int));

    for (int i = 0; i < totalClients; i++) {
        finished[i] = 0;
    }

    for (int i = 0; i < totalResources; i++) {
        work[i] = available[i];
    }

    int cont = 0;
    int found;

    while (cont < totalClients) {
        found = 0;
        for (int i = 0; i < totalClients; i++) {
            if (finished[i] == 0) {
                int j;
                for (j = 0; j < totalResources; j++) {
                    if (need[i][j] > work[j]) {
                        break;
                    }
                }
                if (j == totalResources) {
                    for (int k = 0; k < totalResources; k++) {
                        work[k] += allocation[i][k];
                    }
                    safeSeq[cont++] = i;
                    finished[i] = 1;
                    found = 1;
                }
            }
        }
        if (!found) {
            break;
        }
    }

    free(finished);
    free(work);

    if (cont == totalClients) {
        free(safeSeq);
        return 1;
    } else {
        free(safeSeq);
        return 0;
    }
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

int sumArray (int *array, int totalResources){
    int sum = 0;
    for (int i = 0; i < totalResources; i++){
        sum += array[i];
    }
    return sum;
}

void processRequest(FILE *fp, int totalResources, int totalClients, int **clientMaxResources,FILE *result, int **allocation, int work[], int **max) {

    int max_size[totalResources];
    int allocation_size[totalResources];

    for (int i =0;i<totalResources;i++){
        //printf("%d ",work[i]);

    }

    if (fp == NULL ){
        printf("Fail to read commands.txt");
        return;
    }

    char *line = NULL;
    size_t line_length = 0;
    int client;
    char command[3];
    int *resourceArray;
    int x = 0;
    int y = 0;
    int counter1 = 0;
    int comparison1 = 0;
    int comparison2 = 0;
    int titleincrease1 = 0;
    int titleincrease2 = 0;

    while(getline(&line, &line_length, fp) != -1){
         if (strcmp(line, "*\n") == 0) {
            int max_size[totalResources]; // max_width_max[0] = x
            int allocation_size[totalResources];
            int need_size[totalResources];

            for(int i = 0; i < totalResources; i++){
             max_size[i] = 0;
             allocation_size[i] = 0;
             need_size[i] = 0;
            }

            for (int i = 0; i < totalClients; i++) {
                for (int j = 0; j < totalResources; j++) {
                    int digits = countDigits(max[i][j]);
                    if (digits > max_size[j]) {
                        max_size[j] = digits;
                    }

                    
                }
            }
            for (int i = 0; i < totalClients; i++) {
                for (int j = 0; j < totalResources; j++) {
                    int digits = countDigits(allocation[i][j]);
                    if (digits > allocation_size[j]) {
                        allocation_size[j] = digits;
                    }

                    
                }
            }
            for (int i = 0; i < totalClients; i++) {
                for (int j = 0; j < totalResources; j++) {
                    int digits = countDigits(clientMaxResources[i][j]);
                    if (digits > need_size[j]) {
                        need_size[j] = digits;
                    }

                    
                }
            }
            for(int i = 0; i < totalResources; i++){
                printf("%d ",max_size[i]);
            }
            for(int i = 0; i < totalResources; i++){
                printf("%d ",allocation_size[i]);
            }
            titleincrease1 = sumArray(max_size,totalResources)-totalResources;
            printf("titleincrease: %d\n",titleincrease1);
            titleincrease2 = sumArray(allocation_size,totalResources)-totalResources;
            printf("titleincrease: %d\n",titleincrease2);
            
            fprintf(result, "MAXIMUM ");//"maximum" tem 7 letra
            if (totalResources + (totalResources - 1) > 7){
                    for(int i = 0; i < totalResources + (totalResources - 1) - 7; i++){
                        fprintf(result, " ");
                        x++;
                    }

                }
            
            comparison1 = x + MAX_WORD_SIZE;
            //printf("O título teve de ser aumentado %d espaços\n",comparison1);
            for(int i = 0;i<titleincrease1;i++){
                fprintf(result," ");
            }
            titleincrease1 = 0;
            fprintf(result, "|");
            fprintf(result, " ALLOCATION ");//"allocation" tem 10 letras
            if (totalResources + (totalResources - 1) > ALOCCATION_WORD_SIZE){
                   for(int i = 0; i < totalResources + (totalResources - 1) - ALOCCATION_WORD_SIZE; i++){
                        fprintf(result, " ");
                        y++;
                        
                    }
                }

            comparison2 = y + ALOCCATION_WORD_SIZE;
            //printf("O título teve de ser aumentado %d espaços\n",comparison2);
            for(int i = 0;i<titleincrease2;i++){
                fprintf(result," ");
            }
            
            if(totalResources>5 && totalResources<7){  
                //printf("cai aqui");
                fprintf(result, " ");
                //sleep(2);
            }
            else if (totalResources>6 ){
                //printf("oi");
                fprintf(result, "  ");
                
            }
            fprintf(result, "|");
            fprintf(result, " NEED");//"need" tem 4 letras
            fprintf(result, "\n");
            for (int i = 0; i < totalClients; i++) {
            for (int j = 0; j < totalResources; j++) {

            if (max_size[j]!=1 && countDigits(max[i][j]) != max_size[j]){
                for(int i = 0;i<max_size[j]-1;i++){
                    fprintf(result," ");
                }
            }
                fprintf(result, "%d ", max[i][j]);
                counter1++;
            }
            if (counter1 + totalResources < comparison1){
                for(int i = 0; i < comparison1 - (counter1 + totalResources); i++){
                    fprintf(result, " ");
                }
            }
            counter1 = 0;
            fprintf(result, "| ");
        
            for (int j = 0; j < totalResources; j++) {
                if (allocation_size[j]!=1 && countDigits(allocation[i][j]) != allocation_size[j]){
                    for(int i = 0;i<allocation_size[j]-1;i++){
                        fprintf(result," ");
                    }
                }
                fprintf(result, "%d ", allocation[i][j]);

                counter1++;
            }

            if (counter1 + totalResources + 1 < comparison2) {
                for (int i = 0; i < comparison2 - (counter1 + totalResources + 1); i++) {
                    fprintf(result, " ");
                }
            }
            
            fprintf(result, "| ");
            
            counter1 = 0;

            
            for (int j = 0; j < totalResources; j++) {
                if (need_size[j]!=1 && countDigits(clientMaxResources[i][j]) != need_size[j]){
                    for(int i = 0;i<need_size[j]-1;i++){
                        fprintf(result," ");
                    }
                }
                fprintf(result, "%d ", clientMaxResources[i][j]);
            }
    

            fprintf(result, "\n");
    }

    fprintf(result,"AVAILABLE ");
    for (int i = 0; i < totalResources; i++){
        fprintf(result,"%d ",work[i]);
    }
    fprintf(result,"\n");
        }else{

        char *token = strtok(line, " ");

        strcpy(command,token);
        token = strtok(NULL, " ");
        client = atoi(token);
        token = strtok(NULL, " ");//problema encontrado
        resourceArray = malloc(totalResources * sizeof(int));

        for (int i = 0; i < totalResources; i++) {
            resourceArray[i] = atoi(token);
            token = strtok(NULL, " ");
        }

        int maximumNeedFlag = 1;
        if (strcmp(command, "RQ") == 0) {
            for(int i = 0; i < totalResources; i++){

                if(resourceArray[i] > clientMaxResources[client][i]){
                    fprintf(result,"The customer %d request ", client);

                    for(int i = 0; i < totalResources; i++){
                    fprintf(result, "%d ", resourceArray[i]);
                    }

                    fprintf(result, "was denied because exceed its maximum need\n");
                    maximumNeedFlag = 0;
                    break;

                }else if (resourceArray[i] > work[i]){
                    fprintf(result,"The resources ");

                    for(int i = 0; i < totalResources; i++){
                     fprintf(result, "%d ", work[i]);
                    }

                    fprintf(result, "are not enough to customer %d request ",client);

                    for(int i = 0; i < totalResources; i++){
                    fprintf(result, "%d ", resourceArray[i]);
                    }

                    fprintf(result,"\n");

                    maximumNeedFlag = 0;
                    break;

                }


             }


            if (maximumNeedFlag){

                for(int i = 0; i < totalResources; i++){
                    allocation[client][i] += resourceArray[i];
                    work[i] = work[i] - resourceArray[i];     


                }




                for(int i = 0; i < totalResources; i++){
                    clientMaxResources[client][i] -= resourceArray[i];
                }

                if (!bankersTest(totalClients,totalResources,work,max,allocation,clientMaxResources)){
                    fprintf(result,"The customer %d request ", client);

                     for(int i = 0; i < totalResources; i++){
                        fprintf(result, "%d ", resourceArray[i]);
                     }

                    fprintf(result, "was denied because result in an unsafe state\n");


                    for(int i = 0; i < totalResources; i++){
                        allocation[client][i] -= resourceArray[i];
                        work[i] += resourceArray[i];
                    }//desfazer alocacao

                  }else{
                    fprintf(result,"Allocate to customer %d the resources ", client);

                    for(int i = 0; i < totalResources; i++){
                        fprintf(result, "%d ", resourceArray[i]);
                    }

                    fprintf(result,"\n");

                  }

            }
        } else if (strcmp(command, "RL") == 0) {

            int flag = 1;
            for(int i = 0; i < totalResources; i++){
                if(resourceArray[i] > allocation[client][i]){
                    fprintf(result,"The customer %d released ", client);
                    for(int i = 0; i < totalResources; i++){
                        fprintf(result, "%d ", resourceArray[i]);
                    }
                    fprintf(result,"was denied because exceed its maximum allocation\n");
                    flag = 0;
                    break;
                }
            }
            if (flag){
                fprintf(result,"Release from customer %d the resources ", client);

                for(int i = 0; i < totalResources; i++){
                    fprintf(result,"%d ", resourceArray[i]);
                    work[i] += resourceArray[i];
                    //printf("%d ",work[i]);
                }
                //printf("\n");

                fprintf(result,"\n");

                for(int i = 0; i < totalResources; i++){
                    clientMaxResources[client][i] += resourceArray[i];
                    allocation[client][i] -= resourceArray[i];
                }

            }

        }
         }
}
}

int main(int argc, char *argv[]) {

    FILE *test = fopen("commands.txt", "r");
    
    fseek(test, 0, SEEK_END); 
    long fileSize = ftell(test); 
    fseek(test, 0, SEEK_SET);
    
    if(fileSize == 0){
        printf("Fail to read commands.txt\n");
        exit(EXIT_FAILURE);
    }
    int var = isCommandsProperlyFormated(test,argc-1);
    if (test == NULL || !var){
        printf("Fail to read commands.txt\n");
        exit(EXIT_FAILURE);
    }

    FILE *customers = fopen("customer.txt", "r");

    fseek(customers, 0, SEEK_END); 
    fileSize = ftell(customers); 
    fseek(customers, 0, SEEK_SET);
    
    if(fileSize == 0){
        printf("Fail to read customer.txt\n");
        exit(EXIT_FAILURE);
    }

    if (customers == NULL) {
        printf("Fail to read customer.txt\n");
        exit(EXIT_FAILURE);
    }
    int var2 = isCustomersProperlyFormated(customers,argc-1);
    if (!var2){
        printf("Fail to read customer.txt\n");
        exit(EXIT_FAILURE);
    }

    else if (var2 == 2){
        printf("Incompatibility between customer.txt and command line\n");
        exit(EXIT_FAILURE);
    }

    rewind(customers);

    int totalClients = getAmountOfClients(customers);
    int totalRequests = getAmountOfClients(test);

    // printf("%d\n",totalRequests);

    int available[argc - 1];
    int **cliente;
    int **alocation;
    int **max;

    for(int i = 0 ; i < argc-1; i++){
        available[i] = atoi(argv[i+1]);
    }

    init(&max, totalClients, argc - 1);
    init(&alocation, totalClients, argc - 1);
    init(&cliente, totalClients, argc - 1);

    //printf("Total clients: %d\n", totalClients);

    rewind(customers);
    //printf("var: %d\n",var);

if(var != 2){
    //printf("cai aqui");
     for (int i = 0; i < totalClients; i++) {
        for (int j = 0; j < argc - 1; j++) {
            if (fscanf(customers, "%d,", &cliente[i][j]) != 1) {
                fprintf(stderr, "Fail to read customer.txt\n");
                return 1;
            }
        }
    }

    rewind(customers);

    for (int i = 0; i < totalClients; i++) {
        for (int j = 0; j < argc - 1; j++) {
            if (fscanf(customers, "%d,", &max[i][j]) != 1) {
                fprintf(stderr, "Fail to read customer.txt\n");
                fclose(customers);
                return 1;
            }
        }
    }
}else if (var == 2){
    printf("imcompatibility between commands.txt and command line\n");
    exit(EXIT_FAILURE);  
}
    rewind(test);
    /*
    if(!verifyCommandLineCommands(test,argc-1)){
        printf("Incompatibility between commands.txt and command line\n");
        exit(EXIT_FAILURE);
    }
    */

    
    /*
    for (int i = 0; i < totalClients; i++) {
      printf("Cliente %d:\n",i);  
        for (int j = 0; j < argc - 1; j++) {
            printf("%d ", cliente[i][j]);
        }
        printf("\n");
    }
    */

    rewind(test);
    FILE *result = fopen("result.txt", "w");
    processRequest(test, argc - 1, totalClients, cliente,result,alocation,available,max);


    fclose(customers);  
    fclose(test);
    fclose(result);



    return 0;
}
