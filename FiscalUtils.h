#pragma once
#define ROUND_ALIGNMENT 4999
#define ROUND_COEFFICIENT 10000
#define Integer long long
#include <stdlib.h>
#include <wchar.h>
#include <math.h>
#include <boost/crc.hpp>

const unsigned char INN10[] = { 2, 4, 10, 3, 5, 9, 4, 6, 8 };
const unsigned char INN12_1[] = { 3, 7, 2, 4, 10, 3, 5, 9, 4, 6, 8 };
const unsigned char INN12_2[] = { 7, 2, 4, 10, 3, 5, 9, 4, 6, 8 };

bool __stdcall ValidateINNA(const char *INN);
void __stdcall GenerateRegNumber(const char *DeviceNumber, const char *INN, const char *FNSNumber, char *RegNumber);
bool __stdcall ValidateRegNumberA(const char *DeviceNumber, const char *INN, const char *RegNumber);
bool __stdcall ValidateEMAIL(const char *email);
bool __stdcall GetFiscalIdA(const char *HexString, char *Buffer);
bool __stdcall GetFiscalIdW(const char *HexString, wchar_t *Buffer);

class Decimal
{
public:
	Integer Hight;
	unsigned Integer Low;

	Decimal()
	{
		Hight = 0;
		Low = 0;
	};

	Decimal(double Value, unsigned char Precision)
	{
		Hight = (Integer)Value;
		unsigned Integer uHight = abs(Hight);

		if (Value < 0)
		{
			Value = Value * (-1);
		}

		unsigned Integer prc = 1;

		for (unsigned char i = 0; i < Precision; i++)
		{
			prc = prc * 10;
		}

		Low = (Value * prc) - (uHight * prc);
	};

	~Decimal()
	{
		Hight = 0;
		Low = 0;
	};

	static Integer __stdcall Round(double Value, unsigned Integer Digits)
	{
		Integer dig = 1;

		for (unsigned Integer i = 0; i < Digits; i++)
		{
			dig = dig * 10;
		}

		double value = Value * dig;
		Integer result = (Integer)value;
		Integer tmp = (Integer)((value - (double)result) * ROUND_COEFFICIENT);

		if (tmp >= ROUND_ALIGNMENT)
		{
			result++;
		}

		return result;
	};

	static Integer __stdcall Round(double Value, unsigned Integer Digits, bool ToCeil)
	{
		Integer dig = 1, alignment = ROUND_ALIGNMENT;

		if (!ToCeil)
		{
			alignment += 10;
		}

		for (unsigned Integer i = 0; i < Digits; i++)
		{
			dig = dig * 10;
		}

		double value = Value * dig;
		Integer result = (Integer)value;
		Integer tmp = (Integer)((value - (double)result) * ROUND_COEFFICIENT);

		if (tmp >= alignment)
		{
			result++;
		}

		return result;
	};

	static double __stdcall RoundTo(double Value, unsigned Integer Digits)
	{
		Integer dig = 1;

		for (unsigned Integer i = 0; i < Digits; i++)
		{
			dig = dig * 10;
		}

		double value = Value * dig;
		Integer rounded = (Integer)value;
		Integer tmp = (Integer)((value - (double)rounded) * ROUND_COEFFICIENT);

		if (tmp >= ROUND_ALIGNMENT)
		{
			rounded++;
		}

		double result = (double)rounded / (double)dig;
		return result;
	};

	static double __stdcall RoundTo(double Value, unsigned Integer Digits, bool ToCeil)
	{
		Integer dig = 1, alignment = ROUND_ALIGNMENT;

		if (!ToCeil)
		{
			alignment += 10;
		}

		for (unsigned Integer i = 0; i < Digits; i++)
		{
			dig = dig * 10;
		}

		double value = Value * dig;
		Integer rounded = (Integer)value;
		Integer tmp = (Integer)((value - (double)rounded) * ROUND_COEFFICIENT);

		if (tmp >= alignment)
		{
			rounded++;
		}

		double result = (double)rounded / (double)dig;
		return result;
	};
};

