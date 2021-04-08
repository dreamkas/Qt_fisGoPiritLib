
#include "pirit_low_func.h"

#include <poll.h>

#include <array>
#include <algorithm>

PIRIT_IOCLASS pirit_io;

const unsigned char* password = password_RF;

//=======================================================================
PIRIT_IOCLASS::PIRIT_IOCLASS()
{
	packetID = START_ID;
	packetMode = 0;
    DEBUGLEVEL = DEBUG_OFF;
	setIP_io("127.0.0.1");
	setPort_io(50003);
}


//=======================================================================
// �뢮� ��ࠢ�塞�� � ����砥��� ������ � ���᮫� � ०��� �⫠���
void PIRIT_IOCLASS::printPacket(int n, PacketType type)
{
	//printf("printPacket():: DEBUGLEVEL = %d\n", DEBUGLEVEL);
	if(DEBUGLEVEL == 0)
	{
		return;
	}

	char data[512];
	char dataToSave[50000];
	memset(&data,0,sizeof(data));
	memset(&dataToSave,0,sizeof(dataToSave));

	strcpy(dataToSave, data);
	for (int i = 0; i < n; i++)
	{
		if(type == SEND)
		{
			sprintf(data,"SND:     Ascii='%c' \tHEX=0x%02x \ti=%d\n", dataToSend.data[i], dataToSend.data[i], i);
		}
		else
		{
			sprintf(data,"RCV:     Ascii='%c' \tHEX=0x%02x \ti=%d\n", dataToRead.data[i], dataToRead.data[i], i);
		}
		strcat(dataToSave, data);
	}
	printf("\nPACKET:\n%s\n", dataToSave);

	return;
}


//=======================================================================
// �������� ��砫� ����� (STX	+ ��஫� �裡 + ID ����� + ��� �������)  �� ��� ������( + �����) � ���殢�� (+ ETX + CRC)
int PIRIT_IOCLASS::makeFirstPartPacketToSend(unsigned cmd)
{
    if(DEBUGLEVEL == DEBUG_ON) printf("makeFirstPartPacketToSend():: cmd = 0x%02x\n", cmd);
	int len = 0;
	clearData(SEND);
	// �⠭���⭮� ��砫� �����
	dataToSend.data[0] = STX;
	dataToSend.data[1] = password[0];
	dataToSend.data[2] = password[1];
	dataToSend.data[3] = password[2];
	dataToSend.data[4] = password[3];
	dataToSend.data[5] = packetID;
	len = sprintf(&dataToSend.data[6], "%02X", cmd);
	dataToSend.length = 6 + len;
	incPackedID(); // ���६��� ����� �����
	return 0;
}


//=======================================================================
// ��⠢�� ���� � �६��� � ����� �� ��ࠢ��
void PIRIT_IOCLASS::addDateTime(PLDateTime dt)
{
	int len = 0;
	dt.year = dt.year % 1000;
	len = sprintf(&dataToSend.data[dataToSend.length], "%02d%02d%02d%c%02d%02d%02d%c", dt.day, dt.month, dt.year, 0x1C,
				  dt.hours, dt.minutes, dt.seconds, 0x1C);
	dataToSend.length += len;
}


//=======================================================================
// ��⠢�� ���� � ����� �� ��ࠢ��
void PIRIT_IOCLASS::addDate(PLDate piritDate)
{
	int len = 0;
	piritDate.year = piritDate.year % 1000;
	len = sprintf(&dataToSend.data[dataToSend.length], "%02d%02d%02d%c", piritDate.day, piritDate.month, piritDate.year, 0x1C);
	dataToSend.length += len;
}


//=======================================================================
// ��⠢�� �६��� � ����� �� ��ࠢ��
void PIRIT_IOCLASS::addTime(PLTime piritTime)
{
	int len = 0;
	len = sprintf(&dataToSend.data[dataToSend.length], "%02d%02d%02d%c", piritTime.hours, piritTime.minutes, piritTime.seconds, 0x1C);
	dataToSend.length += len;
}


//=======================================================================
//  ��⠢�� ���� � �६��� � ����� �� ��ࠢ��
void PIRIT_IOCLASS::addDateTime(int day, int month, int year, int hour,int min, int sec)
{
	int len = 0;
	year = year % 1000;
	len = sprintf(&dataToSend.data[dataToSend.length], "%02d%02d%02d%c%02d%02d%02d%c",day, month, year, 0x1C, hour, min, sec, 0x1C);
	dataToSend.length += len;
}


//=======================================================================
// �������� � ����� ���� �� ������ ���� + ᨬ��� ࠧ����⥫� 0x1C
void PIRIT_IOCLASS::addByte(unsigned char byte)
{
	int len = 0;
	len = sprintf(&dataToSend.data[dataToSend.length], "%d%c", byte, 0x1C);
	dataToSend.length += len;
}

//=======================================================================
// ����� ����� ���� ��� �������਩
void PIRIT_IOCLASS::addEmptyByte()
{
	int len = 0;
	len = sprintf(&dataToSend.data[dataToSend.length], "%c", 0x1C);
	dataToSend.length += len;
}


//=======================================================================
// ����� ����� ���� ��� �������਩
void PIRIT_IOCLASS::addInt(int twoByte)
{
	int len = 0;
	len = sprintf(&dataToSend.data[dataToSend.length], "%ld%c", twoByte, 0x1C);
	dataToSend.length += len;
}


//=======================================================================
// ����� ����� ���� ��� �������਩
void PIRIT_IOCLASS::addLong(long fourByte)
{
	int len = 0;
	len = sprintf(&dataToSend.data[dataToSend.length], "%ld%c", fourByte, 0x1C);
	dataToSend.length += len;
}


