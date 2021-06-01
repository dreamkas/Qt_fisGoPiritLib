#include "PiritLib.h"

char BUYER_ADDRESS[MAX_PATH];
unsigned long BUYER_ADDRESS_LENGTH = 0;

// Указатель на нужный вектор ошибок в зависимости от страны


const vector <FB_err>  *FB_errs = &FB_errs_RF;


PLDateTime getDateTime()
{
	PLDateTime dt;
	time_t t;
	struct tm *t_m;
	time(&t);
	t_m = localtime(&t);
	//	printf("Date: %02d.%02d.%4d\n", t_m->tm_mday, t_m->tm_mon + 1, t_m->tm_year + 1900);
	//	printf("Time: %02d:%02d:%02d\n", t_m->tm_hour, t_m->tm_min, t_m->tm_sec);
	dt.year = t_m->tm_year + 1900 - 2000;
	dt.month = t_m->tm_mon + 1;
	dt.day = t_m->tm_mday;
	dt.hours = t_m->tm_hour;
	dt.minutes = t_m->tm_min;
	dt.seconds = t_m->tm_sec;

	//	printf("Date: %02d.%02d.%02d\n", dt.day, dt.month, dt.year);
	//	printf("Time: %02d:%02d:%02d\n", dt.hour, dt.min, dt.sec);
	return dt;
}

/* ------------------------- Commands ----------------------------*/

//Запрос сменных счетчиков и регистров
int libGetCountersAndRegisters_intXd(int requestNumber, int data[], int maxElement, int type)
{
    MData mData;
    mData = pirit_io.commandByte(0x01, requestNumber);
    if (mData.errCode == 0)
    {
        mData.errCode = pirit_io.parseAnswerIntXd(data, maxElement, type);
    }
    return mData.errCode;
}

//Запрос сменных счетчиков и регистров дл  12 типа
int libGetCountersAndRegisters_12(int data[], int maxElement, char *str)
{
    MData mData;
    mData = pirit_io.commandByte(0x01, 12);
    if (mData.errCode == 0)
    {
        mData.errCode = pirit_io.parseAnswerCommand0x01_12(data, maxElement, str);
    }
    return mData.errCode;
}



void setDebugLevel(int level)
{
    pirit_io.setDebugLevel(static_cast<DEBUG_LEVEL>(level));
}

// --------------------------------Команды в ФР -----------------------------------
//Запрос флагов статуса ККТ
MData libGetStatusFlags()
{
    return pirit_io.commandVoidMData(0x00);
}

//Запрос флагов статуса ККТ
int getStatusFlags(int *fatalStatus, int *currentFlagsStatus, int *documentStatus)
{
    MData mData;
    mData = pirit_io.commandVoidMData(0x00);
    if (mData.errCode == 0)
    {
        mData.errCode = pirit_io.parseAnswerCommand0x00(fatalStatus, currentFlagsStatus, documentStatus);
    }
    return mData.errCode;
}

//Запрос сменных счетчиков и регистров
MData libGetCountersAndRegisters(unsigned char numRequest)
{
    return pirit_io.commandByte(0x01, numRequest);
}

//Запрос сменных счетчиков и регистров
int getCountersAndRegisters(int requestNumber, int *data)
{
    int dataInt16[16];
    int result = 1011;
    int maxElements = 0;
    char str[80];

    switch (requestNumber)
    {
        case  1:
        case  2:
            maxElements = 1;
            result = libGetCountersAndRegisters_intXd(requestNumber, &dataInt16[0], maxElements, 1); //(1) Int/Long);
            break;
        case  3:
        case  5:
        case 13:
        case 14:
        case 16:
        case 17:
            maxElements = 16;
            result = libGetCountersAndRegisters_intXd(requestNumber, dataInt16, maxElements, 2); //(2) Float/Double
            break;
        case  4:
        case  6:
            maxElements = 16;
            result = libGetCountersAndRegisters_intXd(requestNumber, dataInt16, maxElements, 1); //(1) Int/Long
            break;
        case  7:
            maxElements = 6;
            result = libGetCountersAndRegisters_intXd(requestNumber, dataInt16, maxElements, 1); //(1) Int/Long
            break;
        case  8:
        case  9:
            maxElements = 4;
            result = libGetCountersAndRegisters_intXd(requestNumber, dataInt16, maxElements, 2); //(2) Float/Double
            break;

        case 10:
        case 11:
            maxElements = 6;
            result = libGetCountersAndRegisters_intXd(requestNumber, dataInt16, maxElements, 2); //(2) Float/Double
            break;
        case 12:
            maxElements = 12;
            result = libGetCountersAndRegisters_12(dataInt16, maxElements, str); //(2) Float/Double
            //			env->SetCharField(obj, strParam01, str);
            //env->ReleaseStringUTFChars(strParam01, str);
            break;

        default:

            break;
    }

    if ( (result == 0) && (maxElements > 0) )
    {
        for (int i = 0; i < maxElements; i++)
        {
            *(data + i) = dataInt16[i];
        }
    }

    return result;
}


//Запрос сведений о ККТ
MData libGetKKTInfo(unsigned char numRequest)
{
    return pirit_io.commandByte(0x02, numRequest);
}


//Запрос сведений о ККТ
int getKKTInfo(unsigned char numRequest, char *data)
{
    return getKKTInfoNum(numRequest, 1, data);
}


int getKKTInfoNum(unsigned char numRequest, int numParam, char *data)
{
    MData mData = pirit_io.commandByte(0x02, numRequest);
    int err = mData.errCode;
    int count = 1;
    if (err == 0)
    {
        switch (numRequest)
        {
            default:
            case 1:
            case 2:
            case 3:
            case 4:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
            case 14:
            case 15:
            case 16:
            case 21:
                count = 1;
                break;
            case 5:
            case 17:
                count = 2;
                break;
            case 23:
                count = 3;
                break;
            case 12:
                count = 4;
                break;

        }
        err = pirit_io.parseAnswerChar(count, numParam > count ? count : numParam, data);
    }
    return err;
}

//Запрос данных по чеку
MData libGetReceiptData(unsigned char numRequest)
{
    return pirit_io.commandByte(0x03, numRequest);
}

//Запрос состо ни  печатающего устройства (ПУ)
MData libGetPrinterStatus()
{
    return pirit_io.commandVoidMData(0x04);
}

int getPrinterStatus(int* result)
{
    MData mData;

    int err;
    mData = pirit_io.commandByteEx(0x04);
    err = mData.errCode;
    if (err == 0)
    {
        err = pirit_io.parseAnswer1Int(result);
    }

    return err;
}

//Запрос сервисной информации П2
MData libGetServiceInfo(unsigned char numRequest)
{
    return pirit_io.commandByte(0x05, numRequest);
}

//Запрос дополнительной информации о ошибках П2
MData libGetExErrorInfo(unsigned char numRequest)
{
    MData mData;
    pirit_io.makeFirstPartPacketToSend(PIRIT_EX_ERROR_INFO);
    pirit_io.addByte(numRequest);
    pirit_io.makeEndPartPacket();

    int err = pirit_io.connectSock();
    if( err != 0 )
    {
        mData.errCode = err;
        mData.dataLength = 0;
        memset(mData.data, 0, sizeof(mData.data));
        return mData;
    }

    err = pirit_io.sendData();
    if (err == 0)
    {
        err = pirit_io.readData();
        if ((err == 0) && (numRequest == PIRIT_PARAM_1))
        {
            int number_error = 0;
            err = pirit_io.parseAnswerN<int> ( PIRIT_PARAM_1, number_error );
            if(err == 0 && number_error == FB_NOT_ERROR)
            {
                const char data[] = "Ошибка. Повторите или обратитесь в техническую поддержку";
                memcpy(mData.data,data,sizeof(data));
            }
            else
            {
                err = pirit_io.parseAnswerN<char> ( PIRIT_PARAM_2, *mData.data);
            }
        }
    }

    mData.errCode = err;
    pirit_io.disconnectSock();
    return mData;
}

bool libIsErrorCritical(const int errCode, FB_OPERATION_CRITICAL_LEVEL opType, std::string& retMess)
{
    // Если сюда попал результат "команда выполнена без ошибок"
    if (errCode == FB_NOT_ERROR)
    {
        retMess = "команда выполнена без ошибок";
        return false;
    }

    if (FB_errs == nullptr)
    {
        printf("%s FB_errs is NULL!", __FUNCTION__);
        return false;
    }
    // Ищем ошибку по коду
    const auto it = std::find_if(begin(*FB_errs), end(*FB_errs), [errCode](const FB_err& fb_err){return (fb_err.code == errCode);});

    if (it != end(*FB_errs))
    {
        FB_err my_err {*it};
        retMess = my_err.mess;
        // тип выполненной операции > уровеня критичности ошибки, то операция выполнена (return FALSE)
        const bool res {(static_cast<int>(opType) <= static_cast<int>(my_err.cLvl))};

        if (pirit_io.DEBUGLEVEL == DEBUG_ON)
        {
            printf("%s:\n\tCommand RES = %d\n-\n" \
                   "\t operation Type = %d  \n-\n" \
                   "\t err Code\t= %d\n-\n" \
                   "\t err Lvl\t= %d\n" \
                   "-\n\t err Mess\t= '%s'\n",
                   __FUNCTION__, res, opType, errCode, my_err.cLvl, retMess.c_str());
        }
        return  res;
    }
    //----------------
    // Если такой ошибки у нас нет, то говорим что операция не выполнена
    if (retMess.empty())
    {
        retMess = "НЕИЗВЕСТНАЯ ОШИБКА! '" + to_string(errCode) + "'";
    }
    return true;
}


//======================================================================================
//Промотка бумаги П2
int scrollPaper()
{
    return pirit_io.scrollPaper();
}

//Начало работы
int libCommandStart(PLDate mpDate, PLTime mpTime)
 {
    int err = 0;
    pirit_io.makeFirstPartPacketToSend(0x10);
    pirit_io.addDate(mpDate);
    pirit_io.addTime(mpTime);
    pirit_io.makeEndPartPacket();
    err = pirit_io.connectSock(); if( err != 0 ) return err;
    err = pirit_io.sendData();
    if (err == 0)
        err = pirit_io.readData();
    pirit_io.disconnectSock();
    return err;
}

MData libReadSettingsTable(unsigned char number, int index)
{
    int err;
    MData mData;
    memset(mData.data, 0, sizeof(mData.data));

    pirit_io.makeFirstPartPacketToSend(0x11);
    pirit_io.addInt(number);
    pirit_io.addInt(index);
    pirit_io.makeEndPartPacket();
    err = pirit_io.connectSock();

    if( err != 0 )
    {
        mData.errCode = err;
        return mData;
    }

    err = pirit_io.sendData();

    if (err == 0)
    {
        if (pirit_io.getPacketMode() == 0)
            err = pirit_io.readData();
    }
    pirit_io.getMData(&mData);
    mData.errCode = err;
    pirit_io.disconnectSock();

    return mData;
}

// Чтение таблицы настроек с возвратом строки
int getStrFromSettingsTable(string &dataTable, uint8_t number, int32_t index, uint16_t numParam)
{
    int err;
    MData mData;
    memset(mData.data, 0, sizeof(mData.data));

    pirit_io.makeFirstPartPacketToSend( PIRIT_READ_TABLE_SETTINGS );
    pirit_io.addInt(number);
    pirit_io.addInt(index);
    pirit_io.makeEndPartPacket();
    err = pirit_io.connectSock(); if( err != 0 ) return err;
    err = pirit_io.sendData();

    if (err == 0)
    {
        if (pirit_io.getPacketMode() == 0)
            err = pirit_io.readData();
    }
    pirit_io.getMData(&mData);
    pirit_io.disconnectSock();

    char data[256];
    memset(data, 0, sizeof(data));

    err = pirit_io.parseAnswerN<char>((PIRIT_PARAM_NUMBER)numParam, *data);

    dataTable = string(data);

    if ( err != 0 )
    {
        printf("getStrFromSettingsTable: \n\t  Parse err =  %d\n", err);
    }

    return err;
}

// Чтение таблицы настроек с возвратом числа или битовой маски
int getIntFromSettingsTable(int &data, uint8_t number, int32_t index, uint16_t numParam)
{
    int err;
    MData mData;
    memset(mData.data, 0, sizeof(mData.data));

    pirit_io.makeFirstPartPacketToSend( PIRIT_READ_TABLE_SETTINGS );
    pirit_io.addInt(number);
    pirit_io.addInt(index);
    pirit_io.makeEndPartPacket();
    err = pirit_io.connectSock(); if( err != 0 ) return err;
    err = pirit_io.sendData();

    if (err == 0)
    {
        if (pirit_io.getPacketMode() == 0)
            err = pirit_io.readData();
    }

    pirit_io.getMData(&mData);
    pirit_io.disconnectSock();

    err = pirit_io.parseAnswerN<int>((PIRIT_PARAM_NUMBER)numParam, data);

    if ( err != 0 )
    {
        printf("getIntFromSettingsTable: \n\t  Parse err =  %d\n", err);
    }

    return err;
}

// Чтение информации из ФН с возвратом строки
int getStrFromInfoFN(uint8_t number, int32_t index, string &dataFN)
{
    int err;
    MData mData;
    memset(mData.data, 0, sizeof(mData.data));

    pirit_io.makeFirstPartPacketToSend( PIRIT_GET_INFO_FROM_FN );
    pirit_io.addByte(number);
    pirit_io.makeEndPartPacket();
    err = pirit_io.connectSock(); if( err != 0 ) return err;
    err = pirit_io.sendData();

    if (err == 0)
    {
        if (pirit_io.getPacketMode() == 0)
            err = pirit_io.readData();
    }
    pirit_io.getMData(&mData);
    pirit_io.disconnectSock();

    char data[256];
    memset(data, 0, sizeof(data));

    err = pirit_io.parseAnswerN<char>((PIRIT_PARAM_NUMBER)index, *data);

    dataFN = string(data);

    if ( err != 0 )
    {
        printf("getStrFromInfoFN: \n\t  Parse err =  %d\n", err);
    }

    return err;
}

//===============================================================
// Чтение информации из ФН с возвратом числа или битовой маски
int getIntFromInfoFN(uint8_t number, int32_t index, int &data)
{
    int err;
    MData mData;
    memset(mData.data, 0, sizeof(mData.data));

    pirit_io.makeFirstPartPacketToSend( PIRIT_GET_INFO_FROM_FN );
    pirit_io.addByte(number);
    pirit_io.makeEndPartPacket();
    err = pirit_io.connectSock(); if( err != 0 ) return err;
    err = pirit_io.sendData();

    if (err == 0)
    {
        if (pirit_io.getPacketMode() == 0)
            err = pirit_io.readData();
    }

    pirit_io.getMData(&mData);
    pirit_io.disconnectSock();

    err = pirit_io.parseAnswerN<int>((PIRIT_PARAM_NUMBER)index, data);

    if ( err != 0 )
    {
        printf("getIntFromInfoFN: \n\t  Parse err =  %d\n", err);
    }

    return err;
}

//===============================================================
//Запись таблицы настроек
int libWriteSettingsTable(unsigned char number, int index, const char* data)
{
    int err;
    pirit_io.makeFirstPartPacketToSend(0x12);
    pirit_io.addInt(number);
    pirit_io.addInt(index);
    pirit_io.addConstChar(data);
    pirit_io.makeEndPartPacket();
    err = pirit_io.connectSock(); if( err != 0 ) return err;
    err = pirit_io.sendData();
    if (err == 0)
        err = pirit_io.readData();
    pirit_io.disconnectSock();
    return err;
}

int libWriteSettingsTable(unsigned char number, int index, uint8_t data)
{
    int err;
    pirit_io.makeFirstPartPacketToSend(PIRIT_WRITE_TABLE_SETTINGS);
    pirit_io.addInt(number);
    pirit_io.addInt(index);
    pirit_io.addByte(data);
    pirit_io.makeEndPartPacket();
    err = pirit_io.connectSock(); if( err != 0 ) return err;
    err = pirit_io.sendData();
    if (err == 0)
        err = pirit_io.readData();
    pirit_io.disconnectSock();
    return err;
}


//===============================================================
//Чтение даты/времени ККТ
MData libGetPiritDateTime()
{
    return pirit_io.commandVoidMData(0x13);
}


//===============================================================
int getPiritDateTime(int* cDate, int* cTime)
{
    MData mData;
    int cData[3];
    mData = pirit_io.commandVoidMData(0x13);
    if (mData.errCode == 0)
    {
        mData.errCode = pirit_io.parseAnswerIntXdEx(cData, 1, 1);
        *cDate = cData[0];
        *cTime = cData[1];
    }
    return mData.errCode;
}



//===============================================================
//Запись даты/времени ККТ
int libSetPiritDateTime(PLDate mpDate, PLTime mpTime)
{
    int err;
    pirit_io.makeFirstPartPacketToSend(0x14);
    pirit_io.addDateTime(mpDate.day, mpDate.month, mpDate.year, mpTime.hours, mpTime.minutes, mpTime.seconds);
    pirit_io.makeEndPartPacket();
    err = pirit_io.connectSock(); if( err != 0 ) return err;
    err = pirit_io.sendData();
    if (err == 0)
    {
        err = pirit_io.readData();
    }
    pirit_io.disconnectSock();
    return err;
}

//===============================================================
//Загрузить логотип в ФР
int libLoadLogo(int size, unsigned char* data)
{
    pirit_io.makeFirstPartPacketToSend(0x15);
    pirit_io.addInt(size);
    pirit_io.makeEndPartPacket();

    return pirit_io.loadDataToPirit(size, data);
}

//===============================================================
//Удалить логотип
int libDeleteLogo()
{
    return pirit_io.commandVoid(0x16);
}

//===============================================================
//Загрузить дизайн чека
int libLoadReceiptDesign(int size, unsigned char* data)
{
    pirit_io.makeFirstPartPacketToSend(0x17);
    pirit_io.addInt(size);
    pirit_io.makeEndPartPacket();

    return pirit_io.loadDataToPirit(size, data);
}

