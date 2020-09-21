#pragma once

#include <map>
#include <string>
#include <vector>

#include <string.h>
#include <unistd.h>

using namespace std;


#define NUM_OF_SECTOINS 16

enum CoefficientTypes
{
	NONE = 0, DISCOUNT_PERCENT, DISCOUNT_SUM, MARGIN_PERCENT, MARGIN_SUM
};

typedef struct tagDataToSend
{
        char data[8192] = {0};
        int length      =   0;
}DataToSend;

typedef struct tagPLDateTime
{
        int year              = 0;
        unsigned char month   = 0;
        unsigned char day     = 0;
        unsigned char hours   = 0;
        unsigned char minutes = 0;
        unsigned char seconds = 0;
}PLDateTime;

typedef struct DateTimeStruct
{
    int year    = 0;
    int month   = 0;
    int day     = 0;
    int hours   = 0;
    int minutes = 0;
    int seconds = 0;
}DateTime;

typedef struct tagPLDate
{
        int year            = 0;
        unsigned char month = 0;
        unsigned char day   = 0;
}PLDate;

typedef struct tagPLTime
{
        unsigned char hours   = 0;
        unsigned char minutes = 0;
        unsigned char seconds = 0;
}PLTime;

typedef struct tagMData {
        int errCode    =   0;
        char data[256] = {0};
        int dataLength =   0;
}MData;

struct CloseDocResult
{
    int errCode  = 0;
    uint32_t fd  = 0;
    uint32_t fpd = 0;
    uint32_t shiftNum   = 0;
    uint32_t docInShift = 0;
    string dateDoc;
    string timeDoc;
};

struct DreamkasKey
{
    uint16_t index = 0;
    uint8_t status = 0;
    string dateEnd;
    string name;
};

// Структура для агентов
struct AdditionalReq
{
	uint16_t           agentType; // (Целое число) Признак агента по предмету расчёта
        string goodNumCode,  // (Строка[1..32]) Код товарной номенклатуры
            addReq,  // (Строка[1..64]) Дополнительный реквизит предмета расчёта
            scale,  // (Строка[1..16]) Единица измерения предмета расчёта
            providerInn,  // (Строка)[0..12] ИНН поставщика
            providerTel,  // (Строка)[0..19] Телефон поставщика
            providerName,  // (Строка)[0..256] Наименование поставщика
            operatorAddr,  // (Строка)[0..256] Адрес оператора перевода (для
                           // банк.пл.агента/банк.пл.субагента, иначе пустой)
            operatorInn,  // (Строка)[0..12] ИНН оператора перевода (для
                          // банк.пл.агента/банк.пл.субагента, иначе пустой)
            operatorName,  // (Строка)[0..64] Наименование оператора перевода
                           // (для банк.пл.агента/банк.пл.субагента, иначе
                           // пустой)
            operatorTel,  // (Строка)[0..19] Телефон оператора перевода (для
                          // банк.пл.агента/банк.пл.субагента, иначе пустой)
            plAgentOperation,  // (Строка)[0..24] Операция платежного агента
                               // (для банк.пл.агента/банк.пл.субагента, иначе
                               // пустой)
            plAgentTel,  // (Строка)[0..19] Телефон платежного агента (для
                         // пл.агента/пл.субагента, иначе пустой)
            payOperatorTel;  // (Строка)[0..19] Телефон оператора по приему
                             // платежей (для пл.агента/пл.субагента, иначе
                             // пустой)
        AdditionalReq() { agentType = 0; }
        void clear() {
          agentType = 0;
          goodNumCode.clear();
          addReq.clear();
          scale.clear();
          providerInn.clear();
          providerTel.clear();
          providerName.clear();
          operatorAddr.clear();
          operatorInn.clear();
          operatorName.clear();
          operatorTel.clear();
          plAgentOperation.clear();
          plAgentTel.clear();
          payOperatorTel.clear();
        }
};

/**
 * @brief The KL_DOC_TYPE enum Структуры документа из КЛ (РБ)
 */
enum KL_DOC_TYPE
{
    KL_DOC_TYPE_OPEN = 1,
    KL_DOC_TYPE_CLOSE,
    KL_DOC_TYPE_CLOSESPEC,
    KL_DOC_TYPE_CANCEL,
    KL_DOC_TYPE_ITEM,
    KL_DOC_TYPE_DISCOUNT,
    KL_DOC_TYPE_MARKUP,
    KL_DOC_TYPE_PAIMENT,
    KL_DOC_TYPE_SUBTOTAL,
    KL_DOC_TYPE_TOTAL,
    KL_DOC_TYPE_ODD,
    KL_DOC_TYPE_CASHINOUT,
    KL_DOC_TYPE_CASH,
    KL_DOC_TYPE_RECV,
    KL_DOC_TYPE_TOTALDISCOUNT,
    KL_DOC_TYPE_TOTALMARKUP,
    KL_DOC_TYPE_TAXGROUP,
    KL_DOC_TYPE_TAX,