//=======================================================================
// ����� ����� ���� ��� �������਩
void PIRIT_IOCLASS::addConstChar(const char* str)
{
	int len = 0;
	if(str == nullptr)
	{
		len = sprintf(&dataToSend.data[dataToSend.length], "%c", 0x1C);
	}
	else
	{
		len = sprintf(&dataToSend.data[dataToSend.length], "%s%c", str, 0x1C);
	}
	dataToSend.length += len;
}


//=======================================================================
// ����� ����� ���� ��� �������਩
void PIRIT_IOCLASS::addChar(char* str, size_t size)
{
    int len;

    if (size == 0)
    {
        len = sprintf(&dataToSend.data[dataToSend.length], "%s%c", str, 0x1C);
    }
    else
    {
    	memcpy(&dataToSend.data[dataToSend.length], str, size);
        dataToSend.data[dataToSend.length + size] = 0x1C;
        len = size+1;
    }

    if(DEBUGLEVEL == DEBUG_ON)
    {
        printf("packet parameter(with 1c) LEN = %d\n", len);
    }

    dataToSend.length += len;
}

void PIRIT_IOCLASS::addString(std::string text, const size_t &limit)
{
    if ((limit > 0) && (text.length() > limit))
    {
        text.erase(begin(text) + limit, end(text));
    }
    pirit_io.addChar(const_cast<char*>(text.c_str()));
}

//=======================================================================
// ����� ����� ���� ��� �������਩
void PIRIT_IOCLASS::addDoubleFromLong(long data, int numFormat)
{
	int len = 0;
	len = sprintf(&dataToSend.data[dataToSend.length], "%ld.%0*ld%c", data / (pow10(numFormat)), numFormat, data % (pow10(numFormat)), 0x1C);
	dataToSend.length += len;
}


//=======================================================================
// ����� ����� ���� ��� �������਩
void PIRIT_IOCLASS::addDouble(double data)
{
	int len = 0;
	setlocale( LC_ALL, "en-US" );
	len = sprintf(&dataToSend.data[dataToSend.length], "%.3lf%c", data, 0x1C);
	setlocale( LC_ALL, "rus" );
	dataToSend.length += len;
}


//=======================================================================
// ����� ����� ���� ��� �������਩
void PIRIT_IOCLASS::addDoubleFromInt64(int64_t data, int numFormat)
{
	int len = 0;
	long lByte1,lByte2;
	int64_t maxL = 1000000000;
	if(data > maxL)
	{
		lByte1 = data/maxL;
		lByte2 = data % maxL;
		len = sprintf(&dataToSend.data[dataToSend.length], "%ld%07ld.%0*ld%c", lByte1, lByte2 / (pow10(numFormat)), numFormat, lByte2 % (pow10(numFormat)), 0x1C);
	}
	else
	{
		lByte2 = data;
		len = sprintf(&dataToSend.data[dataToSend.length], "%ld.%0*ld%c", lByte2 / (pow10(numFormat)), numFormat, lByte2 % (pow10(numFormat)), 0x1C);
	}
	dataToSend.length += len;
}


//=======================================================================
// ���������� (ETX	CRC) � ����� ����� �� ��ࠢ��
int PIRIT_IOCLASS::makeEndPartPacket()
{
	//printf("makeEndPartPacket():: Start!\n");
	int len = 0;
	len = sprintf(&dataToSend.data[dataToSend.length], "%c", ETX);
	dataToSend.length += len;

	int crc = 0;
	for (int i = 1; i < dataToSend.length; i++)
	{
		crc = (crc ^ dataToSend.data[i]) & 0xFF;
	}
	len = sprintf(&dataToSend.data[dataToSend.length], "%02X", crc);
	dataToSend.length += len;
	//printf("makeEndPartPacket dataToSend.length = %d\n", dataToSend.length);
	//printf("makeEndPartPacket strlen = %d\n",     strlen(dataToSend.data) );

	printPacket(dataToSend.length, SEND);
	//printf("makeEndPartPacket end\n");
	return 0;
}


//=======================================================================
// ���⪠ ���� �� ��� ��� ��ࠢ��
void PIRIT_IOCLASS::clearData(PacketType type)
{
	if(type == SEND)
	{
		memset(&dataToSend, 0, sizeof(DataToSend));
	}
	else
	{
		memset(&dataToRead, 0, sizeof(DataToSend));
	}

}


//=======================================================================
// ����� ����� ���� ��� �������਩
long PIRIT_IOCLASS::pow10(long power)
{
	long i;
	long result = 1;
	for(i = 0; i < power; i++){
		result *= 10;
	}
	return result;
}


//=======================================================================
// ����� ����� ���� ��� �������਩
void PIRIT_IOCLASS::setPackedID(unsigned char newID)
{
	packetID = newID;
}


//=======================================================================
// ����� ����� ���� ��� �������਩
unsigned char PIRIT_IOCLASS::getPackedID()
{
	return packetID;
}


//=======================================================================
// // ���६��� ����� �����
unsigned char PIRIT_IOCLASS::incPackedID()
{
	packetID++;
	if (packetID > 63)
		packetID = START_ID;
	return packetID;
}


//=======================================================================
// ����� ����� ���� ��� �������਩
void PIRIT_IOCLASS::setPacketMode(unsigned char newPacketMode)
{
	packetMode = newPacketMode;
}

//=======================================================================
// ����� ����� ���� ��� �������਩
void PIRIT_IOCLASS::setPasswd(Country newCountry)
{
    switch (newCountry)
    {
        case Country::RF:
        {
            password = password_RF;
            break;
        }
        case Country::RB:
        {
            password = password_RB;
            break;
        }
        default:
        {
            printf("setPasswd:: Error, Unknown Country!!!!!");
            break;
        }
    }
}

