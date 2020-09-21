#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <time.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <vector>
#include <sstream>
#include <typeinfo>
#include <typeindex>

#include "pirit_structs.h"

enum DeviceMode { COM_MODE = 0, TCP_MODE };
enum PacketType { SEND = 0, RECIEVE = 1 };          // Тип пакета на отправку/на приём
enum Country    { RF = 0, RB = 1 };                 // Страна, в которой будет работать касса
enum DEBUG_LEVEL { DEBUG_OFF = 0, DEBUG_ON = 1 };   // Флаг отладочного вывода

/**
 * @brief The PACKET_MODE enum - возможные типы формирования команд
 */
enum PACKET_MODE
{
    PACKET_OFF = 0, // Пакетный режим OFF
    PACKET_ON  = 1  // Пакетный режим ON
};

const unsigned char STX = 0x02;                             // Начало пакета
const unsigned char ETX = 0x03;                             // Конец пакета
const unsigned char password_RF[4] = { 'P', 'I', 'R', 'I' };   // Захардкорженый пароль в пакете ДЛЯ РОССИИ
const unsigned char password_RB[4] = { 'D', 'R', 'R', 'B' };   // Захардкорженый пароль в пакете ДЛЯ БЕЛОРУСИИ

const unsigned char START_ID = 0x20;                        // Стартовый ID в пакете


class CheckBuffer
{
private:
    bool accumulate = false;
    string commands;
public:
    inline bool getAccumulate()           { return accumulate;}
    inline void clear()                   { commands.clear();accumulate = false;}
    inline void setAccumulate(bool isAcc) { accumulate = isAcc;}
    void addCommand(string& command)      { commands+=command;}
    bool empty()                          {return commands.empty();}
    string getAllCommands()
    {
        string retStr = commands;
        commands.clear();
        setAccumulate(false);
        return retStr;
    }
};


class PIRIT_IOCLASS
{
private:

    // Сокет
    int err;
    int sd_out;
    struct sockaddr_in dest_out;
    std::string ip;
    int       port;

    // Данные
    DataToSend       dataToSend;      // Пакет на отправку (STX	Пароль связи	ID пакета	Код команды	Данные	ETX	    CRC)
    DataToSend       dataToRead;      // Принятый пакет    (STX	ID пакета	    Код команды	Код ошибки	Данные	ETX	    CRC)
    DataToSend           answer;
    int     AmountDecimalPlaces;
    int   QuantityDecimalPlaces;
    int PercentageDecimalPlaces;
    unsigned char      packetID;
    unsigned char    packetMode;

    /**
     * @brief dataToReadBinary - контейнер для хранения ответных данных в бинарном виде
     */
    vector<uint8_t> dataToReadBinary;

    void printPacket(int n, PacketType type);  // Вывод отправляемых и получаемых данных в консоль в режиме отладки

    void clearData(PacketType type);
    long pow10(long power);
    void setPackedID(unsigned char newID);
    unsigned char incPackedID();
    unsigned char getPackedID();
    int readAck();

    template <typename TYPE> bool strToVal(TYPE &val, char *tmpBuf);     // Перевод строки в переменную др типа


public:
    PIRIT_IOCLASS();

    CheckBuffer checkBuffer;

    DEBUG_LEVEL DEBUGLEVEL;     // 0 - не выводим пакет, 1 - выводим в консоль

    // Однотипные начало и конец пакета на отправку
    int makeFirstPartPacketToSend(unsigned cmd);
    int makeEndPartPacket();

    // Данные в пакет на отправку
    void addChar(char* str, size_t size = 0);
    /**
     * @brief addString добавить строку в пакет
     * @param text текст
     * @param limit предел количества символов
     */
    void addString(std::string text, const size_t &limit = 0);
    void addByte(unsigned char byte);
    void addEmptyByte();
    void addInt(int twoByte);
    void addLong(long fourByte);
    void addDouble(double data);
    void addDoubleFromLong(long data, int numFormat);
    void addDoubleFromInt64(int64_t data, int numFormat);
    void addConstChar(const char* str);

    // Время в пакет на отправку
    void addDate(PLDate piritDate);
    void addTime(PLTime piritTime);
    void addDateTime(int day, int month, int year, int hour,int min, int sec);
    void addDateTime(PLDateTime dt);

    // Формирование команд пирит из входных параметров
    int commandVoid                       (int command);
    MData commandVoidMData                (int command);
    MData commandByteEx                   (int command);
    MData commandByte(int command, unsigned char dByte);
    int commandVoidInt         (int command, int *data);

    // Парсинг ответа
    int parseAnswer();                   // разбор пакета на код ошибки и ДАТА