    KL_DOC_TYPE_ANNUL_DOC,
    KL_DOC_TYPE_HIDE = 32
};

/**
 * @brief The KL_DOC_LENGTH enum Размера данных в структурах документа из КЛ
 */
enum KL_DOC_LENGTH
{
    KL_DOC_LENGTH_STR_OPERATOR  = 18,
    KL_DOC_LENGTH_STR_CASHIN    = 21,
    KL_DOC_LENGTH_STR_PARAM     = 32,
    KL_DOC_LENGTH_DATE_PARAM    = 6,
    KL_DOC_LENGTH_TIME_PARAM    = 6,
    KL_DOC_LENGTH_NUM_PARAM     = 10,
    KL_DOC_LENGTH_FIX_PARAM     = 16,
    KL_DOC_LENGTH_DISCONT_NAME  = 38,
    KL_DOC_LENGTH_POS_NAME      = 256,
    KL_DOC_LENGTH_POS_ART       = 18,
    KL_DOC_LENGTH_POS_NUM       = 4,
    KL_DOC_LENGTH_POS_DIM       = 4,
    KL_DOC_MAX_SIZE_PAYMENT_STR = 18,
    KL_DOC_MAX_SIZE_TAX_STR     = 18,
    KL_DOC_IDENTIFICATOR_LEN    = 12,
    KL_DOC_REQUISITE_STR        = 74
};

#pragma pack(1)

/**
 * @brief The DateR struct - структура даты из КЛ
 */
struct DateR
{
    unsigned char  day   = 0x00; /* 1-31 */
    unsigned char  month = 0x00; /* 1-12 */
    unsigned short year  = 0;    /* 1980-2078 */
};

/**
 * @brief The TimeR struct - структура времени из КЛ
 */
struct TimeR
{
    unsigned char hour   = 0x00;   /* 0-23 */
    unsigned char minute = 0x00;   /* 0-59 */
    unsigned char second = 0x00;   /* 0-59 */
};

/**
 * @brief The CDate struct - структура даты и времени из КЛ
 */
struct CDate
{
    DateR dosdate;
    TimeR dostime;
};

/**
 * @brief The KlDocIndex struct
 */
struct KlDocIndex
{
    uint32_t      DataAdressStart = 0;
    uint32_t      DataAdressEnd   = 0;
    uint32_t      DocNum          = 0;
    uint16_t      Shift           = 0;
    CDate         Date;
    uint8_t       reserved[10]    = {0x00};
    unsigned char Crc             = 0x00;
};

/**
 * @brief The KlDocOpen struct - структура открытия документа
 */
struct KlDocOpen
{
    uint8_t       snum      = 0x00; // Тип структуры
    uint8_t       doctype   = 0x00; // Тип документа
    uint16_t      operatorN = 0;    // Номер оператора
    uint16_t      depnum    = 0;    // Номер отдела
    long          trnnum    = -1;   // Номер чека
    uint16_t      shift     = 0;    // Номер смены
    uint32_t      docnum    = 0;    // Номер документа
    CDate         date;             // Дата
    char          operatorS[KL_DOC_LENGTH_STR_OPERATOR + 1] = {0x00};   // ФИО оператора
    unsigned char Crc       = 0x00; // Контрольная сумма
};

/**
 * @brief The KlDocClose struct - структура закрытия документа
 */
struct KlDocClose
{
    uint8_t       snum  = 0x00; // Тип структуры
    int64_t       total = -1;   // Сумма
    CDate         Date;         // Дата
    uint8_t       Identificator[KL_DOC_IDENTIFICATOR_LEN] = {0x00}; // Идентификатор
    unsigned char Crc   = 0x00; // Контрольная сумма
};

/**
 * @brief The KlDocAnnulDoc struct - структура аннулирования документа
 */
struct KlDocAnnulDoc
{
    uint8_t       snum      = 0;    // Тип структуры
    uint16_t      operatorN = 0;    // Номер оператора
    uint16_t      depnum    = 0;    // Номер отдела
    long          trnnum    = -1;   //
    uint16_t      shift     = 0;    // Номер смены
    uint32_t      docnum    = 0;    // Номер документа
    CDate         Date;             // Дата
    char          operatorS[KL_DOC_LENGTH_STR_OPERATOR + 1] = {0x00};   // ФИО оператора
    uint32_t      docNumberForAnnul = 0;    // Номер аннулирования
    int64_t       total     = 0;            // Сумма
    uint8_t       Identificator[KL_DOC_IDENTIFICATOR_LEN]   = {0x00};   // Идентификатор
    unsigned char Crc       = 0x00; // Контрольная сумма
};

/**
 * @brief The KlDocTotal struct - структура итог/подитог + итог/подитог скидок и наценок
 */
struct KlDocTotal
{
    uint8_t       snum  = 0x00; // Тип структуры
    uint8_t       add   = 0x00; // Тип документа
    int64_t       total = -1;   // Сумма
    unsigned char Crc   = 0x00; // Контрольная сумма
};

