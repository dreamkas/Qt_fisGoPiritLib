#ifndef COUNTERS_H_
#define COUNTERS_H_
#include <vector>
#include <stdio.h>

#include "pirit_structs.h"
#include "pirit_low_func.h"

using namespace std;
// Объект для общения по протоколу пирит, методы ниже его юзают
extern PIRIT_IOCLASS pirit_io;
//========================================================================================
// Счётчики
//========================================================================================
enum CNTRS_TYPE
{
    CNTRS_NONE                  =  0,
    CNTRS_TYPE_CUR_SHIFT        =  1,
    CNTRS_TYPE_NEXT_REC         =  2,
    CNTRS_TYPE_SUM_BY_PAY       =  3,
    CNTRS_TYPE_CNT_BY_PAY       =  4,
    CNTRS_TYPE_SUM_BY_PAY_RET   =  5,
    CNTRS_TYPE_CNT_BY_PAY_RET   =  6,
    CNTRS_TYPE_REC_CNT_BY_TYPE  =  7,
    CNTRS_TYPE_REC_SUM_BY_TYPE  =  8,
    CNTRS_TYPE_DISC_SUMS        =  9,
    CNTRS_TYPE_TAX_SALE_SUMS    = 10,
    CNTRS_TYPE_TAX_RET_SUMS     = 11,
    CNTRS_TYPE_X_Z_DATA         = 12,
    CNTRS_TYPE_DEP_SALE_SUM     = 13,
    CNTRS_TYPE_DEP_SALE_RET_SUM = 14,
    CNTRS_TYPE_REC_COUNT        = 15,
    CNTRS_TYPE_SALE_SUM_BY_TYPE = 16,
    CNTRS_TYPE_RET_SUM_BY_TYPE  = 17,
    CNTRS_TYPE_COR_DATA         = 18
};
//========================================================================================
// Начало пакета на запрос любых счётчиков
//========================================================================================
inline int countersHeader(CNTRS_TYPE field)
{
    int err;
    pirit_io.makeFirstPartPacketToSend(PIRIT_COUNTERS_AND_REGISTERS);
    pirit_io.addInt( (int)field );
    pirit_io.makeEndPartPacket();

    err = pirit_io.connectSock();
    if( err != 0 )
    {
        printf("countersHeader():: Error while connecting with Punix!");
        return err;
    }
    err = pirit_io.sendData();
    if (err == 0)
    {
        err = pirit_io.readData();
    }
    else
    {
        printf("countersHeader()::sendData FAILED!!!");
    }

    pirit_io.disconnectSock();
    return 0;
}
//========================================================================================
/**
 *  Сменные счётчики
 *  суммы      -   double
 *  количество - uint64_t
 */
//========================================================================================
template<class T>
class BiosCounters
{
    public:
        BiosCounters()
        {
            reset();
        }
        ~BiosCounters()
        {
            reset();
        }
    private:
        CNTRS_TYPE countersType = CNTRS_NONE;
        vector<T> countersTable;
        /**
         * Добавить счётчик
         * @brief addCountersValue
         * @param value - значение счётчика
         */
        inline void addCountersValue(T value){ countersTable.push_back(value); }
    public:
        /**
         * Выгрузить тип счётчиков
         * @brief getCountersType
         * @return
         */
        inline CNTRS_TYPE getCountersType(){ return countersType; }
        /**
         * @brief getCounter - выгрузить значение счётчика
         * @param counterNum [0; countersTable.size() - 1]
         * @return
         */
        T getCounter( uint8_t counterNum )
        {
            if(counterNum >= countersTable.size())
            {
                return T(0);
            }
            return countersTable.at(counterNum);
        }
        /**
         * @brief setCountersType - установить тип счётчика
         * @return
         */
        void setCountersType(CNTRS_TYPE type)
        {
            // Тест на валидность типа счётчиков
            if((type < CNTRS_NONE) || (type > CNTRS_TYPE_COR_DATA))
            {
                type = CNTRS_NONE;
            }
            countersType = type;
            countersTable.clear();
        }
        /**
         * @brief getCountersTable - выгрузить таблицу счётчиков
         * @return
         */
        inline vector<T> getCountersTable(){ return countersTable; }
        /**
         * @brief reset - сброс счётчиков
         */
        void reset()
        {
            countersType = CNTRS_NONE;
            countersTable.clear();
        }
        /**
         * @brief piritParamsParse - парсинг счётчиков
         * @param paramQuantity - максимальный номер параметра для парса
         * @return
         */
        int piritParamsParse( PIRIT_PARAM_NUMBER maxNumber )
        {
            int res = -1;
            for(unsigned int valueNumber = PIRIT_PARAM_1; valueNumber <= maxNumber; valueNumber++)
            {
                T value;
                res = pirit_io.parseAnswerN<T>( static_cast<PIRIT_PARAM_NUMBER>(valueNumber), value);
                if(res != 0)
                {
                    return res;
                }
                if(pirit_io.DEBUGLEVEL == DEBUG_ON)
                {
                    string fmt = "Counter value #%u : " + formatGet(typeid(value).hash_code() ) + "\n";
                    printf( fmt.c_str(), valueNumber, value);
                }
                addCountersValue(value);
            }
            return res;
        }
        /**
         * @brief read - считывание счётчиков из bios
         * @return
         */
        int read()
        {
            if(pirit_io.DEBUGLEVEL == DEBUG_ON)
            {
                printf("\t%s\n", __PRETTY_FUNCTION__);
            }
            countersTable.clear();
            int res = countersHeader(getCountersType());
            if( res != 0 )
            {
                return res;
            }
            switch(getCountersType())
            {
                case CNTRS_TYPE_SUM_BY_PAY:
                case CNTRS_TYPE_CNT_BY_PAY:
                case CNTRS_TYPE_SUM_BY_PAY_RET:
                case CNTRS_TYPE_CNT_BY_PAY_RET:
                {
                    res = piritParamsParse( PIRIT_PARAM_16 );
                    break;
                }
                case CNTRS_TYPE_REC_CNT_BY_TYPE:
                {
                    res = piritParamsParse( PIRIT_PARAM_6 );
                    break;
                }
                case CNTRS_TYPE_REC_SUM_BY_TYPE:
                case CNTRS_TYPE_DISC_SUMS:
                {
                    res = piritParamsParse( PIRIT_PARAM_4 );
                    break;
                }
                default:
                {
                    res = -1;
                    break;
                }

            }
            return res;
        }
        /**
         * @brief formatGet - RTTI определения
         *        формата вывода значения
         *        счётчика
         * @param typeHashCode - hash код
         *        типа данных счётчика
         * @return
         */
        string formatGet(size_t typeHashCode)
        {
            // RTTI
            if(typeHashCode == typeid(double).hash_code())
            {
                return "%f";
            }
            else if(typeHashCode== typeid(uint64_t).hash_code())
            {
                return "%llu";
            }
            else
            {
                return "";
            }
        }
};

