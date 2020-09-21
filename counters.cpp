#include "biosCounters.h"
//========================================================================================
//========================================================================================
//======================================= Счётчики =======================================
//========================================================================================
//========================================================================================

extern PIRIT_IOCLASS pirit_io;

//========================================================================================
/**
 *  Чтение сменных счётчиков по типам платежа
 */
//========================================================================================
// 0x01 0x01 Номер тек смены
int libGetCounters_CurShiftNum(uint64_t &shiftNum)
{
    shiftNum = 0;

    int res = countersHeader(CNTRS_TYPE_CUR_SHIFT);
    if( res != 0 )
    {
        return res;
    }

    res |= pirit_io.parseAnswerN<uint64_t>(PIRIT_PARAM_1, shiftNum);
    if(pirit_io.DEBUGLEVEL == DEBUG_ON)
    {
        printf("--------\n");
        printf("libGetCounters_CurShiftNum: PARSE RES = %d  \n",      res);
        printf("\t shiftNum = %llud \n", shiftNum);
        printf("--------\n");
    }
    return res;
}

//========================================================================================
// 0x01 0x02 Вернуть номер следующего чека
int libGetCounters_NextRecNum(uint64_t &nextRecNum)
{
    nextRecNum = 0;

    int res = countersHeader(CNTRS_TYPE_NEXT_REC);
    if( res != 0 )
    {
        return res;
    }

    res |= pirit_io.parseAnswerN<uint64_t>(PIRIT_PARAM_1, nextRecNum);
    if(pirit_io.DEBUGLEVEL == DEBUG_ON)
    {
        printf("--------\n");
        printf("libGetCounters_NextRecNum: \n\tPARSE RES = %d   \n",        res);
        printf("\t nextRecNum = %llud\n", nextRecNum);
        printf("--------\n");
    }
    return res;
}
//========================================================================================
// 0x01 0x03 Вернуть суммы продаж по типам платежа
int libGetCounters_SaleSumByPay(OPERATION<double> &ssbp)
{
    ssbp.advance  = 0;
    ssbp.card     = 0;
    ssbp.cash     = 0;
    ssbp.credit   = 0;
    ssbp.exchange = 0;

    int res = countersHeader(CNTRS_TYPE_SUM_BY_PAY);
    if( res != 0 )
    {
        return res;
    }

    res |= pirit_io.parseAnswerN<double>( PIRIT_PARAM_1 ,     ssbp.cash);
    res |= pirit_io.parseAnswerN<double>( PIRIT_PARAM_2 ,     ssbp.card);
    res |= pirit_io.parseAnswerN<double>( PIRIT_PARAM_14,  ssbp.advance);
    res |= pirit_io.parseAnswerN<double>( PIRIT_PARAM_15,   ssbp.credit);
    res |= pirit_io.parseAnswerN<double>( PIRIT_PARAM_16, ssbp.exchange);
    if(pirit_io.DEBUGLEVEL == DEBUG_ON)
    {
        printf("--------\n");
        printf("libGetCounters_SaleSumByPay: \n\tPARSE RES = '%d'\n",           res);
        printf("\t cash      = %f\n",     ssbp.cash);
        printf("\t card      = %f\n",     ssbp.card);
        printf("\t advance   = %f\n",  ssbp.advance);
        printf("\t credit    = %f\n",   ssbp.credit);
        printf("\t exchange  = %f\n", ssbp.exchange);
        printf("--------\n");
    }
    return res;
}


