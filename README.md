Thread bank application
The program implements a simple TCP-socket as bank (server) and client structure.

Building & using
Run 'make' to build the program. After that run the server with './srv'. and client
with './cli'. Client commands are the following:

b account_number
w account_number withdraw_amount
d account_number deposit_amount
t account_number1 account_number2 transfer_amount

Bank information can be found from bank.txt file.