/**
 * @brief The KlDocItem struct - структура позиции
 */
struct KlDocItem
{
    uint8_t       snum   = 0x00;    // Тип структуры
    uint8_t       storno = 0x00;    // Признак коррекции
    int64_t       price  = -1;      // Цена
    int64_t       qty    = -1;      // Количество
    int64_t       total  = -1;      // Сумма
    char          Dim[KL_DOC_LENGTH_POS_DIM + 1]  = {0x00}; // Мерность
    char          number[KL_DOC_LENGTH_POS_NUM+1] = {0x00}; // Номер позиции
    char          art[KL_DOC_LENGTH_POS_ART+1]    = {0x00}; // Артикул
    char          name[KL_DOC_LENGTH_POS_NAME+1]  = {0x00}; // Наименование
    uint16_t      sect   = 0;       // Номер секции
    uint16_t      taxnum = 0;       // Номер налога в таблице
    unsigned char Crc    = 0x00;    // Контрольная сумма
};

/**
 * @brief The KlDocDiscount struct - структура скидки/наценки
 */
struct KlDocDiscount
{
    uint8_t       snum   = 0x00;    // Тип структуры
    char          onitem = 0x00;    // Признак скидки/наценки на позицию
    char          abs    = 0x00;    // Признак абсолютной скидки/наценки
    char          storno = 0x00;    // Признак коррекции
    char          name[KL_DOC_LENGTH_DISCONT_NAME+1] = {0x00};  // Наименование
    int64_t       perc   = -1;      // Процент
    int64_t       sum    = -1;      // Сумма
    unsigned char Crc    = 0x00;    // Контрольная сумма
};

/**
 * @brief The KlDocPayment struct - структура оплаты
 */
struct KlDocPayment
{
    uint8_t        snum  = 0x00;    // Тип структуры
    char           addText[KL_DOC_LENGTH_STR_PARAM+1]  = {0x00};    // Наименование оплаты
    char           name[KL_DOC_MAX_SIZE_PAYMENT_STR+1] = {0x00};    // Тип оплаты
    int64_t        total = -1;      // Сумма
    unsigned char Crc    = 0x00;    // Контрольная сумма
};

/**
 * @brief The KlDocCashInOut struct - структура внесения/изъятия
 */
struct KlDocCashInOut
{
    uint8_t       snum  = 0x00; // Тип структуры
    char          name[KL_DOC_LENGTH_STR_PARAM + 1] = {0x00};   // Наименование
    int64_t       total = -1;   // Сумма
    unsigned char Crc   = 0x00; // Контрольная сумма
};

/**
 * @brief The KlDocPrintRecv struct - структура реквизитов
 */
struct KlDocPrintRecv
{
    uint8_t       snum = 0x00;  // Тип структуры
    uint8_t       atr  = 0x00;  // Атрибут печати
    uint8_t       lf   = 0x00;  // Признак перевода строки
    char          text[KL_DOC_REQUISITE_STR]  = {0x00}; // Текст реквизита
    unsigned char Crc  = 0x00;  // Контрольная сумма
};

/**
 * @brief The KlDocTax struct - структура налогов
 */
struct KlDocTax
{
    uint8_t       snum   = 0x00;    // Тип структуры
    uint16_t      taxnum = 0;       // Номер налога в таблице
    char          name[KL_DOC_MAX_SIZE_TAX_STR + 1] = {0x00};   // Наименование
    int64_t       perc   = 0;       // Процент
    int64_t       sum    = 0;       // Сумма
    unsigned char Crc    = 0x00;    // Контрольная сумма
};

struct CSumms
{
    int64_t docSumm       = -1;
    int64_t discSumm      = -1;
    int64_t markSumm      = -1;
    int64_t discSummPos   = -1;
    int64_t markSummPos   = -1;
    int64_t summTotal     = -1;
    int64_t summCashless  = -1;
    int64_t summCash      = -1;
    int64_t summCashOther = -1;
};

#pragma pack()