//========================================================================================
// 0x01 0x04 Вернуть количество оплат по продажам
int libGetCounters_SaleCntByPay(OPERATION<uint64_t> &scbp)
{
    scbp.advance  = 0;
    scbp.card     = 0;
    scbp.cash     = 0;
    scbp.credit   = 0;
    scbp.exchange = 0;

    int res = countersHeader(CNTRS_TYPE_CNT_BY_PAY);
    if( res != 0 )
    {
        return res;
    }

    res |= pirit_io.parseAnswerN<uint64_t>( PIRIT_PARAM_1,      scbp.cash);
    res |= pirit_io.parseAnswerN<uint64_t>( PIRIT_PARAM_2,      scbp.card);
    res |= pirit_io.parseAnswerN<uint64_t>( PIRIT_PARAM_14,  scbp.advance);
    res |= pirit_io.parseAnswerN<uint64_t>( PIRIT_PARAM_15,   scbp.credit);
    res |= pirit_io.parseAnswerN<uint64_t>( PIRIT_PARAM_16, scbp.exchange);
    if(pirit_io.DEBUGLEVEL == DEBUG_ON)
    {
        printf("--------\n");
        printf("libGetCounters_SaleCntByPay: \n\tPARSE RES = '%d'\n",           res);
        printf("\t cash      = %llud\n",     scbp.cash);
        printf("\t card      = %llud\n",     scbp.card);
        printf("\t advance   = %llud\n",  scbp.advance);
        printf("\t credit    = %llud\n",   scbp.credit);
        printf("\t exchange  = %llud\n", scbp.exchange);
        printf("--------\n");
    }
    return res;
}


//========================================================================================
// 0x01 0x05 Вернуть суммы возвратов по типам платежа
int libGetCounters_RetSaleSumByPay(OPERATION<double> &rssbp)
{
    rssbp.advance  = 0;
    rssbp.card     = 0;
    rssbp.cash     = 0;
    rssbp.credit   = 0;
    rssbp.exchange = 0;

    int res = countersHeader(CNTRS_TYPE_SUM_BY_PAY_RET);
    if( res != 0 )
    {
        return res;
    }

    res |= pirit_io.parseAnswerN<double>( PIRIT_PARAM_1,     rssbp.cash);
    res |= pirit_io.parseAnswerN<double>( PIRIT_PARAM_2,     rssbp.card);
    res |= pirit_io.parseAnswerN<double>( PIRIT_PARAM_14,  rssbp.advance);
    res |= pirit_io.parseAnswerN<double>( PIRIT_PARAM_15,   rssbp.credit);
    res |= pirit_io.parseAnswerN<double>( PIRIT_PARAM_16, rssbp.exchange);
    if(pirit_io.DEBUGLEVEL == DEBUG_ON)
    {
        printf("--------\n");
        printf("libGetCounters_RetSaleSumByPay: \n\tPARSE RES = '%d'\n",            res);
        printf("\t cash      = %f\n",     rssbp.cash);
        printf("\t card      = %f\n",     rssbp.card);
        printf("\t advance   = %f\n",  rssbp.advance);
        printf("\t credit    = %f\n",   rssbp.credit);
        printf("\t exchange  = %f\n", rssbp.exchange);
        printf("--------\n");
    }
    return res;
}


//========================================================================================
// 0x01 0x06 Вернуть количество оплат по возвратам
int libGetCounters_RetSaleCntByPay(OPERATION<uint64_t> &rscbp)
{
    rscbp.advance  = 0;
    rscbp.card     = 0;
    rscbp.cash     = 0;
    rscbp.credit   = 0;
    rscbp.exchange = 0;

    int res = countersHeader(CNTRS_TYPE_CNT_BY_PAY);
    if( res != 0 )
    {
        return res;
    }

    res |= pirit_io.parseAnswerN<uint64_t>( PIRIT_PARAM_1,     rscbp.cash);
    res |= pirit_io.parseAnswerN<uint64_t>( PIRIT_PARAM_2,     rscbp.card);
    res |= pirit_io.parseAnswerN<uint64_t>( PIRIT_PARAM_14,  rscbp.advance);
    res |= pirit_io.parseAnswerN<uint64_t>( PIRIT_PARAM_15,   rscbp.credit);
    res |= pirit_io.parseAnswerN<uint64_t>( PIRIT_PARAM_16, rscbp.exchange);
    if(pirit_io.DEBUGLEVEL == DEBUG_ON)
    {
        printf("--------\n");
        printf("libGetCounters_RetSaleCntByPay: \n\tPARSE RES = '%d'\n",            res);
        printf("\t cash      = %llud\n",     rscbp.cash);
        printf("\t card      = %llud\n",     rscbp.card);
        printf("\t advance   = %llud\n",  rscbp.advance);
        printf("\t credit    = %llud\n",   rscbp.credit);
        printf("\t exchange  = %llud\n", rscbp.exchange);
        printf("--------\n");
    }
    return res;
}


