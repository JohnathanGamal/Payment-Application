#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "card.h"

EN_cardError_t getCardHolderName(ST_cardData_t* cardData)
{
	//Get the name 
	printf("Card Holder name: ");
	gets(cardData->cardHolderName); 
	//Checking for the length of the name
	if ( cardData->cardHolderName == NULL || strlen(cardData->cardHolderName) > 24 || strlen(cardData->cardHolderName) < 20)
	{
		return WRONG_NAME;
	}
	else
	{
		return CARD_OK;
	}
}
EN_cardError_t getCardExpiryDate(ST_cardData_t* cardData)
{
	//Get expiry date
	printf("Card Expiry Date: ");
	gets(cardData->cardExpirationDate);
	//Checking for the correctness of the date format MM/YY
	bool check1 = cardData->cardExpirationDate != NULL && strlen(cardData->cardExpirationDate) == 5 && cardData->cardExpirationDate[2] == '/';
	bool check2 = isdigit(cardData->cardExpirationDate[0]) && isdigit(cardData->cardExpirationDate[1]) && isdigit(cardData->cardExpirationDate[3]) && isdigit(cardData->cardExpirationDate[4]);
	if(check1 && check2)
	{
		//Checking for the correctness of the month
		int x = (cardData->cardExpirationDate[0] - 48) * 10 + (cardData->cardExpirationDate[1] - 48);
		if (x > 0 && x < 13)
		{
			return CARD_OK;
		}
		else 
			return WRONG_EXP_DATE;
	}
	else
	{
		return WRONG_EXP_DATE;
	}
}

EN_cardError_t getCardPAN(ST_cardData_t* cardData)
{
	//Getting PAN
	printf("Primary Account Number: ");
	gets(cardData->primaryAccountNumber);
	bool iscorrect = true;
	//Checking for the length of the PAN
	if (cardData->primaryAccountNumber != NULL && strlen(cardData->primaryAccountNumber) >= 16 && strlen(cardData->primaryAccountNumber) <= 19)
	{	
		//Checking whether the PAN consists of numeric characters or not
		for (int i = 0, n = strlen(cardData->primaryAccountNumber); i < n; i++)
		{
			if (isdigit(cardData->primaryAccountNumber[i]) == 0)
			{
				iscorrect = false;
				break;
			}
		}
		if (iscorrect == true)
			return CARD_OK;
		else
			return WRONG_PAN;

	}
	return WRONG_PAN;
}