//=======================================================================
// ����� ����� ���� ��� �������਩
unsigned char PIRIT_IOCLASS::getPacketMode()
{
	return packetMode;
}


//=======================================================================
// ���� �⢥⭮�� ����� �� ��⮪��� ����
int PIRIT_IOCLASS::parseAnswer()
{
    int err;
	printPacket(dataToRead.length, RECIEVE);
	// �᫨ ����� ����� 9, ����� ��� ����室���� �����
	if (dataToRead.length < 9)
    {
        printf("\nparseAnswer():1008 dataToRead.length < 9\n");
        return 1008;
    }
	//--
	// 1-� ����
	if (dataToRead.data[0] != STX)
	{
		printf("\nparseAnswer():1001 Error packet !STX\n");
		return 1001;
	}
	//--
	// 6-� ���� ID ����� SEND = 2-�� ����� � �ਭ�⮬ ����� �� ��ࠢ��(�� ��)
	if (dataToSend.data[5] != dataToRead.data[1])
	{
        int snd = (int) dataToSend.data[5];
        int rcv = (int) dataToRead.data[1];
		//packetID = (snd > rcv ? snd : rcv) + 1; // �����稢��� ����� �� ���� �⭮�⥫쭮 ���ᨬ��쭮�� �����
		printf("\nparseAnswer():WARNING! packet SendNumber != ReadNumber. SendNumber = %d, ReadNumber = %d\n", snd, rcv );
        if( snd > rcv )
        {
            printf("\nNEED READ NEXT PACKET!\n");
            return PIRIT_LOW_LVL_ERRS::BAD_PACK_NUM;
        }
        else
        {
            return PIRIT_LOW_LVL_ERRS::NO_ERRORS;
        }

	}
	//--
	// 7-� ���� ����� SEND (1-� ���� �������)
	if (dataToSend.data[6] != dataToRead.data[2])
	{
		printf("\nparseAnswer():1003 Error packet CommandNumberSend != CommandNumberRead First byte\n");
		return 1003;
	}
	// 8-� ���� (2-� ���� �������)
	if (dataToSend.data[7] != dataToRead.data[3])
	{
		printf("\nparseAnswer():1004 Error packet CommandNumberSend != CommandNumberRead Second byte\n");
		return 1004;
	}
	//--
	// ETX ���殢�� ����� �ᥣ��
	if (dataToRead.data[dataToRead.length - 3] != ETX)
	{
		printf("\nparseAnswer():1005 Error packet !ETX\n");
		return 1005;
	}

	//--
	// �ࠢ����� crc
	int crcRead;
	char bb[3];
	bb[0] = dataToRead.data[dataToRead.length - 2];
	bb[1] = dataToRead.data[dataToRead.length - 1];
	bb[2] = 0;
	sscanf(bb, "%x", &crcRead);
	int crc = 0;
	for (int i = 1; i < dataToRead.length - 2; i++)
	{
		crc = (crc ^ dataToRead.data[i]) & 0xFF;
	}
	if (crc != crcRead)
	{
		printf("Error packet CRC(%2X) != CRC(%2X)\n", crc, crcRead);
		return 1006;
    }
	//--
	// ��� �訡��
	bb[0] = dataToRead.data[4];
	bb[1] = dataToRead.data[5];

	sscanf(bb, "%x", &err);
	if (err != 0)
	{
		printf("\nparseAnswer():: ERROR CODE is not NULL! ERR CODE IN PACKET = 0x%02x\n", err);
		return err;
	}

    if (dataToRead.length >= 9)
	{
		memcpy(answer.data, &dataToRead.data[6], dataToRead.length - 9);
		answer.length = dataToRead.length - 9;
		//printf("\n++++ parseAnswer():: ANSWER DATA SIZE= %d ++++\n", answer.length);
	}
	else
	{
		printf("\nparseAnswer():: ERROR! DATA SIZE TOO SMALL! SIZE= %d\n", dataToRead.length);
	}
    return 0;
}

const vector<uint8_t> &PIRIT_IOCLASS::getBinryData()
{
    return dataToReadBinary;
}


//=======================================================================
// ��楦����� �� ��ப� answer ���祭�� INT � data, ����� ��� ASCII � ��ࢮ�� �����
int PIRIT_IOCLASS::parseAnswer1Int(int *data)
{
	int i, len;
	int counter = 0;
	char tmpBuf[32];     				// ���� ��� �᫠
	memset(tmpBuf, 0, 32);
	len = 0;

	for(i = 0; i < answer.length; i++)   // ���� ���� ��騩 ���� �⢥�(���� ����)
	{
		if(answer.data[i] == 0x1C)       // �᫨ ᫥�. ���祭��
		{
			if(counter == 0) *data = atoi(tmpBuf); // ������ ������ ��� �� ���� �� ���࠭���� ���ᨢ�
			memset(tmpBuf, 0, 32);                 // ����塞 ����
			counter ++;                            // �����稢��� ���� 祣�-�
			len = 0;                               // ����塞 ����� ��ப� �᫠
		}
		else // ���� ����ࠥ� ���ᨢ �᫠
		{
			tmpBuf[len] = answer.data[i];
			len++;
		}
	}
	if(counter == 1) return 0;  // �᫨ � ���� DATA �뫮 �ᥣ� ���� �᫮, � ��
	return 1010;
}


