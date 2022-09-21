#include <stdio.h>
#include <string.h>
#include "server.h"



ST_accountsDB_t accountsDB[255] ;
int available_accounts = 0;
int acc_index;
int trans_number = 0;
void get_DB(void)
{
	float amount;
	char AccState[8];
	uint8_t PAN[20] = {0};
	
	FILE* file = fopen("Accounts DB.txt", "r");
	if (file == NULL)
	{
		printf("Accounts DB.txt is not found");
		return;
	}
	int x = 0;
	while (1)
	{
		
		x = fscanf(file, "%f %s %s", &amount, AccState, PAN);
		if (x == EOF)
		{
			break;
		}
		accountsDB[available_accounts].balance = amount;
		if(strcmp(AccState,"RUNNING") == 0)
			accountsDB[available_accounts].state = RUNNING;
		else
			accountsDB[available_accounts].state = BLOCKED;

		strcpy(accountsDB[available_accounts].primaryAccountNumber, PAN);
		available_accounts++;
	}
	fclose(file);
}
void updateDB(void)
{
	FILE* file = fopen("Accounts DB.txt", "w");
	if (file == NULL)
	{
		printf("Accounts DB.txt is not found");
		return;
	}

	for (int i = 0; i < available_accounts; i++)
	{
		char AccState[8];
		if (accountsDB[i].state == RUNNING)
		{
			strcpy(AccState, "RUNNING");
		}
		else
		{
			strcpy(AccState, "BLOCKED");
		}
		fprintf(file, "%f %s %s\n", accountsDB[i].balance, AccState, accountsDB[i].primaryAccountNumber);

	}

}
EN_transState_t recieveTransactionData(ST_transaction_t* transData)
{
	if (isValidAccount(&transData->cardHolderData) != SERVER_OK)
	{
		transData->transState = FRAUD_CARD;
	}
	else if(isBlockedAccount(&accountsDB[acc_index]) != SERVER_OK)
	{
		transData->transState = DECLINED_STOLEN_CARD;
	}
	else if (isAmountAvailable(&transData->terminalData) != SERVER_OK)
	{
		transData->transState = DECLINED_INSUFFECIENT_FUND;
	}
	else
	{
		transData->transState = APPROVED;
		accountsDB[acc_index].balance -= transData->terminalData.transAmount;
		updateDB();
	}
	if (saveTransaction(transData) != SERVER_OK)
		transData->transState = INTERNAL_SERVER_ERROR;
	
	return transData->transState;
}

EN_serverError_t isValidAccount(ST_cardData_t* cardData)
{
	int i = 0;
		while (i < available_accounts)
		{
			if (strcmp(cardData->primaryAccountNumber, accountsDB[i].primaryAccountNumber) == 0)
			{
				acc_index = i;
				return SERVER_OK;
			}
			i++;

		}
	return ACCOUNT_NOT_FOUND;
}

EN_serverError_t isBlockedAccount(ST_accountsDB_t* accountReference)
{
	if (accountReference->state == BLOCKED)
	{
		return BLOCKED_ACCOUNT;
	}
	else
	{
		return SERVER_OK;
	}

}


EN_serverError_t isAmountAvailable(ST_terminalData_t* termData)
{
	if (termData->transAmount > accountsDB[acc_index].balance)
	{
		return LOW_BALANCE;
	}
	return SERVER_OK;


}
EN_serverError_t saveTransaction(ST_transaction_t* transData)
{
	//Reading prev transaction sequence number
	FILE *seqFile = fopen("Sequence.txt", "r");
	if (seqFile == NULL)
	{
		printf("\"Sequence.txt\" is not found.");
	}
	else
	{
		fscanf(seqFile, "%i", &trans_number);
		fclose(seqFile);
	}
	trans_number++;
	//Opening a file for transaction data
	FILE* transFile = fopen("Transactions DB.txt", "a");
	if (transFile == NULL)
	{
		printf("\"Transactions DB.txt\" is not found.");
		return SAVING_FAILED;
	}
	transData->transactionSequenceNumber = trans_number;
	//Adding transaction data to the file
	/*
		- Transaction Sequence Number:
		- Card Holder name:
		- Card Expiry Date:
		- PAN:
		- Amount:
		- Transaction Status:
		- Reason: (if Declined)
	*/
	fprintf(transFile, "\n Transaction:\n");
	fprintf(transFile, "   - Transaction Sequence Number: %i \n", transData->transactionSequenceNumber);
	fprintf(transFile, "   - Card Holder Name: %s \n", transData->cardHolderData.cardHolderName);
	fprintf(transFile, "   - Card Expiry Date: %s \n", transData->cardHolderData.cardExpirationDate);
	fprintf(transFile, "   - PAN: %s \n", transData->cardHolderData.primaryAccountNumber);
	fprintf(transFile, "   - Amount: %f \n", transData->terminalData.transAmount);
	
	if (transData->transState == APPROVED)
	{
		fprintf(transFile, "   - Transaction Status: Approved. \n");
	}
	else if (transData->transState == DECLINED_STOLEN_CARD)
	{
		fprintf(transFile, "   - Transaction Status: Declined. \n");
		fprintf(transFile, "   - Reason: Stolen Card. \n");
	}
	else if (transData->transState == DECLINED_INSUFFECIENT_FUND)
	{
		fprintf(transFile, "   - Transaction Status: Declined. \n");
		fprintf(transFile, "   - Reason: Insufficient Fund in the client's account. \n");
	}


	fclose(transFile);
	seqFile = fopen("Sequence.txt", "w");
	if (seqFile == NULL)
	{
		printf("\"Sequence.txt\" is not found.");
	}
	else
	{
		fprintf(seqFile, "%i", trans_number);
		fclose(seqFile);
	}



	return SERVER_OK;
}


