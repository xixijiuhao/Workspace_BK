#pragma once

#include "QuoteApi_API.h"

extern IQuoteApi *	g_quote_api;

class QuoteDataSpi : public IQuoteDataSpi
{
public:
	virtual ~QuoteDataSpi()
	{
		if (g_quote_api)
			g_quote_api->UnsubAllQuote(this);
	}
public:
	virtual void __stdcall OnQuote(const QContract* cont, const QQuoteUpdate* quote){};
	virtual void __stdcall OnSpreadQuote(const QSpreadContract* scont){};
};

class QuoteSpi : public QuoteDataSpi, public IQuoteEventSpi
{
public:
	QuoteSpi()
	{
		if (g_quote_api)
			g_quote_api->RegEventSpi(this);
	}
	virtual ~QuoteSpi()
	{
		if (g_quote_api)
			g_quote_api->UnregEventSpi(this);
	}

public:
	virtual void __stdcall OnConnect(){};
	virtual void __stdcall OnDisconnect(){};
	virtual void __stdcall OnLogin(const QUserType user){};
	virtual void __stdcall OnInitComplete(){}
};