//========================================================================================
// 0x01 0x07 Вернуть количество оформленных чеков по типам операций
int libGetCounters_RecCntsByType(REC_CNTS &recCnts)
{
    recCnts.sale     = 0;
    recCnts.ret      = 0;
    recCnts.canceled = 0;
    recCnts.deferred = 0;
    recCnts.insert   = 0;
    recCnts.reserve  = 0;

    int res = countersHeader(CNTRS_TYPE_REC_CNT_BY_TYPE);
    if( res != 0 )
    {
        return res;
    }

    res |= pirit_io.parseAnswerN<uint64_t>( PIRIT_PARAM_1,     recCnts.sale);
    res |= pirit_io.parseAnswerN<uint64_t>( PIRIT_PARAM_2,      recCnts.ret);
    res |= pirit_io.parseAnswerN<uint64_t>( PIRIT_PARAM_3, recCnts.canceled);
    res |= pirit_io.parseAnswerN<uint64_t>( PIRIT_PARAM_4, recCnts.deferred);
    res |= pirit_io.parseAnswerN<uint64_t>( PIRIT_PARAM_5,   recCnts.insert);
    res |= pirit_io.parseAnswerN<uint64_t>( PIRIT_PARAM_6,  recCnts.reserve);
    if(pirit_io.DEBUGLEVEL == DEBUG_ON)
    {
        printf("--------\n");
        printf("libGetCounters_RecCntsByType: \n\tPARSE RES = '%d'\n",                  res);
        printf("\t sale        = %llud\n",           recCnts.sale);
        printf("\t ret         = %llud\n",            recCnts.ret);
        printf("\t canceled    = %llud\n",       recCnts.canceled);
        printf("\t deferred    = %llud\n",       recCnts.deferred);
        printf("\t insert      = %llud\n",         recCnts.insert);
        printf("\t reserve     = %llud\n",        recCnts.reserve);
        printf("--------\n");
    }
    return res;
}


//========================================================================================
// 0x01 0x08 Вернуть суммы по оформленным чекам
int libGetCounters_RecSumsByType(REC_SUMS &recSums)
{
    recSums.canceled = 0;
    recSums.deferred = 0;
    recSums.insert   = 0;
    recSums.reserve  = 0;

    int res = countersHeader(CNTRS_TYPE_REC_SUM_BY_TYPE);
    if( res != 0 )
    {
        return res;
    }

    res |= pirit_io.parseAnswerN<double>( PIRIT_PARAM_1, recSums.canceled);
    res |= pirit_io.parseAnswerN<double>( PIRIT_PARAM_2, recSums.deferred);
    res |= pirit_io.parseAnswerN<double>( PIRIT_PARAM_3,   recSums.insert);
    res |= pirit_io.parseAnswerN<double>( PIRIT_PARAM_4,  recSums.reserve);
    if(pirit_io.DEBUGLEVEL == DEBUG_ON)
    {
        printf("--------\n");
        printf("libGetCounters_RecSumsByType: \n\tPARSE RES = '%d'\n", res);
        printf("\t canceled    = %f\n",       recSums.canceled);
        printf("\t deferred    = %f\n",       recSums.deferred);
        printf("\t insert      = %f\n",         recSums.insert);
        printf("\t reserve     = %f\n",        recSums.reserve);
        printf("--------\n");
    }
    return res;
}