typedef struct tagCURRENCY
{
private:
	Integer Value;

public:
	__stdcall tagCURRENCY()
	{
		Value = 0;
	};

	__stdcall tagCURRENCY(int currency)
	{
		Value = ((Integer)currency) * (Integer)100;
	};

	__stdcall tagCURRENCY(Integer hight)
	{
		Value = hight * 100;
	};

	__stdcall tagCURRENCY(Integer hight, unsigned short low)
	{
		if (hight >= 0)
		{
			Value = (hight * 100) + low;
		} 
		else
		{
			Value = (hight * 100) - low;
		}
	};

	__stdcall tagCURRENCY(double currency)
	{
		Value = Decimal::Round(currency, 2);
	};

	__stdcall tagCURRENCY(const char *currency)
	{
		if (currency != NULL)
		{
			sscanf_s(currency, "%lld", &Value);
			Value = Value * 100;
			char *pos = (char *)strchr(currency, '.');

			if (pos == NULL)
			{
				pos = (char *)strchr(currency, ',');
			}

			if (pos != NULL)
			{
				pos++;
				Integer low = strtol(pos, NULL, 10);

				if (low > 99)
				{
					unsigned Integer len = strlen(pos) + 1;
					char *tmp = new char[len];
                    strncpy(tmp, len, pos);
					tmp[3] = 0;
					len = strtoul(&tmp[2], NULL, 10);
					tmp[2] = 0;
					low = strtoul(tmp, NULL, 10);
					delete[] tmp;

					if (len > 4)
					{
						low++;
					}

					if (low > 99)
					{
						low = 0;
						Value += 100;
					}
				}

				Value += low;
			}
		}
	};

	__stdcall tagCURRENCY(const wchar_t *currency)
	{
		if (currency != NULL)
		{
			swscanf_s(currency, L"%lld", &Value);
			Value = Value * 100;
			wchar_t *pos = (wchar_t *)wcschr(currency, '.');

			if (pos == NULL)
			{
				pos = (wchar_t *)wcschr(currency, ',');
			}

			if (pos != NULL)
			{
				pos++;
				Integer low = wcstoul(pos, NULL, 10);

				if (low > 99)
				{
					unsigned Integer len = wcslen(pos) + 1;
					wchar_t *tmp = new wchar_t[len];
					wcscpy_s(tmp, len, pos);
					tmp[3] = 0;
					len = wcstoul(&tmp[2], NULL, 10);
					tmp[2] = 0;
					low = wcstoul(tmp, NULL, 10);
					delete[] tmp;

					if (len > 4)
					{
						low++;
					}

					if (low > 99)
					{
						low = 0;
						Value += 100;
					}
				}

				Value += low;
			}
		}
	};

	Integer __stdcall Hight()
	{
		return Value / 100;
	};

	Integer __stdcall Low()
	{
		return Value % 100;
	};

	Integer __stdcall GetValue()
	{
		return Value;
	};

	double __stdcall GetCurrency()
	{
		return ((double)Value / 100.0);
	}

	void __stdcall AddLow(Integer low)
	{
		Value += low;
	};

	void __stdcall ToStringA(char *Str)
	{
		if (Str != NULL)
		{
			sprintf_s(Str, 260, "%.2f", ((double)Value / 100.0));
		}
	};

	void __stdcall ToStringW(wchar_t *Str)
	{
		if (Str != NULL)
		{
			swprintf_s(Str, 260, L"%.2f", ((double)Value / 100.0));
		}
	};

	std::wstring & __stdcall ToString()
	{
		wchar_t Str[100];
		swprintf_s(Str, 260, L"%.2f", ((double)Value / 100.0));
		return (std::wstring)Str;
	};

	void operator=(double currency)
	{
		Value = Decimal::Round(currency, 2);
	};

	void operator+=(const tagCURRENCY &cur)
	{
		Value += cur.Value;
	};

	friend tagCURRENCY &operator+(const tagCURRENCY &cur1, const tagCURRENCY &cur2)
	{
		Currency result = 0;
		result.Value = cur1.Value + cur2.Value;
		return result;
	};

	friend bool operator==(const tagCURRENCY &cur1, const tagCURRENCY &cur2)
	{
		return (cur1.Value == cur2.Value);
	};

	friend bool operator!=(const tagCURRENCY &cur1, const tagCURRENCY &cur2)
	{
		return (cur1.Value != cur2.Value);
	};

	bool operator>(const tagCURRENCY &cur1)
	{
		return (this->Value > cur1.Value);
	}

	bool operator<(const tagCURRENCY &cur1)
	{
		return (this->Value < cur1.Value);
	}

	friend tagCURRENCY &operator-(const tagCURRENCY &cur1, const tagCURRENCY &cur2)
	{
		Currency result = 0;
		result.Value = cur1.Value - cur2.Value;
		return result;
	};

	friend tagCURRENCY &operator*(const tagCURRENCY &cur1, const tagCURRENCY &cur2)
	{
		Currency result = 0;
		result.Value = cur1.Value * cur2.Value;
		return result;
	};

	friend tagCURRENCY &operator/(const tagCURRENCY &cur1, const tagCURRENCY &cur2)
	{
		Currency result = 0;
		result.Value = cur1.Value / cur2.Value;
		return result;
	};
}Currency;