//------------------------------------------------------------------
//                        Суммы операций
//------------------------------------------------------------------
template< class T >
class OPERATION
{
    public:
    OPERATION()
    {
        cash     = 0;
        card     = 0;
        advance  = 0;
        credit   = 0;
        exchange = 0;
    };

    T         cash, // наличными
              card, // электронными
           advance, // аванс
            credit, // кредит
          exchange; // встречное представление
};
//------------------------------------------------------------------
//                 Количество оформленных чеков
//------------------------------------------------------------------
struct REC_CNTS
{
    REC_CNTS()
    {
        sale      = 0;
        ret       = 0;
        canceled  = 0;
        deferred  = 0;
        insert    = 0;
        reserve   = 0;

    };
    uint64_t     sale, // количество чеков продажи
                  ret, // количество чеков возврата
             canceled, // количество аннулированных чеков
             deferred, // количество отложенных чеков
               insert, // количество чеков внесения
              reserve; // количество чеков изъятия
};
//------------------------------------------------------------------
//                 Суммы по оформленным чекам
//------------------------------------------------------------------
struct REC_SUMS
{
    REC_SUMS()
    {
        canceled  = 0;
        deferred  = 0;
        insert    = 0;
        reserve   = 0;

    };
    double  canceled, // суммы по аннулированным чекам
            deferred, // суммы по отложенным чекам
              insert, // суммы по чекам внесения
             reserve; // суммы по чекам изъятия
};
//------------------------------------------------------------------
//                    Суммы по скидкам
//------------------------------------------------------------------
struct DISCOUNT_SUMS
{
    DISCOUNT_SUMS()
    {
        sale  = 0;
        ret  = 0;
    };
    double     sale, // сумма скидок по продажам
                ret; // сумма скидок по возвратам
};
//------------------------------------------------------------------
//                     Суммы налогов
//------------------------------------------------------------------
struct TAX_SUMS
{
    TAX_SUMS()
    {
        nds_18       = 0;
        nds_10       = 0;
        nds_0        = 0;
        without_nds  = 0;
        nds_18_118   = 0;
        nds_10_110   = 0;
    };
    double      nds_18,
                nds_10,
                 nds_0,
           without_nds,
            nds_18_118,
            nds_10_110;
};

//------------------------------------------------------------------
// Данные по последнему X отчету или отчету о закрытии смены
//------------------------------------------------------------------
struct X_Z_DATA
{
    X_Z_DATA()
    {
        operation_cnt.clear();
        doc_num        = 0;
        cash_in_drawer = 0;
        sale_cnt       = 0;
        sale_sum       = 0;
        ret_cnt        = 0;
        ret_sum        = 0;
        canceled_cnt   = 0;
        canceled_sum   = 0;
        insert_cnt     = 0;
        insert_sum     = 0;
        reserve_cnt    = 0;
        reserve_sum    = 0;
    };