    /**
     * @brief getBinryData - получить ответ бинарных данных
     * @return контейнер с бинарными данными
     */
    const vector<uint8_t>& getBinryData();
    //------------------
    // Кайфовый парс полей
    template <typename TYPE> int parseAnswerN(uint32_t n, TYPE &val, int *length = NULL); // Выцеживаем из строки answer значение int, double, char*, int32 или int64,   порядковый номер N в пакете, отсчёт от 0
    int parseAnswerChN_constCount  ( int number, char **val, int &valLength );       // Выцеживаем из строки answer массив char val[valLength], порядковый номер N в пакете, отсчёт от 0
    //------------------
    // Мусорный парс полей
    int parseAnswer1Int    (int  *data); // Выцеживаем из строки answer значение INT в data, данные как ASCII   (первое значение)
    int parseAnswer1Char   (char *data); // Выцеживаем из строки answer значение char в data, данные как ASCII  (первое значение)     ПЕРВЫЙ найденный ИГНОРИМ !!!
    int parseAnswer1CharEx (char *data); // Выцеживаем из строки answer значение char* в data, данные как ASCII (первое значение)     ПЕРВЫЙ найденный ИГНОРИМ
    int parseAnswer2Char   (char *data); // Выцеживаем из строки answer значение char* в data, данные как ASCII (первые 2 значения)   ПЕРВЫЙ найденный ИГНОРИМ
    int parseAnswerChar(int count, int num, char *data); // парс строки чаров, переменная номер "num"[0..n-1] из "count" переменных
    int parseAnswerIntXd(int intData[], int maxElement, int type);
    int parseAnswerIntXdEx(int intData[], int maxElement, int type);
    int parseAnswer1Int2Int3Int(int *i1, int *i2, int *i3);
    int parseAnswerCommand0x00(int *fatalStatus, int *currentFlagsStatus, int *documentStatus);        // Запрос флагов статуса ККТ (0x00)
    int parseAnswerCommand0x00_4args(int *fatalStatus, int *currentFlagsStatus, int *documentStatus, int *sknoStatus);  // Запрос флагов статуса ККТ (0x00) (4 аргумента, но это неточно)
    int parseAnswerCommand0x01_12(int intData[], int maxElement, char *str);

    //------------------
    // Настройки
    void setIP_io(std::string   newIP)  { ip = newIP;    if(DEBUGLEVEL == DEBUG_ON) printf("\n\n\tPIRIT_IO:: SetIP:: ip = '%s'\n", ip.c_str() ); }
    void setPort_io(int  newPort)  { port = newPort; }
    void setDebugLevel(DEBUG_LEVEL level);
    void setPacketMode(unsigned char newPacketMode);
    void setPasswd(Country newCountry);
    unsigned char getPacketMode();

    //------------------
    // Сеть
    int connectSock();
    int disconnectSock();
    int sendData();
    int readData();
    int readBinaryData();
    void getMData(MData *mData);
    int scrollPaper();
    int loadDataToPirit(int size, unsigned char* data);
    int loadPictureToPirit(int width, int height, int sm, unsigned char* data);

    void setAmountDecimalPlaces(int decimalPlaces);
    void setQuantityDecimalPlaces(int decimalPlaces);
    void setPercentageDecimalPlaces(int decimalPlaces);
    int getAmountDecimalPlaces();
    int getQuantityDecimalPlaces();
    int getPercentageDecimalPlaces();
};




//======================================================================================
//
template <typename TYPE> bool PIRIT_IOCLASS::strToVal(TYPE &val, char *tmpBuf)
{
   if(typeid(TYPE) == typeid(int))
    {
        val = atoi(tmpBuf);
    }
    else if( typeid(TYPE) == typeid(uint32_t) || typeid(TYPE) == typeid(unsigned int) )
    {
        val = strtoul(tmpBuf, NULL, 10);
    }
    else if( typeid(TYPE) == typeid(uint64_t)  || typeid(TYPE) == typeid(long long unsigned int) )
    {
        val = strtoull(tmpBuf, NULL, 10);
    }
    else if( typeid(TYPE) == typeid(double) )
    {
        val =  strtod(tmpBuf, NULL);
    }
    else if( typeid(TYPE) == typeid(float) )
    {
        val = strtof(tmpBuf, NULL);
    }
    else if( typeid(TYPE) == typeid(char) )
    {
        strcpy((char*)&val, tmpBuf);
    }
    else
    {
        printf("<typename TYPE> strToVal:: Unknown type!");
        return false;
    }
    return true;
}


//======================================================================================
// Вычлинение из пакета поля порядковый номер N, отсчёт от 0,  типа инт, юинт32, юинт64, дабл, флот, чар*
template <typename TYPE> int PIRIT_IOCLASS::parseAnswerN( uint32_t n, TYPE &val,int *length)
{
    int i, len;
    uint32_t counter = 0;

    const char BUF_LEN = 128;
    char tmpBuf[BUF_LEN] = {0};

    len = 0;
    for(i = 0; i < answer.length; i++)
    {
        if(answer.data[i] == 0x1C)
        {
            if(counter == n)
            {
                if( !strToVal<TYPE>(val, tmpBuf) )
                {

                    return 1989;
                }
                if (length != NULL )  // Сохраняем длину, если есть указатель
                {
                    *length = len;
                }
            }
            memset(tmpBuf, 0, sizeof(tmpBuf));
            counter ++;
            len = 0;
        }
        else
        {
            tmpBuf[len] = answer.data[i];
            len++;
        }
    }
    // Если последний блок по какой то причине не имеет признака окончания 1С, то его тоже пишем
    if ( len > 0 )
    {
        if(counter == n)
        {
            if( !strToVal<TYPE>(val, tmpBuf) )
            {
                return 1989;
            }
            if (length != NULL )  // Сохраняем длину, если есть указатель
            {
                *length = len;
            }
        }
        memset(tmpBuf, 0, sizeof(tmpBuf));
        counter ++;
        len = 0;
    }
    if(counter >= (n+1) )
    {
        return 0;
    }
    printf("%s:: Error! Num of fields in packet = %d, try get PIRIT_PARAM_%d \n",__func__, counter, n);
    return 2222;
}