//===============================================================
//Загрузить графическое изображение
int libLoadPicture(int width, int height, int sm, const char* name, int number, unsigned char* data)
{
    pirit_io.makeFirstPartPacketToSend(0x18);
    pirit_io.addInt(width);
    pirit_io.addInt(height);
    pirit_io.addInt(sm);
    pirit_io.addConstChar(name);
    pirit_io.addInt(number);
    pirit_io.makeEndPartPacket();

    return pirit_io.loadPictureToPirit(width, height, sm, data);
}


//===============================================================
//Распечатать отчет без гашения  (X-отчет)
int libPrintXReport(const char* nameCashier)
{
    int err;
    pirit_io.makeFirstPartPacketToSend(0x20);
    pirit_io.addConstChar(nameCashier);
    pirit_io.makeEndPartPacket();
    err = pirit_io.connectSock(); if( err != 0 ) return err;
    err = pirit_io.sendData();
    if (err == 0)
        err = pirit_io.readData();
    pirit_io.disconnectSock();
    return err;
}

//===============================================================
//Закрыть смену (Z-отчет)
int libPrintZReport(const char* nameCashier, int options)
{
    int err;
    pirit_io.makeFirstPartPacketToSend(0x21);
    pirit_io.addConstChar(nameCashier);
    if (options == 11)
    {
        pirit_io.addByte(options);
    }
    pirit_io.makeEndPartPacket();
    err = pirit_io.connectSock(); if( err != 0 ) return err;
    err = pirit_io.sendData();
    if (err == 0)
    {
        err = pirit_io.readData();
    }
    pirit_io.disconnectSock();
    return err;
}

//===============================================================
//Закрыть смену (Z-отчет) для РБ
int libPrintZReport_By(const char* nameCashier)
{
    int err;
    pirit_io.makeFirstPartPacketToSend(0x21);
    pirit_io.addConstChar(nameCashier);
    pirit_io.makeEndPartPacket();

    err = pirit_io.connectSock(); if( err != 0 ) return err;

    err = pirit_io.sendData();
    if (err == 0)
    {
        err = pirit_io.readData();
    }
    pirit_io.disconnectSock();
    return err;
}

//===============================================================
//Открыть смену
int libOpenShift(const char* nameCashier)
{
    int err;
    pirit_io.makeFirstPartPacketToSend(0x23);
    pirit_io.addConstChar(nameCashier);
    pirit_io.makeEndPartPacket();
    err = pirit_io.connectSock(); if( err != 0 ) return err;
    err = pirit_io.sendData();
    if (err == 0)
        err = pirit_io.readData();
    pirit_io.disconnectSock();
    return err;
}


//Открыть смену для РБ
int libOpenShift_By()
{
    int err;
    pirit_io.makeFirstPartPacketToSend(0x23);
    pirit_io.makeEndPartPacket();
    err = pirit_io.connectSock(); if( err != 0 ) return err;
    err = pirit_io.sendData();
    if (err == 0)
        err = pirit_io.readData();
    pirit_io.disconnectSock();
    return err;
}
// формирует чек коррекции с записью в ФН
int libDoCheckCorrection(const char *nameCashier, double cash, double cashless, unsigned char correctionFlags)
{
    int err;
    pirit_io.makeFirstPartPacketToSend(0x58);
    pirit_io.addConstChar(nameCashier); //(Им  оператора)Код и/или им  оператора
    pirit_io.addDouble(cash);
    pirit_io.addDouble(cashless);
    pirit_io.addEmptyByte();
    pirit_io.addEmptyByte();
    pirit_io.addEmptyByte();
    pirit_io.addByte(correctionFlags);
    pirit_io.makeEndPartPacket();
    err = pirit_io.connectSock(); if( err != 0 ) return err;
    err = pirit_io.sendData();
    if (err == 0)
    {
        err = pirit_io.readData();
    }
    pirit_io.disconnectSock();
    return err;
}

int libDoCheckCorrectionEx(const char *nameCashier, double cash, double cashless, double sum1, double sum2, double sum3, unsigned char correctionFlags, const PLDate *docDate, const char *docNumber,
                           const char *correctionName, double sum_18, double sum_10, double sum_0, double sum_WT, double sum_18_118, double sum_10_110)
{
    int err;
    pirit_io.makeFirstPartPacketToSend(0x58);
    pirit_io.addConstChar(nameCashier); //(Им  оператора)Код и/или им  оператора
    pirit_io.addDouble(cash);
    pirit_io.addDouble(cashless);
    pirit_io.addDouble(sum1);
    pirit_io.addDouble(sum2);
    pirit_io.addDouble(sum3);
    pirit_io.addByte(correctionFlags);
    pirit_io.addDate(*docDate);
    pirit_io.addConstChar(docNumber);
    pirit_io.addConstChar(correctionName);
    pirit_io.addDouble(sum_18);
    pirit_io.addDouble(sum_10);
    pirit_io.addDouble(sum_0);
    pirit_io.addDouble(sum_WT);
    pirit_io.addDouble(sum_18_118);
    pirit_io.addDouble(sum_10_110);
    pirit_io.makeEndPartPacket();
    err = pirit_io.connectSock(); if( err != 0 ) return err;
    err = pirit_io.sendData();
    if (err == 0)
    {
        err = pirit_io.readData();
    }
    pirit_io.disconnectSock();
    return err;
}


// Печать отчета о текущем состоянии расчетов
int libCurrentStatusReport(char* nameCashier)
{
    int err;
    pirit_io.makeFirstPartPacketToSend(0x59);
    pirit_io.addConstChar(nameCashier); //(Им  оператора)Код и/или им  оператора
    pirit_io.makeEndPartPacket();
    err = pirit_io.connectSock(); if( err != 0 ) return err;
    err = pirit_io.sendData();
    if (err == 0)
    {
        err = pirit_io.readData();
    }
    pirit_io.disconnectSock();
    return err;
}

//Открыть документ
int libOpenDocument(unsigned char type, unsigned char numDepart, char* nameCashier, long docNumber)
{
    int err;
    pirit_io.makeFirstPartPacketToSend(0x30);
    pirit_io.addByte(type); //(Целое число)Режим и тип документа,
    pirit_io.addByte(numDepart); //(Целое число 1..99)Номер отдела
    pirit_io.addConstChar(nameCashier); //(Им  оператора)Код и/или им  оператора
    pirit_io.addLong(docNumber);
    pirit_io.makeEndPartPacket();
    err = pirit_io.connectSock(); if( err != 0 ) return err;
    err = pirit_io.sendData();
    if (err == 0)
    {
        if ((type & 16) == 16)
            pirit_io.setPacketMode(1);
        err = pirit_io.readData();
    }
    pirit_io.disconnectSock();
    return err;
}

int libOpenDocumentEx(unsigned char type, unsigned char numDepart, char* nameCashier, long docNumber, unsigned char taxN)
{
    int err;
    pirit_io.makeFirstPartPacketToSend(0x30);
    pirit_io.addByte(type); //(Целое число)Режим и тип документа,
    pirit_io.addByte(numDepart); //(Целое число 1..99)Номер отдела
    pirit_io.addConstChar(nameCashier); //(Им  оператора)Код и/или им  оператора
    pirit_io.addLong(docNumber);
    pirit_io.addByte(taxN);
    pirit_io.makeEndPartPacket();
    err = pirit_io.connectSock(); if( err != 0 ) return err;
    err = pirit_io.sendData();
    if (err == 0)
    {
        if ((type & 16) == 16)
            pirit_io.setPacketMode(1);
        err = pirit_io.readData();
    }
    pirit_io.disconnectSock();
    return err;
}

//Установить адрес покупателя
unsigned long libSetBuyerAddress(const char *buyerAddress)
{
    if (buyerAddress != NULL)
    {
        BUYER_ADDRESS_LENGTH = strlen(buyerAddress);

        if (BUYER_ADDRESS_LENGTH > 0)
        {
            strncpy(BUYER_ADDRESS, buyerAddress, MAX_PATH);
        }
    }
    return BUYER_ADDRESS_LENGTH;
}

//Получить адрес покупател
unsigned long libGetBuyerAddress(char *buyerAddress, unsigned long baLength)
{
    unsigned long written = 0;

    if (BUYER_ADDRESS_LENGTH > 0 && baLength > BUYER_ADDRESS_LENGTH)
    {
        strncpy(buyerAddress, BUYER_ADDRESS, baLength);
        written = BUYER_ADDRESS_LENGTH;
    }
    return written;
}

//======================================================
//Стереть адрес покупател
void libCleanBuyerAddress()
{
    BUYER_ADDRESS_LENGTH = 0;
}

//======================================================
CloseDocResult libCloseDocument(const CloseDocParam &closeDocParam)
{
    int err = 0;
    CloseDocResult result;

    pirit_io.makeFirstPartPacketToSend(PIRIT_CLOSE_DOCUMENT);
    pirit_io.addByte(closeDocParam.cutPaper);                   // (Целое число) Флаг отрезки
    pirit_io.addString(closeDocParam.buyerAddress);             // (Строка) Адрес покупателя
    pirit_io.addLong(closeDocParam.flags);                      // (Число) Разные флаги
    pirit_io.addString(closeDocParam.clcPlace);                 // (Строка) Место расчётов
    pirit_io.addString(closeDocParam.recSendAddress);           // (Строка) Адрес отправителя чеков
    pirit_io.addString(closeDocParam.automaticNumber);          // (Строка) Номер автомата
    pirit_io.addString(closeDocParam.addUserReq.first);         // (Строка) Название дополнительного реквизита пользователя
    pirit_io.addString(closeDocParam.addUserReq.second);        // (Строка) Значение дополнительного реквизита пользователя
    pirit_io.addString(closeDocParam.buyerName);                // (Строка)[0..128] Покупатель
    pirit_io.addString(closeDocParam.buyerInn);                 // (Строка)[0..12] ИНН покупателя

    pirit_io.makeEndPartPacket();
    // Открываем в непакетном
    if (pirit_io.getPacketMode() == 0)
    {
        err = pirit_io.connectSock();
    }

    if( err != 0 )
    {
        result.errCode = err;
        pirit_io.setPacketMode(0);
        return result;

    }
    pirit_io.checkBuffer.setAccumulate(false);
    err = pirit_io.sendData();

    if (err == 0)
    {
        err = pirit_io.readData();
        if (err == 0)
        {
            err += pirit_io.parseAnswerN<uint32_t> ( PIRIT_PARAM_3, result.fd);
            err += pirit_io.parseAnswerN<uint32_t> ( PIRIT_PARAM_4, result.fpd);
            err += pirit_io.parseAnswerN<uint32_t> ( PIRIT_PARAM_5, result.shiftNum);
            err += pirit_io.parseAnswerN<uint32_t> ( PIRIT_PARAM_6, result.docInShift);

            char date[16] = {0};
            err += pirit_io.parseAnswerN<char> ( PIRIT_PARAM_7, *date);
            result.dateDoc = string(date);

            char time[16] = {0};
            err += pirit_io.parseAnswerN<char> ( PIRIT_PARAM_8, *time);
            result.timeDoc = string(time);
        }
    }

    result.errCode = err;
    BUYER_ADDRESS_LENGTH = 0;
    pirit_io.setPacketMode(0);
    pirit_io.disconnectSock();

    if (err != 0)
    {
        printf("%s:: Error while closing document!\n", __func__);
    }
    return result;
}

//======================================================
CloseDocResult libCloseDocumentLite(unsigned        char cutPaper,    //  Флаг отрезки
                                    string          buyerAddress,     //  Адрес покупателя
                                    unsigned char   internetSign)     //  Установить признак расчетов в интернет
{
    int err = 0;
    CloseDocResult result;

    pirit_io.makeFirstPartPacketToSend(PIRIT_CLOSE_DOCUMENT);
    pirit_io.addByte(cutPaper);
    if ( !buyerAddress.empty() )
    {
        pirit_io.addChar((char *) buyerAddress.c_str());
    }
    pirit_io.makeEndPartPacket();

    // Открываем в непакетном
    if (pirit_io.getPacketMode() == 0)
    {
        err = pirit_io.connectSock();
    }

    if( err != 0 )
    {
        result.errCode = err;
        pirit_io.setPacketMode(0);
        return result;

    }
    pirit_io.checkBuffer.setAccumulate(false);
    err = pirit_io.sendData();

    if (err == 0)
    {
        err = pirit_io.readData();
    }

    result.errCode = err;
    BUYER_ADDRESS_LENGTH = 0;
    pirit_io.setPacketMode(0);
    pirit_io.disconnectSock();

    if (err != 0)
    {
        printf("%s:: Error while closing document!\n", __func__);
    }
    return result;
}


//======================================================
//Закрыть документ Кайфовый By
MData libCloseDocument_By(unsigned       char cutPaper)
{
    int err = 0;
    MData mData;

    pirit_io.makeFirstPartPacketToSend(PIRIT_CLOSE_DOCUMENT);
    pirit_io.addByte(cutPaper);
    pirit_io.makeEndPartPacket();

    if ( pirit_io.getPacketMode() == 0 )
    {
        err = pirit_io.connectSock();
    }

    if( err != 0 )
    {
        mData.errCode = err;
        mData.dataLength = 1;
        memset(mData.data, 0, sizeof(mData.data));
        return mData;
    }

    err = pirit_io.sendData();
    if ( err == 0 )
    {
        err = pirit_io.readData();
        pirit_io.getMData(&mData);
    }

    mData.errCode = err;
    pirit_io.setPacketMode(0);
    pirit_io.disconnectSock();
    return mData;
}

//======================================================
//Аннулировать документ
int libCancelDocument()
{
    pirit_io.setPacketMode(0);
    return pirit_io.commandVoid(PIRIT_CANCEL_DOCUMENT);
}

int libCancelDocument_By(int numDoc, string nameCashier, double (&arr)[PIRIT_PARAM_16])
{
    int err = 0;

    pirit_io.makeFirstPartPacketToSend(PIRIT_RB_CANCEL_DOC);
    pirit_io.addInt( numDoc );
    for ( uint8_t i=0; i<PIRIT_PARAM_16; ++i )
    {
        pirit_io.addDouble( arr[i] );
    }
    pirit_io.addConstChar( nameCashier.c_str() );
    pirit_io.makeEndPartPacket();

    err = pirit_io.connectSock();
    if( err != 0 )
    {
        return err;
    }

    err = pirit_io.sendData();
    if( err != 0 )
    {
        return err;
    }
    err = pirit_io.readData();
    pirit_io.disconnectSock();

    return err;
}

//Отложить документ
int libPostponeDocument(const char* info)
{
    int err;
    pirit_io.makeFirstPartPacketToSend(0x33);
    pirit_io.addConstChar(info);
    pirit_io.makeEndPartPacket();
    err = pirit_io.connectSock(); if( err != 0 ) return err;
    err = pirit_io.sendData();
    if (err == 0)
    {
        err = pirit_io.readData();
    }
    pirit_io.disconnectSock();
    pirit_io.setPacketMode(0);
    return err;
}

//Отрезать документ
int libCutDocument()
{
    return pirit_io.commandVoid(0x34);
}


//Печать текста любого с заданными аттрибутами
int libPrintString(char           *textStr,                 // Печатаемый текст 0..72 символа
                   FONTS               fnt)                 // аттрибуты из enum FONTS
{
    unsigned char attribute = fnt;
    int err;
    pirit_io.makeFirstPartPacketToSend(0x40);
    pirit_io.addChar(textStr);
    pirit_io.addByte(attribute);
    pirit_io.makeEndPartPacket();
    err = pirit_io.connectSock(); if( err != 0 ) return err;
    err = pirit_io.sendData();
    if (err == 0)
    {
        if (pirit_io.getPacketMode() == 0)
            err = pirit_io.readData();
    }
    pirit_io.disconnectSock();
    return err;
}

//Печать штрих-кода
int libPrintBarCode(unsigned char posText, unsigned char widthBarCode, unsigned char heightBarCode,
                                  unsigned char typeBarCode, const char* barCode)
{
    int err;
    pirit_io.makeFirstPartPacketToSend(0x41);
    pirit_io.addByte(posText);
    pirit_io.addByte(widthBarCode);
    pirit_io.addByte(heightBarCode);
    pirit_io.addByte(typeBarCode);
    pirit_io.addConstChar(barCode);

    pirit_io.makeEndPartPacket();
    err = pirit_io.connectSock(); if( err != 0 ) return err;
    err = pirit_io.sendData();

    if (err == 0)
    {
        if (pirit_io.getPacketMode() == 0)
            err = pirit_io.readData();
    }
    pirit_io.disconnectSock();
    return err;
}

int commandAddDelPosition(int numCom, const char* goodsName, const char* barcode, double quantity, double price,
                          unsigned char taxNumber, int numGoodsPos, unsigned char numDepart,
                          unsigned char CoefficientType, const char *CoefficientName, double CoefficientValue)
{
    int err;
    pirit_io.makeFirstPartPacketToSend(numCom);
    pirit_io.addConstChar(goodsName); //Наименование товара 0..224
    pirit_io.addConstChar(barcode); //Артикул или ШК 0..18
    pirit_io.addDouble(quantity);
    pirit_io.addDouble(price);
    pirit_io.addByte(taxNumber);
    pirit_io.addEmptyByte();
    pirit_io.addByte(numDepart);

    if (CoefficientType > 0)
    {
        pirit_io.addByte(CoefficientType);
        (CoefficientName != NULL) ? pirit_io.addConstChar(CoefficientName) : pirit_io.addEmptyByte();
        pirit_io.addDouble(CoefficientValue);
    }
    else
    {
        pirit_io.addEmptyByte();
        pirit_io.addEmptyByte();
        pirit_io.addEmptyByte();
    }

    pirit_io.makeEndPartPacket();
    err = pirit_io.connectSock(); if( err != 0 ) return err;
    err = pirit_io.sendData();
    if (err == 0) {
        if (pirit_io.getPacketMode() == 0)
            err = pirit_io.readData();
    }
    pirit_io.disconnectSock();
    return err;
}

