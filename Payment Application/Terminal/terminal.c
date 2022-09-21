#include <stdio.h>
#include <string.h>
#include "../Card/card.h"
#include "terminal.h"
#define MAX_AMOUNT 10000.0

EN_terminalError_t getTransactionDate(ST_terminalData_t* termData)
{
	//Getting Transaction date
	printf("Transaction Date: ");
	gets(termData->transactionDate);
	//Checking for the length of the date
	if (strlen(termData->transactionDate) == 10)
	{
		//Checking for the date format DD/MM/YYYY
		if (termData->transactionDate[2] == '/' && termData->transactionDate[5] == '/')
		{
			bool areDigits = true;
			for (int i = 0; i < 10; i++)
			{
				if (i == 2 || i == 5)
					continue;
				else if (isdigit(termData->transactionDate[i]) == 0)
				{
					areDigits = false;
					break;
				}
			}
			if (areDigits == true)
			{
				return TERMINAL_OK;
			}
			else
				return WRONG_DATE;

		}
	}
	return WRONG_DATE;

}

EN_terminalError_t isCardExpired(ST_cardData_t cardData, ST_terminalData_t termData)
{
	//Converting date strings to digits in order to compare them
	int exp_MM = (cardData.cardExpirationDate[0] - 48) * 10 + (cardData.cardExpirationDate[1] - 48);
	int trans_MM = (termData.transactionDate[3] - 48) * 10 + (termData.transactionDate[4] - 48);
	int exp_YY = (cardData.cardExpirationDate[3] - 48) * 10 + (cardData.cardExpirationDate[4] - 48);
	int trans_YY = (termData.transactionDate[8] - 48) * 10 + (termData.transactionDate[9] - 48);
	//Comparing expiry date with transaction date
	if (exp_YY < trans_YY)
	{
		return EXPIRED_CARD;
	}
	else if (exp_YY > trans_YY)
	{
		return TERMINAL_OK;
	}
	else
	{
		if (exp_MM < trans_MM)
		{
			return EXPIRED_CARD;
		}
		else
		{
			return TERMINAL_OK;
		}
	}
}

EN_terminalError_t getTransactionAmount(ST_terminalData_t* termData)
{
	float amount;
	//Getting Transaction amount
	printf("Please Enter Transaction Amount: ");
	scanf_s("%f", &amount);
	if (amount <= 0)
	{
		return INVALID_AMOUNT;
	}
	else
	{
		termData->transAmount = amount;
		return TERMINAL_OK;

	}
}
EN_terminalError_t isBelowMaxAmount(ST_terminalData_t* termData)
{
	//Comparing transaction amount with Max Amount 
	if (termData->transAmount > termData->maxTransAmount)
	{
		return EXCEED_MAX_AMOUNT;
	}
	else
	{
		return TERMINAL_OK;
	}
}

EN_terminalError_t setMaxAmount(ST_terminalData_t* termData)
{
	//Setting Max amount of transaction
	termData->maxTransAmount = MAX_AMOUNT;
	if (termData->maxTransAmount <= 0)
	{
		return INVALID_MAX_AMOUNT;
	}
	else
		return TERMINAL_OK;
}