typedef struct tagGOOD
{
	wchar_t *Name;
	double Price, Quantity;

	__stdcall tagGOOD()
	{
		Name = NULL;
		Price = 0;
		Quantity = 0;
	};

	__stdcall tagGOOD(const wchar_t *name, double price, double quantity)
	{
		Name = NULL;
		Create(name, price, quantity);
	};

	__stdcall ~tagGOOD()
	{
		if (Name != NULL)
		{
			delete[] Name;
		}
	};

	void __stdcall Create(const wchar_t *name, double price, double quantity)
	{
		if (Name != NULL)
		{
			delete[] Name;
			Name = NULL;
		}
		
		if (name != NULL)
		{
			unsigned Integer len = wcslen(name);

			if (len > 0)
			{
				len++;
				Name = new wchar_t[len];
				wcscpy_s(Name, len, name);
			}
		}

		Price = price;
		Quantity = quantity;
	};
}Good, *PGood;

class Goods
{
private:
	double AllowDifference;
	unsigned long count;

public:
	PGood *Items;
	
	__stdcall Goods()
	{
		Items = NULL;
		count = 0;
		AllowDifference = 0.001;
	};

	__stdcall Goods(const wchar_t *name, double quantity, double amount)
	{
		Items = NULL;
		count = 0;
		AllowDifference = 0.001;
		Assign(name, quantity, amount);
	};

	__stdcall ~Goods()
	{
		Clear();
	};

	unsigned long __stdcall Count()
	{
		return count;
	}

	void __stdcall Add(const wchar_t *name, double price, double quantity)
	{
		count++;
		Items = (PGood *)realloc(Items, sizeof(PGood) * count);
		Items[count - 1] = new Good(name, price, quantity);
	};