//------------------------------
// Добавить товарную позицию ЭТА НОРМ!
int PiritLib::libAddPosition(const Position &position)
{
    int err;
    pirit_io.makeFirstPartPacketToSend(PIRIT_ADD_POSITION);
    pirit_io.addString(position.goodsName, LIMIT_GOODS_NAME); //Наименование товара 0..224
    pirit_io.addString(position.barcode, LIMIT_ARTICLE_BARCODE); //Артикул или ШК 0..18
    pirit_io.addDouble(position.quantity);
    pirit_io.addDouble(position.price);
    pirit_io.addByte(position.taxNumber);
    pirit_io.addEmptyByte();
    pirit_io.addByte(position.numDepart);
    pirit_io.addByte(position.coefType);
    pirit_io.addEmptyByte();
    pirit_io.addDouble(position.coefValue);
    pirit_io.addLong(position.signMethodCalculation);
    pirit_io.addLong(position.signCalculationObject);
    pirit_io.addString(position.originCountryCode, LIMIT_ORIGIN_COUNTRY_CODE);
    pirit_io.addString(position.customEntryNum, LIMIT_CUSTOM_ENTRY_NUM);
    pirit_io.addDouble(position.exciseDuty);
    pirit_io.makeEndPartPacket();
    // Открываем сокет в непакетном режиме
    if (pirit_io.getPacketMode() == 0)
    {
        err = pirit_io.connectSock(); if( err != 0 ) return err;
    }
    err = pirit_io.sendData();
    if (err == 0)
    {
        if (pirit_io.getPacketMode() == 0)
        {
            err = pirit_io.readData();
        }
    }
    // Закрываем сокет в непакетном режиме
    if (pirit_io.getPacketMode() == 0)
    {
        pirit_io.disconnectSock();
    }
    return err;
}

//----------------------------------------------
// Выгрузить документ по номеру из КЛ
//----------------------------------------------
int libKLDocRead( long doc_num, std::vector<std::string> &strs )
{
    //------------------------------------------
    int err;
    //------------------------------------------
    pirit_io.makeFirstPartPacketToSend( PIRIT_RB_DOC_FROM_KL_READ );
    pirit_io.addInt( doc_num );
    pirit_io.makeEndPartPacket();
    //------------------------------------------
    err = pirit_io.connectSock(); if( err != 0 ) return err;
    //------------------------------------------
    err = pirit_io.sendData();
    //------------------------------------------
    if (err == 0)
    {
        //------------------------------------------
        err = pirit_io.readData();

        //------------------------------------------
        if(err == 0)
        {
            //------------------------------------------
            int res = 0;
            uint32_t num = 0;

            strs.clear();

            const uint16_t VAL_LEN = 1024;
            char val[VAL_LEN] = {0};
            //------------------------------------------
            while(res == 0)
            {
                memset( val, 0x00, sizeof(val) );
                //------------------------------------------
                res = pirit_io.parseAnswerN<char>( num++, *val );
                //------------------------------------------
                if(res == 0)
                {
                    std::string  str( val );
                    if(pirit_io.DEBUGLEVEL == DEBUG_ON)
                    {
                        printf("%s\n", str.c_str());
                    }
                    strs.push_back( str );
                }
                //------------------------------------------
            }
            //------------------------------------------
        }
        //------------------------------------------
    }
    //------------------------------------------
    pirit_io.disconnectSock();
    //------------------------------------------
    return err;
}
//----------------------------------------------
// Распечатать документ по номеру из КЛ
//----------------------------------------------
int libKLDocPrint( long doc_num )
{
    int err;

    pirit_io.makeFirstPartPacketToSend( PIRIT_RB_DOC_FROM_KL_PRINT );
    pirit_io.addInt( doc_num );
    pirit_io.makeEndPartPacket();

    err = pirit_io.connectSock(); if( err != 0 ) return err;
    err = pirit_io.sendData();

    if (err == 0)
    {
        if (pirit_io.getPacketMode() == 0)
        {
            err = pirit_io.readData();
        }
    }
    pirit_io.disconnectSock();

    return err;
}
//----------------------------------------------
// Добавить товарную позицию РБ
int libAddPositionLarge_By   (const char      *goodsName,  // (Строка[0...224])   Название товара
                              const char        *barcode,  // (Строка[0..18])     Артикул или штриховой код + A-не GTIN, B-GTIN, C-услуга
                              double            quantity,  // (Дробное число)     Количество товара в товарной позиции
                              double               price,  // (Дробное число)     Цена товара по данному артикулу
                              unsigned char    taxNumber,  // (Целое число)       Номер ставки налога
                              int            numGoodsPos,  // (Строка[0..4])      Номер товарной позиции
                              unsigned char    numDepart) // (Целое число 1..16) Номер секции
{
    int err;
    pirit_io.makeFirstPartPacketToSend(PIRIT_ADD_POSITION);
    pirit_io.addConstChar(goodsName);
    pirit_io.addConstChar  (barcode);
    pirit_io.addDouble    (quantity);
    pirit_io.addDouble       (price);
    pirit_io.addByte     (taxNumber);
    pirit_io.addByte   (numGoodsPos);
    pirit_io.addByte     (numDepart);
    pirit_io.makeEndPartPacket();

    if ( pirit_io.getPacketMode() == 0 )
    {
        err = pirit_io.connectSock(); if( err != 0 ) return err;
    }

    err = pirit_io.sendData();

    if ( (err == 0) &&  (pirit_io.getPacketMode() == 0) )
    {
        err = pirit_io.readData();
        pirit_io.disconnectSock();
    }

    return err;
}

//------------------------------
int libSetPosAddReq(string requisite)
{
    pirit_io.makeFirstPartPacketToSend(PIRIT_ADD_POS_REQUISITE);
    pirit_io.addConstChar(requisite.c_str());
    pirit_io.makeEndPartPacket();

    int err;

    if(pirit_io.getPacketMode() == 0)
    {
        err = pirit_io.connectSock();
        if(err != 0)
        {
            return err;
        }
    }

    err = pirit_io.sendData();

    if(pirit_io.getPacketMode() == 0)
    {
        if(err == 0)
        {
            err = pirit_io.readData();
        }
        pirit_io.disconnectSock();
    }

    return err;
}

int libSetPosAddReq( const AdditionalReq &ar )
{
    const uint16_t AGENTS_OFF = 0x00;

    pirit_io.makeFirstPartPacketToSend(PIRIT_ADD_POS_REQUISITE);

    pirit_io.addConstChar         (ar.goodNumCode.c_str());
    pirit_io.addConstChar         (ar.addReq.c_str());
    pirit_io.addConstChar         (ar.scale.c_str());
    if(ar.agentType != AGENTS_OFF)
    {
        pirit_io.addInt           (ar.agentType);
        pirit_io.addConstChar     (ar.providerInn.c_str());
        pirit_io.addConstChar     (ar.providerTel.c_str());
        pirit_io.addConstChar     (ar.providerName.c_str());
        pirit_io.addConstChar     (ar.operatorAddr.c_str());
        pirit_io.addConstChar     (ar.operatorInn.c_str());
        pirit_io.addConstChar     (ar.operatorName.c_str());
        pirit_io.addConstChar     (ar.operatorTel.c_str());
        pirit_io.addConstChar     (ar.plAgentOperation.c_str());
        pirit_io.addConstChar     (ar.plAgentTel.c_str());
        pirit_io.addConstChar     (ar.payOperatorTel.c_str());
    }
    pirit_io.makeEndPartPacket();

    int err;

    if(pirit_io.getPacketMode() == 0)
    {
        err = pirit_io.connectSock();
        if(err != 0)
        {
            return err;
        }
    }

    err = pirit_io.sendData();

    if(pirit_io.getPacketMode() == 0)
    {
        if(err == 0)
        {
            err = pirit_io.readData();
        }
        pirit_io.disconnectSock();
    }

    return err;
}

//------------------------------
int libAddPosition(const char     *goodsName, // (Строка[0...224]) Название товара
                   const char       *barcode, // (Строка[0..18]) Артикул или штриховой код товара/номер ТРК
                   double           quantity, // (Дробное число) Количество товара в товарной позиции
                   double              price, // (Дробное число) Цена товара по данному артикулу
                   unsigned char   taxNumber, // (Целое число) Номер ставки налога
                   int           numGoodsPos, // (Строка[0..4]) Номер товарной позиции
                   unsigned char   numDepart, // (Целое число 1..16) Номер секции
                   unsigned char    coefType, //  0x00
                   const char      *coefName, //   0x00
                   double          coefValue) //  0
{
    return commandAddDelPosition(0x42, goodsName, barcode, quantity, price, taxNumber, numGoodsPos, numDepart, coefType, coefName, coefValue);
}

//Сторнировать товарную позицию
int libDelPosition(const char* goodsName, const char* barcode, double quantity, double price,
                   unsigned char taxNumber, int numGoodsPos, unsigned char numDepart)
{
    return commandAddDelPosition(0x43, goodsName, barcode, quantity, price, taxNumber, numGoodsPos, numDepart, NONE, NULL, 0);
}

// Подитог
int libSubTotal()
{
    int err = 0;

    pirit_io.makeFirstPartPacketToSend(PIRIT_SUB_TOTAL);
    pirit_io.makeEndPartPacket();

    if ( pirit_io.getPacketMode() == 0 )
    {
        err = pirit_io.connectSock(); if( err != 0 ) return err;
    }

    err = pirit_io.sendData();

    if ( (err == 0) &&  (pirit_io.getPacketMode() == 0) )
    {
        err = pirit_io.readData();
        pirit_io.disconnectSock();
    }

    return err;
}


int commandAddDiscMarg(int numCom, unsigned char typeDiscount, const char* nameDiscount, long sum)
{
    int err;
    pirit_io.makeFirstPartPacketToSend(numCom);
    pirit_io.addByte(typeDiscount);
    pirit_io.addConstChar(nameDiscount);

    if (typeDiscount == 0)
    {
        pirit_io.addDoubleFromLong(sum, pirit_io.getPercentageDecimalPlaces());
    }
    else
    {
        pirit_io.setAmountDecimalPlaces( 2 );
        pirit_io.addDoubleFromLong(sum, pirit_io.getAmountDecimalPlaces());
    }

    pirit_io.makeEndPartPacket();

    if ( pirit_io.getPacketMode() == 0 )
    {
        err = pirit_io.connectSock(); if( err != 0 ) return err;
    }

    err = pirit_io.sendData();

    if ( (err == 0) &&  (pirit_io.getPacketMode() == 0) )
    {
        err = pirit_io.readData();
        pirit_io.disconnectSock();
    }

    return err;
}

// Добавить скидку
int libAddDiscount(unsigned char typeDiscount, const char* nameDiscount, long sum)
{
    return commandAddDiscMarg(PIRIT_ADD_DISCOUNT, typeDiscount, nameDiscount, sum);
}

// Добавить наценку
int libAddMargin(unsigned char typeMargin, const char* nameMargin, long sum)
{
    return commandAddDiscMarg(PIRIT_ADD_MARGIN, typeMargin, nameMargin, sum);
}

//===================================
// Добавить тип оплаты Кайфовая
int libAddPayment(unsigned char typePayment,    // (Целое число 0..15) Код типа платежа
                  double                sum,    // (Дробное число) Сумма, принятая от покупателя по данному платежу
                  const char*       infoStr)    // (Строка[0..40]) Дополнительный текст
{
    int err;
    pirit_io.makeFirstPartPacketToSend(PIRIT_ADD_PAYMENT);
    pirit_io.addByte(typePayment);
    pirit_io.addDouble(sum);
    //pirit_io.addDoubleFromInt64(sum, pirit_io.getAmountDecimalPlaces());
    pirit_io.addConstChar(infoStr);
    pirit_io.makeEndPartPacket();

    // Открываем в непакетном
    if (pirit_io.getPacketMode() == 0)
    {
        err = pirit_io.connectSock();
        if (err != 0) return err;
    }

    err = pirit_io.sendData();

    if ( (err == 0) &&  (pirit_io.getPacketMode() == 0) )
    {
        err = pirit_io.readData();
        pirit_io.disconnectSock();
    }

    return err;
}

//===================================
// Добавить тип оплаты Кайфовая для Белоруссии
int libAddPayment_By(unsigned char typePayment,    // (Целое число 0..15) Код типа платежа
                     double                sum,    // (Дробное число)     Сумма, принятая от покупателя по данному платежу
                     const char*       infoStr)    // (Строка[0..40])     Дополнительный текст
{
    int err;
    pirit_io.makeFirstPartPacketToSend(0x47);
    pirit_io.addByte(typePayment);
    pirit_io.addDouble(sum);
    pirit_io.addConstChar(infoStr);
    pirit_io.makeEndPartPacket();

    // Открываем в непакетном
    if (pirit_io.getPacketMode() == 0)
    {
        err = pirit_io.connectSock();
        if (err != 0) return err;
    }

    err = pirit_io.sendData();
    if (err == 0)
    {
        if (pirit_io.getPacketMode() == 0)
        {
            err = pirit_io.readData();
        }
    }

    // Закрываем в непакетном
    if (pirit_io.getPacketMode() == 0)
    {
        pirit_io.disconnectSock();
    }

    return err;
}

//===================================
// Добавить тип оплаты
int libAddPaymentD(unsigned char typePayment, double sum, const char* infoStr)
{
    int err;
    pirit_io.makeFirstPartPacketToSend(0x47);
    pirit_io.addByte(typePayment);
    pirit_io.addDouble(sum);
    pirit_io.addConstChar(infoStr);
    pirit_io.makeEndPartPacket();

    err = pirit_io.connectSock(); if( err != 0 ) return err;
    err = pirit_io.sendData();
    if (err == 0)
    {
        if (pirit_io.getPacketMode() == 0)
        {
            err = pirit_io.readData();
        }
    }
    pirit_io.disconnectSock();
    return err;
}

//Внесение / изъ тие суммы
int libCashInOut(const char* infoStr, double sum)
{
    int err;
    pirit_io.makeFirstPartPacketToSend(0x48);
    pirit_io.addConstChar(infoStr);
    pirit_io.addDouble(sum);
    pirit_io.makeEndPartPacket();

    if ( pirit_io.getPacketMode() == 0 )
    {
        err = pirit_io.connectSock();
        if( err != 0 )
        {
            return err;
        }
    }

    err = pirit_io.sendData();

    if (err == 0)
    {
        if (pirit_io.getPacketMode() == 0)
        {
            err = pirit_io.readData();
        }
    }

    if (pirit_io.getPacketMode() == 0)
    {
        pirit_io.disconnectSock();
    }
    return err;
}

//Печать реквизита
int libPrintRequisite(unsigned char codeReq, unsigned char attributeText,
                      const char* str1, const char* str2,
                      const char* str3, const char* str4)
{
    int err;
    pirit_io.makeFirstPartPacketToSend(PIRIT_PRINT_REQUISITE);

    pirit_io.addByte(codeReq);
    pirit_io.addByte(attributeText);
    pirit_io.addConstChar(str1);
    pirit_io.addConstChar(str2);
    pirit_io.addConstChar(str3);
    pirit_io.addConstChar(str4);

    pirit_io.makeEndPartPacket();

    // Открываем сокет в непакетном режиме
    if (pirit_io.getPacketMode() == PACKET_OFF)
    {
        err = pirit_io.connectSock(); if( err != 0 ) return err;
    }
    err = pirit_io.sendData();
    if (err == 0)
    {
        if (pirit_io.getPacketMode() == PACKET_OFF)
        {
            err = pirit_io.readData();
        }
    }

    // Закрываем сокет в непакетном режиме
    if (pirit_io.getPacketMode() == PACKET_OFF)
    {
        pirit_io.disconnectSock();
    }

    return err;
}

//Печать реквизита дл  ОФД
int libPrintRequsitOFD(int codeReq, unsigned char attributeText, const char* reqName, const char* reqStr)
{
    if ( (reqName == nullptr) || (reqStr == nullptr) )
    {
        return EXIT_FAILURE;
    }

    int err;
    pirit_io.makeFirstPartPacketToSend(PIRIT_PRINT_REQUISIT_OFD);
    pirit_io.addInt(codeReq);
    pirit_io.addByte(attributeText);
    pirit_io.addConstChar(reqName);
    pirit_io.addConstChar(reqStr);
    pirit_io.makeEndPartPacket();

    // Открываем в непакетном
    if ( pirit_io.getPacketMode() == 0 )
    {
        err = pirit_io.connectSock();

        if (err != 0)
        {
            return err;
        }
    }

    err = pirit_io.sendData();

    if ( err == 0 )
    {
        if ( pirit_io.getPacketMode() == 0 )
        {
            err = pirit_io.readData();
        }
    }

    if ( pirit_io.getPacketMode() == 0 )
    {
        pirit_io.disconnectSock();
    }

    return err;
}

//Зарегистрировать сумму по отделу
int libRegisterSumToDepart(unsigned char typeOperation, unsigned char numberDepart, long sum)
{
    int err;
    pirit_io.makeFirstPartPacketToSend(0x50);
    pirit_io.addByte(typeOperation);
    pirit_io.addByte(numberDepart);
    pirit_io.addDoubleFromLong(sum, pirit_io.getAmountDecimalPlaces());
    pirit_io.makeEndPartPacket();

    err = pirit_io.connectSock(); if( err != 0 ) return err;
    err = pirit_io.sendData();
    if (err == 0)
    {
        if (pirit_io.getPacketMode() == 0)
        {
            err = pirit_io.readData();
        }
    }
    pirit_io.disconnectSock();

    return err;
}

//Зарегистрировать сумму налога
int libRegisterTaxSum(unsigned char numberTax, long sum)
{
    int err;
    pirit_io.makeFirstPartPacketToSend(0x51);
    pirit_io.addByte(numberTax);
    pirit_io.addDoubleFromLong(sum, pirit_io.getAmountDecimalPlaces());
    pirit_io.makeEndPartPacket();

    err = pirit_io.connectSock(); if( err != 0 ) return err;
    err = pirit_io.sendData();
    if (err == 0)
    {
        if (pirit_io.getPacketMode() == 0)
        {
            err = pirit_io.readData();
        }
    }
    pirit_io.disconnectSock();

    return err;
}