//=======================================================================
// ��楦����� �� ��ப� answer ���祭�� char � data, ����� ��� ASCII (��ࢮ� ���祭��)             ������ �������� �������
int PIRIT_IOCLASS::parseAnswer1Char(char *data)
{
	int i, len;
	int counter = 0;
	char tmpBuf[128];
	memset(tmpBuf, 0, 128);
	len = 0;
	for(i = 0; i < answer.length; i++)
	{
		if(answer.data[i] == 0x1C)
		{
			if(counter == 1)        // ������ ������� �� ���ᨢ �� ���࠭���� ���ᨢ�
			{
				strcpy(data, tmpBuf);
			}
			memset(tmpBuf, 0, 32);
			counter ++;
			len = 0;
		}
		else
		{
			tmpBuf[len] = answer.data[i];
			len++;
		}
	}
	if(counter == 2)
	{
		return 0;
	}
	return 1010;
}

//=======================================================================
// ��楦����� �� ��ப� answer ���ᨢ char[], ���浪��� ����� N � �����, ������ �� 0
int PIRIT_IOCLASS::parseAnswerChN_constCount( int number, char **val, int &valLength )
{
	int i, len = 0;
	int counter = 0;
	char tmpBuf[1024];
	memset(tmpBuf, 0, 1024);
	//----
	for(i = 0; i < answer.length; i++)
	{
		if( answer.data[i] == 0x1C )  // �᫨ ����⨫� �ਧ��� ���� ���� 1� (...<1C>...<1C>...)
		{
			//--------
			if(counter == number) // �᫨ �㦭� ����� ��ࠬ��� � �����
			{
                valLength = len;
                *val = new char[valLength];
				memcpy(*val, tmpBuf, valLength); // ��
				return 0;
			}
			//--------
			memset(tmpBuf, 0, 1024);
			counter ++;
			len = 0;
		}
		else
		{
			tmpBuf[len] = answer.data[i];
			len++;
		}
	}
	// �᫨ ��᫥���� ���� �� ����� � ��稭� �� ����� �ਧ���� ����砭�� 1�, � ��� ⮦� ��襬
	if ( len > 0 )
	{
		if(counter == number) // �᫨ �㦭� ����� ��ࠬ��� � �����
		{
            valLength = len;
            *val = new char[valLength];
            printf("parseAnswerChN_constCount()::(No '0x1C') valLength = %d, readed len = %d\n", valLength, len);
            memcpy(*val, tmpBuf, valLength); // ��
            for(unsigned int i = 0; i< valLength; i++)
            {
                printf("LIB i=%d \t", i);
                usleep(100);
                printf(" Ascii='%c'  \t", *((*val) + i) );
                usleep(100);
                printf("\tHEX=0x%02x\n",  *((*val) + i) );
                usleep(100);

            }
            return 0;
		}
	}
	//----
	if(counter >= (number+1) )
	{
		return 0;
	}
	printf("parseAnswerChN():: counter = %d, number = %d", counter, number);
	return 2222;
}


//=======================================================================
// ��楦����� �� ��ப� answer ���祭�� char � data, ����� ��� ASCII (��ࢮ� ���祭��)             ������ �������� �������
int PIRIT_IOCLASS::parseAnswer1Int2Int3Int(int *i1, int *i2, int *i3)
{
	int i, len;
	int counter = 0;
	char tmpBuf[128];
	memset(tmpBuf, 0, 128);
	len = 0;
	for(i = 0; i < answer.length; i++)
	{
		if(answer.data[i] == 0x1C)
		{
			if(counter == 1)
			{
				 *i1 = atoi(tmpBuf); // ������ ������ ��� �� ���� �� ���࠭���� ���ᨢ�
			}
			else if(counter == 2)
			{
				*i2 = atoi(tmpBuf); // ������ 2 ��� �� ���� �� ���࠭���� ���ᨢ�
			}
			else if(counter == 3)
			{
				*i3 = atoi(tmpBuf); // ������ 3 ��� �� ���� �� ���࠭���� ���ᨢ�
			}

			memset(tmpBuf, 0, 32);
			counter ++;
			len = 0;
		}
		else
		{
			tmpBuf[len] = answer.data[i];
			len++;
		}
	}
	if(counter == 3)
	{
		return 0;
	}
	return 1010;
}

//=======================================================================
// ��楦����� �� ��ப� answer ���祭�� char* � data, ����� ��� ASCII    ������ �������� �������
int PIRIT_IOCLASS::parseAnswer1CharEx(char *data)
{
	int i, len;
	int counter = 0;
	char tmpBuf[128];
	memset(tmpBuf, 0, 128);
	len = 0;
	for(i = 0; i < answer.length; i++)
	{
		if(answer.data[i] == 0x1C)
		{
			if(counter == 1)  // �᫨ �� 2-� ��६����� � ����, � �����㥬
			{
				strcpy(data, tmpBuf);
			}
			memset(tmpBuf, 0, 32);
			counter ++;
			len = 0;
		}
		else
		{
			tmpBuf[len] = answer.data[i];
			len++;
		}
	}
	if(counter == 1)
	{
		strcpy(data, tmpBuf);
		return 0;
	}
	return 1010;
}


//=======================================================================
// ��楦����� �� ��ப� answer ���祭�� char* � data, ����� ��� ASCII (���� 2 ���祭��)   ������ �������� �������
int PIRIT_IOCLASS::parseAnswer2Char(char *data)
{
	int i, len;
	int counter = 0;
	char tmpBuf[32];
	memset(tmpBuf, 0, 32);
	len = 0;
	for(i = 0; i < answer.length; i++)
	{
		if(answer.data[i] == 0x1C)
		{
			if(counter == 1)
			{
				strcpy(data, tmpBuf);
			}
			if(counter == 2)
			{
				strcat(data, tmpBuf);
			}
			memset(tmpBuf, 0, 32);
			counter ++;
			len = 0;
		}
		else
		{
			tmpBuf[len] = answer.data[i];
			len++;
		}
	}
	if(counter == 3)
	{
		return 0;
	}
	return 1010;
}


