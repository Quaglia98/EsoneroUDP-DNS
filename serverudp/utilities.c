#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "utilities.h"


double division(int* operand1, int* operand2){
	return ( (double) *operand1 / (double) *operand2 ) ;
}

double add(int* operand1,int* operand2){
	return ((double)*operand1+*operand2);
}

double sub(int* operand1,  int* operand2){
	return (double)(*operand1-*operand2);
}

double mult(int* operand1, int* operand2){
	return (double)((*operand1)*(*operand2));
}

char CheckSymbol (char symbol){
	if (symbol == '+' || symbol == '-' || symbol == '/' || symbol =='x') return symbol ;
	else
	return -1 ;

}


int parseExp (char*exp, char* operator, int*operand1, int* operand2, int* neg1, int* neg2){

	int size = strlen(exp);    //size of exp
	int start = 0 ; int end = 0;
	int i = 0;
	int count1 = 0;
    int count2 = 0;  //count of digits

	char Temp[MAXBUFF];     //local buffer


	for(i = 0 ; i<size ; i++){             //checking whitespaces before the operator
		if (exp[i]== ' ' && (start == end)){
			start++;
			end++;
			continue;
		}
		else if (CheckSymbol(exp[i]) != -1){     //operator found
			end +=1;
			*operator = exp[i];

			if(exp[i+1] != ' ') return -1;  //checking if there's no whitespace after the operator
		}
		else break;
	}


	start=end;
	i = end;

	while((exp[i] == ' ') && (i<size)){
		start++;
		end++;
		i++;
	}


	i = end;

	//case negative op1
    if(exp[i] == '-' &&  isdigit(exp[i+1])){
    	i++;
    	end++;
    	*neg1=1;
    	while(isdigit(exp[i]) && i < size){
    		end++;
    		i++;
    		count1++;
    	}
    }
    //non negative op1
    else if(isdigit(exp[i]) && i < size){
        	end++;
        	i++;
        	count1++;
            while(isdigit(exp[i]) && i < size){
        	      end++;
        	      i++;
        	      count1++;
        	}

    }
    else return -1;


    if(exp[i] == '-') return -1;

	//using Temp for copying the operand characters, then assign the converted value to operand1
	if (start != end){
		strncpy(Temp, exp+start, end - start);
		*operand1 = atoi(Temp);
	} else return -1 ;

	//reset buffer
	memset(Temp,0,strlen(Temp));


    start=end;

    i = end;

    //whitespaces after the first operand
	while((exp[i] == ' ') && (i<size)){
		start++;
		end++;
		i++;
	}



	i = end;

	//case negative op2
	if(exp[i] == '-' &&  isdigit(exp[i+1])){
		i++;
		end++;
	    *neg2=1;
	    while(isdigit(exp[i]) && i < size){
	    	end++;
	        i++;
	        count2++;
	    }

	}
	//non negative op2
	else if(isdigit(exp[i]) && i < size){
		end++;
		i++;
		count2++;
		while(isdigit(exp[i]) && i < size){
			end++;
	        i++;
	        count2++;
		}
	}
	else return -1;


	//using Temp for copying the operand characters, then assign the converted value to operand1
	if (start != end){
		strncpy(Temp, exp+start, end - start+1);
		*operand2 = atoi(Temp);
	} else return -1 ;

	//reset buffer
	memset(Temp,0,strlen(Temp));


	i = end;


	//only whitespaces after the second operand are accepted.
    while(i < size){
    	if (exp[i] != ' ') return -1;
    	i++;
    }



    /*operand1 and operand2 must have not more than 7 digits
    the result must have not more than 8 digits
    Range accepted operand = [-9999999,9999999] */

    if(count1 > MAX_OPDIGITS || count2 > MAX_OPDIGITS){
    	count1 = 0;
    	count2 = 0;
    	return -1;
    }
    if(*operator == 'x'){
    	if(count1+count2 > MAX_RDIGITS) {
    		count1 = 0;
    		count2 = 0;
    		return -1;
    	}
    }



	return 0 ;

}
