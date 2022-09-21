
#include "app.h"



void appStart (void)
{
	get_DB();

	ST_transaction_t transaction;
	ST_cardData_t card;
	ST_terminalData_t terminal;
	//Card Data	
	EN_cardError_t carderror;

	//Getting card holder name
	carderror = getCardHolderName(&card);
	while(carderror == WRONG_NAME)
	{
		printf("Invalid name.\n");
		carderror = getCardHolderName(&card);
	}
	//Getting Card Expiry Date
	carderror = getCardExpiryDate(&card);
	while (carderror == WRONG_EXP_DATE)
	{
		printf("Invalid Date.\n");
		carderror = getCardExpiryDate(&card);
	}
	//Getting PAN
	carderror = getCardPAN(&card);
	while (carderror == WRONG_PAN)
	{
		printf("Invalid name.\n");
		carderror = getCardPAN(&card);
	}
	transaction.cardHolderData = card;

	//Terminal Data
	EN_terminalError_t term_error;
	
	//Getting Transaction Date
	term_error = getTransactionDate(&terminal);
	while (term_error == WRONG_DATE)
	{
		printf("Wrong Date.\n");
		term_error = getTransactionDate(&terminal);

	}
	//Checking if it is expired
	term_error = isCardExpired(card, terminal);
	if (term_error == EXPIRED_CARD)
	{
		printf("Declined.\n Expired Card.\n");
		return;
	}

	//Getting transaction amount
	term_error = getTransactionAmount(&terminal);
	while (term_error == INVALID_AMOUNT)
	{
		printf("Invalid Amount.\n");
		term_error = getTransactionAmount(&terminal);
	}
	
	// Setting Max Amount
	term_error = setMaxAmount(&terminal);
	while(term_error == INVALID_MAX_AMOUNT)
	{
		printf("Invalid Max Amount.\n");
		term_error = setMaxAmount(&terminal);
	}

	term_error = isBelowMaxAmount(&terminal);
	
	//Checking if the amount exceeds max amount 
	if(term_error == EXCEED_MAX_AMOUNT)
	{
		printf("Declined.\nAmount Exceeding Limit.");
		return;
	}
	transaction.terminalData = terminal;
	//Server Check
	EN_serverError_t server_error;	
	server_error = recieveTransactionData(&transaction);
	if (server_error == FRAUD_CARD)
	{
		printf("Declined.\nFraud Card.");
		return;
	}
	else if (server_error == DECLINED_STOLEN_CARD)
	{
		printf("Declined.\nStolen Card.");
		return;
	}
	else if (server_error == DECLINED_INSUFFECIENT_FUND)
	{
		printf("Declined.\nInsufficient Funds.");
		return;
	}
	else if (server_error == INTERNAL_SERVER_ERROR)
	{
		printf("Approved.\nBut It can't be saved due to Internal Server Error.");
		return;
	}
	else
	{
		printf("Approved.\n");
		return;
	}
}