	bool __stdcall Assign(const wchar_t *name, double quantity, double amount)
	{
		Clear();
		bool Currect = false;
		double quant = Decimal::RoundTo(quantity, 3);
		double price = Decimal::RoundTo(amount / quantity, 2);
		Integer cPrice = Decimal::Round(price * quant, 2), cAmount = Decimal::Round(amount, 2);
		long dif = cAmount - cPrice;

		if (dif != 0)
		{
			price = amount / quantity;
			Integer PriceTmp = abs((Integer)(price * ROUND_COEFFICIENT) - ((Integer)(price * 100) * 10));
			double PriceFirst = (double)((unsigned Integer)(price * 100)) / 100.0, PriceLast = 0;
			(PriceTmp == 0) ? PriceLast = PriceFirst : PriceLast = PriceFirst + 0.01;
			double QuantityFirst = Decimal::RoundTo(quantity / 2, 3, false);
			double QuantityLast = Decimal::RoundTo(quantity / 2, 3, true);
			double /*Total = 0, */Difference = 0;
			Currency Total = 0;

			if ((Integer)(quant * ROUND_COEFFICIENT) - (Integer)(floor(quant) * ROUND_COEFFICIENT) > 0)
			{
				long qdif = 0;
				
				if (dif > 0)
				{
					PriceLast += 0.01;
				} 
				else if (dif < 0)
				{
					PriceFirst -= 0.01;
				}
				
				do
				{
					//Total = Decimal::RoundTo(PriceFirst * QuantityFirst, 2) + Decimal::RoundTo(PriceLast * QuantityLast, 2);
					Total = (Decimal::RoundTo(PriceFirst, 2) * Decimal::RoundTo(QuantityFirst, 3));
					Total += (Decimal::RoundTo(PriceLast, 2) * Decimal::RoundTo(QuantityLast, 3));

					//dif = cAmount - Decimal::Round(Total, 2);
					dif = cAmount - Total.GetValue();
					qdif = Decimal::Round(quantity, 3) - Decimal::Round(QuantityFirst + QuantityLast, 3);
					
					if (dif > 0)
					{
						QuantityFirst -= 0.001;
						QuantityLast += 0.001;
					}
					else if (dif < 0)
					{
						QuantityFirst += 0.001;
						QuantityLast -= 0.001;
					}

					if ((QuantityFirst < 0.001 || QuantityLast < 0.001) && dif != 0)
					{
						if (dif > 0)
						{
							PriceLast += 0.01;
						}
						else if (dif < 0)
						{
							PriceFirst -= 0.01;
						}

						QuantityFirst = Decimal::RoundTo(quantity / 2, 3, false);
						QuantityLast = Decimal::RoundTo(quantity / 2, 3, true);
					}

				} while ((dif != 0 || qdif != 0) && QuantityFirst >= 0.001 && QuantityLast >= 0.001 && PriceFirst >= 0.01 && PriceLast >= 0.01);
			} 
			else
			{
				if (PriceFirst == PriceLast)
				{
					QuantityFirst = quantity;
					QuantityLast = 0;
				} 
				else
				{
					QuantityFirst = quantity / 2;

					if ((Integer)(QuantityFirst * ROUND_COEFFICIENT) - (Integer)(floor(QuantityFirst) * ROUND_COEFFICIENT) == 0)
					{
						QuantityLast = QuantityFirst;
					}
					else
					{
						QuantityFirst = ceil(QuantityFirst);
						QuantityLast = QuantityFirst - 1;
					}

					do 
					{
						cPrice = Decimal::Round(QuantityFirst, 0) * Decimal::Round(PriceFirst, 2);
						cPrice += Decimal::Round(QuantityLast, 0) * Decimal::Round(PriceLast, 2);
						dif = cAmount - cPrice;

						if (dif > 0)
						{
							QuantityFirst--;
							QuantityLast++;
						}
						else if (dif < 0)
						{
							QuantityFirst++;
							QuantityLast--;
						}
					} while (dif != 0 && QuantityFirst > 0 && QuantityLast > 0);

					if (dif != 0)
					{
						if (QuantityFirst == 0)
						{
							QuantityFirst++;
							QuantityLast--;
						} 
						else
						{
							QuantityFirst--;
							QuantityLast++;
						}
					}

				}
			}

			if (PriceFirst == PriceLast)
			{
				QuantityFirst = quantity;
				QuantityLast = 0;
			}

			if ((Integer)(QuantityFirst * ROUND_COEFFICIENT) > 0 && (Integer)(QuantityLast * ROUND_COEFFICIENT) > 0)
			{
				if ((Integer)(QuantityLast * ROUND_COEFFICIENT) > (Integer)(QuantityFirst * ROUND_COEFFICIENT))
				{
					Add(name, PriceLast, QuantityLast);
					Add(name, PriceFirst, QuantityFirst);
				} 
				else
				{
					Add(name, PriceFirst, QuantityFirst);
					Add(name, PriceLast, QuantityLast);
				}
			} 
			else
			{
				if ((Integer)(QuantityFirst * ROUND_COEFFICIENT) > 0)
				{
					Add(name, PriceFirst, QuantityFirst);
				}

				if ((Integer)(QuantityLast * ROUND_COEFFICIENT) > 0)
				{
					Add(name, PriceLast, QuantityLast);
				}
			}
		} 
		else
		{
			Add(name, price, quantity);
		}

		Currect = (dif == 0);

		return Currect;
	};