//=======================================================================
// ���� ��ப� �஢, ��६����� ����� "num" �� "count" ��६�����
int PIRIT_IOCLASS::parseAnswerChar(int count, int num, char *data)
{
	int i, len;
	int counter = 0;
	static const size_t tmpSize = 64;
	char tmpBuf[tmpSize];
	len = 0;
	for (i = 0; i < answer.length; i++)
	{
		if (answer.data[i] == 0x1C)
		{
			if (counter == num)            // �᫨ ����� ���� � ���� = num
			{
				strcpy(data, tmpBuf);
			}
			memset(tmpBuf, 0, tmpSize);
			counter++;
			len = 0;
		}
		else
		{
			tmpBuf[len] = answer.data[i];
			len++;
		}
	}
	if (counter == (count + 1))             // �᫨ ��饥 ������⢮ �����  � ���� = count
	{
		return 0;
	}
	return 1010;
}


//=======================================================================
// ����� ����� ���� ��� �������਩
int PIRIT_IOCLASS::parseAnswerCommand0x00(int *fatalStatus, int *currentFlagsStatus, int *documentStatus)
{
	int i, len;
	int counter = 0;
	char tmpBuf[32];
	memset(tmpBuf, 0, 32);
	len = 0;
	for(i = 0; i < answer.length; i++)
	{
		if(answer.data[i] == 0x1C)
		{
			if(counter == 0) *fatalStatus = atoi(tmpBuf);         // 1-� ��६�����  ����� �⠫쭮�� ���ﭨ� ���
			if(counter == 1) *currentFlagsStatus = atoi(tmpBuf);  // 2-� ��६�����  ����� ⥪��� 䫠��� ���
			if(counter == 2) *documentStatus = atoi(tmpBuf);      // 3-� ��६�����  ����� ���㬥��
			memset(tmpBuf, 0, 32);
			counter ++;
			len = 0;
		}
		else
		{
			tmpBuf[len] = answer.data[i];
			len++;
		}
	}
	if(counter == 3)              // �᫨ ������⢮ ��६����� � ���� = 3, � ��
	{
		return 0;
	}
	return 1010;
}



//=======================================================================
// ����� ����� ���� ��� �������਩
int PIRIT_IOCLASS::parseAnswerCommand0x00_4args(int *fatalStatus, int *currentFlagsStatus, int *documentStatus, int *sknoStatus)
{
    if(fatalStatus == NULL || currentFlagsStatus == NULL || documentStatus == NULL || sknoStatus == NULL)
    {
        printf("parseAnswerCommand0x00_4args:: Error, NULL arg!");
        return 1010;
    }
    int i, len;
    int counter = 0;
    char tmpBuf[32];
    memset(tmpBuf, 0, 32);
    len = 0;
    for(i = 0; i < answer.length; i++)
    {
        if(answer.data[i] == 0x1C)
        {
            if(counter == 0) *fatalStatus        = atoi(tmpBuf);         // 1-� ��६�����  ����� �⠫쭮�� ���ﭨ� ���
            if(counter == 1) *currentFlagsStatus = atoi(tmpBuf);         // 2-� ��६�����  ����� ⥪��� 䫠��� ���
            if(counter == 2) *documentStatus     = atoi(tmpBuf);         // 3-� ��६�����  ����� ���㬥��
            if(counter == 3) *sknoStatus         = atoi(tmpBuf);         // 4-� ��६�����  ����� ����
            memset(tmpBuf, 0, 32);
            counter ++;
            len = 0;
        }
        else
        {
            tmpBuf[len] = answer.data[i];
            len++;
        }
    }
    if(counter == 4)              // �᫨ ������⢮ ��६����� � ���� = 3, � ��
    {
        return 0;
    }
    if(len != 0 && counter == 3)    // �᫨ � ���� ��� 0x1� ᨬ����
    {
        *sknoStatus         = atoi(tmpBuf);         // 4-� ��६�����  ����� ����
        return 0;
    }
    return 1010;
}
//=======================================================================
// ������� ���ᨢ ��⮢, � 2-�� �����!!!! type=1 楫��   type=2 ������饥
int PIRIT_IOCLASS::parseAnswerIntXd(int intData[], int maxElement, int type)
{
	int i, len;
	int counter = 0;
	char tmpBuf[32];

	memset(tmpBuf, 0, 32);
	len = 0;
	for(i = 0; i < answer.length; i++)
	{
		if(answer.data[i] == 0x1C)
		{
			if((counter >= 1) && (counter <= maxElement))
			{
				if(type == 1)// (1) Int/Long
				{
					intData[counter-1] = atol(tmpBuf);
				}
				else if(type == 2)// (2) Float/Double
				{
					intData[counter-1] = (int)(atof(tmpBuf) * (double)pow10(AmountDecimalPlaces));
				}
			}
			memset(tmpBuf, 0, 32);
			counter ++;
			len = 0;
		}
		else
		{
			tmpBuf[len] = answer.data[i];
			len++;
		}
	}
	if(counter == (maxElement+1))
	{
		return 0;
	}
	return 1010;
}