//Сравнить сумму по чеку
int libCompareSum(long sum)
{
	int err;
	pirit_io.makeFirstPartPacketToSend(0x52);
	pirit_io.addDoubleFromLong(sum, pirit_io.getAmountDecimalPlaces());
	pirit_io.makeEndPartPacket();

	err = pirit_io.connectSock(); if( err != 0 ) return err;
	err = pirit_io.sendData();
	if (err == 0)
	{
		if (pirit_io.getPacketMode() == 0)
		{
			err = pirit_io.readData();
		}
	}
	pirit_io.disconnectSock();

	return err;
}

//Открыть копию чека
int libOpenCopyReceipt(unsigned char type, unsigned char numDepart, const char* nameCashier,
					   int numCheck, int numCash, PLDate mpDate, PLTime mpTime)
{
	int err;
	pirit_io.makeFirstPartPacketToSend(0x53);
	pirit_io.addByte(type);
	pirit_io.addByte(numDepart);
	pirit_io.addConstChar(nameCashier);
	pirit_io.addInt(numCheck);
	pirit_io.addInt(numCash);
	pirit_io.addDateTime(mpDate.day, mpDate.month, mpDate.year,
						 mpTime.hours, mpTime.minutes, mpTime.seconds);
	pirit_io.makeEndPartPacket();

	err = pirit_io.connectSock(); if( err != 0 ) return err;
	err = pirit_io.sendData();
	if (err == 0)
	{
		if (pirit_io.getPacketMode() == 0)
		{
			err = pirit_io.readData();
		}
	}
	pirit_io.disconnectSock();
	return err;
}


//Обнулить наличные в денежном  щике
int libSetToZeroCashInCashDrawer()
{
	return pirit_io.commandVoid(0x54);
}

//Печать графики в документе
int libPrintPictureInDocument(int width, int height, int sm, unsigned char* data)
{
	pirit_io.makeFirstPartPacketToSend(0x55);
	pirit_io.addInt(width);
	pirit_io.addInt(height);
	pirit_io.addInt(sm);
	pirit_io.makeEndPartPacket();
	return pirit_io.loadPictureToPirit(width, height, sm, data);
}

//Печать загруженной картинки
int libPrintPreloadedPicture(int sm, int number)
{
	int err;
	pirit_io.makeFirstPartPacketToSend(0x56);
	pirit_io.addInt(sm);
	pirit_io.addInt(number);
	pirit_io.makeEndPartPacket();

	err = pirit_io.connectSock(); if( err != 0 ) return err;
	err = pirit_io.sendData();
	if (err == 0)
	{
		if (pirit_io.getPacketMode() == 0)
		{
			err = pirit_io.readData();
		}
	}
	pirit_io.disconnectSock();
	return err;
}

//Технологическое обнуление
int libTechnologicalReset(const PLDateTime *dateTime)
{
	int err;
	pirit_io.makeFirstPartPacketToSend(0x63);
	if (dateTime != NULL)
	{
		pirit_io.addDateTime((int)dateTime->day,   (int)dateTime->month,        dateTime->year,
							 (int)dateTime->hours, (int)dateTime->minutes, (int)dateTime->seconds);
	}
	else
	{
		time_t dt;
		time(&dt);
		tm dts;
		localtime_r(&dt, &dts);
		pirit_io.addDateTime(dts.tm_mday, dts.tm_mon + 1, dts.tm_year + 1900, dts.tm_hour, dts.tm_min, dts.tm_sec);
	}
	pirit_io.makeEndPartPacket();

	err = pirit_io.connectSock(); if( err != 0 ) return err;
	err = pirit_io.sendData();
	pirit_io.readData();
	pirit_io.disconnectSock();
	return err;
}

//Фискализация /Перерегистрация
int libFiscalization(const char *oldPassword, const char *regNumber, const char *INN, const char *newPassword)
{
	int err;
	pirit_io.makeFirstPartPacketToSend(0x60);
	pirit_io.addConstChar(oldPassword);
	pirit_io.addConstChar(regNumber);
	pirit_io.addConstChar (INN);
	pirit_io.addConstChar(newPassword);
	pirit_io.makeEndPartPacket();

	err = pirit_io.connectSock(); if( err != 0 ) return err;
	err = pirit_io.sendData();
	if (err == 0)
	{
		err = pirit_io.readData();
	}
	pirit_io.disconnectSock();
	return err;
}

//Фискализаци /Перерегистрация
int libRegistration(unsigned char type, const char *regNumber, const char *INN, int systemTax, int rej, const char *cashierName)
{
	int err;
	pirit_io.makeFirstPartPacketToSend(0x60);
	pirit_io.addByte(type);
	pirit_io.addConstChar(regNumber);
	pirit_io.addConstChar(INN);
	pirit_io.addInt(systemTax);
	pirit_io.addInt(rej);
	pirit_io.addConstChar(cashierName);
	pirit_io.makeEndPartPacket();

	err = pirit_io.connectSock(); if( err != 0 ) return err;
	err = pirit_io.sendData();
	if (err == 0)
	{
		err = pirit_io.readData();
	}
	pirit_io.disconnectSock();

	return err;
}

int libRegistrationEx(unsigned char type, const char *regNumber, const char *INN, int systemTax, int rej, const char *cashierName,
					  PLDateTime regDateTime, const char *userName1, const char *userName2,
					  const char *addressSettle1, const char *addressSettle2, const char *placeSettle,
					  const char *autoNumber, const char *OFDName, const char *OFDINN, const char *senderEmail,
					  const char *FNSEmail, bool automatic)
{
	int err;
	pirit_io.makeFirstPartPacketToSend(0x60);
	pirit_io.addByte(type);
	pirit_io.addConstChar(regNumber);
	pirit_io.addConstChar(INN);
	pirit_io.addInt(systemTax);
	pirit_io.addInt(rej);
	pirit_io.addConstChar(cashierName);
	pirit_io.addDateTime(regDateTime.day, regDateTime.month, regDateTime.year,
						 regDateTime.hours, regDateTime.minutes, regDateTime.seconds);
	pirit_io.addConstChar(userName1);
	pirit_io.addConstChar(userName2);
	pirit_io.addConstChar(addressSettle1);
	pirit_io.addConstChar(addressSettle2);
	pirit_io.addConstChar(placeSettle);
	pirit_io.addConstChar(autoNumber);
	pirit_io.addConstChar(OFDName);
	pirit_io.addConstChar(OFDINN);
	pirit_io.addConstChar(senderEmail);
	pirit_io.addConstChar(FNSEmail);
	(automatic) ? pirit_io.addByte(1) : pirit_io.addByte(0);
	pirit_io.makeEndPartPacket();

	err = pirit_io.connectSock(); if( err != 0 ) return err;
	err = pirit_io.sendData();
	if (err == 0)
	{
		err = pirit_io.readData();
	}
	pirit_io.disconnectSock();

	return err;
}

//Распечатать фискальный отчет по сменам
int libPrintFiscalReportByShifts(unsigned char typeReport, int startShiftNumber, int endShiftNumber, const char *password)
{
	int err;
	pirit_io.makeFirstPartPacketToSend(0x61);
	pirit_io.addByte(typeReport);
	pirit_io.addInt(startShiftNumber);
	pirit_io.addInt(endShiftNumber);
	pirit_io.addConstChar(password);
	pirit_io.makeEndPartPacket();

	err = pirit_io.connectSock(); if( err != 0 ) return err;
	err = pirit_io.sendData();
	if (err == 0)
	{
		err = pirit_io.readData();
	}
	pirit_io.disconnectSock();
	return err;
}

//Распечатать фискальный отчет по датам
int libPrintFiscalReportByDate(unsigned char typeReport, PLDate startDate, PLDate endDate, const char *password, const char *cashier)
{
	int err;
    pirit_io.makeFirstPartPacketToSend(PIRIT_FISCAL_REPORT_BY_DATE);
	pirit_io.addByte(typeReport);
	pirit_io.addDate(startDate);
	pirit_io.addDate(endDate);
	pirit_io.addConstChar(password);
    pirit_io.addConstChar(cashier);
	pirit_io.makeEndPartPacket();
	err = pirit_io.connectSock(); if( err != 0 ) return err;
	err = pirit_io.sendData();
	if (err == 0)
	{
		err = pirit_io.readData();
	}
	pirit_io.disconnectSock();

	return err;
}

//Активизаци  ЭКЛЗ
int libActivizationECT()
{
	return pirit_io.commandVoid(0x70);
}

//Закрытие архива ЭКЛЗ
int libCloseArchiveECT()
{
	return pirit_io.commandVoid(0x71);
}

int libCloseFN(const char *cashierName, uint32_t& fd, uint32_t& fpd, string& dateTime)
{
	int err;
    pirit_io.makeFirstPartPacketToSend(PIRIT_CLOSE_FN);
	pirit_io.addConstChar(cashierName);
	pirit_io.makeEndPartPacket();

	err = pirit_io.connectSock(); if( err != 0 ) return err;
	err = pirit_io.sendData();
	if (err == 0)
	{
		err = pirit_io.readData();
        if (err == 0)
        {
            char dateCloseFN[20] = {0};
            char timeCloseFN[20] = {0};
            err += pirit_io.parseAnswerN<uint32_t> ( PIRIT_PARAM_0, fd);
            err += pirit_io.parseAnswerN<uint32_t> ( PIRIT_PARAM_1, fpd);
            err += pirit_io.parseAnswerN<char> ( PIRIT_PARAM_2, *dateCloseFN);
            err += pirit_io.parseAnswerN<char> ( PIRIT_PARAM_3, *timeCloseFN);

            dateTime = string(dateCloseFN);
            dateTime += string(timeCloseFN);
        }
	}
	pirit_io.disconnectSock();

	return err;
}

//Распечатать контрольную ленту из ЭКЛЗ
int libPrintControlTapeFromECT(int shiftNumber)
{
	int err;
	pirit_io.makeFirstPartPacketToSend(0x72);
	pirit_io.addLong(shiftNumber);
	pirit_io.makeEndPartPacket();

	err = pirit_io.connectSock(); if( err != 0 ) return err;
	err = pirit_io.sendData();
	if (err == 0)
	{
		err = pirit_io.readData();
	}
	pirit_io.disconnectSock();

	return err;
}

//Распечатать документ из ЭКЛЗ
int libPrintDocumentFromECT(int KPKNumber)
{
	int err;
	pirit_io.makeFirstPartPacketToSend(0x73);
	pirit_io.addLong(KPKNumber);
	pirit_io.makeEndPartPacket();

	err = pirit_io.connectSock(); if( err != 0 ) return err;
	err = pirit_io.sendData();
	if (err == 0)
	{
		err = pirit_io.readData();
	}
	pirit_io.disconnectSock();

	return err;
}

//Распечатать отчет по сменам из ЭКЛЗ
int libPrintReportFromECTByShifts(unsigned char typeReport, int startShiftNumber, int endShiftNumber)
{
	int err;
	pirit_io.makeFirstPartPacketToSend(0x74);
	pirit_io.addByte(typeReport);
	pirit_io.addLong(startShiftNumber);
	pirit_io.addLong(endShiftNumber);
	pirit_io.makeEndPartPacket();

	err = pirit_io.connectSock(); if( err != 0 ) return err;
	err = pirit_io.sendData();
	if (err == 0)
	{
		err = pirit_io.readData();
	}
	pirit_io.disconnectSock();

	return err;
}

//Распечатать отчет по датам из ЭКЛЗ
int libPrintReportFromECTByDate(unsigned char typeReport, PLDate startDate, PLDate endDate)
{
	int err;
	pirit_io.makeFirstPartPacketToSend(0x75);
	pirit_io.addByte(typeReport);
	pirit_io.addDate(startDate);
	pirit_io.addDate(endDate);
	pirit_io.makeEndPartPacket();

	err = pirit_io.connectSock(); if( err != 0 ) return err;
	err = pirit_io.sendData();
	if (err == 0)
	{
		err = pirit_io.readData();
	}
	pirit_io.disconnectSock();

	return err;
}

//Распечатать отчет по активизации ЭКЛЗ
int libPrintReportActivizationECT()
{
	return pirit_io.commandVoid(0x76);
}

//Распечатать отчет по смене из ЭКЛЗ
int libPrintReportFromECTByShift(int shiftNumber)
{
	int err;
	pirit_io.makeFirstPartPacketToSend(0x77);
	pirit_io.addInt(shiftNumber);
	pirit_io.makeEndPartPacket();

	err = pirit_io.connectSock(); if( err != 0 ) return err;
	err = pirit_io.sendData();
	if (err == 0)
	{
		err = pirit_io.readData();
	}
	pirit_io.disconnectSock();

	return err;
}

//Запрос информации из ЭКЛЗ
MData libGetInfoFromECT(unsigned char number, long dataL1, long dataL2)
{
	int err;
	MData mData;
	pirit_io.makeFirstPartPacketToSend(PIRIT_GET_INFO_FROM_FN);
	pirit_io.addByte(number);
	if ((number >= 10) && (number < 14))
		pirit_io.addLong(dataL1);
	if (number == 13)
		pirit_io.addLong(dataL2);
	pirit_io.makeEndPartPacket();

    err = pirit_io.connectSock();
    if( err != 0 )
    {
        mData.errCode = err;
        mData.dataLength = 1;
        memset(mData.data, 0, sizeof(mData.data));
        return mData;

    }
	err = pirit_io.sendData();
	if (err == 0)
	{
		if (pirit_io.getPacketMode() == 0)
		{
			err = pirit_io.readData();
		}
	}
	pirit_io.getMData(&mData);
	pirit_io.disconnectSock();

	mData.errCode = err;
	return mData;
}

//Запрос информации из ЭКЛЗ
int libGetInfoFromECT_NumberDoc(int *numDoc)
{
	int err;
	MData mData;
	pirit_io.makeFirstPartPacketToSend(PIRIT_GET_INFO_FROM_FN);
	pirit_io.addByte(3);
	pirit_io.makeEndPartPacket();

	err = pirit_io.connectSock(); if( err != 0 ) return err;
	err = pirit_io.sendData();
	if (err == 0)
	{
		err = pirit_io.readData();
	}
	pirit_io.getMData(&mData);

	int a[3];
	mData.errCode = pirit_io.parseAnswerIntXd(&a[0], 1, 1);
	*numDoc = a[0];
	mData.errCode = err;
	pirit_io.disconnectSock();
	return mData.errCode;
}


/*!
 * Получить номер смены
 * @param[out] shiftNumber
 * @return код ошибки
 */
int libGetInfoFromECT_ShiftNumber(int *shiftNumber)
{
	int err;
	int a[3];
	pirit_io.makeFirstPartPacketToSend(PIRIT_GET_INFO_FROM_FN);
	pirit_io.addByte(6);
	pirit_io.makeEndPartPacket();
	err = pirit_io.connectSock(); if( err != 0 ) return err;
	err = pirit_io.sendData();
	if (err == 0) err = pirit_io.readData();
	if (err == 0) err = pirit_io.parseAnswerIntXd(a, 3, 1);
	if (err == 0) *shiftNumber = a[0];
	pirit_io.disconnectSock();
	return err;
}


//Запрос информации из ЭКЛЗ
int libGetInfoFromECT_NumberFP(char *data)
{
	int err;
	MData mData;
	pirit_io.makeFirstPartPacketToSend(PIRIT_GET_INFO_FROM_FN);
	pirit_io.addByte(1);
	pirit_io.makeEndPartPacket();

	err = pirit_io.connectSock(); if( err != 0 ) return err;
	err = pirit_io.sendData();
	if (err == 0)
	{
		err = pirit_io.readData();
	}
	pirit_io.getMData(&mData);
	err = pirit_io.parseAnswer1Char(data);
	mData.errCode = err;
	pirit_io.disconnectSock();

	return mData.errCode;
}

//Запрос информации из ЭКЛЗ
int libGetInfoFromECT_FP(int numDoc, char *data)
{
	int err;
	MData mData;
	pirit_io.makeFirstPartPacketToSend(PIRIT_GET_INFO_FROM_FN);
	pirit_io.addByte(11);
	pirit_io.addLong(numDoc);
	pirit_io.makeEndPartPacket();

	err = pirit_io.connectSock(); if( err != 0 ) return err;
	err = pirit_io.sendData();
	if (err == 0)
	{
		err = pirit_io.readData();
	}
	pirit_io.getMData(&mData);
	pirit_io.parseAnswer1CharEx(data);
	mData.errCode = err;
	pirit_io.disconnectSock();
	return mData.errCode;
}

//Открыть денежный  щик
int libOpenCashDrawer(int pulseDuration)
{
	int err;
	pirit_io.makeFirstPartPacketToSend(0x80);
	pirit_io.addInt(pulseDuration);
	pirit_io.makeEndPartPacket();

	err = pirit_io.connectSock(); if( err != 0 ) return err;
	err = pirit_io.sendData();
	if (err == 0)
	{
		err = pirit_io.readData();
	}
	pirit_io.disconnectSock();

	return err;
}

//Получить статус денежного  щика
MData libGetCashDrawerStatus()
{
	return pirit_io.commandVoidMData(0x81);
}

//Получить статус денежного  щика
int getCashDrawerStatus(int *drawerStatus)
{
	return pirit_io.commandVoidInt(0x81, drawerStatus);
}

//Подать звуковой сигнал
int libBeep(int duration)
{
	int err;
	pirit_io.makeFirstPartPacketToSend(0x82);
	pirit_io.addInt(duration);
	pirit_io.makeEndPartPacket();

	err = pirit_io.connectSock(); if( err != 0 ) return err;
	err = pirit_io.sendData();
	if (err == 0)
	{
		err = pirit_io.readData();
	}
	pirit_io.disconnectSock();

	return err;
}

