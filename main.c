#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main() {
    char validKeywords[10][10] = {"int", "move", "add", "to", "sub", "from", "loop", "times", "out", "newline" }; //Valid keywords for BigAdd language are "int", "move", "to", "add", "sub", "from", "loop", "times", "out" and "newline".
    int endOfLineCount = 0; // Count how many lines does the file have.
    char flName[100]; //File name that the program will read.
    char result[100]; //Analysis result file name that the program will create.
    flName[0] = '\0';
	result[0] = '\0';
    while(1) { //At the beginning, file name is asked. Until the correct input (myscript) is entered, loop continues.
        printf("Type the filename (with or without extension) that the program will read : ");
        int isValidName = 0;
        scanf(" %[^\n]s", flName); //Accepts spaces
        if (strstr(flName, ".") != NULL) {//If file name entered has extension, this code block will be run.
            strrev(flName);
            if (flName[0] == 'a' && flName[1] == 'b' && flName[2] == '.') {
            	int i;
            	for (i = 0; i < strlen(flName); i++) {
            		result[i] = flName[i + 3];
            	}
            	strrev(flName);
                strrev(result);
                int len = (int)strlen(result);
            	result[len] = '\0';
                isValidName = 1;
            } else {
                printf("File extension is wrong!\n");
            }
        } else { //If file name entered does not have extension, true extension "ba" is added.
			int len = (int)strlen(flName);
			int i;
			for (i = 0; i < len; i++) { //flname (without extension) is assigned to result variable to be able to use later (while creating result file).
            		result[i] = flName[i];
            	}
            result[len] = '\0';
            flName[len] = '.';
            flName[len + 1] = 'b';
            flName[len + 2] = 'a';
            flName[len + 3] = '\0';
            isValidName = 1;
        }
        if (isValidName == 1) {
            if( access(flName, F_OK) == -1 ) { //Checks whether if the file exists.
                printf("File does not exist! Please check the filename...\n"); //File does not exist, continue the loop
            } else {
                break; // File exists, break the loop
            }
        }
    }
	
    FILE *filePointer = fopen (flName, "r"); //To read from file, fopen returns a FILE pointer to "filePointer" in reading mode.
    char lexeme[100]; //Temporary char array for each lexeme
    int i = 0;
    int isComment = 0, isCodeString = 0;
    
    FILE *output_file = fopen(strcat(result,".lx"), "w"); //To write the results to file, fopen returns a FILE pointer to "output_file" in writing mode.
    while(1) {
        char c = (char)fgetc(filePointer); //Read one char from source
        if (c == EOF){ //if read character is EndOfFile (-1), while loop is broke.
            break;
        }
        if(c == '{' && !isComment && !isCodeString){ //Ignore comment blocks "{" and "}"
            isComment = 1;
            continue;
        } else if (c == '}' && isComment && !isCodeString) {
            isComment = 0;
            continue;
        }
		
		 //After this point, we check code strings only.
        if(!isComment == 1){
            if(c == '"'){ // Only valid quotation mark ""
                if(isCodeString){ //End of a string
                    lexeme[i++] = c;
                    lexeme[i] = '\0';
                    fprintf(output_file, "StringConstant %s\n", lexeme);
                    i = 0;
                }else{
                    lexeme[i++] = c;
                }
                isCodeString = !isCodeString;
                continue;
            }
            if(isCodeString == 1){
                lexeme[i++] = c;
            }

            if(!isCodeString == 1) { //Checks whether if they are valid  characters below.
                if((c>=65 && c<=90) // A – Z
				|| (c>=97 && c<=122) // a – z
				|| (c>=48 && c<=57) // 0 – 9
				|| (c>=43 && c<=46) // "+" ,"," , "-", "."
				|| c==91 // [
				|| c==93 // ]
				|| c==95 // _
				|| c==123 // { 
				|| c==125 // }
				|| c==32 //Space
				|| c==9 //TAB \t
				|| c==10){ //NEWLINE \n
                  
                    //if(c == '_'){
                    //    fprintf(output_file, "Unexpected character %s\n", lexeme);
					//	break;
                    //} else 
					if(c == '['){ // Open bracket for loop
                        fputs("OpenBlock\n", output_file);
                        continue;
                    } else if(c == ']'){ // Close bracket for loop
                        fputs("CloseBlock\n", output_file);
                        continue;
                    }

                    if(c!=' ' && c!='.' && c!='\t' && c!='\n' && c!=','){ 
                        lexeme[i++] = c; //Continues to read another lexemes
                    } else {
                        if (i != 0) { //Checks if lexeme is not null or empty
                            lexeme[i] = '\0';
                            int j;
                            for (j = 0; j < 10; j++) { //for loop that checks if lexeme is a keyword
                                if(strcmp(lexeme, validKeywords[j]) == 0){ //Checks whether if the lexeme is a keyword
                                    fprintf(output_file, "Keyword %s\n", lexeme);
                                    break;
                                }
                                if(j == 9 && lexeme[0]==95){ //Checks whether if the lexeme is an invalid lexeme
                                    fprintf(output_file, "Invalid Lexeme %s\n", lexeme);
                                } else if((j == 9 && lexeme[0]>=48 && lexeme[0]<=57  && (lexeme[1]<48 || lexeme[1]>57))  || (j == 9 && lexeme[0]== '+')){ //Checks whether if the lexeme is an invalid identifier
                                    fprintf(output_file, "Invalid Identifier %s\n", lexeme);
                                } else if(j == 9 &&  (lexeme[0]<48 || lexeme[0]>57) && lexeme[0] != '-'){ //Checks whether if the lexeme is an identifier
                                    fprintf(output_file, "Identifier %s\n", lexeme);
                                } else if((j == 9 && lexeme[0]>=48 && lexeme[0]<=57&& (lexeme[1]== '.' || lexeme[1]== ',' || lexeme[1]== '-' || lexeme[1]== '_')) || (j == 9 && lexeme[0]== '+' ) || (j == 9 && lexeme[0]== '-' && lexeme[1]== '-')){ //Checks whether if the lexeme is an integer 
                                    fprintf(output_file, "Invalid IntConstant %s\n", lexeme);
                                }else if((j == 9 && lexeme[0]>=48 && lexeme[0]<=57) || (j == 9 && lexeme[0]== '-')){ //Checks whether if the lexeme is an integer 
                                    fprintf(output_file, "IntConstant %s\n", lexeme);
                                } 
                            }
                        }
                        if (c == '.') {
                            endOfLineCount++;
                            fputs("EndOfLine\n", output_file);
                        }else if(c == ','){
                            fputs("Seperator\n", output_file);
                        }
                        i = 0;
                    }
                }else{
                    printf("Unexpected character: %c in line %d\n", c, endOfLineCount + 1); // If character is none of the valid character...
                    fputs("Unexpected character: %c\n", c); // ...error message will be shown at the console and be written to result file.
                    //return -1;
                    continue;
                }
            }
        }
    }
    
    printf("Analysis has been carried out successfully.");

    fclose(filePointer); //Closes the file read.
    fclose(output_file); //Closes the file written.
    return 0;
}