struct DocFromKL
{
    DocFromKL()
    {
        typeToLen.insert({KL_DOC_TYPE_OPEN,            sizeof(KlDocOpen)});
        typeToLen.insert({KL_DOC_TYPE_CLOSE,           sizeof(KlDocClose)});
        typeToLen.insert({KL_DOC_TYPE_CLOSESPEC,       sizeof(KlDocClose)});
        typeToLen.insert({KL_DOC_TYPE_CANCEL,          sizeof(KlDocClose)});
        typeToLen.insert({KL_DOC_TYPE_ITEM,            sizeof(KlDocItem)});
        typeToLen.insert({KL_DOC_TYPE_DISCOUNT,        sizeof(KlDocDiscount)});
        typeToLen.insert({KL_DOC_TYPE_MARKUP,          sizeof(KlDocDiscount)});
        typeToLen.insert({KL_DOC_TYPE_PAIMENT,         sizeof(KlDocPayment)});
        typeToLen.insert({KL_DOC_TYPE_SUBTOTAL,        sizeof(KlDocTotal)});
        typeToLen.insert({KL_DOC_TYPE_TOTAL,           sizeof(KlDocTotal)});
        typeToLen.insert({KL_DOC_TYPE_ODD,             sizeof(KlDocTotal)});
        typeToLen.insert({KL_DOC_TYPE_CASHINOUT,       sizeof(KlDocCashInOut)});
        typeToLen.insert({KL_DOC_TYPE_CASH,            sizeof(KlDocTotal)});
        typeToLen.insert({KL_DOC_TYPE_RECV,            sizeof(KlDocPrintRecv)});
        typeToLen.insert({KL_DOC_TYPE_TOTALDISCOUNT,   sizeof(KlDocTotal)});
        typeToLen.insert({KL_DOC_TYPE_TOTALMARKUP,     sizeof(KlDocTotal)});
        typeToLen.insert({KL_DOC_TYPE_TAXGROUP,        sizeof(KlDocPrintRecv)});
        typeToLen.insert({KL_DOC_TYPE_TAX,             sizeof(KlDocTax)});
    }

    KlDocIndex             index;
    KlDocOpen              open;
    KlDocClose             close;
    KlDocAnnulDoc          annulDoc;
    vector<KlDocTotal>     totals;
    vector<KlDocItem>      items;
    vector<KlDocDiscount>  discounts;
    vector<KlDocPayment>   payments;
    KlDocCashInOut         cashInOut;
    vector<KlDocPrintRecv> printRecvs;
    KlDocTax               tax;
    CSumms                 sums;

    map<KL_DOC_TYPE, size_t> typeToLen;

    static constexpr uint8_t SOH_BYTE = 0x01;
    static constexpr uint8_t STX_BYTE = 0x02;
    static constexpr uint8_t ACK_BYTE = 0x06;

    bool parse(const vector<uint8_t>& data)
    {
        size_t pos = 0;
        const size_t DATA_LEN = data.size();

        while (pos < data.size())
        {
            if (data[pos] != SOH_BYTE)
            {
                printf("\n Invalid delimeter (0x%02x)!\n", data[pos]);
                return false;
            }

            KL_DOC_TYPE step = static_cast<KL_DOC_TYPE>(data[++pos]);

            if (typeToLen.count(step) == 0)
            {
                printf("\n Invalid type struct (%d)!\n", step);
                return false;
            }

            const size_t STRUCT_LEN = typeToLen.at(step);
            if ((pos + STRUCT_LEN) > DATA_LEN)
            {
                printf("\n Error parse data!!!\n");
                return false;
            }

            switch (step)
            {
                case KL_DOC_TYPE_OPEN:
                {
                    memcpy(&open, &data[pos], STRUCT_LEN);
                    break;
                }
                case KL_DOC_TYPE_CLOSE:
                case KL_DOC_TYPE_CLOSESPEC:
                {
                    memcpy(&close, &data[pos], STRUCT_LEN);
                    break;
                }
                case KL_DOC_TYPE_CANCEL:
                {
                    memcpy(&annulDoc, &data[pos], STRUCT_LEN);
                    break;
                }
                case KL_DOC_TYPE_ITEM:
                {
                    KlDocItem item;
                    memcpy(&item, &data[pos], STRUCT_LEN);
                    items.push_back(item);
                    break;
                }
                case KL_DOC_TYPE_DISCOUNT:
                case KL_DOC_TYPE_MARKUP:
                {
                    KlDocDiscount discount;
                    memcpy(&discount, &data[pos], STRUCT_LEN);
                    discounts.push_back(discount);
                    break;
                }
                case KL_DOC_TYPE_PAIMENT:
                {
                    KlDocPayment payment;
                    memcpy(&payment, &data[pos], STRUCT_LEN);
                    payments.push_back(payment);
                    break;
                }
                case KL_DOC_TYPE_SUBTOTAL:
                case KL_DOC_TYPE_TOTAL:
                case KL_DOC_TYPE_TOTALDISCOUNT:
                case KL_DOC_TYPE_TOTALMARKUP:
                case KL_DOC_TYPE_CASH:
                case KL_DOC_TYPE_ODD:
                {
                    KlDocTotal total;
                    memcpy(&total, &data[pos], STRUCT_LEN);
                    totals.push_back(total);
                    break;
                }
                case KL_DOC_TYPE_CASHINOUT:
                {
                    memcpy(&cashInOut, &data[pos], STRUCT_LEN);
                    break;
                }
                case KL_DOC_TYPE_RECV:
                case KL_DOC_TYPE_TAXGROUP:
                {
                    KlDocPrintRecv printRecv;
                    memcpy(&printRecv, &data[pos], STRUCT_LEN);
                    printRecvs.push_back(printRecv);
                    break;
                }
                case KL_DOC_TYPE_TAX:
                {
                    memcpy(&tax, &data[pos], STRUCT_LEN);
                    break;
                }
                default:
                {
                    printf("\n Error parse binary data (Unknown struct type)!!!\n");
                    return false;
                }
            }

            pos += STRUCT_LEN;

            if (data[pos] == STX_BYTE)
            {
                break;
            }

            if (data[pos] == ACK_BYTE)
            {
                ++pos;
            }
        }

        return true;
    }