//=======================================================================
// ����� ����� ���� ��� �������਩
int PIRIT_IOCLASS::parseAnswerIntXdEx(int intData[], int maxElement, int type)
{
	int i, len;
	int counter = 0;
	char tmpBuf[32];

	memset(tmpBuf, 0, 32);
	len = 0;
	for(i = 0; i < answer.length; i++){
		if(answer.data[i] == 0x1C){
			if((counter >= 0) && (counter <= maxElement))
			{
				if(type == 1)// (1) Int/Long
				{
					intData[counter] = atol(tmpBuf);
				}
				else if(type == 2)// (2) Float/Double
				{
					intData[counter] = (int)(atof(tmpBuf) * (double)pow10(AmountDecimalPlaces));
				}
			}
			memset(tmpBuf, 0, 32);
			counter ++;
			len = 0;
		}
		else
		{
			tmpBuf[len] = answer.data[i];
			len++;
		}
	}
	if( counter == (maxElement+1) )
	{
		return 0;
	}
	return 1010;
}


//=======================================================================
// ������ ����� �� ��᫥����� X ����� ��� ����� � �����⨨ ᬥ��
int PIRIT_IOCLASS::parseAnswerCommand0x01_12(int intData[], int maxElement, char *str)
{
	int i, len;
	int counter = 0;
	char tmpBuf[32];

	memset(tmpBuf, 0, 32);
	len = 0;
	for(i = 0; i < answer.length; i++)
	{
		if(answer.data[i] == 0x1C)
		{
			if((counter >= 2) && (counter <= maxElement))
			{
				if((counter/2)*2 == counter)
				{
					intData[counter-2] = atol(tmpBuf);
				}
				else
				{
					intData[counter-2] = (int)( atof(tmpBuf) * (double)pow10(AmountDecimalPlaces) );
				}
			}
			else if(counter == 1)
			{
				strcpy(str, tmpBuf);
                if(DEBUGLEVEL == DEBUG_ON) printf( "parseAnswerCommand0x01_12():: '%s'\n",str );
			}
			memset(tmpBuf, 0, 32);
			counter ++;
			len = 0;
		}
		else
		{
			tmpBuf[len] = answer.data[i];
			len++;
		}
	}
	if(counter == (maxElement+2))
	{
		return 0;
	}
	return 1010;
}

//========================================================================
// ������� ᮪��
// AF_INET - ���୥� �����
// SOCK_STREAM - ��।�� ��⮪�� ������ � �।���⥫쭮� ��⠭����� ᮥ�������
// PROTOCOL �� 㬮�砭��
int PIRIT_IOCLASS::connectSock()
{
	//printf( "connectSock():: Start \n" );

	int enable = 1;

	bzero( &dest_out, sizeof(dest_out) );
	dest_out.sin_family = AF_INET;
	dest_out.sin_port = htons( port );

    if(DEBUGLEVEL == DEBUG_ON) printf ("connectSock::    IP= %s\n", ip.c_str());
    if(DEBUGLEVEL == DEBUG_ON) printf ("connectSock::  port= %d\n", port);
	inet_aton( ip.c_str(), &dest_out.sin_addr );

	// �������� ᮪��
	if(sd_out <= 0)    // Socket �� ᮧ���
	{
		sd_out = socket( PF_INET, SOCK_STREAM, 0 );
		//printf("connectSock():: socket = %d\n", sd_out);
	}
	else
	{
		printf("\nconnect():: WARNING skip creating socket\n");
	}

	if (setsockopt( sd_out, SOL_SOCKET, SO_REUSEPORT,  &enable, sizeof(int)) < 0 )
	{
		printf("\nconnectSock()::ERROR! setsockopt(SO_REUSEPORT) failed\n");
	}

	if (sd_out >= 0)
	{
		// ����� ᮧ���
		//printf( "connectSock():: Socket created\n" );
		// ��⠭������� ᮥ������� � �ࢥ஬ ���
		err = connect( sd_out, (struct sockaddr *) &dest_out, sizeof(dest_out) );
		if (err == 0)
		{
			// ���������� ��⠭������
			//printf( "connectSock():: Connection established\n" );
		}
		else  // ! connect
		{
			// ���������� �� ��⠭������
			printf( "\nconnectSock():: CONNECTION ERROR: %d !!!!!\n", err );
			close( sd_out );
			sd_out = 0;
			return 2010;
		}
	}
	else  // ! socket(
	{
		// ����� �� ᮧ���
		printf( "\nconnectSock():: SOCKET ERROR: %d !!!!!\n", sd_out );
		sd_out = 0;
		return 2011;
	}

	// ����� ᮧ���. ������ ���ਯ��
    if(DEBUGLEVEL == DEBUG_ON) printf( "connectSock():: SOCKET CREATE SUCCESS\n" );
	return 0;
}


//========================================================================
// ����뢠�� ᮪��
int PIRIT_IOCLASS::disconnectSock()
{
    // ���� �� ᮪��
	if (sd_out <= 0)
	{
		printf( "\ndisconnectSock():: ERROR! INVALID SOCKET!!!\n" );
		sd_out = 0;
		return 0;
	}
//	// ����뢠�� ᮪��
	if (close( sd_out ) != 0)
	{
		printf( "\ndisconnectSock():: CLOSE ERROR!!!\n" );
		sd_out = 0;
		return -1;
	}

	// ����뢠�� ᮥ�������
	// "2" - ����頥� �⥭�� � ������
//	if (shutdown( sd_out, 2 ) != 0)
//	{
//		printf( "\ndisconnectSock():: SHUTDOWN ERROR!!!\n" );
//		sd_out = 0;
//		return -2;
//	}
	sd_out = 0;
	return 0;
}