//Авторизация  ККТ
int libAuthorization(PLDate mpDate, PLTime mpTime, const char *numKKT)
{
	int err;
	pirit_io.makeFirstPartPacketToSend(0x90);
	pirit_io.addDateTime(mpDate.day, mpDate.month, mpDate.year, mpTime.hours, mpTime.minutes, mpTime.seconds);
	pirit_io.addConstChar(numKKT);
	pirit_io.makeEndPartPacket();

	err = pirit_io.connectSock(); if( err != 0 ) return err;
	err = pirit_io.sendData();
	if (err == 0)
	{
		err = pirit_io.readData();
	}
	pirit_io.disconnectSock();

	return err;
}

//Чтение блока пам ти ККТ
MData libReadMemoryBlock(unsigned char type, long startAdress, long numBytes)
{
    // Блокировка мютексом не нужна (вызывается внутри других методов)
	int err;
	MData mData;
	pirit_io.makeFirstPartPacketToSend(0x91);
	pirit_io.addByte(type);
	pirit_io.addLong(startAdress);
	pirit_io.addLong(numBytes);
	pirit_io.makeEndPartPacket();

    err = pirit_io.connectSock();
    if( err != 0 )
    {
        mData.errCode = err;
        mData.dataLength = 1;
        memset(mData.data, 0, sizeof(mData.data));
        return mData;
    }
	err = pirit_io.sendData();
	if (err == 0)
	{
		err = pirit_io.readData();
	}
	pirit_io.disconnectSock();
	pirit_io.getMData(&mData);
	mData.errCode = err;
	return mData;
}

//Распечатать сервисные данные
int libPrintServiceData()
{
	return pirit_io.commandVoid(PIRIT_PRINT_SERVICE_DATA);
}

//Распечатать сервисные данные
int libPrintServiceDataSKNO_By()
{
    return pirit_io.commandVoid(PIRIT_PRINT_SERVICE_DATA_SKNO);
}


//Очищение фискальной пам ти
int command0x9A()
{
	return pirit_io.commandVoid(0x9A);
}

//Очищение ЭКЛЗ
int command0x9B()
{
	return pirit_io.commandVoid(0x9B);
}

//Аварийное закрытие смены
int libEmergencyCloseShift()
{
	return pirit_io.commandVoid(0xA0);
}

//Печать копии последнего Z-отчета
int libPrintCopyLastZReport()
{
	return pirit_io.commandVoid(0xA1);
}

//Включение сервисного канала к ЭКЛЗ
int libEnableServiceChannelToECT()
{
	return pirit_io.commandVoid(0xA2);
}
//Печать копии чека фискализации/перерегистрации
int libPrintCopyReportFiscalization()
{
	return pirit_io.commandVoid(0xA3);
}


void libSetIP(std::string newIP)
{
    pirit_io.setIP_io(  newIP);
}


void libSetPort(int newPort)
{
    pirit_io.setPort_io(newPort);
}

// Выбор пароля в пакете взависимости от страны
void libSetPasswd(Country newCountry)
{
    pirit_io.setPasswd(newCountry);
}

// Выбор вектора ошибок взависимости от страны
void libSetErrors(Country newCountry)
{
    switch (newCountry)
    {
        case  Country::RF:
        {
            FB_errs = &FB_errs_RF;
            break;
        }
        case  Country::RB:
        {
            FB_errs = &FB_errs_RB;
            break;
        }
        default:
        {
            printf("libSetErrors:: Error, Unknown Country!!!!!");
            break;
        }
    }
}

//Начало работы
int commandStart()
{
	PLDate mpDate;
	PLTime mpTime;
	getCurMPTime(&mpDate, &mpTime);
	int err = 0;
	pirit_io.makeFirstPartPacketToSend(0x10);
	pirit_io.addDate(mpDate);
	pirit_io.addTime(mpTime);
	pirit_io.makeEndPartPacket();
	err = pirit_io.connectSock(); if( err != 0 ) return err;
	err = pirit_io.sendData();
	if (err == 0)
	{
		err = pirit_io.readData();
	}
	else
	{
		printf("commandStart(): ERROR SendData() = %d\n", err);
	}
	pirit_io.disconnectSock();

	return err;
}

//Распечатать документы из СКЛ
int libPrintDocsFromECTSDByNumberDoc(int startNumber, int endNumber)
{
	int err;
	pirit_io.makeFirstPartPacketToSend(0xB1);
	pirit_io.addInt(startNumber);
	pirit_io.addInt(endNumber);
	pirit_io.makeEndPartPacket();

	err = pirit_io.connectSock(); if( err != 0 ) return err;
	err = pirit_io.sendData();
	if (err == 0)
	{
		err = pirit_io.readData();
	}
	pirit_io.disconnectSock();

	return err;
}

//Распечатать данные по сменам из СКЛ
int libPrintDocsFromECTSDByNumberShift(int startNumber, int endNumber)
{
	int err;
	pirit_io.makeFirstPartPacketToSend(0xB2);
	pirit_io.addInt(startNumber);
	pirit_io.addInt(endNumber);
	pirit_io.makeEndPartPacket();

	err = pirit_io.connectSock(); if( err != 0 ) return err;
	err = pirit_io.sendData();
	if (err == 0)
	{
		err = pirit_io.readData();
	}
	pirit_io.disconnectSock();

	return err;
}

//Распечатать данные по датам из СКЛ
int libPrintDocsFromECTSDByDate(PLDate mpDateStart, PLDate mpDateEnd)
{
	int err;
	pirit_io.makeFirstPartPacketToSend(0xB3);
	pirit_io.addDate(mpDateStart);
	pirit_io.addDate(mpDateEnd);
	pirit_io.makeEndPartPacket();

	err = pirit_io.connectSock(); if( err != 0 ) return err;
	err = pirit_io.sendData();
	if (err == 0)
	{
		err = pirit_io.readData();
	}
	pirit_io.disconnectSock();
	return err;
}

//Распечатать контрольную ленту из ЭКЛЗ
int libBLRPrintControlTapeFromECT()
{
	int err;
	pirit_io.makeFirstPartPacketToSend(0x22);
	pirit_io.makeEndPartPacket();

	err = pirit_io.connectSock(); if( err != 0 ) return err;
	err = pirit_io.sendData();
	if (err == 0)
	{
		err = pirit_io.readData();
	}
	pirit_io.disconnectSock();
	return err;
}
int getCurMPTime(PLDate *mpDate, PLTime *mpTime)
{
	PLDateTime dt;
	dt = getDateTime();
	mpDate->day = dt.day;
	mpDate->month = dt.month;
	mpDate->year = dt.year;
	mpTime->hours = dt.hours;
	mpTime->minutes = dt.minutes;
	mpTime->seconds = dt.seconds;
	return 0;
}

int saveLogoToFile(char *fileName)
{
	const int sizeBMPHeader = 0x3E;
	MData mData;
	mData = libReadMemoryBlock(6, 0, 64);
	if (mData.errCode == 0)
	{
		long sm;
		char tempStr[8];
		char *tempPnt;
		int type = (mData.data[7] - 0x30);

		tempStr[0] = mData.data[14];
		tempStr[1] = mData.data[15];
		tempStr[2] = mData.data[12];
		tempStr[3] = mData.data[13];
		tempStr[4] = mData.data[10];
		tempStr[5] = mData.data[11];
		tempStr[6] = mData.data[8];
		tempStr[7] = mData.data[9];
		sm = strtol(tempStr, &tempPnt, 16);

		mData = libReadMemoryBlock(type, sm, 2);
		if (mData.errCode == 0)
		{
			tempStr[0] = mData.data[6];
			tempStr[1] = mData.data[7];
			tempStr[2] = 0;
			tempStr[3] = 0;
			tempStr[4] = 0;
			tempStr[5] = 0;
			tempStr[6] = 0;
			tempStr[7] = 0;

			long heightPict = strtol(tempStr, &tempPnt, 16);
			unsigned char bmpHeader[sizeBMPHeader] = { 0x42,0x4D,
													   0x00,0x00,0x00,0x00, //! 02(4) Размер файла
													   0x00,0x00,0x00,0x00, // 06(4) Резерв
													   0x3E,0x00,0x00,0x00, // 10(4) Смещение, с которого начинаетс  само изображение
													   0x28,0x00,0x00,0x00, // 14(4) Размер заголовка BITMAP (в байтах) равно 40
													   0x40,0x02,0x00,0x00, // 18(4) Ширина изображени  в пиксел х
													   0x00,0x00,0x00,0x00, //! 22(4) Высота изображени  в пиксел х
													   0x01,0x00,           // 26(2) Число плоскостей, должно быть 1
													   0x01,0x00,           // 28(2) Бит/пиксел: 1, 4, 8 или 24
													   0x00,0x00,0x00,0x00, // 30(4)Тип сжати
													   0x00,0x00,0x00,0x00, // 34(4) 0 или размер сжатого изображени  в байтах.
													   0x00,0x00,0x00,0x00, // 38(4)Горизонтальное разрешение, пиксел/м
													   0x00,0x00,0x00,0x00, // 42(4)Вертикальное разрешение, пиксел/м
													   0x02,0x00,0x00,0x00, // 46(4)Количество используемых цветов
													   0x00,0x00,0x00,0x00, // 50(4)Количество "важных" цветов.
					//Палитра (Карта цветов дл  N цветов), если используетс
													   0x00,0x00,0x00,0x00, //54	4*N	Палитра
													   0xFF,0xFF,0xFF,0x00 };
			bmpHeader[22] = (unsigned char)((heightPict & 0x000000FF));
			bmpHeader[23] = (unsigned char)((heightPict & 0x0000FF00) >> 8);
			bmpHeader[24] = (unsigned char)((heightPict & 0x00FF0000) >> 16);
			bmpHeader[25] = (unsigned char)((heightPict & 0xFF000000) >> 24);

			unsigned char *bmpBody = (unsigned char*)malloc(heightPict * 576 / 8 + 10);

			int pictureLength = 0;
			char tmpByte[2];
			int i, j;
			for (i = 0; i < heightPict * 80; i += 80) {
				mData = libReadMemoryBlock(4, sm + 2 + i, 64);
				if (mData.errCode == 0)
				{
					for (j = 14; j < mData.dataLength - 3; j += 2)
					{
						tmpByte[0] = mData.data[j];
						tmpByte[1] = mData.data[j + 1];
						bmpBody[pictureLength] = (unsigned char)strtol(tmpByte, &tempPnt, 16);
						pictureLength++;
					}
				}
				else return mData.errCode;
				mData = libReadMemoryBlock(4, sm + 2 + 64 + i, 16);
				if (mData.errCode == 0)
				{
					for (j = 6; j < mData.dataLength - 3 - 8; j += 2)
					{
						tmpByte[0] = mData.data[j];
						tmpByte[1] = mData.data[j + 1];
						bmpBody[pictureLength] = (unsigned char)strtol(tmpByte, &tempPnt, 16);
						pictureLength++;
					}
				}
				else
				{
					return mData.errCode;
				}
			}
			unsigned char tmpB;
			for (i = 0; i<heightPict / 2; i++)
			{
				for (int j = 0; j<72; j++)
				{

					tmpB = bmpBody[i * 72 + j] ^ 0xFF;
					bmpBody[i * 72 + j] = bmpBody[(heightPict - 1 - i) * 72 + j] ^ 0xFF;
					bmpBody[(heightPict - 1 - i) * 72 + j] = tmpB;

				}
			}

			int fileDesc;
			fileDesc = open(fileName, O_RDWR | O_NOCTTY | O_NDELAY);
			if (fileDesc <= 0)
			{
				return 1100;
			}

			long writtenBytes;
			long fullFileLength;
			fullFileLength = pictureLength + sizeBMPHeader;

			bmpHeader[2] = (unsigned char)((fullFileLength & 0x000000FF));
			bmpHeader[3] = (unsigned char)((fullFileLength & 0x0000FF00) >> 8);
			bmpHeader[4] = (unsigned char)((fullFileLength & 0x00FF0000) >> 16);
			bmpHeader[5] = (unsigned char)((fullFileLength & 0xFF000000) >> 24);

			bmpHeader[34] = (unsigned char)((pictureLength & 0x000000FF));
			bmpHeader[35] = (unsigned char)((pictureLength & 0x0000FF00) >> 8);
			bmpHeader[36] = (unsigned char)((pictureLength & 0x00FF0000) >> 16);
			bmpHeader[37] = (unsigned char)((pictureLength & 0xFF000000) >> 24);
#ifndef LINUX
			WriteFile(fileDesc, &bmpHeader[0], sizeBMPHeader, (LPDWORD)&writtenBytes, NULL);
#else
			writtenBytes = read(fileDesc, &bmpHeader[0], sizeBMPHeader);
#endif

			if (sizeBMPHeader != writtenBytes)
			{
				return 1102;
			}
#ifndef LINUX
			WriteFile(fileDesc, &bmpBody[0], pictureLength, (LPDWORD)&writtenBytes, NULL);
#else
			writtenBytes = read(fileDesc, &bmpBody[0], pictureLength);
#endif
			if (pictureLength != writtenBytes)
			{
				return 1102;
			}

			free(bmpBody);
		}
	}
	return mData.errCode;
}

long long getDriverVersion()
{
    if(pirit_io.DEBUGLEVEL == DEBUG_ON)
    {
        printf( "ver = %ld, build = %ld, drv_ver = %ld\n",DRIVER_VERSION, DRIVER_BUILD, (DRIVER_VERSION | (DRIVER_BUILD << 32)) );
    }
	return (DRIVER_VERSION | (DRIVER_BUILD << 32));
}

void setAmountDecimalPlaces(int decimalPlaces)
{
	pirit_io.setAmountDecimalPlaces(decimalPlaces);
}

void setQuantityDecimalPlaces(int decimalPlaces)
{
	pirit_io.setQuantityDecimalPlaces(decimalPlaces);
}

void setPercentageDecimalPlaces(int decimalPlaces)
{
	pirit_io.setPercentageDecimalPlaces(decimalPlaces);
}

int getAmountDecimalPlaces()
{
	return pirit_io.getAmountDecimalPlaces();
}

int getQuantityDecimalPlaces()
{
	return pirit_io.getQuantityDecimalPlaces();
}

int getPercentageDecimalPlaces()
{
	return pirit_io.getPercentageDecimalPlaces();
}

unsigned long libFormatMessage(int errorCode, char *msgBuffer)
{
	string        msg;
	unsigned long cb = 0;

	switch (errorCode)
	{
		case 0:
			msg = "Команда выполнена без ошибок";
			break;
		case 1:
			msg = "Функция  не выполнима при данном статусе ККТ";
			break;
		case 2:
			msg = "В команде указан неверный номер функции";
			break;
		case 3:
			msg = "Некорректный формат или параметр команды";
			break;
		case 4:
			msg = "Переполнение буфера коммуникационного порта";
			break;
		case 5:
			msg = "Таймаут при передаче байта информации";
			break;
		case 6:
			msg = "В протоколе указан неверный пароль";
			break;
		case 7:
			msg = "Ошибка контрольной суммы в команде";
			break;
		case 8:
			msg = "Конец бумаги";
			break;
		case 9:
			msg = "Принтер не готов";
			break;
		case 10:
			msg = "Текущая  смена больше 24 часов.";
			break;
		case 11:
			msg = "Разница во времени, ККТ и указанной в команде начала работы, больше 8 минут";
			break;
		case 12:
			msg = "Вводимая  дата более ранняя, чем дата последней фискальной операции";
			break;
		case 13:
			msg = "Неверный пароль доступа к ФП";
			break;
		case 14:
			msg = "Отрицательный результат";
			break;
		case 15:
			msg = "Для  выполнени  команды необходимо закрыть смену";
			break;
		case 32:
			msg = "Фатальная ошибка ККТ.";
			break;
		case 33:
			msg = "Нет свободного места в фискальной памяти ККТ";
			break;
		case 65:
			msg = "Некорректный  формат или параметр команды ЭКЛЗ";
			break;
		case 66:
			msg = "Некорректное состояние ЭКЛЗ";
			break;
		case 67:
			msg = "Авария ЭКЛЗ";
			break;
		case 68:
			msg = "Авария КС (Криптографического сопроцессора) в составе ЭКЛЗ";
			break;
		case 69:
			msg = "Исчерпан временной ресурс использовани  ЭКЛЗ";
			break;
		case 70:
			msg = "ЭКЛЗ переполнена";
			break;
		case 71:
			msg = "Неверные дата или врем ";
			break;
		case 72:
			msg = "Нет запрошенных данных";
			break;
		case 73:
			msg = "Переполнение (отрицательный итог документа, слишком много отделов для  клиента)";
			break;
		case 74:
			msg = "Нет ответа от ЭКЛЗ";
			break;
		case 75:
			msg = "Ошибка при обмене данными с ЭКЛЗ";
			break;
		case 1001:
			msg = "При приёме: первый байт не STX";
			break;
		case 1002:
			msg = "При приёме: не совпадает ID пакета";
			break;
		case 1003:
			msg = "При приёме: не совпадает номер команды 1-ый байт";
			break;
		case 1004:
			msg = "При приёме: не совпадает номер команды 2-ой байт";
			break;
		case 1005:
			msg = "При приёме: последний байт не ETX";
			break;
		case 1006:
			msg = "При приёме: ошибка контрольной суммы CRC";
			break;
		case 1007:
			msg = "При отправке данных данные отправились не полностью";
			break;
		case 1008:
			msg = "При приёме: слишком короткий пакет";
			break;
		case 1009:
			msg = "При приёме: потеря связи с ФР";
			break;
		case 1010:
			msg = "При обработке ответа: кол-во параметров не соответствует заявленному";
			break;
		case 1011:
			msg = "ФР не отвечает на команду Ping";
			break;
		case 1100:
			msg = "Ошибка открытия/создания файла";
			break;
		case 1101:
			msg = "Ошибка чтения из файла";
			break;
		case 1102:
			msg = "Ошибка записи в файл";
			break;
		case 1103:
			msg = "Ошибка записи в файл. Данные записаны не полностью.";
			break;
		case 2000:
			msg = "Порт не открыт";
			break;
		case 2001:
			msg = "Невозможно открыть порт";
			break;
		case 2002:
			msg = "Ошибка порта при открытии";
			break;
		case 2003:
			msg = "Ошибка порта при инициализации";
			break;
		case 2010:
			msg = "Ошибка соединения с БИОС";
			break;
		case 2011:
			msg = "Ошибка создания сокета";
			break;
		default:
			msg = "Неизвестная ошибка";
			break;
	}

	cb = msg.length();

	if ( cb > 0 && msgBuffer != NULL )
	{
		strncpy(msgBuffer, msg.c_str(), cb);
	}

	return cb;
}




