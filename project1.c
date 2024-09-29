#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>

void test_bench(const char *directory_path, const char *top_level, const char *end1, const char *end2, FILE* filename, const char *top_level_verilog);

int main(int argc, char *argv[]) {

    struct dirent *directory; //directory entry
    char buffer[1024], line[1024] , A2[10][30] , A1[10][30], names[10][30]; //10 arxeia k 30 grammata sto onoma
    char full_path[1024];
    char *x , *y , *k , *z , *token, *token2; // store the return value of strstr function
    char end1[] = ".vhd"; //Ending for VHDL
    char end2[] = ".v"; //Ending for Verilog
    int flag = 0 , flag1 = 0 , i = 0 , j = 0 ;
    int index = 0, line_number = 1, counter = 0;
    char top_level[20] , entity[100], top_level_verilog[20], files[10][30], alone[10][30];
    FILE *file, *file2;

    if(argc < 2){ //not enough arguments in command line
        printf("Please put a directory as an argument\n");
        return 1;
    }

    DIR *dir = opendir(argv[1]); // not argv[0] because its the name of the program
    if(dir == NULL){
        printf("Coulden't find it! Please give the whole path of the directory\n");
        return 1;
    }

    directory = readdir(dir); // read directory

    for (int i = 0; i < 10; i++) {
        A2[i][0] = '\0';
        A1[i][0] = '\0';
        names[i][0] = '\0';
        files[i][0] = '\0';
        alone[i][0] = '\0';
    }

    while (directory != NULL){ // while the directory has files in it

        sprintf(buffer, "%s\\%s", argv[1], directory->d_name); //store the current path of the directory with the file in buffer
        //buffer changes every time we want to open the next file of the directory with the next path

        file = fopen(buffer, "r"); //open the file for read only

        x = strstr(buffer,end1); //compair with .vhd
        y = strstr(buffer,end2); //compair with .v

        if (x != NULL) { // file ending matches .vhd
            printf("\n");
            strcpy(files[counter], directory->d_name); // store all the names of the files
            counter++;

            while (fgets(line, 1024, file) != NULL) { // read every line from the file

                if (strstr(line , "entity") != NULL) { // find if in the file exists the word entity
                    strcpy(entity , line);
                    printf("The file %s is a VHDL file\n", directory->d_name);

                    while (fgets(line, 1024, file) != NULL) { // after entiny word founds read again every line of the file to find component word
                        k = strstr(line, "component");
                        if (k != NULL) { // we did this to proccess each line one at a time
                            strcpy(top_level , directory->d_name); // this file is in the top level
                            token = strtok(line, " "); //separate all the words of each line

                            while (token != NULL) { // when you find a word
                                if (strcmp(token, "component") != 0 && strcmp(token, "component;\n") != 0 && strcmp(token, "is\n") != 0 && strcmp(token, "end") != 0) {
                                    // if the word isn't any af the key words then the word is the name of the component
                                    strcpy(A2[i], token); // store the names of components
                                    i++;
                                }
                                token = strtok(NULL, " "); // no more words left. Continue to the next line
                            }
                        }
                    }
                    token = strtok(entity, " ");
                    while (token != NULL){
                        if (strcmp(token, "entity") != 0 && strcmp(token, "is\n") != 0){
                            strcpy(A1[j], token); // store the names of entities
                            j++;
                        }
                        token = strtok(NULL, " ");
                    }
                }
            }
        }
        else if (y != NULL){ //file ending matches .v
            line_number = 1;
            printf("\n");
            while (fgets(line, 1024, file) != NULL){
                
                z = strstr(line , "module"); // find if in the file exists the word module
                if (z != NULL){
                    printf("The file %s is a Verilog file\n", directory->d_name);
                    
                    while (fgets(line, 1024, file) != NULL){
                        line_number++;
                        if(strstr(line , "assign") != NULL){ // mh synthesimos typos
                            printf("\n*in %s file ", directory->d_name);
                            printf("a non-composite type has found in line: %d\n", line_number);
                        }
                    }
                    flag = 1;
                    break;
                }
                if(flag == 1){ 
                    z = strstr(line , "include");//if the word include exists then the file is the top level
                    if (z != NULL){
                        // printf("The file %s also is the top level\n", directory->d_name);
                        strcpy(top_level_verilog , directory->d_name); // this file is in the top level
                            token = strtok(line, " "); //separate all the words of each line

                            while (token != NULL) { // when you find a word
                                if (strcmp(token, "`include") != 0) {
                                    // if the word isn't any af the key words then the word is the name of the file
                                    strcpy(names[index], token); // store the names of the files
                                    index++;
                                }
                                token = strtok(NULL, " "); // no more words left. Continue to the next line
                            }
                    }
                }
            }
        }
        else{ // The file isn't VHDL or Verilog file
            printf("The file %s doen't have .vhd or .v ending type\n", buffer);
        }
        
        fclose(file);
        directory = readdir(dir); // read next file 
    }

    int count = 0;
    for(int k = 0; k < i; k++){ // buffer2
        for(int l = 0; l < j; l++){ //A1
            if(strcmp(A1[l] , A2[k]) == 0){ // check if the name of the entity matches the name of the component 
                count++;
            }
        }
    }

    if(count == i){ // if the amount of entities are the same as the amount of components 
        // then all corresponding files will have exist in the directory
        printf("\nAll VHDL files exist in the directory\n");
        printf("Top level file:\t%s\n", top_level);
    }
    else{
        printf("not all files exist in the directory\n");
        return 1;
    }

    FILE* filename = fopen("log_file.txt", "w"); // create the log file as a .txt
    fprintf(filename, "FILE HIERARCHY:\n\n");
    fprintf(filename, "-FOR VHDL FILES-\n");
    fprintf(filename, "Level 1: %s\n", top_level);
    fprintf(filename, "Level 2: ");

    printf("second level VHDL files of %s are: \t", top_level);
    int found, counter2 = 0;
    for(int j = 0; j < counter; j++){
        if(strcmp(files[j], top_level) != 0){ // if the file is different than the the top level file

            sprintf(buffer, "%s\\%s", argv[1], files[j]); //store the current path of the directory with the file in buffer
            //buffer changes every time we want to open the next file of the directory with the next path
            FILE *file2 = fopen(buffer, "r"); //open the file for read only
            found = 0;

            while (fgets(line, 1024, file2) != NULL){ // in every line
                for(int k = 0; k < i; k++){ // for all the entities

                    if(strstr(line, A2[k])){
                        found = 1; 
                        break; // we found the file stop compairing the other entities
                    }
                }
            }

            if(found == 1){ // if the entity matches the file
                printf(" %s\t", files[j]);
                fprintf(filename, "\n\t%s\n", files[j]); // write them in the log file
            }
            else{
                strcpy(alone[counter2], files[j]);
                counter2++;
            }

            fclose(file2);
        }
    }
    printf("\n");

    fprintf(filename, "Alone files:\n");
    for(int i = 0; i < counter2; i++){ // if there are files that doesnt import others and they are also not imported by any file
        if(strcmp(alone[i], "\0")){
            printf("\nThe file %s doesn't import other files neigther is imported by one so it is top level\n", alone[i]);
            fprintf(filename, "\t%s\n", alone[i]); // write them in the log file
        }
    }

    //verilog
    for(int i = 0; i < index; i++){  
        token2 = strtok(names[i], "\";"); //take the names of the files without the " ";
        if (token2 != NULL) {

            sprintf(full_path, "%s\\%s", argv[1], token2); // store the whole path with the directory and the filename
            if (access(full_path, F_OK) != -1) { // check if the path exists
                flag1++; // we found one
            } 
            token2 = strtok(NULL,"\";");
        }
    }

    fprintf(filename, "\n-FOR VERILOG FILES-\n"); // write the verilog files in the log file
    fprintf(filename, "Level 1: %s\n", top_level_verilog);
    fprintf(filename, "Level 2:\n");

    if (flag1 == index){ // if all the files exicts in the directory
        printf("\nAll verilog files exist in the directory\n");
        printf("Top level file: %s\n", top_level_verilog);
            printf("Second level verilog files of %s are: ", top_level_verilog);
            for(int j = 0; j < index; j++){
                printf("%s\t", names[j]+1); // +1 to avoid print the " in example "design_c2.v
                fprintf(filename, "\t%s\n", names[j]+1);
            }
            printf("\n");
    }

    closedir(dir);

    // TEST BENCH
    test_bench(argv[1], top_level, end1, end2, filename, top_level_verilog);

    return 0;
}