//=======================================================================
// ����� ����� ���� ��� �������਩
int PIRIT_IOCLASS::sendData()
{
	int ssize = 0;
	clearData(RECIEVE);

	int bytes_send = 0;                             //����� �� ��ࠢ��
    if( checkBuffer.getAccumulate() )
    {
        dataToSend.data[dataToSend.length] = '\0';
		string sendStr(dataToSend.data);
		checkBuffer.addCommand(sendStr);
    }
    else
    {
		if(  checkBuffer.empty() )
		{
			write(sd_out, dataToSend.data, dataToSend.length);
            if(DEBUGLEVEL == DEBUG_ON)
			{
				printf( "\nsendData():: Sended %d bytes\n", dataToSend.length );
			}
			return 0;
		}

		string sendStr  = checkBuffer.getAllCommands();
		       sendStr += string(dataToSend.data);

		write(sd_out, sendStr.c_str(), sendStr.length());
        if(DEBUGLEVEL == DEBUG_ON)
        {
            printf( "\nsendData():: Sended %d bytes\n", sendStr.length() );
        }
    }
	return 0;

}

//=======================================================================
// ����� ����� ���� ��� �������਩
int PIRIT_IOCLASS::readData(int TIMEOUT)
{

    const size_t MAX_PACKET_LEN = 2048;
    int res = PIRIT_LOW_LVL_ERRS::NO_ERRORS;

    do
    {
        pollfd pfd;
        pfd.fd     = sd_out;
        pfd.events = POLLIN | POLLPRI;

        int ret = poll(&pfd, 1, TIMEOUT);
        if ((ret > 0) && (pfd.revents & POLLIN) || (pfd.revents & POLLPRI))
        {
            array<int8_t, MAX_PACKET_LEN> tmpBuf = {0};
            int readBytes = read(sd_out, tmpBuf.data(), tmpBuf.size());
            if ( readBytes > 0)
            {
                copy_n(tmpBuf.begin(), readBytes, dataToRead.data);
                dataToRead.length = readBytes;
            }
        }
        else if (ret == PIRIT_LOW_LVL_ERRS::NO_ERRORS)
        {
            if( password == password_RF )
            {
                string killCommand{"killall punix"};
                system(killCommand.c_str() );
            }
            return EXIT_FAILURE;
        }
        res = parseAnswer();
    }
    while( res == PIRIT_LOW_LVL_ERRS::BAD_PACK_NUM );
    return res;
}