	void __stdcall Clear()
	{
		if (count > 0)
		{
			for (unsigned long i = 0; i < count; i++)
			{
				delete Items[i];
			}

			delete[] Items;
			Items = NULL;
			count = 0;
		}
	};
};

bool __stdcall ValidateINNA(const char *INN)
{
	bool result = false;

	if (INN != NULL)
	{
		unsigned long INNLength = strlen(INN);
		unsigned Integer InnDec = 0;
		sscanf_s(INN, "%llu", &InnDec);

		if (InnDec > 0 && INNLength > 9)
		{
			char *tmpINN = new char[INNLength + 1];
			unsigned long INNindex = 0, tmpINNindex = 0;

			for (INNindex = 0; INNindex < INNLength; INNindex++)
			{
				if (INN[INNindex] >= 0x30 && INN[INNindex] <= 0x39)
				{
					tmpINN[tmpINNindex] = INN[INNindex];
					tmpINNindex++;
				}
			}

			tmpINN[tmpINNindex] = 0;

			INNLength = strlen(tmpINN);

			if (INNLength == 10 || INNLength == 12)
			{
				unsigned long INNCRC1 = 0, INNCRC2 = 0, INNCRC = 0;
				unsigned char *INNNumbers = new unsigned char[INNLength];
				char *num = new char[2];
				num[1] = 0;

				for (INNindex = 0; INNindex < INNLength; INNindex++)
				{
					num[0] = tmpINN[INNindex];
					INNNumbers[INNindex] = (char)strtoul(num, NULL, 10);
				}

				delete[] num;

				if (INNLength == 10)
				{
					for (INNindex = 0; INNindex < 9; INNindex++)
					{
						INNCRC1 += INNNumbers[INNindex] * INN10[INNindex];
					}

					INNCRC = INNCRC1 - ((INNCRC1 / 11) * 11);

					if (INNCRC == 10)
					{
						INNCRC = 0;
					}

					result = (INNCRC == INNNumbers[INNLength - 1]);
				}
				else
				{
					for (INNindex = 0; INNindex < 11; INNindex++)
					{
						INNCRC1 += INNNumbers[INNindex] * INN12_1[INNindex];

						if (INNindex < 10)
						{
							INNCRC2 += INNNumbers[INNindex] * INN12_2[INNindex];
						}
					}

					INNCRC = INNCRC1 - ((INNCRC1 / 11) * 11);

					if (INNCRC == 10)
					{
						INNCRC = 0;
					}

					if ((INNCRC == INNNumbers[INNLength - 1]))
					{
						INNCRC = INNCRC2 - (INNCRC2 / 11) * 11;

						if (INNCRC == 10)
						{
							INNCRC = 0;
						}

						result = (INNCRC == INNNumbers[INNLength - 2]);
					}
				}

				delete[] INNNumbers;
			}

			delete[] tmpINN;
		}
	}

	return result;
};