//========================================================================================
// 0x01 0x09 Вернуть суммы по скидкам
int libGetCounters_DiscountSums(DISCOUNT_SUMS &discSums)
{
    discSums.sale     = 0;
    discSums.ret      = 0;

    int res = countersHeader(CNTRS_TYPE_DISC_SUMS);
    if( res != 0 )
    {
        return res;
    }

    res |= pirit_io.parseAnswerN<double>( PIRIT_PARAM_1, discSums.sale);
    res |= pirit_io.parseAnswerN<double>( PIRIT_PARAM_3, discSums.ret);
    if(pirit_io.DEBUGLEVEL == DEBUG_ON)
    {
        printf("--------\n");
        printf("libGetCounters_DiscountSums: \n\tPARSE RES = '%d'\n", res);
        printf("\t sale    = %f\n",       discSums.sale);
        printf("\t ret     = %f\n",       discSums.ret);
        printf("--------\n");
    }
    return res;
}


//========================================================================================
// 0x01 0x10 Вернуть суммы налогов по продажам
int libGetCounters_TaxSumsSale(TAX_SUMS &taxSums)
{
    taxSums.nds_18         = 0;
    taxSums.nds_10         = 0;
    taxSums.nds_0          = 0;
    taxSums.without_nds    = 0;
    taxSums.nds_18_118     = 0;
    taxSums.nds_10_110     = 0;

    int res = countersHeader(CNTRS_TYPE_TAX_SALE_SUMS);
    if( res != 0 )
    {
        return res;
    }

    res |= pirit_io.parseAnswerN<double>( PIRIT_PARAM_1, taxSums.nds_18);
    res |= pirit_io.parseAnswerN<double>( PIRIT_PARAM_2, taxSums.nds_10);
    res |= pirit_io.parseAnswerN<double>( PIRIT_PARAM_3, taxSums.nds_0);
    res |= pirit_io.parseAnswerN<double>( PIRIT_PARAM_4, taxSums.without_nds);
    res |= pirit_io.parseAnswerN<double>( PIRIT_PARAM_5, taxSums.nds_18_118);
    res |= pirit_io.parseAnswerN<double>( PIRIT_PARAM_6, taxSums.nds_10_110);
    if(pirit_io.DEBUGLEVEL == DEBUG_ON)
    {
        printf("--------\n");
        printf("libGetCounters_TaxSumsSale: \n\tPARSE RES = '%d'\n", res);
        printf("\t nds_18        = %f\n",       taxSums.nds_18);
        printf("\t nds_10        = %f\n",       taxSums.nds_10);
        printf("\t nds_0         = %f\n",       taxSums.nds_0);
        printf("\t without_nds   = %f\n",       taxSums.without_nds);
        printf("\t nds_18_118    = %f\n",       taxSums.nds_18_118);
        printf("\t nds_10_110    = %f\n",       taxSums.nds_10_110);
        printf("--------\n");
    }
    return res;
}


//========================================================================================
// 0x01 0x11 Вернуть суммы налогов по возвратам
int libGetCounters_TaxSumsRets(TAX_SUMS &taxSums)
{
    taxSums.nds_18         = 0;
    taxSums.nds_10         = 0;
    taxSums.nds_0          = 0;
    taxSums.without_nds    = 0;
    taxSums.nds_18_118     = 0;
    taxSums.nds_10_110     = 0;

    int res = countersHeader(CNTRS_TYPE_TAX_RET_SUMS);
    if( res != 0 )
    {
        return res;
    }

    res |= pirit_io.parseAnswerN<double>( PIRIT_PARAM_1, taxSums.nds_18);
    res |= pirit_io.parseAnswerN<double>( PIRIT_PARAM_2, taxSums.nds_10);
    res |= pirit_io.parseAnswerN<double>( PIRIT_PARAM_3, taxSums.nds_0);
    res |= pirit_io.parseAnswerN<double>( PIRIT_PARAM_4, taxSums.without_nds);
    res |= pirit_io.parseAnswerN<double>( PIRIT_PARAM_5, taxSums.nds_18_118);
    res |= pirit_io.parseAnswerN<double>( PIRIT_PARAM_6, taxSums.nds_10_110);
    if(pirit_io.DEBUGLEVEL == DEBUG_ON)
    {
        printf("--------\n");
        printf("libGetCounters_TaxSumsRets: \n\tPARSE RES = '%d'\n", res);
        printf("\t nds_18        = %f\n",       taxSums.nds_18);
        printf("\t nds_10        = %f\n",       taxSums.nds_10);
        printf("\t nds_0         = %f\n",       taxSums.nds_0);
        printf("\t without_nds   = %f\n",       taxSums.without_nds);
        printf("\t nds_18_118    = %f\n",       taxSums.nds_18_118);
        printf("\t nds_10_110    = %f\n",       taxSums.nds_10_110);
        printf("--------\n");
    }
    return res;
}