int PIRIT_IOCLASS::readBinaryData()
{
    vector<uint8_t> emptyBinaryData;
    swap(dataToReadBinary, emptyBinaryData);

    const int TIMEOUT = 2000;
    const size_t MAX_PACKET_LEN = 1024;
    dataToReadBinary.reserve(MAX_PACKET_LEN);

    pollfd pfd;
    pfd.fd     = sd_out;
    pfd.events = POLLIN | POLLPRI;

    while (true)
    {
        int ret = poll(&pfd, 1, TIMEOUT);
        if ((ret > 0) && (pfd.revents & POLLIN) || (pfd.revents & POLLPRI))
        {
            array<int8_t, MAX_PACKET_LEN> tmpBuf = {0};
            int readBytes = read(sd_out, tmpBuf.data(), tmpBuf.size());
            if ( readBytes > 0)
            {
                copy_n(tmpBuf.begin(), readBytes, back_inserter(dataToReadBinary));
            }
        }
        else if (ret == 0)
        {
            break;
        }
        else
        {
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}


//=======================================================================
// ����� ����� ���� ��� �������਩
int PIRIT_IOCLASS::readAck()
{
	clearData(RECIEVE);
	int i = 0;
	int len = 0;
	int cb = 0;

	do
	{
//          if (TCPDevice->Receive(&dataToRead.data[i], 1))
		{
			len = 1;
		}
		if(dataToRead.data[i] == 6)
		{
			cb = 5;
		}
		dataToRead.length += len;
		i++;
	}
	while(cb < 3);

	return 0;
}


//=======================================================================
// ��ࠢ�� ������� ��� ������  � ��� �⢥⭮�� �����
int PIRIT_IOCLASS::commandVoid(int command)
{
	//printf( "commandVoid():: command = 0x%02x\n", command );
	int err;

	makeFirstPartPacketToSend(command);
	makeEndPartPacket();
	err = connectSock(); if( err != 0 ) return err;
	err = sendData();

	if (err == 0)
 	{
		if (packetMode == 0)
			err = readData();
	}
	disconnectSock();
	return err;
}


//=======================================================================
// ����� ����� ���� ��� �������਩
MData PIRIT_IOCLASS::commandVoidMData(int command)
{
	int err;
	MData mData;
	memset(&mData, 0, sizeof(mData));
	makeFirstPartPacketToSend(command);
	makeEndPartPacket();
	mData.errCode = 5;
	err = connectSock();
    if( err != 0 )
    {
        mData.errCode = err;
        mData.dataLength = 1;
        memset(mData.data, 0, sizeof(mData.data));
        return mData;

    }
	err = sendData();

	if (err == 0)
	{
		if (packetMode == 0)
		{
			err = readData();
		}
	}
	mData.errCode = err;
	for (int j = 0; j < dataToRead.length; j++)
	{
		mData.data[j] = dataToRead.data[j];
		mData.dataLength = dataToRead.length;
	}
	disconnectSock();
	return mData;
}


//=======================================================================
// ��ࠢ�� ������� ��� ������ � � ��� � �⢥⭮� �����
int PIRIT_IOCLASS::commandVoidInt(int command, int *data)
{
	int err;
	makeFirstPartPacketToSend(command);
	makeEndPartPacket();
	err = connectSock(); if( err != 0 ) return err;
	err = sendData();

	if (err == 0)
	{
		if (packetMode == 0)
		{
			err = readData();
		}
	}

	if (err == 0)
	{
		err = parseAnswer1Int(data);
	}
	disconnectSock();
	return err;
}


//=======================================================================
// ��ࠢ�� ������� � ������ � ���� ����� � ��� �⢥⭮�� �����
MData PIRIT_IOCLASS::commandByte(int command, unsigned char dByte)
{
	int err;
	MData mData;
	makeFirstPartPacketToSend(command);
	addByte(dByte);
	makeEndPartPacket();
    err = connectSock();
    if( err != 0 )
    {
        mData.errCode = err;
        mData.dataLength = 1;
        memset(mData.data, 0, sizeof(mData.data));
        return mData;

    }
	err = sendData();

	if (err == 0)
	{
		err = readData();
	}
	mData.errCode = err;
	for (int j = 0; j < dataToRead.length; j++)
	{
		mData.data[j] = dataToRead.data[j];
		mData.dataLength = dataToRead.length;
	}
	disconnectSock();
	return mData;
}


//=======================================================================
// ����� ����� ���� ��� �������਩
//╧єёЄр  ъюьрэфр
MData PIRIT_IOCLASS::commandByteEx(int command)
{
	int err;
	MData mData;
	makeFirstPartPacketToSend(command);
	makeEndPartPacket();
	connectSock();
	err = sendData();
	if (err == 0)
	{
		err = readData();
	}
	mData.errCode = err;
	for (int j = 0; j < dataToRead.length; j++)
	{
		mData.data[j] = dataToRead.data[j];
		mData.dataLength = dataToRead.length;
	}
	disconnectSock();
	return mData;
}


//=======================================================================
// ��⠭���� �஢�� �⫠���
void PIRIT_IOCLASS::setDebugLevel(DEBUG_LEVEL level)
{
	DEBUGLEVEL = level;
}


//=======================================================================
// ����� ����� ���� ��� �������਩
void PIRIT_IOCLASS::getMData(MData *mData)
{
	for (int j = 0; j < dataToRead.length; j++)
	{
		mData->data[j] = dataToRead.data[j];
	}
	mData->dataLength = dataToRead.length;
}


//=======================================================================
// ����� ����� ���� ��� �������਩
int PIRIT_IOCLASS::scrollPaper()
{
	int err;
	clearData(SEND);
	dataToSend.data[0] = 0x0A;
	dataToSend.length = 1;
	err = connectSock(); if( err != 0 ) return err;
	err = sendData();
	disconnectSock();
	return err;
}


//=======================================================================
// ����� ����� ���� ��� �������਩
int PIRIT_IOCLASS::loadDataToPirit(int size, unsigned char* data)
{
	int numPack;
	int sizeB;
	int i, err;

	unsigned char dTS[50];
	memcpy(dTS, dataToSend.data, 40);
	err = connectSock(); if( err != 0 ) return err;
	err = sendData();

	if (err == 0)
	{
		err = readAck();
	}
	if (err == 0)
	{
		sizeB = size;
		numPack = size / 4096;
		if((size % 4096) > 0) numPack++;
		for(i = 0; i < numPack; i++)
		{
			memcpy(&dataToSend.data[0], (data + (i * 4096)), ((sizeB > 4096) ? 4096 : sizeB));
			dataToSend.length = ((sizeB > 4096) ? 4096 : sizeB);
			err = sendData();
			sizeB -= 4096;
		}
	}
	memcpy(dataToSend.data, dTS, 40);
	if (err == 0)
	{
		err = readData();
	}
	disconnectSock();

	return err;
}


//=======================================================================
// ����� ����� ���� ��� �������਩
int PIRIT_IOCLASS::loadPictureToPirit(int width, int height, int sm, unsigned char* data)
{
	int i, err;
	int numPack;
	int sizeB;
	unsigned char dTS[80];
	memcpy(dTS, dataToSend.data, 70);

	err = connectSock(); if( err != 0 ) return err;
	err = sendData();
	if (err == 0)
	{
		err = readAck();
	}
	if (err == 0)
	{
		sizeB = width*height/8;
		numPack = sizeB / 4096;
		if((sizeB % 4096) > 0)
		{
			numPack++;
		}
		for(i = 0; i < numPack; i++)
		{
			memcpy(&dataToSend.data[0], (data+(i*4096)), ((sizeB > 4096) ? 4096 : sizeB));
			dataToSend.length = ((sizeB > 4096) ? 4096 : sizeB);
			err = sendData();
			sizeB -= 4096;
		}
	}
	memcpy(dataToSend.data, dTS, 70);
	if (err == 0)
	{
		err = readData();
	}
	disconnectSock();
	return err;
}


//=======================================================================
// ����� ����� ���� ��� �������਩
void PIRIT_IOCLASS::setAmountDecimalPlaces(int decimalPlaces)
{
	AmountDecimalPlaces = decimalPlaces;
}


//=======================================================================
// ����� ����� ���� ��� �������਩
void PIRIT_IOCLASS::setQuantityDecimalPlaces(int decimalPlaces)
{
	QuantityDecimalPlaces = decimalPlaces;
}


//=======================================================================
// ����� ����� ���� ��� �������਩
void PIRIT_IOCLASS::setPercentageDecimalPlaces(int decimalPlaces)
{
	PercentageDecimalPlaces = decimalPlaces;
}


//=======================================================================
// ����� ����� ���� ��� �������਩
int PIRIT_IOCLASS::getAmountDecimalPlaces()
{
	return AmountDecimalPlaces;
}


//=======================================================================
// ����� ����� ���� ��� �������਩
int PIRIT_IOCLASS::getQuantityDecimalPlaces()
{
	return QuantityDecimalPlaces;
}


//=======================================================================
// ����� ����� ���� ��� �������਩
int PIRIT_IOCLASS::getPercentageDecimalPlaces()
{
	return PercentageDecimalPlaces;
}