void __stdcall GenerateRegNumber(const char *DeviceNumber, const char *INN, const char *FNSNumber, char *RegNumber)
{
	if (DeviceNumber != NULL && INN != NULL && FNSNumber != NULL && RegNumber != NULL)
	{
		char ControlNumber[43];
		ZeroMemory(ControlNumber, 43);
		unsigned long length = strlen(DeviceNumber), i1 = 0, i2 = 0;
		char *deviceNumber = new char[length + 1];

		for (i1 = 0; i1 < length; i1++)
		{
			if (DeviceNumber[i1] != 0 && DeviceNumber[i1] != ' ')
			{
				deviceNumber[i2] = DeviceNumber[i1];
				i2++;
			}
		}

		deviceNumber[i2] = 0;
		length = strlen(deviceNumber);
        strncpy(&ControlNumber[42 - length], deviceNumber, length + 1);
		length = strlen(INN);
		memcpy_s(&ControlNumber[22 - length], length, INN, length);
		length = strlen(FNSNumber);
		memcpy_s(&ControlNumber[10 - length], length, FNSNumber, length);

		for (i1 = 0; i1 < 42; i1++)
		{
			if (ControlNumber[i1] == 0)
			{
				ControlNumber[i1] = '0';
			}
		}

		boost::crc_ccitt_type CRC;
		CRC.process_bytes(ControlNumber, 42);
		sprintf_s(RegNumber, 17, "%.10u%.6u", strtoul(FNSNumber, NULL, 10), CRC.checksum());
		delete[] deviceNumber;
	}
};

bool __stdcall ValidateRegNumberA(const char *DeviceNumber, const char *INN, const char *RegNumber)
{
	bool result = false;

	if (DeviceNumber != NULL && INN != NULL && RegNumber != NULL)
	{
		if (ValidateINNA(INN))
		{
			char *DeviceRegNumber1 = new char[17], *DeviceRegNumber2 = new char[17], *FNSNumber = new char[11];
			ZeroMemory(DeviceRegNumber1, 17);
			unsigned long RNLength = strlen(RegNumber);
            strncpy(&DeviceRegNumber1[16 - RNLength], RegNumber, RNLength + 1);

			for (unsigned long i = 0; i < 16; i++)
			{
				if (DeviceRegNumber1[i] == 0)
				{
					DeviceRegNumber1[i] = '0';
				}
			}

			strncpy_s(FNSNumber, 11, DeviceRegNumber1, 10);
			GenerateRegNumber(DeviceNumber, INN, FNSNumber, DeviceRegNumber2);
			result = (strcmp(DeviceRegNumber1, DeviceRegNumber2) == 0);
			delete[] DeviceRegNumber1;
			delete[] DeviceRegNumber2;
			delete[] FNSNumber;
		}
	}

	return result;
};

bool __stdcall ValidateEMAIL(const char *email)
{
	bool result = false;

	if (email != NULL)
	{
		unsigned int len = strlen(email);
		
		if (len > 4)
		{
			unsigned int sepcnt = 0;

			for (unsigned int i = 0; i < len; i++)
			{
				if (email[i] == '@')
				{
					sepcnt++;
				}
			}

			result = (sepcnt == 1);
		}
	}

	return result;
}

bool __stdcall GetFiscalIdA(const char *HexString, char *Buffer)
{
	bool result = false;

	if (HexString != NULL && Buffer != NULL)
	{
		unsigned long len = strlen(HexString);

		if (len > 32)
		{
			char ccc[8], fiscalSign[10];
            strncpy(ccc, 8, &HexString[18]);
			fiscalSign[0] = ccc[6];
			fiscalSign[1] = ccc[7];
			fiscalSign[2] = ccc[4];
			fiscalSign[3] = ccc[5];
			fiscalSign[4] = ccc[2];
			fiscalSign[5] = ccc[3];
			fiscalSign[6] = ccc[0];
			fiscalSign[7] = ccc[1];
			fiscalSign[8] = L' ';
			fiscalSign[9] = 0;

			unsigned long fpd = strtoul(fiscalSign, NULL, 16);
			sprintf_s(Buffer, MAX_PATH, "%u", fpd);
			result = true;
		}
	}

	return result;
};

bool __stdcall GetFiscalIdW(const char *HexString, wchar_t *Buffer)
{
	bool result = false;

	if (HexString != NULL && Buffer != NULL)
	{
		char buf[MAX_PATH];
		result = GetFiscalIdA(HexString, buf);

		if (result)
		{
			result = (mbstowcs(Buffer, buf, MAX_PATH) > 0);
		}
	}

	return result;
};