//========================================================================================
// 0x01 0x12 Вернуть данные по последнему X отчету или отчету о закрытии смены
int libGetCounters_XZ(X_Z_DATA &xzData)
{
    xzData.operation_cnt.clear();
    xzData.doc_num        = 0;
    xzData.cash_in_drawer = 0;
    xzData.sale_cnt       = 0;
    xzData.sale_sum       = 0;
    xzData.ret_cnt        = 0;
    xzData.ret_sum        = 0;
    xzData.canceled_cnt   = 0;
    xzData.canceled_sum   = 0;
    xzData.insert_cnt     = 0;
    xzData.insert_sum     = 0;
    xzData.reserve_cnt    = 0;
    xzData.reserve_sum    = 0;

    int res = countersHeader(CNTRS_TYPE_X_Z_DATA);
    if( res != 0 )
    {
        return res;
    }

    char data[256];
    memset(data, 0, sizeof(data));
    res |= pirit_io.parseAnswerN<char>    ( PIRIT_PARAM_1,                 *data);
    xzData.operation_cnt = string(data);

    res |= pirit_io.parseAnswerN<uint64_t>( PIRIT_PARAM_2,       xzData.doc_num);
    res |= pirit_io.parseAnswerN<double>  ( PIRIT_PARAM_3,xzData.cash_in_drawer);

    res |= pirit_io.parseAnswerN<uint64_t>( PIRIT_PARAM_4,      xzData.sale_cnt);
    res |= pirit_io.parseAnswerN<double>  ( PIRIT_PARAM_5,      xzData.sale_sum);

    res |= pirit_io.parseAnswerN<uint64_t>( PIRIT_PARAM_6,       xzData.ret_cnt);
    res |= pirit_io.parseAnswerN<double>  ( PIRIT_PARAM_7,       xzData.ret_sum);

    res |= pirit_io.parseAnswerN<uint64_t>( PIRIT_PARAM_8,  xzData.canceled_cnt);
    res |= pirit_io.parseAnswerN<double>  ( PIRIT_PARAM_9,  xzData.canceled_sum);

    res |= pirit_io.parseAnswerN<uint64_t>( PIRIT_PARAM_10,   xzData.insert_cnt);
    res |= pirit_io.parseAnswerN<double>  ( PIRIT_PARAM_11,   xzData.insert_sum);

    res |= pirit_io.parseAnswerN<uint64_t>( PIRIT_PARAM_12,  xzData.reserve_cnt);
    res |= pirit_io.parseAnswerN<double>  ( PIRIT_PARAM_13,  xzData.reserve_sum);

    if(pirit_io.DEBUGLEVEL == DEBUG_ON)
    {
        printf("--------\n");
        printf("libGetCounters_XZ: \n\tPARSE RES = '%d'\n", res);
        printf("\t operation_cnt  = '%s'\n", xzData.operation_cnt.c_str());
        printf("\t doc_num        = %llud\n",               xzData.doc_num);
        printf("\t cash_in_drawer = %f\n",        xzData.cash_in_drawer);
        printf("\t sale_cnt       = %llud\n",              xzData.sale_cnt);
        printf("\t sale_sum       = %f\n",              xzData.sale_sum);
        printf("\t ret_cnt        = %llud\n",               xzData.ret_cnt);
        printf("\t ret_sum        = %f\n",               xzData.ret_sum);
        printf("\t canceled_cnt   = %llud\n",          xzData.canceled_cnt);
        printf("\t canceled_sum   = %f\n",          xzData.canceled_sum);
        printf("\t insert_cnt     = %llud\n",            xzData.insert_cnt);
        printf("\t insert_sum     = %f\n",            xzData.insert_sum);
        printf("\t reserve_cnt    = %llud\n",           xzData.reserve_cnt);
        printf("\t reserve_sum    = %f\n",           xzData.reserve_sum);
        printf("--------\n");
    }
    return res;
}