//===================================================================================
// 1
// Получить номер ФН
int libGetFN_Number(char *fnNum)
{
    int err;
    MData mData;
    pirit_io.makeFirstPartPacketToSend(PIRIT_GET_INFO_FROM_FN);
    pirit_io.addByte(1);
    pirit_io.makeEndPartPacket();

	err = pirit_io.connectSock(); if( err != 0 ) return err;
    err = pirit_io.sendData();
    if (err == 0)
    {
        err = pirit_io.readData();
    }
    pirit_io.disconnectSock();
    int res = pirit_io.parseAnswer1Char(fnNum);
    if(pirit_io.DEBUGLEVEL == DEBUG_ON)
	{
	printf("libGetFN_Number: PARSE RES = %d  \n", res);
	printf("libGetFN_Number: FN NUMBER = '%s'\n", fnNum);
	}

    return res;
}


//===================================================================================
// 2
// Получить номер последнего фискального документа
int libLastFiscDoc_Number(char *fiscalDocNumber)
{
    int err;
    MData mData;
    pirit_io.makeFirstPartPacketToSend(PIRIT_GET_INFO_FROM_FN);
    pirit_io.addByte(3);
    pirit_io.makeEndPartPacket();

	err = pirit_io.connectSock(); if( err != 0 ) return err;
    err = pirit_io.sendData();
    if (err == 0)
    {
        err = pirit_io.readData();
    }
    pirit_io.disconnectSock();
    int res = pirit_io.parseAnswer1Char(fiscalDocNumber);
    if(pirit_io.DEBUGLEVEL == DEBUG_ON)
	{
		printf("libLastFiscDoc_Number: PARSE RES = %d\n", res);
		printf("libLastFiscDoc_Number: LastFiscal Doc number = '%s'\n", fiscalDocNumber);
	}


    return res;
}


//===================================================================================
// 3
// Вернуть состояние текущей смены
int libGetCurShiftState(int *shiftNum, int *isOpened, int *numRecieptInShift)
{
    int err;
//  MData mData;
    pirit_io.makeFirstPartPacketToSend(PIRIT_GET_INFO_FROM_FN);
    pirit_io.addByte(6);
    pirit_io.makeEndPartPacket();

	err = pirit_io.connectSock(); if( err != 0 ) return err;
    err = pirit_io.sendData();
    if (err == 0)
    {
        err = pirit_io.readData();
    }
    pirit_io.disconnectSock();
    int res1 = pirit_io.parseAnswerN<int>( PIRIT_PARAM_1,          *shiftNum );
    int res2 = pirit_io.parseAnswerN<int>( PIRIT_PARAM_2,          *isOpened );
    int res3 = pirit_io.parseAnswerN<int>( PIRIT_PARAM_3, *numRecieptInShift );
    int res = res1 | res2 | res3;
    if(pirit_io.DEBUGLEVEL == DEBUG_ON)
	{

		printf("libGetCurShiftState: PARSE RES = %d\n", res);
		printf("libGetCurShiftState:    \n\tshiftNum = %d   \n\tisOpened = %d   \n\tnumRecieptInShift = %d   \n",
           *shiftNum,
           *isOpened,
           *numRecieptInShift);
	}

    return res;
}


//===================================================================================
// 4
// Открыть документ 0x30
int libOpenDocument(DOC_TYPES                   type,   //  тип открываемого документа
					DOC_FORMATS                  fmt,   //  режим формирования документа
					DOC_REQ_PRINT            doc_req,   //  режим печати реквизитов
					int              numOfDepartment,   //  (Целое число 1..99) Номер отдела
					string            nameOfOperator,   //  Имя оператора
					int                     numOfDoc,   //  Обязателен при уст флага "Нумер чеков со стороны внешн. программы"
					SNO_TYPES                    sno    //  Система налогообложения
					)
{
    int err;
	//----------------------
    pirit_io.makeFirstPartPacketToSend(0x30);
    //----------------------
	// Режим и тип документа
	unsigned char regAndTypeOfDoc =  type | fmt | doc_req;

	pirit_io.addByte(regAndTypeOfDoc);
	// Номер отдела
	pirit_io.addByte(numOfDepartment);
	// Имя оператора
	pirit_io.addConstChar(nameOfOperator.c_str());
	// Номер документа
	pirit_io.addByte(numOfDoc);
	// СНО
	pirit_io.addByte(sno);
    pirit_io.makeEndPartPacket();
	//----------------------

	err = pirit_io.connectSock(); if( err != 0 ) return err;
    err = pirit_io.sendData();

    unsigned char isPackMode = ( fmt & DOC_FORMAT_PACKET ) ? 0x01 : 0x00;
    pirit_io.setPacketMode(isPackMode);

    err = pirit_io.readData();

    pirit_io.checkBuffer.clear();
    pirit_io.checkBuffer.setAccumulate(false);

    const bool close {(err != 0) || (pirit_io.getPacketMode() == 0x00)};
    if (close)
    {
        pirit_io.disconnectSock();
    }
    else if (fmt == 0x10)
    {
        pirit_io.checkBuffer.setAccumulate(true);
    }

    if (pirit_io.DEBUGLEVEL == DEBUG_ON)
	{
        printf("libOpenDocument: packMode=%d\t RESULT = %d\n ", pirit_io.getPacketMode(), err);
	}

    return err;
}

//===================================================================================
// 4
// Открыть документ 0x30 для РБ
int libOpenDocument_By    (DOC_TYPES_BY                type,   //  тип открываемого документа
                           DOC_FORMATS                  fmt,   //  режим формирования документа
                           DOC_REQ_PRINT            doc_req,   //  режим печати реквизитов
                           int              numOfDepartment,   //  Номер отдела
                           string            nameOfOperator,   //  Имя оператора
                           int                     numOfDoc)  //  Обязателен при уст флага "Нумер чеков со стороны внешн. программы"
{
    int err;
    //----------------------
    pirit_io.makeFirstPartPacketToSend(PIRIT_OPEN_DOCUMENT);
    // Режим и тип документа
    unsigned char regAndTypeOfDoc =  type | fmt | doc_req;
    pirit_io.addByte(regAndTypeOfDoc);
    // Номер отдела
    pirit_io.addByte(numOfDepartment);
    // Имя оператора
    pirit_io.addConstChar(nameOfOperator.c_str());
    // Номер документа
    pirit_io.addByte(numOfDoc);
    pirit_io.makeEndPartPacket();
    //----------------------

    err = pirit_io.connectSock(); if( err != 0 ) return err;
    err = pirit_io.sendData();

    unsigned char isPackMode = ( fmt & DOC_FORMAT_PACKET ) ? 0x01 : 0x00;
    pirit_io.setPacketMode(isPackMode);

    err = pirit_io.readData();

    pirit_io.checkBuffer.clear();
    pirit_io.checkBuffer.setAccumulate(false);

    const bool close {(err != 0) || (pirit_io.getPacketMode() == 0x00)};
    if (close)
    {
        pirit_io.disconnectSock();
    }

    if (pirit_io.DEBUGLEVEL == DEBUG_ON)
    {
        printf("libOpenDocument_By: RESULT = %d\n", err);
    }

    return err;
}

//===================================================================================
// 5
// Вернуть состояние ФН
int libGetCurFNState(int     *fnState, // Состояние ФН
                     int *curDocState, // Состояние текущего документа
                     int   *warnFlags) // Флаги предупреждения
{
    int err;

    pirit_io.makeFirstPartPacketToSend(PIRIT_GET_INFO_FROM_FN);
    pirit_io.addByte(2);
    pirit_io.makeEndPartPacket();

	err = pirit_io.connectSock(); if( err != 0 ) return err;
    err = pirit_io.sendData();
    if (err == 0)
    {
        err = pirit_io.readData();
    }
    pirit_io.disconnectSock();
    int res1 = pirit_io.parseAnswerN<int>(PIRIT_PARAM_1,              *fnState);
    int res2 = pirit_io.parseAnswerN<int>(PIRIT_PARAM_2,          *curDocState);
    int res3 = pirit_io.parseAnswerN<int>(PIRIT_PARAM_3,            *warnFlags);
    int res = res1 | res2 | res3;
    if(pirit_io.DEBUGLEVEL == DEBUG_ON)
	{
		printf("libGetCurFNState: PARSE RES = %d\n", res);
		printf("libGetCurFNState:    \n\tfnState = %d   \n\tcurDocState = %d   \n\twarnFlags = 0x%02x   \n",
																										   *fnState,
																										   *curDocState,
																										   *warnFlags);
	}
    return res;
}


//===================================================================================
// 6
// Вернуть состояние обмена с ОФД
int libGetStatusOFD( int            *ofdState,                     // Статус обмена
					 int     *numOfDocsToSend,                     // Количество документов для передачи     в ОФД
					 int       *numFirstToSnd,                     // Номер первого документа для передачи   в ОФД
					 char      *firstDateTime )                    // Дата/время первого док-та для передачи в ОФД
{
    int err = 0;
    pirit_io.makeFirstPartPacketToSend(PIRIT_GET_INFO_FROM_FN);
	pirit_io.addByte(7);
	pirit_io.makeEndPartPacket();
	err = pirit_io.connectSock(); if( err != 0 ) return err;
	err = pirit_io.sendData();
	if (err == 0)
	{
		err = pirit_io.readData();
	}
	pirit_io.disconnectSock();

	char myDate[44];
	int res1 = pirit_io.parseAnswerN<int>  ( PIRIT_PARAM_1,                 *ofdState);
	int res2 = pirit_io.parseAnswerN<int>  ( PIRIT_PARAM_2,          *numOfDocsToSend);
	int res3 = pirit_io.parseAnswerN<int>  ( PIRIT_PARAM_3,            *numFirstToSnd);
	int res4 = pirit_io.parseAnswerN<char> ( PIRIT_PARAM_4,                   *myDate);

	int res = res1 | res2 | res3 | res4;

    if(pirit_io.DEBUGLEVEL == DEBUG_ON)
	{
		printf("libGetStatusOFD: Command RES = %d\n", res);
		printf("libGetStatusOFD:    \n\t ofdState = %d \n\t numOfDocsToSend = %d \n\t numFirstToSnd = %d \n\t myDate = '%s'",
			   *ofdState,
			   *numOfDocsToSend,
			   *numFirstToSnd,
			   myDate);
	}

	memcpy( firstDateTime, myDate, 44);

	return res;

}


//===================================================================================
// 	Вернуть дату и время регистрации  4
int libGetRegDateTime( char      *regDateTime )
{
    int err = 0;
    pirit_io.makeFirstPartPacketToSend(PIRIT_GET_INFO_FROM_FN);
    pirit_io.addByte(4);
    pirit_io.makeEndPartPacket();
    err = pirit_io.connectSock(); if( err != 0 ) return err;
    err = pirit_io.sendData();
    if (err == 0)
    {
        err = pirit_io.readData();
    }
    pirit_io.disconnectSock();

    char regData[20];
    memset(regData, 0, sizeof(regData));
    int res = pirit_io.parseAnswerN<char> ( PIRIT_PARAM_1, *regData);

    strcpy(regDateTime, regData);

    memset(regData, 0, sizeof(regData));
    res |= pirit_io.parseAnswerN<char> ( PIRIT_PARAM_2, *regData);

    strcat(regDateTime, regData);

    if(pirit_io.DEBUGLEVEL == DEBUG_ON)
    {
        printf("libGetRegDateTime: Command RES = %d\n", res);
        printf("libGetRegDateTime:    \n\t  regDateTime = '%s'", regData);
    }

    return res;
}


//===================================================================================
// 	Вернуть номер ФД последней регистрации  5
int libGetRegFDNumber( int &fdNumber )
{
    int err = 0;
    pirit_io.makeFirstPartPacketToSend(PIRIT_GET_INFO_FROM_FN);
    pirit_io.addByte(5);
    pirit_io.makeEndPartPacket();
    err = pirit_io.connectSock(); if( err != 0 ) return err;
    err = pirit_io.sendData();
    if (err == 0)
    {
        err = pirit_io.readData();
    }
    pirit_io.disconnectSock();

    int res = pirit_io.parseAnswerN<int> ( PIRIT_PARAM_1, fdNumber);
    if(pirit_io.DEBUGLEVEL == DEBUG_ON)
    {
        printf("libGetRegFDNumber: \n\t  Command RES =  %d\n",      res);
        printf("libGetRegFDNumber: \n\t     fdNumber = '%d'",  fdNumber);
    }
    return res;

}
//===================================================================================
int libSetFnDateTime(const DateTime &dateTime)
{
    pirit_io.makeFirstPartPacketToSend(PIRIT_GET_INFO_FROM_FN);
    pirit_io.addByte(PIRIT_INFO_FN_SET_DATETIME);
    pirit_io.addDateTime(dateTime.day,
                         dateTime.month,
                         dateTime.year,
                         dateTime.hours,
                         dateTime.minutes,
                         dateTime.seconds);
    pirit_io.makeEndPartPacket();
    int res = pirit_io.connectSock();
    if( res != 0 )
    {
        return res;
    }
    res = pirit_io.sendData();
    if (res == 0)
    {
        res = pirit_io.readData();
    }
    pirit_io.disconnectSock();

    return res;
}
//===================================================================================
// 	Запрос номера ФД и общей длины ТЛВ-структуры последней рег/перерег (для последующей выгрузки)
int libGetRegistrSTLVProps( int   &numberOfFD,    // Номер ФД, для которого требуется получить ТЛВ
    					    int     &lenTotal,    // Длина полученного ТЛВ пакета
    					    int     &docType)     // Тип документа
{
    int res = 0;

    //---------------------------
    // 1 Получаем номер ФД последней регистрации/перерегистрации
    numberOfFD = 0;
    res = libGetRegFDNumber( numberOfFD );

    if (res != 0)
    {
        printf("\nlibGetRegistrTLV(1 step): \n\t  Num FD of reg/rereg not found! res = %d\n",      res);
        return -1;
    }
    if(pirit_io.DEBUGLEVEL == DEBUG_ON)
    {
        printf("\nlibGetRegistrTLV(1 step): \n\t  Command RES =  %d\n",      res);
        printf("libGetRegistrTLV(1 step): \n\t Num FD of reg/rereg = %d\n", numberOfFD );
    }
    //---------------------------
    // 2 Получаем по номеру ФД последней регистрации/перерегистрации аттрибуты ТЛВ структуры
    pirit_io.makeFirstPartPacketToSend(PIRIT_GET_INFO_FROM_FN);
    pirit_io.addByte(EXCHANGE_FN_SELECT_TLV_DOC);
	pirit_io.addByte(numberOfFD);
    pirit_io.makeEndPartPacket();
    res = pirit_io.connectSock(); if( res != 0 ) return res;
    res = pirit_io.sendData();
    if (res == 0)
    {
        res = pirit_io.readData();
    }
    pirit_io.disconnectSock();

    res = pirit_io.parseAnswerN<int> ( PIRIT_PARAM_1, docType ) | pirit_io.parseAnswerN<int> ( PIRIT_PARAM_2, lenTotal );
    if(pirit_io.DEBUGLEVEL == DEBUG_ON)
    {
        printf("\nlibGetRegistrTLV(2 step): \n\t  Command RES =  %d\n",      res);
        printf("libGetRegistrTLV(2 step): \n\t Doc Type = %d, len of TLV struct = %d\n", docType, lenTotal);
    }
    // Обнуляемся в случае ошибки
	if(res != 0)
	{
        lenTotal = 0;
        docType = 0;
	}
    return res;
}
//===================================================================================
// 	Запрос номера ФД и общей длины ТЛВ-структуры последней (для последующей выгрузки)
int libGetDocSTLV         ( const int   &numberOfFD,    // Номер ФД, для которого требуется получить ТЛВ
                                  int     &lenTotal,    // Длина полученного ТЛВ пакета
                                  int     &docType)     // Тип документа
{
    int res = 0;

    //---------------------------
    // 1 Получаем по номеру ФД аттрибуты ТЛВ структуры
    pirit_io.makeFirstPartPacketToSend(PIRIT_GET_INFO_FROM_FN);
    pirit_io.addByte(EXCHANGE_FN_SELECT_TLV_DOC);
    pirit_io.addByte(numberOfFD);
    pirit_io.makeEndPartPacket();
    res = pirit_io.connectSock(); if( res != 0 ) return res;
    res = pirit_io.sendData();
    if (res == 0)
    {
        res = pirit_io.readData();
    }
    pirit_io.disconnectSock();

    res = pirit_io.parseAnswerN<int> ( PIRIT_PARAM_1, docType ) | pirit_io.parseAnswerN<int> ( PIRIT_PARAM_2, lenTotal );
    if(pirit_io.DEBUGLEVEL == DEBUG_ON)
    {
        printf("\nlibGetDocSTLV: \n\t  Command RES =  %d\n",      res);
        printf("libGetDocSTLV  : \n\t Doc Type = %d, len of TLV struct = %d\n", docType, lenTotal);
    }
    // Обнуляемся в случае ошибки
    if(res != 0)
    {
        lenTotal = 0;
    }
    return res;
}

