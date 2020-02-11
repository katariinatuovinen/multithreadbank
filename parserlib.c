#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>

#include "log.h"

char* DB_FILE = "bank.txt";
char* DB_TMP = "tmp.txt";
#define MIN(x, y) ((x) < (y) ? (x) : (y))

int get_balance(int account_number) {

  FILE* db_file;
  int db_account_number, db_account_balance;

  db_file = fopen(DB_FILE, "r");
  if(!db_file) {
    perror("fopen() error");
  }

  if(flock(fileno(db_file), LOCK_EX) == -1) {
    perror("flock() error");
  }

  while(fscanf(db_file, "%d %d", &db_account_number, &db_account_balance) != EOF) {
    if(db_account_number == account_number) {
      if(flock(fileno(db_file), LOCK_UN) == -1) {
        perror("flock() error");
    }
      return db_account_balance;
    }
  }

  printf("Didn't find account number %d in the back database!", account_number);
  if(flock(fileno(db_file), LOCK_UN) == -1) {
    perror("flock() error");
  }

  fclose(db_file);
  return -1;

}

// Deposit or withdraw money
// Write the new account information to a new file called bank.txt
int transaction(char command, int account_number, int amount){

  FILE* old_file;
  FILE* tmp_file;
  int db_account_number, db_account_balance;

  old_file = fopen(DB_FILE, "r");
  if(!old_file) {
      perror("fopen() error");
      return -1;
  }

  tmp_file = fopen(DB_TMP, "w");
  if (!tmp_file) {
    perror("fopen() error");
    return -1;
  }

  // Place exclusive locks to both files
  if(flock(fileno(old_file), LOCK_EX) == -1) {
    perror("flock() error");
    return -1;
  }

  if(flock(fileno(tmp_file), LOCK_EX) == -1) {
    perror("flock() error");
    return -1;
  }

  char w = 'w';
  char d = 'd';

  while(fscanf(old_file, "%d %d", &db_account_number, &db_account_balance) != EOF) {
    if(db_account_number == account_number) {
      if(command == w) {
        amount = MIN(amount, db_account_balance); // check that theres enough cash
        db_account_balance -= amount;
      } else if(command == d){
        db_account_balance += amount;
      }
    }
    fprintf(tmp_file, "%d %d \n", db_account_number,  db_account_balance);
  }


  if(flock(fileno(old_file), LOCK_UN) == -1) {
    perror("flock() error");
    return -1;
  }

  fclose(old_file);
  remove(DB_FILE);

  if(flock(fileno(tmp_file), LOCK_UN) == -1) {
    perror("flock() error");
    return -1;
  }

  fclose(tmp_file);
  rename(DB_TMP, DB_FILE);  // rename temp.txt -> bank.txt

  return 0;

}

// Transfer amount from account_1 to account_2
int transfer(int account_1, int account_2, int amount) {

  // Check if amount can be withdrawn from account_1
  int balance, transfer, retval;
  balance = get_balance(account_1);

  if (balance - amount < 0) {
    printf("Can't withdraw %d euros from account %d", amount, account_1);
  }

  transfer = MIN(balance, amount); // Withdraw money from account 1
  retval = transaction('w', account_1, transfer);
  if (retval != 0) {
    printf("Transaction failed, couldn't withdraw %d euros from account %d", transfer, account_1);
    return -1;
  }

  retval = transaction('d', account_2, transfer);  // Deposit money to account 2
  if (retval != 0) {
    printf("Transaction failed, couldn't deposit %d euros to account %d", transfer, account_2);
    return -1;
  }

  return 0;

}



int execute_command(char* buffer) {

  int ret, account_1, account_2, withdraw, deposit, amount;
  int ret_balance, ret_transaction, ret_transfer;
  char command = buffer[0];

  switch(command) {
    case 'b':
      ret = sscanf(buffer, "%c %d", &command, &account_1);
      if(ret == 2) {
        ret_balance = get_balance(account_1);
        if(ret_balance < 0 ){
          printf("Get balance for account number %d failed \n", account_1);
          return -1;
        } else {
          printf("Balance for account number %d: %d euros \n", account_1, ret_balance);
          return 0;
        }
      } else {
        printf("Usage: b accountnumber");
      }
      break;

    case 'w':
      ret = sscanf(buffer, "%c %d %d", &command, &account_1, &withdraw);
      if(ret == 3) {
        ret_transaction = transaction(command, account_1, withdraw);
        if(ret_transaction < 0) {
          printf("Transaction for account number %d with withdraw %d failed \n", account_1, withdraw);
          return -1;
        } else {
          printf("Transaction for account number %d with withdraw %d succeeded \n", account_1, withdraw);
          return 0;
        }
      } else {
        printf("Usage: w accountnumber amount");
      }
      break;

    case 'd':
      ret = sscanf(buffer, "%c %d %d", &command, &account_1, &deposit);
      if(ret == 3) {
        ret_transaction = transaction(command, account_1, deposit);
        if(ret_transaction < 0) {
          printf("Transaction for account number %d with deposit %d failed \n", account_1, deposit);
          return -1;
        } else {
          printf("Transaction for account number %d with deposit %d succeeded \n", account_1, deposit);
          return 0;
        }
      } else {
        printf("Usage: d accountnumber amount");
      }
      break;

    case 't':
      ret = sscanf(buffer, "%c %d %d %d", &command, &account_1, &account_2, &amount);
      if(ret == 4) {
        ret_transfer = transfer(account_1, account_2, amount);
        if(ret_transfer < 0) {
          printf("Transferring amount %d from account number %d to account number %d failed \n", amount, account_1, account_2);
          return -1;
        } else {
          printf("Transferring amount %d from account number %d to account number %d succeeded \n", amount, account_1, account_2);
          return 0;
        }
      } else {
        printf("Usage: t accountnumber1 accountnumber2 amount");
      }
      break;
  }
  return 0;
}