    string    operation_cnt; // текущий операционный счетчик
    uint64_t        doc_num; // номер документа
    double   cash_in_drawer; // сумма в кассе
    uint64_t       sale_cnt; // кол-во продаж (кол-во приходов)
    double         sale_sum; // сумма продаж (сумма приходов)
    uint64_t        ret_cnt; // кол-во возвратов (возвратов прихода)
    double          ret_sum; // сумма возвратов (возвратов прихода)
    uint64_t   canceled_cnt; // количество аннулированных
    double     canceled_sum; // сумма аннулированных
    uint64_t     insert_cnt; // количество внесений
    double       insert_sum; // сумма внесений
    uint64_t    reserve_cnt; // оличество изъятий
    double      reserve_sum; // сумма изъятий
};
//------------------------------------------------------------------
// Вернуть данные по коррекциям
//------------------------------------------------------------------
struct COR_DATA
{
    COR_DATA()
    {
        cnt  = 0;
        cash = 0;
        card = 0;
    }
    uint64_t  cnt; // количество коррекций
    double   cash; // суммы коррекций  наличными
    double   card; // суммы коррекций  безналичными
};
//------------------------------------------------------------------
struct COUNTERS_RF
{
    uint64_t                     shift_num; // номер текущей смены
    uint64_t                  next_rec_num; // номер следующего чека
    OPERATION <double>           sale_sums; // суммы продаж по типам платежа
    OPERATION <uint64_t>         sale_cnts; // количество оплат по продажам
    OPERATION <double>       ret_sale_sums; // суммы возвратов прихода по типам платежа
    OPERATION <uint64_t>     ret_sale_cnts; // количество оплат по возвратам прихода
    REC_CNTS                      rec_cnts; // количество оформленных чеков по типам операций
    REC_SUMS                      rec_sums; // суммы по оформленным чекам
    DISCOUNT_SUMS            discount_sums; // суммы по скидкам
    TAX_SUMS                     sale_taxs; // суммы налогов по продажам
    TAX_SUMS                      ret_taxs; // суммы налогов по возвратам
    X_Z_DATA                      x_z_data; // данные по последнему X отчету или отчету о закрытии смены
    double   dep_sale_sum[NUM_OF_SECTOINS]; // суммы по секциям/отделам по продажам
    double    dep_ret_sum[NUM_OF_SECTOINS]; // суммы по секциям/отделам по возвратам
    uint64_t                  purchase_cnt; // кол-во чеков расходов          0x15
    uint64_t              ret_purchase_cnt; // кол-во чеков возвратов расхода 0x15
    OPERATION <double>       purchase_sums; // суммы расходов по типам платежа (по максимально возможному количеству типов платежей)
    OPERATION <double> ret_purchase_sums; // суммы возвратов расхода  по типам платежа (по максимально возможному количеству типов платежей)
    COR_DATA                      cor_data; // данные по коррекциям
};

int libGetCounters_CurShiftNum     ( uint64_t               &shiftNum );  // 0x01 Номер тек смены
int libGetCounters_NextRecNum      ( uint64_t             &nextRecNum );  // 0x02 Вернуть номер следующего чека
int libGetCounters_SaleSumByPay    ( OPERATION<double>          &ssbp );  // 0x03 Вернуть суммы продаж по типам платежа
int libGetCounters_SaleCntByPay    ( OPERATION<uint64_t>        &scbp );  // 0x04 Вернуть количество оплат по продажам
int libGetCounters_RetSaleSumByPay ( OPERATION<double>         &rssbp );  // 0x05 Вернуть суммы возвратов по типам платежа
int libGetCounters_RetSaleCntByPay ( OPERATION<uint64_t>       &rscbp );  // 0x06 Вернуть количество оплат по возвратам
int libGetCounters_RecCntsByType   ( REC_CNTS                &recCnts );  // 0x07 Количество оформленных чеков
int libGetCounters_RecSumsByType   ( REC_SUMS                &recSums );  // 0x08 Суммы по оформленным чекам
int libGetCounters_DiscountSums    ( DISCOUNT_SUMS          &discSums );  // 0x09 Суммы по скидкам
int libGetCounters_TaxSumsSale     ( TAX_SUMS                &taxSums );  // 0x10 Суммы налогов  по продажам
int libGetCounters_TaxSumsRets     ( TAX_SUMS                &taxSums );  // 0x11 Суммы налогов  по возвратам
int libGetCounters_XZ              ( X_Z_DATA                 &xzData );  // 0x12 Данные по последнему X отчету или отчету о закрытии смены
int libGetCounters_depSaleSum      ( double         (&depSaleSum)[NUM_OF_SECTOINS] );  // 0x13 Вернуть суммы по секциям/отделам по продажам
int libGetCounters_depRetSum       ( double          (&depRetSum)[NUM_OF_SECTOINS] );  // 0x14 Вернуть суммы по секциям/отделам по возвратам
int libGetCounters_RecCount        ( uint64_t            &purchaseCnt ,   // 0x15 Кол-во чеков расходов
                                     uint64_t         &retPurchaseCnt );  // 0x15 Кол-во чеков возвратов расхода
int libGetCounters_SaleSumByType   ( OPERATION<double>     &purchSums );  // 0x16 Вернуть суммы покупок по типам платежа
int libGetCounters_RetSumByType    ( OPERATION<double>  &purchSumsRet );  // 0x17 Вернуть суммы возвратов покупок по типам платежа
int libGetCounters_CorData         ( COR_DATA                &corData );  // 0x18 Вернуть данные по коррекциям

#endif