    void viewCDate(const CDate &cdate)
    {
        printf("\tDATE\n");
        printf("\t\tDay:\t%u\n",   static_cast<uint16_t>(cdate.dosdate.day));
        printf("\t\tMonth:\t%u\n", static_cast<uint16_t>(cdate.dosdate.month));
        printf("\t\tYear:\t%u\n",  cdate.dosdate.year);
        printf("\t\tHour:\t%u\n",  static_cast<uint16_t>(cdate.dostime.hour));
        printf("\t\tMinutes:%u\n", static_cast<uint16_t>(cdate.dostime.minute));
        printf("\t\tSeconds:%u\n", static_cast<uint16_t>(cdate.dostime.second));
    }

    void viewIndex()
    {
        printf("INDEX\n");
        printf("\tDataAdressStart:\t%u\n", index.DataAdressStart);
        printf("\tDataAdressEnd:\t%u\n",   index.DataAdressEnd);
        printf("\tDoc num:\t%u\n",         index.DocNum);
        printf("\tShift:\t%u\n",           index.Shift);
        viewCDate(index.Date);
        printf("\tCRC:\t%u\n",             static_cast<uint16_t>(index.Crc));
    }

    void viewOpen()
    {
        printf("OPEN\n");
        printf("\tSnum:\t%u\n",      static_cast<uint16_t>(open.snum));
        printf("\tDoc type:\t%u\n",  static_cast<uint16_t>(open.doctype));
        printf("\tOperatorN:\t%u\n", open.operatorN);
        printf("\tDepnum:\t%u\n",    open.depnum);
        printf("\tTrnum:\t%ld\n",    open.trnnum);
        printf("\tShift:\t%u\n",     open.shift);
        printf("\tDoc num:\t%u\n",   open.docnum);
        viewCDate(open.date);
        printf("\tOperators:\t%s\n", open.operatorS);
        printf("\tCRC:\t%u\n",       static_cast<uint16_t>(open.Crc));
    }

    void viewClose()
    {
        printf("CLOSE\n");
        printf("\tsnum:\t%u\n",     static_cast<uint16_t>(close.snum));
        printf("\ttotal:\t%u\n",    static_cast<uint16_t>(close.total));
        viewCDate(close.Date);
        printf("\tIdentificator: ");
        for (uint64_t i = 0; i < sizeof(close.Identificator); ++i)
        {
            printf("%02x", close.Identificator[i]);
        }
        printf("\n");
        printf("\tCRC:\t\t%u\n",    static_cast<uint16_t>(close.Crc));
    }

    void viewAnnulDoc()
    {
        printf("Annul Doc\n");
        printf("\tsnum:\t%u\n",          static_cast<uint16_t>(annulDoc.snum));
        printf("\toperatorN:\t%u\n",     annulDoc.operatorN);
        printf("\tdepnum:\t%u\n",        annulDoc.depnum);
        printf("\tTrnum:\t%ld\n",        annulDoc.trnnum);
        printf("\tShift:\t%u\n",         annulDoc.shift);
        printf("\tDoc num:\t%u\n",       annulDoc.docnum);
        viewCDate(annulDoc.Date);
        printf("\tOperators:\t%s\n",     annulDoc.operatorS);
        printf("\tTotal:\t%lld\n",       annulDoc.total);
        printf("\tIdentificator:\t%s\n", annulDoc.Identificator);
        printf("\tCRC:\t%u\n",           static_cast<uint16_t>(annulDoc.Crc));
    }

    void viewTotals()
    {
        for (const KlDocTotal &total : totals)
        {
            printf("TOTAL\n");
            printf("\tsnum:\t%u\n",    static_cast<uint16_t>(total.snum));
            printf("\tadd:\t%u\n",     static_cast<uint16_t>(total.add));
            printf("\tTotal:\t%lld\n", total.total);
            printf("\tCRC:\t%u\n\n",   static_cast<uint16_t>(total.Crc));
        }
    }

    void viewItems()
    {
        for (const KlDocItem &item : items)
        {
            printf("ITEM\n");
            printf("\tsnum:\t%u\n",    static_cast<uint16_t>(item.snum));
            printf("\tstorno:\t%u\n",  static_cast<uint16_t>(item.storno));
            printf("\tprice:\t%lld\n", item.price);
            printf("\tqty:\t%lld\n",   item.qty);
            printf("\tTotal:\t%lld\n", item.total);
            printf("\tDim:\t%s\n",     item.Dim);
            printf("\tnumber:\t%s\n",  item.number);
            printf("\tart:\t%s\n",     item.art);
            printf("\tname:\t%s\n",    item.name);
            printf("\tsect:\t%u\n",    item.sect);
            printf("\ttaxnum:\t%u\n",  item.taxnum);
            printf("\tCRC:\t%u\n\n",   static_cast<uint16_t>(item.Crc));
        }
    }