//========================================================================================
// 0x01 0x13 Вернуть суммы по секциям/отделам по продажам
int libGetCounters_depSaleSum(double (&depSaleSum)[NUM_OF_SECTOINS])
{
    for(int i = 0; i < NUM_OF_SECTOINS; i++)
    {
        depSaleSum[i] = 0;
    }

    int res = countersHeader(CNTRS_TYPE_DEP_SALE_SUM);
    if( res != 0 )
    {
        return res;
    }

    for(int i = 0; i < NUM_OF_SECTOINS; i++)
    {
        res |= pirit_io.parseAnswerN<double>( (PIRIT_PARAM_NUMBER)(i+1),     depSaleSum[i] );
    }

    if(pirit_io.DEBUGLEVEL == DEBUG_ON)
    {
        printf("--------\n");
        printf("libGetCounters_depSaleSum: \n\tPARSE RES = '%d'\n",              res);
        for(int i = 0; i < NUM_OF_SECTOINS; i++)
        {
            printf("\t Number of section = %d \t Sum = %f\n", i + 1, depSaleSum[i]);
        }
        printf("--------\n");
    }
    return res;
}


//========================================================================================
// 0x01 0x14 Вернуть суммы по секциям/отделам по возвратам
int libGetCounters_depRetSum(double (&depRetSum)[NUM_OF_SECTOINS])
{
    for(int i = 0; i < NUM_OF_SECTOINS; i++)
    {
        depRetSum[i] = 0;
    }

    int res = countersHeader(CNTRS_TYPE_DEP_SALE_RET_SUM);
    if( res != 0 )
    {
        return res;
    }

    for(int i = 0; i < NUM_OF_SECTOINS; i++)
    {
        res |= pirit_io.parseAnswerN<double>( (PIRIT_PARAM_NUMBER)(i+1),     depRetSum[i] );
    }

    if(pirit_io.DEBUGLEVEL == DEBUG_ON)
    {
        printf("--------\n");
        printf("libGetCounters_depRetSum: \n\tPARSE RES = '%d'\n",              res);
        for(int i = 0; i < NUM_OF_SECTOINS; i++)
        {
            printf("\t Number of section = %d \t Sum = %f\n", i + 1, depRetSum[i]);
        }
        printf("--------\n");
    }
    return res;
}


//========================================================================================
// 0x01 0x15 Вернуть кол-во чеков покупок (расходов), кол-во чеков возвратов покупок (возвратов расхода)
int libGetCounters_RecCount( uint64_t &purchaseCnt, uint64_t &retPurchaseCnt )
{
    purchaseCnt    = 0;
    retPurchaseCnt = 0;

    int res = countersHeader(CNTRS_TYPE_REC_COUNT);
    if( res != 0 )
    {
        return res;
    }

    res |= pirit_io.parseAnswerN<uint64_t>( PIRIT_PARAM_1,    purchaseCnt );
    res |= pirit_io.parseAnswerN<uint64_t>( PIRIT_PARAM_2, retPurchaseCnt );

    if(pirit_io.DEBUGLEVEL == DEBUG_ON)
    {
        printf("--------\n");
        printf("libGetCounters_RecCount: \n\tPARSE RES = '%d'\n",              res);
        printf("\t  purchaseCnt    = %llud\n",    purchaseCnt);
        printf("\t  retPurchaseCnt = %llud\n", retPurchaseCnt);
        printf("--------\n");
    }
    return res;
}


