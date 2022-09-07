#ifndef _SERVER_C_
#define _SERVER_C_
#include "../server/server.h"
#include <stdio.h>
#include "../card/card.h"

ST_accountsDB_t arr_acounts[255] = {
		{0100.00,"123456789123456789"},
		{6000.00,"234567891234567891"},
		{3250.25,"567891234567891234"},
		{1500.12,"456789123456789123"},
		{0500.00,"258649173258649173"},
		{2100.00,"654823719654823719"},
		{0000.00,"971362485971362485"},
		{0001.00,"793148625793148625"},
		{0010.12,"123123456123123456"},
		{0000.55,"456789321456789321"}
};

ST_transaction_t arr_transaction[255] = { 0 };
int loc, _index;


EN_transState_t recieveTransactionData(ST_transaction_t* transData)
{
	if (isValidAccount(&transData->cardHolderData))
	{
		arr_transaction[_index].transState = DECLINED_STOLEN_CARD;
		return DECLINED_STOLEN_CARD;
	}

	else if (isAmountAvailable(&transData->terminalData) == LOW_BALANCE)
	{
		arr_transaction[_index].transState = DECLINED_INSUFFECIENT_FUND;
		return DECLINED_INSUFFECIENT_FUND;
	}

	else if (saveTransaction(transData) == SAVING_FAILED)
	{
		arr_transaction[_index].transState = INTERNAL_SERVER_ERROR;
		return INTERNAL_SERVER_ERROR;
	}

	else
	{
		arr_transaction[_index].transState = APPROVED;
		arr_acounts[loc].balance -= transData->terminalData.transAmount;
		printf("new balance = %.2f\n", arr_acounts[loc].balance);
		return APPROVED;
	}
}


EN_serverError_t isValidAccount(ST_cardData_t* cardData)
{
	int flag = 0;
	for (loc = 0; loc < 255; loc++)
	{
		if (strcmp((char*)cardData->primaryAccountNumber, (char*)arr_acounts[loc].primaryAccountNumber) == 0)
		{
			flag = 1;
			break;
		}
	}

	if (flag)
		return OK_SERVER;

	return ACCOUNT_NOT_FOUND;
}


EN_serverError_t isAmountAvailable(ST_terminalData_t* termData)
{
	if (termData->transAmount > arr_acounts[loc].balance)
		return LOW_BALANCE;

	return OK_SERVER;
}


EN_serverError_t saveTransaction(ST_transaction_t* transData)
{
	if (_index >= 255)
		return SAVING_FAILED;

	arr_transaction[_index] = *transData;
	arr_transaction[_index].transactionSequenceNumber = _index;
	arr_transaction[_index].transState = APPROVED;
	_index++;
	return OK_SERVER;
}


EN_serverError_t getTransaction(uint32_t transactionSequenceNumber, ST_transaction_t* transData)
{
	for (int i = 0; i < 255; i++)
	{
		if (transactionSequenceNumber == arr_transaction[i].transactionSequenceNumber)
		{
			*transData = arr_transaction[i];
			return OK_SERVER;
		}
	}
	return TRANSACTION_NOT_FOUND;
}
#endif