    void viewDiscounts()
    {
        for (const KlDocDiscount &discount: discounts)
        {
            printf("DISCOUNT\n");
            printf("\tsnum:\t%u\n",   static_cast<uint16_t>(discount.snum));
            printf("\tonitem:\t%u\n", static_cast<uint16_t>(discount.onitem));
            printf("\tabs:\t%u\n",    static_cast<uint16_t>(discount.abs));
            printf("\tstorno:\t%u\n", static_cast<uint16_t>(discount.storno));
            printf("\tname:\t%s\n",   discount.name);
            printf("\tperc:\t%lld\n", discount.perc);
            printf("\tsum:\t%lld\n",  discount.sum);
            printf("\tCRC:\t%u\n\n",  static_cast<uint16_t>(discount.Crc));
        }
    }

    void viewPayments()
    {
        for (const KlDocPayment &payment : payments)
        {
            printf("PAYMENT\n");
            printf("\tsnum:\t%u\n",    static_cast<uint16_t>(payment.snum));
            printf("\taddText:\t%s\n", payment.addText);
            printf("\tname:\t%s\n",    payment.name);
            printf("\tTotal:\t%lld\n", payment.total);
            printf("\tCRC:\t%u\n\n",   static_cast<uint16_t>(payment.Crc));
        }
    }

    void viewCashInOut()
    {
        printf("CASH IN OUT\n");
        printf("\tsnum:\t%u\n",    static_cast<uint16_t>(cashInOut.snum));
        printf("\tname:\t%s\n",    cashInOut.name);
        printf("\tTotal:\t%lld\n", cashInOut.total);
        printf("\tCRC:\t%u\n",     static_cast<uint16_t>(cashInOut.Crc));
    }

    void viewPrintRecvs()
    {
        for (const KlDocPrintRecv& printRecv : printRecvs)
        {
            printf("PRINT RECV\n");
            printf("\tsnum:\t%u\n", static_cast<uint16_t>(printRecv.snum));
            printf("\tatr:\t%u\n",  static_cast<uint16_t>(printRecv.atr));
            printf("\tlf:\t%u\n",   static_cast<uint16_t>(printRecv.lf));
            printf("\ttext:\t%s\n", printRecv.text);
            printf("\tCRC:\t%u\n\n",  static_cast<uint16_t>(printRecv.Crc));
        }
    }

    void viewTax()
    {
        printf("TAX\n");
        printf("\tsnum:\t%u\n",   static_cast<uint16_t>(tax.snum));
        printf("\ttaxnum:\t%u\n", tax.taxnum);
        printf("\tname:\t%s\n",   tax.name);
        printf("\tperc:\t%lld\n", tax.perc);
        printf("\tsum:\t%lld\n",  tax.sum);
        printf("\tCRC:\t%u\n",    static_cast<uint16_t>(tax.Crc));
    }

    void viewSums()
    {
        printf("SUMS\n");
        printf("\tdocSumm:\t%lld\n",       sums.docSumm);
        printf("\tdocSumm:\t%lld\n",       sums.discSumm);
        printf("\tmarkSumm:\t%lld\n",      sums.markSumm);
        printf("\tdiscSummPos:\t%lld\n",   sums.discSummPos);
        printf("\tmarkSummPos:\t%lld\n",   sums.markSummPos);
        printf("\tsummTotal:\t%lld\n",     sums.summTotal);
        printf("\tsummCashless:\t%lld\n",  sums.summCashless);
        printf("\tsummCash:\t%lld\n",      sums.summCash);
        printf("\tsummCashOther:\t%lld\n", sums.summCashOther);
    }
};

