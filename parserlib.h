#ifndef PARSERLIB_H
#define PARSERLIB_H

int get_balance(int account_number);
int transfer(int account_1, int account_2, int amount);
int transaction(int account_number, int amount);
int execute_command(char* buffer);

#endif