//===================================================================================
// 	Запрос данных последней регистрации/перерегистрации
int libGetLastRegData( uint32_t& fd, uint32_t& fpd )
{
    int res = 0;
    constexpr int emptyByte{0};
    //---------------------------
    // 1 Получаем по номеру ФД аттрибуты ТЛВ структуры
    pirit_io.makeFirstPartPacketToSend(PIRIT_GET_INFO_FROM_FN);
    pirit_io.addByte(EXCHANGE_FN_LAST_REG_DATA);
    pirit_io.addByte(emptyByte);
    pirit_io.makeEndPartPacket();
    res = pirit_io.connectSock(); if( res != 0 ) return res;
    res = pirit_io.sendData();
    if (res == 0)
    {
        res = pirit_io.readData();
    }
    pirit_io.disconnectSock();
    int type{0};

    res = pirit_io.parseAnswerN<uint32_t> ( PIRIT_PARAM_1, fd ) +  pirit_io.parseAnswerN<uint32_t> ( PIRIT_PARAM_2, fpd );

    if(pirit_io.DEBUGLEVEL == DEBUG_ON)
    {
        printf("%s:: fd  = %u",__func__,    fd);
        printf("%s:: fpd = %u",__func__, fpd);
    }
    // Обнуляемся в случае ошибки
    if(res != 0)
    {
        printf("%s:: ERROR!  res = %d",__func__,    res);
    }
    return res;
}
//===================================================================================
// 	Запрос номера отчета о регистрации/перерегистрации + Возвращается тип регистрации: 1- регистрация, 11-перерегистраци (для последующей выгрузки)
int libGetRegSTLV         ( PIRIT_REG_DOC_TYPE &regType )      // Тип документа 1- reg, 11 - rereg
{
    int res = 0;
    constexpr int emptyByte{0};
    //---------------------------
    // 1 Получаем по номеру ФД аттрибуты ТЛВ структуры
    pirit_io.makeFirstPartPacketToSend(PIRIT_GET_INFO_FROM_FN);
    pirit_io.addByte(EXCHANGE_FN_SELECT_TLV_REG);
    pirit_io.addByte(emptyByte);
    pirit_io.makeEndPartPacket();
    res = pirit_io.connectSock(); if( res != 0 ) return res;
    res = pirit_io.sendData();
    if (res == 0)
    {
        res = pirit_io.readData();
    }
    pirit_io.disconnectSock();
    int type{0};
    res = pirit_io.parseAnswerN<int> ( PIRIT_PARAM_1, type );
    regType = static_cast<PIRIT_REG_DOC_TYPE>(type);
    if(pirit_io.DEBUGLEVEL == DEBUG_ON)
    {
        printf("\nlibGetRegSTLV: \n\t  Command RES =  %d\n",      res);
        printf("libGetRegSTLV  : \n\t registration type = %d\n", regType);
    }
    // Обнуляемся в случае ошибки
    if(res != 0)
    {
        regType = PIRIT_REG_DOC_TYPE::PIRIT_REG_DOC_TYPE_UNDEFINED;
    }
    return res;
}
//===================================================================================
// 	Запрос документа из архива по номеру
int libGetFnDocBin(  int        docNum,   // Номер документа
                     char       **data,   // Тело  документа
                     int      &dataLen,   // Длина документа
                     bool     &isCheck,   // Это квитанция( подтверждён в ОФД )
                     int      &docType)   // Тип выгружаемого документа
{
    int res = 0;
    //---------------------------
    // 3 Читаем сам ТЛВ массив
    pirit_io.makeFirstPartPacketToSend(PIRIT_GET_INFO_FROM_FN);
    pirit_io.addByte(EXCHANGE_FN_DOC_FROM_ARC); // Чтоб сеньору в протокол не лезть, а то непаняяятна
    pirit_io.addByte(docNum);                           // номер документа
    pirit_io.makeEndPartPacket();
    res = pirit_io.connectSock(); if( res != 0 ) return res;
    res = pirit_io.sendData();
    if (res == 0)
    {
        res = pirit_io.readData();
    }
    pirit_io.disconnectSock();
    if( res != 0 ) return res;

    dataLen = 0;
    isCheck = false;
    int iIsCheck;
    res = pirit_io.parseAnswerN<int> (PIRIT_PARAM_0, iIsCheck);
    if( res != 0 ) return res;
    isCheck = (iIsCheck == 1) ? true : false;


    res = pirit_io.parseAnswerChN_constCount(1, data, dataLen);
    if( res != 0 )
    {
        return res;
    }
    res = pirit_io.parseAnswerN<int>(PIRIT_PARAM_2, docType);
    if( res != 0 )
    {
        return res;
    }

    if(pirit_io.DEBUGLEVEL == DEBUG_ON)
    {
        printf("\n----------------------------------------------------------------\n");
        printf("libGetFnDocBin: \n\t  Command RES =  %d",      res);
        printf("\n\t real dataLen = %d \n\tisCheck = %d \n\tdocType = %d", dataLen, isCheck, docType);
        printf("\n----------------------------------------------------------------\n");
    }
    return res;
}

//===================================================================================
// 	Запрос документа из архива в формате TLV, до ошибки !! 48h !!, ПОСЛЕ КОМАНДЫ (78 - 15, libGetRegistrSTLVProps!!!!)
int libGetRegistrTLV( char             *tlv,   // Тело ТЛВ структуры             (компоненты общей СТЛВ)
                      int           &lenTLV,   // Длина полученной ТЛВ структуры (компоненты общей СТЛВ)
                      const bool   isRegTLV )  // true - send command 78/20, false - 78/16
{
    int res = 0;
    //---------------------------
    // 3 Читаем сам ТЛВ массив
    pirit_io.makeFirstPartPacketToSend(PIRIT_GET_INFO_FROM_FN);
    pirit_io.addByte( (isRegTLV ? EXCHANGE_FN_GET_TLV_REG : EXCHANGE_FN_GET_TLV_DOC) );   // Запрос данных TLV
    pirit_io.makeEndPartPacket();
    res = pirit_io.connectSock(); if( res != 0 ) return res;
    res = pirit_io.sendData();
    if (res == 0)
    {
        res = pirit_io.readData();
    }
    pirit_io.disconnectSock();

    lenTLV = 0;
    if( res != 0 ) return res;

    res = pirit_io.parseAnswerN<char> ( PIRIT_PARAM_0, *tlv, &lenTLV );
    if(pirit_io.DEBUGLEVEL == DEBUG_ON)
    {
        printf("libGetRegistrTLV(3 step): \n\t  Command RES =  %d\n",      res);
        printf("libGetRegistrTLV(3 step): \n\t real TLV length = %d\n", lenTLV);
    }
    return res;
}

//===================================================================================
// 	Запрос документа из архива в формате TLV, до ошибки !! 48h !!, ПОСЛЕ КОМАНДЫ (78 - 15, libGetRegistrSTLVProps!!!!)
int libGetLastRecieptData(  int                &type,   // тип чека (для аннулиров. = 0)
                            string       &cur_op_cnt,   // текущий операц. счетчик
                            int             &rec_num,   // номер чека
                            int             &doc_num,   // номер документа
                            double              &sum,   // сумма чека
                            double         &discount,   // сумма скидки
                            string              &fpd,   // строка ФП (фиск. признак)
                            int                  &fd )  // номер ФД
{
    int     res = 0;
    char tmpStr[128];
    //----
    pirit_io.makeFirstPartPacketToSend(PIRIT_RECEIPT_DATA);
    pirit_io.addByte(0x02);
    pirit_io.makeEndPartPacket();
    res = pirit_io.connectSock(); if( res != 0 ) return res;
    res = pirit_io.sendData();
    if (res == 0)
    {
        res = pirit_io.readData();
    }
    pirit_io.disconnectSock();
    //----
    // Чистим
    type = 0;
    cur_op_cnt.clear();
    rec_num = 0;
    doc_num = 0;
    sum = 0;
    discount = 0;
    fpd.clear();
    fd = 0;
    //----
    if( res != 0 ) return res;
    //----
    res |= pirit_io.parseAnswerN<int>   ( PIRIT_PARAM_1, type    );
    res |= pirit_io.parseAnswerN<char>  ( PIRIT_PARAM_2, *tmpStr   );    cur_op_cnt = tmpStr;
    res |= pirit_io.parseAnswerN<int>   ( PIRIT_PARAM_3, rec_num );
    res |= pirit_io.parseAnswerN<int>   ( PIRIT_PARAM_4, doc_num );
    res |= pirit_io.parseAnswerN<double>( PIRIT_PARAM_5, sum     );
    res |= pirit_io.parseAnswerN<double>( PIRIT_PARAM_6, discount);
    // resreved                     7
    res |= pirit_io.parseAnswerN<char> ( PIRIT_PARAM_8, *tmpStr   );    fpd = tmpStr;
    res |= pirit_io.parseAnswerN<int>  ( PIRIT_PARAM_9, fd      );

    if(pirit_io.DEBUGLEVEL == DEBUG_ON)
    {
        printf("libGetLastRecieptData(): \n\t  Command RES =  %d\n",      res);
        printf("\t type       = %d  \n", type);
        printf("\t cur_op_cnt = '%s'\n", cur_op_cnt.c_str());
        printf("\t rec_num    = %d  \n", rec_num);
        printf("\t doc_num    = %d  \n", doc_num);
        printf("\t sum        = %lf \n", sum);
        printf("\t discount   = %lf \n", discount);
        printf("\t fpd        = '%s'\n", fpd.c_str());
        printf("\t fd         = %d  \n", fd);
    }
    return res;
}

int libGetLastRecieptData_By(int &type, string &cur_op_cnt, int &rec_num, int &doc_num, double &sum, double &discount, double &markup)
{
    int     res = 0;
    char tmpStr[128];
    memset(tmpStr, 0, sizeof(tmpStr));

    type     =   0;
    rec_num  =   0;
    doc_num  =   0;
    sum      = 0.0;
    discount = 0.0;
    markup   = 0.0;
    cur_op_cnt.clear();

    pirit_io.makeFirstPartPacketToSend(PIRIT_RECEIPT_DATA);
    pirit_io.addByte(PIRIT_RECEIPT_DATA_LAST_DOC);
    pirit_io.makeEndPartPacket();

    res = pirit_io.connectSock();
    if ( res != 0 )
    {
        return res;
    }

    res = pirit_io.sendData();
    if ( res == 0 )
    {
        res = pirit_io.readData();
    }
    pirit_io.disconnectSock();

    if ( res != 0 )
    {
        return res;
    }

    res |= pirit_io.parseAnswerN<int>   (PIRIT_PARAM_1, type    );
    res |= pirit_io.parseAnswerN<char>  (PIRIT_PARAM_2, *tmpStr );
    res |= pirit_io.parseAnswerN<int>   (PIRIT_PARAM_3, rec_num );
    res |= pirit_io.parseAnswerN<int>   (PIRIT_PARAM_4, doc_num );
    res |= pirit_io.parseAnswerN<double>(PIRIT_PARAM_5, sum     );
    res |= pirit_io.parseAnswerN<double>(PIRIT_PARAM_6, discount);
    res |= pirit_io.parseAnswerN<double>(PIRIT_PARAM_7, markup  );

    cur_op_cnt = string(tmpStr);

    if ( pirit_io.DEBUGLEVEL == DEBUG_ON )
    {
        printf("libGetLastRecieptData_By(): \n\t  Command RES =  %d\n",      res);
        printf("\t type       = %d  \n", type);
        printf("\t cur_op_cnt = '%s'\n", cur_op_cnt.c_str());
        printf("\t rec_num    = %d  \n", rec_num);
        printf("\t doc_num    = %d  \n", doc_num);
        printf("\t sum        = %lf \n", sum);
        printf("\t discount   = %lf \n", discount);
        printf("\t markup     = %lf \n", markup);
    }

    return res;
}
//===================================================================================
int libGetStatusOFD(int &result)
{
    int err;
    MData mData;
    memset(mData.data, 0, sizeof(mData.data));

    pirit_io.makeFirstPartPacketToSend(PIRIT_GET_STATUS_OFD);
    pirit_io.makeEndPartPacket();
    err = pirit_io.connectSock(); if( err != 0 ) return err;
    err = pirit_io.sendData();

    if (err == 0)
    {
        if (pirit_io.getPacketMode() == 0)
            err = pirit_io.readData();
    }

    pirit_io.getMData(&mData);
    pirit_io.disconnectSock();

    err = pirit_io.parseAnswerN<int>(PIRIT_PARAM_0, result);

    if ( pirit_io.DEBUGLEVEL == DEBUG_ON )
    {
        printf("libGetStatusOFD: \n\t  Command RES =  %d\n",    err);
        printf("libGetStatusOFD: \n\t  Статус ОФД  = '%d'\n",  result);
    }

    return err;
}


//===================================================================================
// Аварийное закрытие ФН (0xA4)
int libEmegencyCloseFN()
{
    int err;

    pirit_io.makeFirstPartPacketToSend(PIRIT_EMERGENCY_CLOSE_FN);
    pirit_io.makeEndPartPacket();

    err = pirit_io.connectSock(); if( err != 0 ) return err;
    err = pirit_io.sendData();

    if (err == 0)
    {
        if (pirit_io.getPacketMode() == 0)
            err = pirit_io.readData();
    }
    pirit_io.disconnectSock();

    if ( pirit_io.DEBUGLEVEL == DEBUG_ON )
    {
        printf("libEmegencyCloseFN: \n\t  Command RES =  %d\n",    err);
    }

    return err;
}


//=======================================================================================================
// Чтение значения из таблицы настроек КСА
int libGetSettingsKSA_By(int   number,          // Номер поля
                         int    index,          // Индекс(для массива)
                         char   *data)          // Значение
{
    if(pirit_io.DEBUGLEVEL == DEBUG_ON)
    {
        printf("libGetSettingsKSA_By: \n\t  Чтение настройки: № = '%d' Индекс ='%d'\n",  number, index);
    }

    int err = 0;
    pirit_io.makeFirstPartPacketToSend(0x11);
    pirit_io.addByte(number);
    pirit_io.addByte(index);
    pirit_io.makeEndPartPacket();
    err = pirit_io.connectSock(); if( err != 0 ) return err;
    err = pirit_io.sendData();
    if (err == 0)
    {
        err = pirit_io.readData();
    }
    pirit_io.disconnectSock();

    int res = pirit_io.parseAnswerN<char>( PIRIT_PARAM_0, *data );


    if(pirit_io.DEBUGLEVEL == DEBUG_ON)
    {
        printf("libGetSettingsKSA_By: \n\t  Результат =  '%d'  Data = '%s'\n", res, data);
    }
    return err;
}


//=======================================================================================================
// Запись значения в  таблицу настроек КСА
int libSetSettingsKSA_By(int   number,         // Номер поля
                         int    index,         // Индекс(для массива)
                         char   *data)         // Значение
{
    if(pirit_io.DEBUGLEVEL == DEBUG_ON)
    {
        printf("libSetSettingsKSA_By: \n\t  Запись настройки: № = '%d' Индекс ='%d' data = '%s'\n",  number,
                                                                                                    index,
                                                                                                    data);
    }

    int err = 0;
    pirit_io.makeFirstPartPacketToSend(0x12);
    pirit_io.addByte(number);
    pirit_io.addByte(index);
    pirit_io.addConstChar(data);
    pirit_io.makeEndPartPacket();
    err = pirit_io.connectSock(); if( err != 0 ) return err;
    err = pirit_io.sendData();
    if (err == 0)
    {
        err = pirit_io.readData();
    }
    pirit_io.disconnectSock();

    if(pirit_io.DEBUGLEVEL == DEBUG_ON)
    {
        printf("libSetSettingsKSA_By: \n\t  Результат =  %d\n",      err);
    }
    return err;
}

//========================================================================================
// РБ. Запрос дополнительной информации о ошибках
int libGetInfoErrorsExt_By(int *errs)
{
    if(pirit_io.DEBUGLEVEL == DEBUG_ON)
    {
        printf("libGetInfoErrorsExt_By:\t   Запрос дополнительной информации о ошибках..." );
    }
    int err = 0;
    pirit_io.makeFirstPartPacketToSend(0x06);
    pirit_io.addByte(0x01);
    pirit_io.makeEndPartPacket();
    err = pirit_io.connectSock(); if( err != 0 ) return err;
    err = pirit_io.sendData();
    if (err == 0)
    {
        err = pirit_io.readData();
    }
    pirit_io.disconnectSock();

    int res = pirit_io.parseAnswerN<int> ( PIRIT_PARAM_1, *errs );

    if(pirit_io.DEBUGLEVEL == DEBUG_ON)
    {
        printf("libGetInfoErrorsExt_By: \t  Результат =  %d\n",      res);
    }
    return res;
}


//========================================================================================
// РБ. Запрос флагов статуса КСА
int libGetStatusFlags_By  ( int *statusFatal, int *statusCurrent, int *statusDoc, int *statusSKNO )
{
    if(statusFatal == NULL || statusCurrent == NULL || statusDoc == NULL || statusSKNO == NULL)
    {
        printf("parseAnswerCommand0x00_4args:: Error, NULL arg!");
        return 1010;
    }
    *statusFatal   = -0xFFFF;
    *statusCurrent = -0xFFFF;
    *statusDoc     = -0xFFFF;
    *statusSKNO    = -0xFFFF;
    MData mData;
    mData = pirit_io.commandVoidMData(0x00);
    if (mData.errCode == 0)
    {
//        mData.errCode = pirit_io.parseAnswerCommand0x00(statusFatal, statusCurrent, statusDoc);
        mData.errCode = pirit_io.parseAnswerCommand0x00_4args(statusFatal, statusCurrent, statusDoc, statusSKNO);
    }
    return mData.errCode;

}



//======================================================================================
//============ КОМАДЫ ДЛЯ ПЕЧАТИ НЕФИСКАЛЬНЫХ ДОКУМЕНТОВ В ПАКЕТНОМ РЕЖИМЕ =============
//======================================================================================

int libOpeNotFiscalDoc(const bool header)
{
    int err = 0;

    int        num_dep  = 0;
    int        num_doc  = 1;
    string     cashier  = "";
    SNO_TYPES      sno  = SNO_OSN;
    uint8_t   doc_data  = DOC_TYPE_SERVICE | DOC_FORMAT_PACKET;

    if (!header)
    {
        doc_data |= DOC_REQ_PRINT_DELAYED;
    }

    pirit_io.makeFirstPartPacketToSend( PIRIT_OPEN_DOCUMENT );
    pirit_io.addByte( doc_data );
    pirit_io.addByte( num_dep );
    pirit_io.addConstChar( cashier.c_str() );
    pirit_io.addByte( num_doc );
    pirit_io.addByte( sno );
    pirit_io.makeEndPartPacket();

    err = pirit_io.connectSock();
    if ( err != 0 )
    {
        return err;
    }

    err = pirit_io.sendData();

    if ( err == 0 )
    {
        err = pirit_io.readData();
    }

    if( err != 0 )
    {
        pirit_io.disconnectSock();
    }

    return err;
}