//-------------------------------------------------
//  Errors in exchange/io socket
enum PIRIT_LOW_LVL_ERRS
{
   NO_ERRORS    =    0,
   BAD_PACK_NUM = 1002
};
//-------------------------------------------------
//  Номер параметра
enum PIRIT_PARAM_NUMBER
{
	PIRIT_PARAM_0 = 0,
	PIRIT_PARAM_1,
	PIRIT_PARAM_2,
	PIRIT_PARAM_3,
	PIRIT_PARAM_4,
	PIRIT_PARAM_5,
	PIRIT_PARAM_6,
	PIRIT_PARAM_7,
	PIRIT_PARAM_8,
	PIRIT_PARAM_9,
	PIRIT_PARAM_10,
	PIRIT_PARAM_11,
	PIRIT_PARAM_12,
	PIRIT_PARAM_13,
	PIRIT_PARAM_14,
	PIRIT_PARAM_15,
	PIRIT_PARAM_16,
	PIRIT_PARAM_17,
	PIRIT_PARAM_18,
	PIRIT_PARAM_19,
	PIRIT_PARAM_20,
	PIRIT_PARAM_21,
	PIRIT_PARAM_22,
	PIRIT_PARAM_23,
	PIRIT_PARAM_24,
	PIRIT_PARAM_25,
	PIRIT_PARAM_26,
	PIRIT_PARAM_27,
	PIRIT_PARAM_28,
	PIRIT_PARAM_29,
	PIRIT_PARAM_30,
	PIRIT_PARAM_31,
	PIRIT_PARAM_32,
};
//-------------------------------------------------
//  Команды PIRIT RF
enum PIRIT_COMMAND
{
    PIRIT_KKT_STATUS                = 0x00,     //  Запрос флагов статуса ККТ
    PIRIT_COUNTERS_AND_REGISTERS    = 0x01,     //  Запрос сменных счетчиков и регистров
    PIRIT_KKT_INFO                  = 0x02,     //  Запрос сведений о ККТ
    PIRIT_RECEIPT_DATA              = 0x03,     //  Запрос данных по чеку
    PIRIT_PRINTER_STATUS            = 0x04,     //  Запрос состояния печатающего устройства (ПУ)
    PIRIT_SERVICE_INFO              = 0x05,     //  Запрос сервисной информации
    PIRIT_EX_ERROR_INFO             = 0x06,     //  Запрос дополнительной информации об ошибках
    PIRIT_COMMAND_START             = 0x10,     //  Начало работы с ККТ
    PIRIT_READ_TABLE_SETTINGS       = 0x11,     //  Чтение таблицы настроек
    PIRIT_WRITE_TABLE_SETTINGS      = 0x12,     //  Запись таблицы настроек
    PIRIT_READ_DATE_TIME_KKT        = 0x13,     //  Чтение даты/времени ККТ
    PIRIT_WRITE_DATE_TIME_KKT       = 0x14,     //  Запись даты/времени ККТ
    PIRIT_LOAD_LOGO                 = 0x15,     //  Программировать логотип
    PIRIT_DELETE_LOGO               = 0x16,     //  Удалить логотип
    PIRIT_LOAD_RECEIPT_DESIGN       = 0x17,     //  Загрузить дизайн чека
    PIRIT_LOAD_PICTURE              = 0x18,     //  Загрузить графическое изображение
    PIRIT_PRINT_X_REPORT            = 0x20,     //  Сформировать отчет без гашения (X-отчет)
    PIRIT_PRINT_Z_REPORT            = 0x21,     //  Сформировать отчет о закрытии смены
    PIRIT_OPEN_SHIFT                = 0x23,     //  Открыть смену
    PIRIT_ADD_POS_REQUISITE         = 0x24,     //  Установить дополнительные реквизиты позиции
    PIRIT_OPEN_DOCUMENT             = 0x30,     //  Открыть документ
    PIRIT_CLOSE_DOCUMENT            = 0x31,     //  Завершить документ
    PIRIT_CANCEL_DOCUMENT           = 0x32,     //  Аннулировать документ
    PIRIT_POSTPONE_DOCUMENT         = 0x33,     //  Отложить документ
    PIRIT_CUT_DOCUMENT              = 0x34,     //  Отрезать документ
    PIRIT_PRINT_STRING              = 0x40,     //  Печать текста
    PIRIT_PRINT_BARCODE             = 0x41,     //  Печатать штрих-код
    PIRIT_ADD_POSITION              = 0x42,     //  Добавить товарную позицию
    PIRIT_DELETE_POSITION           = 0x43,     //  Удалить товарную позицию
    PIRIT_SUB_TOTAL                 = 0x44,     //  Подытог
    PIRIT_ADD_DISCOUNT              = 0x45,     //  Скидка на чек
    PIRIT_ADD_MARGIN                = 0x46,     //  Надбавка на чек
    PIRIT_ADD_PAYMENT               = 0x47,     //  Оплата
    PIRIT_CASH_IN_OUT               = 0x48,     //  Внесение / изъятие суммы
    PIRIT_PRINT_REQUISITE           = 0x49,     //  Печать реквизита
    PIRIT_REG_SUM_TO_DEPT           = 0x50,     //  Зарегистрировать сумму по отделу
    PIRIT_REG_TAX_SUM               = 0x51,     //  Зарегистрировать сумму налога
    PIRIT_COMPARE_SUM               = 0x52,     //  Сравнить сумму по чеку
    PIRIT_OPEN_COPY_RECEIPT         = 0x53,     //  Открыть копию чека
    PIRIT_ZEROING_CASH_IN_DRAWER    = 0x54,     //  Обнулить наличные в денежном ящике
    PIRIT_PRINT_PICTURE_IN_DOCUMENT = 0x55,     //  Печать графики в документе
    PIRIT_PRINT_PRELOAD_PICTURE     = 0x56,     //  Печать загруженной картинки
    PIRIT_PRINT_REQUISIT_OFD        = 0x57,     //  Печать реквизита для ОФД
    PIRIT_RECEIPT_CORRECTION        = 0x58,     //  Печать чека коррекции
    PIRIT_CURRENT_STATUS_REPORT     = 0x59,     //  Печать отчета о текущем состоянии расчетов
    PIRIT_REGISTRATION              = 0x60,     //  Регистрация / перерегистрация
    PIRIT_FISCAL_REPORT_BY_SHIFT    = 0x61,     //  Фискальный отчет по сменам
    PIRIT_FISCAL_REPORT_BY_DATE     = 0x62,     //  Фискальный отчет по дате
    PIRIT_TECHNOLOGICAL_RESET       = 0x63,     //  Технологическое обнуление
    PIRIT_ACTIVIZATION_FN           = 0x70,     //  Активизация ФН ???
    PIRIT_CLOSE_FN                  = 0x71,     //  Закрытие ФН
    PIRIT_GET_DOC_FROM_KL           = 0x71,     //  Получить на ПК документ из КЛ
    PIRIT_PRINT_CONTROL_TAPE_FN     = 0x72,     //  Печать контрольной ленты ФН
    PIRIT_PRINT_DOC_FROM_FN         = 0x73,     //  Распечатать документ из ФН
    PIRIT_REPORT_FROM_FN_BY_SHIFTS  = 0x74,     //  Распечатать отчет по сменам из ФН
    PIRIT_REPORT_FROM_FN_BY_DATE    = 0x75,     //  Распечатать отчет по дате из ФН
    PIRIT_REPORT_ACTIVIZATION_FN    = 0x76,     //  Распечатать отчет по активизации ФН
    PIRIT_REPORT_FROM_FN_BY_SHIFT   = 0x77,     //  Распечатать отчет по смене из ФН
    PIRIT_GET_INFO_FROM_FN          = 0x78,     //  Обмен информацией с ФН
    PIRIT_OPEN_DRAWER               = 0x80,     //  Открыть денежный ящик
    PIRIT_GET_DRAWER_STATUS         = 0x81,     //  Получить статус денежного ящика
    PIRIT_BEEP                      = 0x82,     //  Подать звуковой сигнал
    PIRIT_AUTHORIZATION             = 0x90,     //  Авторизация ККТ
    PIRIT_READ_MEMORY_BLOCK         = 0x91,     //  Чтение блока памяти ККТ
    PIRIT_SET_SPEED                 = 0x93,     //  Установить скорость обмена
    PIRIT_PRINT_SERVICE_DATA        = 0x94,     //  Распечатать сервисные данные
    PIRIT_PRINT_SERVICE_DATA_SKNO   = 0x96,     //  Распечатать сервисные данные СКНО
    PIRIT_SET_NVR                   = 0x97,     // Запись NVR
    PIRIT_RESTART_KKT               = 0x9C,     //  Перезапуск ККТ (переинициализация без перезапуска процесса)
    PIRIT_EMERGENCY_CLOSE_SHIFT     = 0xA0,     //  Аварийное закрытие смены
    PIRIT_PRINT_COPY_LAST_Z_REPORT  = 0xA1,     //  Печать копии последнего отчета о закрытии смены
    PIRIT_PRINT_COPY_REPORT_REG     = 0xA3,     //  Печать копии чека регистрации/перерегистрации
    PIRIT_EMERGENCY_CLOSE_FN        = 0xA4,     //  Аварийное закрытие ФН
    PIRIT_GET_STATUS_OFD            = 0xA5,     //  Проверка доступности сервера ОФД
    PIRIT_DREAMKAS_KEY              = 0xAA      //  Команды для работы с Дримкас-Ключ
};