//========================================================================================
// 0x01 0x16 Вернуть суммы покупок по типам платежа
int libGetCounters_SaleSumByType(OPERATION<double> &purchSums)
{
    purchSums.advance  = 0;
    purchSums.card     = 0;
    purchSums.cash     = 0;
    purchSums.credit   = 0;
    purchSums.exchange = 0;

    int res = countersHeader(CNTRS_TYPE_SALE_SUM_BY_TYPE);
    if( res != 0 )
    {
        return res;
    }

    res |= pirit_io.parseAnswerN<double>( PIRIT_PARAM_1,      purchSums.cash);
    res |= pirit_io.parseAnswerN<double>( PIRIT_PARAM_2,      purchSums.card);
    res |= pirit_io.parseAnswerN<double>( PIRIT_PARAM_14,  purchSums.advance);
    res |= pirit_io.parseAnswerN<double>( PIRIT_PARAM_15,   purchSums.credit);
    res |= pirit_io.parseAnswerN<double>( PIRIT_PARAM_16, purchSums.exchange);
    if(pirit_io.DEBUGLEVEL == DEBUG_ON)
    {
        printf("--------\n");
        printf("libGetCounters_SaleSumByType: \n\tPARSE RES = '%d'\n",            res);
        printf("\t cash      = %f\n",     purchSums.cash);
        printf("\t card      = %f\n",     purchSums.card);
        printf("\t advance   = %f\n",  purchSums.advance);
        printf("\t credit    = %f\n",   purchSums.credit);
        printf("\t exchange  = %f\n", purchSums.exchange);
        printf("--------\n");
    }
    return res;
}


//========================================================================================
// 0x01 0x17 Вернуть суммы возвратов покупок по типам платежа
int libGetCounters_RetSumByType(OPERATION<double> &purchSumsRet)
{
    purchSumsRet.advance  = 0;
    purchSumsRet.card     = 0;
    purchSumsRet.cash     = 0;
    purchSumsRet.credit   = 0;
    purchSumsRet.exchange = 0;

    int res = countersHeader(CNTRS_TYPE_RET_SUM_BY_TYPE);
    if( res != 0 )
    {
        return res;
    }

    res |= pirit_io.parseAnswerN<double>( PIRIT_PARAM_1,      purchSumsRet.cash);
    res |= pirit_io.parseAnswerN<double>( PIRIT_PARAM_2,      purchSumsRet.card);
    res |= pirit_io.parseAnswerN<double>( PIRIT_PARAM_14,  purchSumsRet.advance);
    res |= pirit_io.parseAnswerN<double>( PIRIT_PARAM_15,   purchSumsRet.credit);
    res |= pirit_io.parseAnswerN<double>( PIRIT_PARAM_16, purchSumsRet.exchange);
    if(pirit_io.DEBUGLEVEL == DEBUG_ON)
    {
        printf("--------\n");
        printf("libGetCounters_RetSumByType: \n\tPARSE RES = '%d'\n",            res);
        printf("\t cash      = %f\n",     purchSumsRet.cash);
        printf("\t card      = %f\n",     purchSumsRet.card);
        printf("\t advance   = %f\n",  purchSumsRet.advance);
        printf("\t credit    = %f\n",   purchSumsRet.credit);
        printf("\t exchange  = %f\n", purchSumsRet.exchange);
        printf("--------\n");
    }
    return res;
}


//========================================================================================
// 0x01 0x18 Вернуть данные по коррекциям
int libGetCounters_CorData(COR_DATA &corData )
{
    corData.cnt      = 0;
    corData.cash     = 0;
    corData.card     = 0;

    int res = countersHeader(CNTRS_TYPE_COR_DATA);
    if( res != 0 )
    {
        return res;
    }

    res |= pirit_io.parseAnswerN<uint64_t>( PIRIT_PARAM_1,      corData.cnt);
    res |= pirit_io.parseAnswerN<double>  ( PIRIT_PARAM_2,     corData.cash);
    res |= pirit_io.parseAnswerN<double>  ( PIRIT_PARAM_3,     corData.card);

    if(pirit_io.DEBUGLEVEL == DEBUG_ON)
    {
        printf("--------\n");
        printf("libGetCounters_CorData: \n\tPARSE RES = '%d'\n", res);
        printf("\t cash      = %llud\n",      corData.cnt);
        printf("\t card      = %f\n",     corData.cash);
        printf("\t advance   = %f\n",     corData.card);
        printf("--------\n");
    }
    return res;
}