void test_bench(const char *directory_path, const char *top_level, const char *end1, const char *end2, FILE* filename, const char *top_level_verilog){

    // 20 signals with max length 100
    char port_in[20][100] , port_out[20][100], name[100], signals[100], buffer[1024], line[1024], entity[100] , verilog[100], ports[20][100];
    int p = 0, p_in = 0 , p_out = 0 , v_in = 0 , v_out = 0;
    char* separate, *x , *y, *token;
    float time_vhd , time_v;
    struct dirent *directory; //directory entry
    FILE* filename2, *filename3, *file2;

    DIR *dir2 = opendir(directory_path); // open the directory again
    directory = readdir(dir2); // read directory

    for (int i = 0; i < 20; i++) {
        ports[i][0] = '\0';
        port_in[i][0] = '\0';
        port_out[i][0] = '\0';
    }

    while (directory != NULL){ // while the directory has files in it

        sprintf(buffer, "%s\\%s", directory_path, directory->d_name); //store the current path of the directory with the file in buffer
        //buffer changes every time we want to open the next file of the directory with the next path

        file2 = fopen(buffer, "r"); //open the file for read only

        if(strcmp(directory->d_name, top_level) == 0 || strcmp(directory->d_name, top_level_verilog) == 0){ // only top level files has test bench

            x = strstr(buffer,end1); //compair with .vhd
            y = strstr(buffer,end2); //compair with .v

            if (x != NULL) { // file ending matches .vhd
                
                // store the signals
                while (fgets(line, 1024, file2) != NULL) { // read every line from the file
                    
                    if (strstr(line , "entity") != NULL) { // find if in the file exists the word entity
                    
                        strcpy(entity , line);
                        token = strtok(entity, " ");
                        while (token != NULL){
                            if (strcmp(token, "entity") != 0 && strcmp(token, "is\n") != 0){
                
                                strcpy(entity, token); // store the names of entities
                            }
                            token = strtok(NULL, " ");
                        }
                    }
                    if (strstr(line , "Port") != NULL) { // find if in the line exists the word Port 
                        
                        while (fgets(line, 1024, file2) != NULL){

                            strcpy(ports[p], line);
                            p++;

                            if(strstr(line, "in") != NULL){ // find the input signals
                                
                                token = strtok(line, ":"); // split the line at ":"
                                if (token != NULL) {
                                    
                                    separate = strtok(token, " \t\n"); // ignore the rest of the line after
                                    if (separate != NULL) {
                                        strcpy(port_in[p_in], separate);  // store only the name of the signal
                                        p_in++; // increace the row of the array
                                    }
                                }
                            }

                            if(strstr(line, "out") != NULL){
                                
                                token = strtok(line, ":"); //split the line at ":"
                                if (token != NULL) {
                                    
                                    separate = strtok(token, " \t\n");
                                    if (separate != NULL) {
                                        strcpy(port_out[p_out], separate); 
                                        p_out++;
                                    }
                                }
                            }
                        
                            if (strstr(line , ");") != NULL){ // ); its the end of the port break the loop
                                break;
                            }
                        }
                    }
                    if (strstr(line , ");") != NULL){ // the port is over break the loop so other lines with the word port won't be included
                        break;
                    }
                }
                while (fgets(line, 1024, file2) != NULL) { // read every line from the file
                    if (strstr(line , "signal") != NULL){ // asynchronous signals
                        strcpy(signals, line);
                    }
                }

                //--------------------------------------------------------------------------------------------------------------------------------
                printf("\n(VHDL)GIVE THE TIME YOU WANT THE INPUTS IS GOING TO HAVE BETWEEN THEM: ");
                scanf("%f", &time_vhd); // ask user to give the wait time
                printf("\nTHE VHDL TEST BENCH WAS CREATED SUCSESSFULLY WITH THE ENTITY'S NAME IN THE FILE!!\n");
                printf("THE FILE WAS CREATED ON THE CURRENT DIRECTORY YOU HAVE THE CODE ON");

                strcpy(name, entity);
                strcat(name, end1); //strcat so the extend will add to the entity's name and not over write it

                filename2 = fopen(name, "w"); // create the test bench with the same name as the entity's name with the extension .vhd
                // every time we have a top level file, this file will have its own test bench

                // write specific lines to the file
                fprintf(filename2, "LIBRARY IEEE;\nUSE IEEE.STD_LOGIC_1164.ALL;\n\n");
                fprintf(filename2, "ENTITY %s IS\nEND %s;\n\n", entity, entity);
                fprintf(filename2, "ARCHITECTURE behavior OF %s IS\n", entity);
                fprintf(filename2, "--Component Declaration: Unit Under Test (UTT)\n");
                fprintf(filename2, "--COMPONENT example\n");
                fprintf(filename2, "PORT(\n");
                for(int i = 0; i < p; i++){

                    fprintf(filename2, "%s\n" , ports[i]);
                }
                fprintf(filename2, "END COMPONENT;\n");
                fprintf(filename2, "--I/O Declaration\n");
                fprintf(filename2, "SIGNAL");
                for(int i = 0; i < p_in; i++){

                    fprintf(filename2, " %s", port_in[i]);
                    if(i != p_in-1){ // if we are in the last repetition then dont print the ,
                        fprintf(filename2 , " , ");
                    }
                }
                fprintf(filename2, ": STD_LOGIC_VECTOR;\n");
                fprintf(filename2, "SIGNAL");
                for(int i = 0; i < p_out; i++){

                    fprintf(filename2, " %s", port_out[i]);
                    if(i != p_out-1){ // if we are in the last repetition then dont print the ,
                        fprintf(filename2 , " , ");
                    }
                }
                fprintf(filename2, ": std_logic_vector;\n");
                fprintf(filename2, "BEGIN\n");
                fprintf(filename2, " -- UTT\n");
                fprintf(filename2, " utt: example PORT MAP(");
                for(int i = 0; i < p_in; i++){
                    fprintf(filename2, " %s => %s", port_in[i], port_in[i]);
                    if(i != p_in-1){ // if we are in the last repetition then dont print the ,
                        fprintf(filename2 , " , ");
                    }
                }
                fprintf(filename2, " , ");
                for(int i = 0; i < p_out; i++){
                    fprintf(filename2, "%s => %s", port_out[i], port_out[i]);
                    if(i != p_out-1){ // if we are in the last repetition then dont print the ,
                        fprintf(filename2 , " , ");
                    }
                }
                fprintf(filename2, ");\n");
                fprintf(filename2, " PROCESS\n");
                fprintf(filename2, " Stimulus\n");
                for(int i = 0; i < p_in; i++){
                    fprintf(filename2, " %s <= '000'\n", port_in[i]);
                    fprintf(filename2, " WAIT FOR %f ns\n" , time_vhd);
                    fprintf(filename2, " %s <= '001'\n", port_in[i]);
                    fprintf(filename2, " WAIT FOR %f ns\n" , time_vhd);
                    fprintf(filename2, " %s <= '010'\n", port_in[i]);
                    fprintf(filename2, " WAIT FOR %f ns\n" , time_vhd);
                    fprintf(filename2, " %s <= '100'\n", port_in[i]);
                    fprintf(filename2, " WAIT FOR %f ns\n" , time_vhd);
                    fprintf(filename2, " %s <= '011'\n", port_in[i]);
                    fprintf(filename2, " WAIT FOR %f ns\n" , time_vhd);
                    fprintf(filename2, " %s <= '101'\n", port_in[i]);
                    fprintf(filename2, " WAIT FOR %f ns\n" , time_vhd);
                    fprintf(filename2, " %s <= '110'\n", port_in[i]);
                    fprintf(filename2, " WAIT FOR %f ns\n" , time_vhd);
                    fprintf(filename2, " %s <= '111'\n", port_in[i]);
                    fprintf(filename2, " WAIT FOR %f ns\n" , time_vhd);
                }
                fprintf(filename2, " END PROCESS;\n");
                fprintf(filename2, "END;");

                
                fclose(filename2);
            }
            else if (y != NULL){  // file ending matches .v
                
                while (fgets(line, 1024, file2) != NULL){
                    
                    if(strstr(line , "endmodule") != NULL){ // if its endmodule and not module stop the loop
                        break;
                    }
                    if(strstr(line , "module") != NULL){
                        strcpy(verilog , line); // store the name of the module, the inputs and the outputs
                    }
                }

                //--------------------------------------------------------------------------------------------------------------------------------
                printf("\n\n(VERILOG)GIVE THE TIME YOU WANT THE INPUTS IS GOING TO HAVE BETWEEN THEM: ");
                scanf("%f", &time_v); // ask user to give the wait time
                printf("\nTHE VERILOG TEST BENCH WAS CREATED SUCSESSFULLY WITH THE MODULE'S NAME IN THE FILE!!\n");
                printf("THE FILE WAS CREATED ON THE CURRENT DIRECTORY YOU HAVE THE CODE ON\n");

                strcpy(name, "MUX2.v"); 
                filename3 = fopen(name, "w");

                // write specific lines to the file
                fprintf(filename3, "`timescale lns / lps\n");
                fprintf(filename3, "module %s;\n" , name);
                fprintf(filename3, "%s\n" , verilog);
                fprintf(filename3, "initial\n");
                fprintf(filename3, "   begin\n");
                fprintf(filename3, "\n      --CASES--\n");
                fprintf(filename3, "end");

                fclose(filename3); // close file
            }
        }

        fclose(file2); // close file
        directory = readdir(dir2); // read next file
    }
    //----------------------------------------------------------------------------------------------------------------------------------------
    // write info in log file
    fprintf(filename, "\n-FOR VHDL-\n");
    fprintf(filename, "\nThe wait time user gave for the inputs is: %f", time_vhd); // write time in the log file
    fprintf(filename, "\nAsynchronous signals: \n");   // write the asynchronous signals in the log file
    fprintf(filename, "%s", signals);

    fprintf(filename, "\n\n-FOR VERILOG-\n");
    fprintf(filename, "\nThe wait time user gave is: %f", time_v); // write time in the log file
    
    fclose(filename);
    closedir(dir2); // close directory
}