//  Команды PIRIT RF
enum EXCHANGE_FN
{
	EXCHANGE_FN_REG_NUM      = 0x01,
	EXCHANGE_FN_STATUS       = 0x02,
	EXCHANGE_FN_FD_LAST_DOC = 0x03,
	EXCHANGE_FN_DATE_RED = 0x04,
	EXCHANGE_FN_LAST_FD = 0x05,
	EXCHANGE_FN_SHIFT_STATE = 0x06,
	EXCHANGE_FN_OFD_STATE = 0x07,
	EXCHANGE_FN_DOC_FROM_ARC = 0xB,
	EXCHANGE_FN_REQ_QUITTANCE = 0x0C,
	EXCHANGE_FN_LAST_ERRS = 0x0D,
	EXCHANGE_FN_AND_FFD_VER = 0x0E,
	EXCHANGE_FN_SELECT_TLV_DOC = 0x0F,
	EXCHANGE_FN_GET_TLV_DOC = 0x10,
	EXCHANGE_FN_FIRST_FD_IN_SHIFT = 0x11,
	EXCHANGE_FN_LAST_REG_DATA = 0x13,

    // get reg rep from fn for crpt
    EXCHANGE_FN_SELECT_TLV_REG = 0x14,
    EXCHANGE_FN_GET_TLV_REG = 0x15
};