int libCloseNotFiscalDoc(uint8_t cutPaper)
{
    int err = 0;

    pirit_io.makeFirstPartPacketToSend( PIRIT_CLOSE_DOCUMENT );
    pirit_io.addByte( cutPaper );
    pirit_io.makeEndPartPacket();

    err = pirit_io.sendData();

    if (err != 0)
    {
        return err;
    }

    err = pirit_io.readData();

    pirit_io.disconnectSock();

    return err;
}

int libPrintNotFiscalStr(char *textStr, FONTS fnt)
{
    int err = 0;
    uint8_t font = fnt;

    constexpr size_t maxLen {72};
    size_t len {strlen(textStr)};
    size_t offset {0};

    while (len > 0)
    {
        const size_t curLen {(len > maxLen) ? maxLen : len};

        pirit_io.makeFirstPartPacketToSend(PIRIT_PRINT_STRING);
        pirit_io.addChar(textStr + offset, curLen);
        pirit_io.addByte(font);
        pirit_io.makeEndPartPacket();

        err = pirit_io.sendData();

        if (err != 0)
        {
            break;
        }

        len -= curLen;
        offset += curLen;
    }

    return err;
}


int libGetBatteryVoltage(int &voltage)
{
    MData mData = pirit_io.commandByte(PIRIT_SERVICE_INFO, PIRIT_BATTERY_VOLTAGE);

    voltage = 0;
    if (mData.errCode == 0)
    {
        mData.errCode = pirit_io.parseAnswerN<int>(PIRIT_PARAM_1, voltage);
    }

    if(pirit_io.DEBUGLEVEL == DEBUG_ON)
    {
        printf("libGetBatteryVoltage: ERROR CODE = %d\n", mData.errCode);
        printf("libGetBatteryVoltage: VOLTAGE = %d\n", voltage);
    }

    return mData.errCode;
}

int getStrServiceInfoSKNO(uint8_t number, uint32_t index, string &dataSKNO)
{
    pirit_io.makeFirstPartPacketToSend( PIRIT_SERVICE_INFO );
    pirit_io.addByte(number);
    pirit_io.makeEndPartPacket();

    int err = pirit_io.connectSock();
    if( err != 0 )
    {
        return err;
    }

    err = pirit_io.sendData();
    if (err == 0)
    {
        if (pirit_io.getPacketMode() == 0)
        {
            err = pirit_io.readData();

            if ( err == 0 )
            {
                char data[256] = {0};
                err = pirit_io.parseAnswerN<char>(index, *data);
                dataSKNO = string(data);
            }
        }
    }

    pirit_io.disconnectSock();

    if ( pirit_io.DEBUGLEVEL == DEBUG_ON )
    {
        printf("getStrServiceInfoSKNO: \n\t  Parse err =  %d\n", err);
    }

    return err;
}

int getIntServiceInfoSKNO(uint8_t number, uint32_t index, int &data)
{
    pirit_io.makeFirstPartPacketToSend( PIRIT_SERVICE_INFO );
    pirit_io.addByte(number);
    pirit_io.makeEndPartPacket();

    int err = pirit_io.connectSock();
    if( err != 0 )
    {
        return err;
    }

    err = pirit_io.sendData();
    if (err == 0)
    {
        if (pirit_io.getPacketMode() == 0)
        {
            err = pirit_io.readData();

            if ( err == 0 )
            {
                err = pirit_io.parseAnswerN<int>(index, data);
            }
        }
    }

    pirit_io.disconnectSock();

    if ( err != 0 )
    {
        printf("getIntServiceInfoSKNO: \n\t  Parse err =  %d\n", err);
    }

    return err;
}

//==========================  КОМАНДЫ ДРИМКАС-КЛЮЧ =====================================
int libDreamkasKeySet(string &strKey)
{
    pirit_io.makeFirstPartPacketToSend( PIRIT_DREAMKAS_KEY );
    pirit_io.addByte( PIRIT_DREAMKAS_KEY_SET );
    pirit_io.addChar( const_cast<char*>(strKey.c_str()) );
    pirit_io.makeEndPartPacket();

    int err = pirit_io.connectSock();
    if( err != 0 )
    {
        return err;
    }

    err = pirit_io.sendData();
    if ( err == 0 )
    {
        err = pirit_io.readData();
    }
    pirit_io.disconnectSock();

    if ( pirit_io.DEBUGLEVEL == DEBUG_ON )
    {
        printf("%s: Parse err =  %d\n", __PRETTY_FUNCTION__, err);
    }

    return err;
}

int libDreamkasKeyGetInfo(const uint8_t &index, DreamkasKey &keyInfo)
{
    pirit_io.makeFirstPartPacketToSend( PIRIT_DREAMKAS_KEY );
    pirit_io.addByte( PIRIT_DREAMKAS_KEY_GET_INFO );
    pirit_io.addByte( index );
    pirit_io.makeEndPartPacket();

    int err = pirit_io.connectSock();
    if( err != 0 )
    {
        return err;
    }

    err = pirit_io.sendData();
    if (err == 0)
    {
        err = pirit_io.readData();
        if (err == 0)
        {
            pirit_io.parseAnswerN<uint8_t>(PIRIT_PARAM_1, keyInfo.status);

            char name[256] = {0};
            pirit_io.parseAnswerN<char>(PIRIT_PARAM_2, *name);
            keyInfo.name = string(name);

            char dateEnd[16] = {0};
            pirit_io.parseAnswerN<char>(PIRIT_PARAM_3, *dateEnd);
            keyInfo.dateEnd = string(dateEnd);
        }
    }

    pirit_io.disconnectSock();

    if ( pirit_io.DEBUGLEVEL == DEBUG_ON )
    {
        printf("%s: Parse err =  %d\n", __PRETTY_FUNCTION__, err);
    }

    return err;
}

int libDreamkasKeyPrintAll()
{
    pirit_io.makeFirstPartPacketToSend( PIRIT_DREAMKAS_KEY );
    pirit_io.addByte( PIRIT_DREAMKAS_KEY_PRINT_ALL );
    pirit_io.makeEndPartPacket();

    int err = pirit_io.connectSock();
    if( err != 0 )
    {
        return err;
    }

    err = pirit_io.sendData();
    if ( err == 0 )
    {
        err = pirit_io.readData();
    }
    pirit_io.disconnectSock();

    if ( pirit_io.DEBUGLEVEL == DEBUG_ON )
    {
        printf("%s: Parse err =  %d\n", __PRETTY_FUNCTION__, err);
    }

    return err;
}

int libDreamkasKeyGetInfoList(vector<DreamkasKey> &keyInfoList)
{
    string infoStr;

    pirit_io.makeFirstPartPacketToSend( PIRIT_DREAMKAS_KEY );
    pirit_io.addByte( PIRIT_DREAMKAS_KEY_GET_ALL );
    pirit_io.makeEndPartPacket();

    int err = pirit_io.connectSock();
    if( err != 0 )
    {
        return err;
    }

    err = pirit_io.sendData();
    if (err == 0)
    {
        err = pirit_io.readData();
        if (err == 0)
        {
            uint32_t ver = 0;
            pirit_io.parseAnswerN<uint32_t>(PIRIT_PARAM_1, ver);
            infoStr  = to_string(ver);
            infoStr += ";";

            uint32_t count = 0;
            pirit_io.parseAnswerN<uint32_t>(PIRIT_PARAM_2, count);
            infoStr += to_string(count);
            infoStr += ";";

            for ( uint8_t index = 0; index < count; ++index )
            {
                char infoArr[256] = {0};
                pirit_io.parseAnswerN<char>(PIRIT_PARAM_3 + index, *infoArr);
                infoStr += string(infoArr);
            }
        }
    }

    pirit_io.disconnectSock();

    if ( !infoStr.empty() )
    {
        if ( libDreamkasKeyParseInfoList(keyInfoList, infoStr) < 0 )
        {
            err = 0x03; // Некорректный формат/пар-р команды
            printf("%s: Ошибка парса списка активированных услуг!!!\n", __PRETTY_FUNCTION__);
        }
    }

    if ( pirit_io.DEBUGLEVEL == DEBUG_ON )
    {
        printf("%s: Parse err =  %d\n", __PRETTY_FUNCTION__, err);
    }

    return err;
}

int libDreamkasKeyParseInfoList(vector<DreamkasKey> &keyInfoList, const string &infoStr)
{
    const uint8_t INFO_LIST_SIZE = 4;

    vector<string> valInfoList = libSplitString(infoStr, ';');

    if ( valInfoList.size() < 2 )
    {
        printf("%s: Ошибка парса списка активированных услуг (мало параметров)!!!\n", __PRETTY_FUNCTION__);
        return -1;
    }

    int ver   = strtol(valInfoList.at(0).c_str(), nullptr, 10);
    valInfoList.erase(valInfoList.begin());

    int count = strtol(valInfoList.at(0).c_str(), nullptr, 10);
    valInfoList.erase(valInfoList.begin());

    keyInfoList.reserve(static_cast<size_t>(count));

    if ( (valInfoList.size() % INFO_LIST_SIZE) != 0 )
    {
        printf("%s: Ошибка парса списка активированных услуг!!!\n", __PRETTY_FUNCTION__);
        return -1;
    }

    for (vector<string>::const_iterator iter = valInfoList.begin(); iter != valInfoList.end(); ++iter)
    {
        DreamkasKey key;

        key.index = strtol((*iter).c_str(), nullptr, 10);
        ++iter;

        key.name = (*iter);
        ++iter;

        key.status = strtol((*iter).c_str(), nullptr, 10);
        ++iter;

        key.dateEnd = (*iter);

        keyInfoList.push_back(key);
    }

    return ver;
}

vector<string> libSplitString(string divisibleStr, char delimiter)
{
    vector<string> rv;

    if ( divisibleStr.empty() )
    {
        return rv;
    }

    string tmpString;
    istringstream streamStr(divisibleStr);

    while ( getline(streamStr, tmpString, delimiter) )
    {
       rv.push_back( tmpString );
    }

    return rv;
}
//======================================================================================

int libRestartKKT()
{
    pirit_io.setPacketMode(PACKET_ON);
    int err = pirit_io.commandVoidMData(PIRIT_RESTART_KKT).errCode;
    pirit_io.setPacketMode(PACKET_OFF);
    return err;
}

int libGetDocFromKL(const uint32_t &docNum, DocFromKL& doc)
{
    pirit_io.makeFirstPartPacketToSend(PIRIT_GET_DOC_FROM_KL);
    pirit_io.addInt( docNum );
    pirit_io.makeEndPartPacket();

    int err = pirit_io.connectSock();
    if( err != 0 )
    {
        return err;
    }

    err = pirit_io.sendData();
    if( err != 0 )
    {
        return err;
    }

    err = pirit_io.readBinaryData();
    if (err == 0)
    {
        const vector<uint8_t>& binaryData = pirit_io.getBinryData();
        err = doc.parse(binaryData) ? FB_NOT_ERROR : FB_ERROR_NO_DATA_REQUESTED;

        if ( pirit_io.DEBUGLEVEL == DEBUG_ON )
        {
            doc.viewOpen();
            doc.viewItems();
            doc.viewTotals();
            doc.viewPayments();
            doc.viewDiscounts();
            doc.viewClose();
        }
    }

    pirit_io.disconnectSock();

    return err;
}

int libSetNVR(const SetNVRcmd &setNVRcmd)
{
    if (setNVRcmd.data == nullptr)
    {
        return -1;
    }

    pirit_io.makeFirstPartPacketToSend(PIRIT_SET_NVR);
    pirit_io.addInt(SetNVRcmd::TYPE);
    pirit_io.addInt(setNVRcmd.address);
    pirit_io.addInt(setNVRcmd.size / SetNVRcmd::ASCII_BYTE_LEN);
    pirit_io.addChar(setNVRcmd.data, setNVRcmd.size);
    pirit_io.makeEndPartPacket();

    int err = pirit_io.connectSock();
    if (err != 0)
    {
        return err;
    }
    err = pirit_io.sendData();
    if (err == 0)
    {
        err = pirit_io.readData();
    }
    pirit_io.disconnectSock();

    return err;
}

int libSetWiFiModule(WIFI_MODULE_PARAM wifiModuleParam, const WiFiModuleData &wiFiModuleData)
{
    constexpr unsigned int maxHostLen = 32;
    constexpr unsigned int maxPassLen = 64;
    pirit_io.makeFirstPartPacketToSend(PIRIT_SET_WIFI_NETWORK);
    pirit_io.addInt(wifiModuleParam);
    int err {-1};

    switch (wifiModuleParam)
    {
        case WIFI_MODULE_PARAM::WIFI_MODULE_SET_HOME_NETWORK:
        {
            pirit_io.addString((wiFiModuleData.SSID.length() > maxHostLen) ? wiFiModuleData.SSID.substr(0, maxHostLen - 1) : wiFiModuleData.SSID);
            pirit_io.addString((wiFiModuleData.pass.length() > maxPassLen) ? wiFiModuleData.pass.substr(0, maxPassLen - 1) : wiFiModuleData.pass);
            break;
        }
        case WIFI_MODULE_PARAM::WIFI_MODULE_SET_LOGS:
        {
            pirit_io.addString((wiFiModuleData.logIp.length() > maxHostLen) ? wiFiModuleData.logIp.substr(0, maxHostLen - 1) : wiFiModuleData.logIp);
            pirit_io.addString((wiFiModuleData.lopPort.length() > maxPassLen) ? wiFiModuleData.lopPort.substr(0, maxPassLen - 1) : wiFiModuleData.lopPort);
            break;
        }
        default: err = -1; break;
    }

    pirit_io.makeEndPartPacket();

    err = pirit_io.connectSock();

    if (err != 0)
    {
        return err;
    }

    err = pirit_io.sendData();
    pirit_io.disconnectSock();

    return err;
}

int libGetWiFiModuleNetwork(WIFI_MODULE_NETWORK_PARAM wifiModuleNetworkParam, WiFiModuleNetwork &wifiModuleNetwork)
{
    pirit_io.makeFirstPartPacketToSend(PIRIT_GET_WIFI_NETWORK);
    pirit_io.addInt(wifiModuleNetworkParam);
    pirit_io.makeEndPartPacket();

    int err = pirit_io.connectSock();

    if (err != 0)
    {
        return err;
    }

    err = pirit_io.sendData();

    if (err == 0)
    {
        err = pirit_io.readData();
        if (err == 0)
        {
            wifiModuleNetwork.clear();
            constexpr uint32_t maxValueLen {32};
            char tmp[maxValueLen] = {0x00};
            err = pirit_io.parseAnswerN<char>(PIRIT_PARAM_0, *tmp);

            if (err == 0)
            {
                switch (wifiModuleNetworkParam)
                {
                    case WIFI_MODULE_NETWORK_PARAM::WIFI_MODULE_GET_HOME_NETWORK:
                    {
                        wifiModuleNetwork.host = std::string(tmp);
                        memset(tmp, 0x00, maxValueLen);
                        err = pirit_io.parseAnswerN<char>(PIRIT_PARAM_1, *tmp);

                        if (err == 0)
                        {
                            wifiModuleNetwork.ip = std::string(tmp);
                        }
                        break;
                    }
                    case WIFI_MODULE_NETWORK_PARAM::WIFI_MODULE_GET_LOCAL_NETWORK:
                    {
                        wifiModuleNetwork.localhost = std::string(tmp);
                        memset(tmp, 0x00, maxValueLen);
                        err = pirit_io.parseAnswerN<char>(PIRIT_PARAM_1, *tmp);

                        if (err == 0)
                        {
                            wifiModuleNetwork.localIp = std::string(tmp);
                        }
                        break;
                    }
                    case WIFI_MODULE_NETWORK_PARAM::WIFI_MODULE_GET_MAC_ADDRESS: wifiModuleNetwork.mac = std::string(tmp); break;
                    case WIFI_MODULE_NETWORK_PARAM::WIFI_MODULE_GET_DNS: wifiModuleNetwork.dns = std::string(tmp); break;
                    default: err = -1; break;
                }
            }
        }
    }

    pirit_io.disconnectSock();

    return err;
}

int libGetFiscalVersion(FISCAL_DEVICE device, string &version)
{
    pirit_io.makeFirstPartPacketToSend(PIRIT_KKT_INFO);
    pirit_io.addInt(device?PIRIT_KKT_INFO_STRING_VERSION_PRINTER:PIRIT_KKT_INFO_STRING_VERSION_WIFI);
    pirit_io.makeEndPartPacket();
	const int max_size_version = 12;
    int err = pirit_io.connectSock();

    if (err != 0)
    {
        return err;
    }

    err = pirit_io.sendData();

    if (err == 0)
    {
        err = pirit_io.readData();
        if (err == 0)
        {
            version.clear();
            char data[max_size_version];
            memset(data, 0, sizeof(data));

            err = pirit_io.parseAnswerN<char>(PIRIT_PARAM_1, *data);
            version = string(data);
        }
    }

    pirit_io.disconnectSock();

    return err;
}

int libUpdateFiscal(FISCAL_DEVICE device,const string& url,const string& md5)
{
    pirit_io.makeFirstPartPacketToSend(PIRIT_UPDATE_FISCAL);
    pirit_io.addInt(device);
    pirit_io.addConstChar(url.c_str());
    pirit_io.addConstChar(md5.c_str());
    pirit_io.makeEndPartPacket();

    int err = pirit_io.connectSock();

    if (err != 0)
    {
        return err;
    }

    err = pirit_io.sendData();

    if (err == 0)
    {
        const int TIMEOUT = 360000;
        err = pirit_io.readData(TIMEOUT);

    }

    pirit_io.disconnectSock();

    return err;
}

