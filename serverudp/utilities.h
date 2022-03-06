
#ifndef UTILITIES_H_
#define UTILITIES_H_

#define MAX_OPDIGITS 7    //max digits for an operand
#define MAX_RDIGITS 8     //max digits for a result
#define MAXBUFF 512    //size of local buffer


//math operations for the expression
double add(int* operand1,int* operand2);

double sub(int* operand1,  int* operand2);

double mult(int* operand1, int* operand2);

double division(int* operand1, int* operand2);

//function that check if the operator symbol is valid or not
char CheckSymbol (char symbol);

/*function for parsing the client expression exp
 * it finds and upgrade the value of operator, operand1, operand2
 * neg1 and neg2: booleans used for negative operand 1 and negative operand2
 */
int parseExp (char*exp, char* operator, int*operand1, int* operand2, int* neg1, int* neg2);


#endif /* UTILITIES_H_ */
