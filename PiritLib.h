#ifndef PIRIT_LIB_H_
#define PIRIT_LIB_H_

#pragma once
#if defined(__GNUC__)
#define LINUX 7
#endif

#include <stdio.h>   /* Стандартные объ влени  ввода/вывода */
#include <fcntl.h>   /* Объ влени  управлени  файлами */
#include <errno.h>   /* Объ влени  кодов ошибок */
#include <stdlib.h>
#include <time.h>
#include <sstream>
#include <vector>
#include <algorithm>
#include <functional>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <resolv.h>
#include <unistd.h>  /* Объ влени  стандартных функций UNIX */
#include <termios.h> /* Объ влени  управлени  POSIX-терминалом */
#include <inttypes.h>
#include <typeinfo>
#include <typeindex>
#include <mutex>

#include "pirit_structs.h"
#include "pirit_low_func.h"
#include "biosCounters.h"

using namespace std;

#define DRIVER_VERSION (unsigned long long)2
#define DRIVER_BUILD   (unsigned long long)120

#define MAX_PATH 260

#define CORRECTION_INDEPENDENT  0x00
#define CORRECTION_PRESCRIPTION 0x01
#define CORRECTION_INCOMING     0x00
#define CORRECTION_OUTGOING     0x02
#define CORRECTION_TAX_1        0x00
#define CORRECTION_TAX_2        0x04
#define CORRECTION_TAX_4        0x08
#define CORRECTION_TAX_8        0x0C
#define CORRECTION_TAX_16       0x10
#define CORRECTION_TAX_32       0x14

#define DEVICE_MODE_DATA_ENCRYPTION                0x00000001UL
#define DEVICE_MODE_OFFLINE                        0x00000002UL
#define DEVICE_MODE_AUTOMATIC                      0x00000004UL
#define DEVICE_MODE_SERVICE_SIGN                   0x00000008UL
#define DEVICE_MODE_BSO_SING                       0x00000010UL
#define DEVICE_MODE_CALC_ONLINE_SIGN               0x00000020UL
#define DEVICE_MODE_SALE_EXCISABLE_GOODS           0x00000040UL
#define DEVICE_MODE_SIGN_OF_GAMBLING               0x00000080UL
#define DEVICE_MODE_SIGN_OF_BANK_PAYMENT_AGENTS    0x00000100UL
#define DEVICE_MODE_SIGN_OF_BANK_PAYMENT_SUBAGENTS 0x00000200UL
#define DEVICE_MODE_SIGN_OF_PAYMENT_AGENTS         0x00000400UL
#define DEVICE_MODE_SIGN_OF_PAYMENT_SUBAGENTS      0x00000800UL
#define DEVICE_MODE_SIGN_OF_ATTORNEY               0x00001000UL
#define DEVICE_MODE_SIGN_OF_COMISSIONERS           0x00002000UL
#define DEVICE_MODE_SIGN_OF_AGENTS                 0x00004000UL
#define DEVICE_MODE_SIGN_OF_LOTTERY                0x00008000UL

// Объект для общения по протоколу пирит, методы ниже его юзают
extern PIRIT_IOCLASS pirit_io;

//==============================================================================================
// Уровень критичности ошибки из ответного пакета
enum FB_CRITICAL_LEVEL
{
    CL_WARNING           = 0,    // Ошибка НЕ критична, операция выполнена, но с варнингом
    CL_FISCAL            = 1,    // Ошибка критична для фиск        операций
    CL_FISC_AND_NOT_FISC = 2,    // Ошибка критична для фиск/нефиск операций
    CL_ALL               = 3     // Ошибка Критична для всех типов операций (по факту, для соединения)
};

enum PIRIT_LOCAL_LICENSE_OPTIONS_MASK
{
    OPTION_1200     = 0x01,
    OPTION_FFD11    = 0x02,
    OPTION_SELL_SIZ = 0x04
};

//==============================================================================================
// Управление расчетами (битовая маска)
enum BIOS_CALC_BIT_MASK
{
    CALC_CASH_CONTROL        = 0x01,   //	0 - Включен контроль наличных в денежном ящике
    CALC_CHECK_ANNUL_CHECKS  = 0x02,   //   0 - Учитывать чеки, аннулированные при включении питания
    CALC_AUTO_INCAS          = 0x04,   //   0 - Автоматическая инкассация выключена
    CALC_RASHOD              = 0x08,   //   0 - Счетчики покупок(расходов) выключены
    CALC_AUTO_PRINT_SCL      = 0x10,   //   0 - Автоматическая печать СКЛ выключена
    CALC_SCL                 = 0x20,   //   0 - СКЛ отключена
    CALC_ITOG_PROD           = 0x40,   //   0 - Печать суммы нарастающего итога продаж(приходов)/покупок(расходов) на X-отчетах и отчетах о закрытии смены отключена
    CALC_ITOG_VOZVR          = 0x80    //   0 - Печать суммы нарастающего итога возвратов (прихода и расхода) на X-отчетах и отчетах о закрытии смены отключена
};


// Тип проводимой операции для определения критичности полученной ошибки
enum FB_OPERATION_CRITICAL_LEVEL
{
    OP_FISCAL            = 1,
    OP_NOT_FISCAL        = 2,
    OP_CONNECTION        = 3

};

enum FB_ERROR_CODE
{
    FB_NOT_ERROR = 0x00, // Команда выполнена без ошибок

    // Ошибки выполнение команд
    FB_ERROR_FUNCTION_IS_NOT_FEASIBLE_WITH_STATUS = 0x01,   // Функция невыполнима при данном статусе ККТ
    FB_ERROR_INVALID_FUNCTION_NUMBER_IN_COMMAND = 0x02,     // В команде указан неверный номер функции
    FB_ERROR_INVALID_COMMAND_FORMAT_OR_PARAMETER = 0x03,    // Некорректный формат или параметр команды

    // Ошибки протокола передачи данных
    FB_ERROR_COMMUNICATION_PORT_BUFFER_OWERFLOW = 0x04,     // 	Переполнение буфера коммуникационного порта
    FB_ERROR_TIMEOUT_WHEN_TRANSMITTING_INFORMATION = 0x05,  // Таймаут при передаче байта информации
    FB_ERROR_INCORRECT_PASSWORD = 0x06,                     // 	В протоколе указан неверный пароль
    FB_ERROR_COMMAND_CHECKSUM = 0x07,                       // Ошибка контрольной суммы в команде

    // Ошибки печатающего устройства
    FB_ERROR_END_OF_PAPER = 0x08,       // Конец бумаги
    FB_ERROR_PRINTER_NOT_READY = 0x09,  // Принтер не готов

    // Ошибки даты/времени
    FB_ERROR_CURRENT_SHIFT_MORE_24 = 0x0A,              // Текущая смена больше 24 часов
    FB_ERROR_DIFFERENT_TIME_MORE_8_MIN = 0x0B,          // Разница во времени, ККТ и указанной в команде начала работы, больше 8 минут
    FB_ERROR_ENTERED_DATE_EARLIER_LAST_FISCAL = 0x0C,   // Вводимая дата более ранняя, чем дата последней фискальной операции

    // Прочие ошибки
    FB_ERROR_NEGATIVE_RESULT = 0x0E,        // Отрицательный результат
    FB_ERROR_NEED_CLOSE_SHIFT = 0x0F,       // Для выполнения команды необходимо закрыть смену
    FB_ERROR_NO_DATA_IN_JOURNAL = 0x10,     // Нет данных в журнале
    FB_ERROR_CONTROL_TAPE = 0x11,           // Ошибка контрольной ленты
    FB_ERROR_SENDING_DATA_TO_OFD = 0x12,    // Ошибка посылки данных в ОФД

    // Фатальные ошибки
    FB_ERROR_FATAL = 0x20,  // Фатальная ошибка ККТ. Причины возникновения данной ошибки можно уточнить в ?Статусе фатальных ошибок ККТ?

    // Ошибки ФН
    FB_ERROR_INVALID_FORMAT_OR_PARAMETER_FN = 0x41,     // Некорректный формат или параметр команды ФН
    FB_ERROR_INCORRECT_STATE_FN = 0x42,                 // Некорректное состояние ФН
    FB_ERROR_FN = 0x43,                                 // Ошибка ФН
    FB_ERROR_CRYPTOGRAPHIC_COPROCESSOR = 0x44,          // Ошибка КС (Криптографического сопроцессора) в составе ФН
    FB_ERROR_EXHAUSTED_TIME_RESOURCE_FN = 0x45,         // Исчерпан временной ресурс использования ФН
    FB_ERROR_FN_OVERFLOW = 0x46,                        // ФН переполнен
    FB_ERROR_INVALID_DATE_OR_TIME = 0x47,               // Неверные дата или время
    FB_ERROR_NO_DATA_REQUESTED = 0x48,                  // Нет запрошенных данных
    FB_ERROR_INVALID_DATA_PARAMETERS = 0x49,            // Некорректные параметры команды
    FB_ERROR_TLV_DATA_SIZE_EXCEEDED = 0x50,             // Превышен размер данных TLV
    FB_ERROR_NO_TRANSPORT_CONNECTION = 0x51,            // Нет транспортного соединения
    FB_ERROR_EXHAUSTED_RESOURCE_COP = 0x52,             // Исчерпан ресурс КС
    FB_ERROR_MEMORY_TO_OFD_EXHAUSTED = 0x54,            // Исчерпана память хранения ОФД
    FB_ERROR_TIME_TO_OFD_MORE_30_DAYS = 0x55,           // Время нахождения в очереди самого старого сообщения на выдачу более 30 календарных дней.
    FB_ERROR_DURATION_SHIFT_IN_FN_MORE_24 = 0x56,       // Продолжительность смены ФН более 24 часов
    FB_ERROR_DIFFERENT_TIME_MORE_5_MINUTS_IN_FN = 0x57, // Разница более чем на 5 минут отличается от разницы, определенной по внутреннему таймеру ФН.
    FB_ERROR_INVALID_MESSAGE_FROM_OFD = 0x60,           // Неверное сообщение от ОФД
    FB_ERROR_NO_CONNECTION_EITH_FN = 0x61,              // Нет связи с ФН
    FB_ERROR_EXHANGE_ERROR_WITH_FN = 0x62,              // Ошибка обмена с ФН
    FB_ERROR_TOO_LONG_COMMAND_TO_FN = 0x63,             // Слишком длинная команда для посылки в ФН

    FB_ERROR_ALL
};

// Структура для хранения кодов ошибок из ответного пакета пуникса
struct FB_err
{
    int                     code;     // Код ошибки
    string                  mess;     // Расшифровка кода ошибки
    FB_CRITICAL_LEVEL       cLvl;     // Ошибка является критической для данного уровня и выше

    FB_err(int c, const char* m, FB_CRITICAL_LEVEL newCLvl)
    {
        code =         c;
        mess = string(m);
        cLvl =   newCLvl;
    }

    FB_err(const FB_err& other) : code(other.code), mess(other.mess), cLvl(other.cLvl) {}
};


// ===================================== Ошибки для РФ Биос =================================
const vector <FB_err> FB_errs_RF
        {
/*                     КОД |  РАСШИФРОВКА                          | УРОВЕНЬ КРИТИЧНОСТИ ОШИБКИ                                 */

                // Ошибки выполнение команд
                FB_err(FB_ERROR_FUNCTION_IS_NOT_FEASIBLE_WITH_STATUS, "тек.статус ККТ: Функция невыполнима", FB_CRITICAL_LEVEL:: CL_ALL),
                FB_err(FB_ERROR_INVALID_FUNCTION_NUMBER_IN_COMMAND, "Неверный номер функции",                FB_CRITICAL_LEVEL:: CL_ALL),
                FB_err(FB_ERROR_INVALID_COMMAND_FORMAT_OR_PARAMETER, "Некорректный формат/пар-р команды",    FB_CRITICAL_LEVEL:: CL_ALL),
                FB_err(0x40, "Неверный параметр команды",           FB_CRITICAL_LEVEL:: CL_ALL           ),
                // Ошибки протокола передачи данных
                FB_err(FB_ERROR_COMMUNICATION_PORT_BUFFER_OWERFLOW, "Переполнение буфера коммуник. порта",  FB_CRITICAL_LEVEL:: CL_ALL),
                FB_err(FB_ERROR_TIMEOUT_WHEN_TRANSMITTING_INFORMATION, "Таймаут при передаче инф-ции",      FB_CRITICAL_LEVEL:: CL_ALL),
                FB_err(FB_ERROR_INCORRECT_PASSWORD, "Неверный пароль в пакете",                             FB_CRITICAL_LEVEL:: CL_ALL),
                FB_err(FB_ERROR_COMMAND_CHECKSUM, "Ошибка контр.суммы в команде",                           FB_CRITICAL_LEVEL:: CL_ALL),
                // Ошибки печатающего устройства
                FB_err(FB_ERROR_END_OF_PAPER, "Конец бумаги",           FB_CRITICAL_LEVEL:: CL_FISC_AND_NOT_FISC),
                FB_err(FB_ERROR_PRINTER_NOT_READY, "Принтер не готов",  FB_CRITICAL_LEVEL:: CL_FISC_AND_NOT_FISC),
                // 	Ошибки даты/времени
                FB_err(FB_ERROR_CURRENT_SHIFT_MORE_24, "Текущая смена больше 24 часов",             FB_CRITICAL_LEVEL:: CL_ALL),
                FB_err(FB_ERROR_DIFFERENT_TIME_MORE_8_MIN, "Разница во времени >8мин.",             FB_CRITICAL_LEVEL:: CL_FISCAL),
                FB_err(FB_ERROR_ENTERED_DATE_EARLIER_LAST_FISCAL, "дата<даты посл. фиск.оп-ции",    FB_CRITICAL_LEVEL:: CL_FISCAL),
                FB_err(FB_ERROR_NEGATIVE_RESULT, "Отрицательный результат",                         FB_CRITICAL_LEVEL:: CL_FISC_AND_NOT_FISC),
                FB_err(FB_ERROR_NEED_CLOSE_SHIFT, "Необходимо закрыть смену",                       FB_CRITICAL_LEVEL:: CL_FISCAL),
                FB_err(FB_ERROR_NO_DATA_IN_JOURNAL, "Нет данных в журнале",                         FB_CRITICAL_LEVEL:: CL_FISCAL),
                FB_err(FB_ERROR_CONTROL_TAPE, "Ошибка контрольной ленты",                           FB_CRITICAL_LEVEL:: CL_FISCAL),
                FB_err(FB_ERROR_SENDING_DATA_TO_OFD, "Ошибка отпр.данных в ОФД",                    FB_CRITICAL_LEVEL:: CL_FISCAL),
                // 	Фатальные ошибки
                FB_err(FB_ERROR_FATAL, "Фатальная ошибка ККТ", FB_CRITICAL_LEVEL:: CL_ALL),
                // Ошибки ФН
                FB_err(0x21, "ФН переполнен", FB_CRITICAL_LEVEL:: CL_FISCAL),
                FB_err(0x30, "Неверный ФН",   FB_CRITICAL_LEVEL:: CL_FISCAL),
                FB_err(FB_ERROR_INVALID_FORMAT_OR_PARAMETER_FN, "Некорректный формат команды ФН",   FB_CRITICAL_LEVEL:: CL_FISCAL),
                FB_err(FB_ERROR_INCORRECT_STATE_FN, "Некорректное состояние ФН",                    FB_CRITICAL_LEVEL:: CL_FISCAL),
                FB_err(FB_ERROR_FN, "Ошибка ФН",                                                    FB_CRITICAL_LEVEL:: CL_FISCAL),
                FB_err(FB_ERROR_CRYPTOGRAPHIC_COPROCESSOR, "Ошибка КС в ФН",                        FB_CRITICAL_LEVEL:: CL_FISCAL),
                FB_err(FB_ERROR_EXHAUSTED_TIME_RESOURCE_FN, "Исчерпан временной ресурс ФН",         FB_CRITICAL_LEVEL:: CL_FISCAL),
                FB_err(FB_ERROR_FN_OVERFLOW, "ФН переполнен",                                       FB_CRITICAL_LEVEL:: CL_FISCAL),
                FB_err(FB_ERROR_INVALID_DATE_OR_TIME, "Неверные дата или время",                    FB_CRITICAL_LEVEL:: CL_FISCAL),
                FB_err(FB_ERROR_NO_DATA_REQUESTED, "Нет запрошенных данных",                        FB_CRITICAL_LEVEL:: CL_FISCAL),
                FB_err(FB_ERROR_INVALID_DATA_PARAMETERS, "Некорректные параметры команды",          FB_CRITICAL_LEVEL:: CL_FISCAL),
                FB_err(FB_ERROR_TLV_DATA_SIZE_EXCEEDED, "Превышен размер данных TLV",               FB_CRITICAL_LEVEL:: CL_FISCAL),
                FB_err(FB_ERROR_NO_TRANSPORT_CONNECTION, "Нет транспортного соединения",            FB_CRITICAL_LEVEL:: CL_FISCAL),
                FB_err(FB_ERROR_EXHAUSTED_RESOURCE_COP, "Исчерпан ресурс КС",                       FB_CRITICAL_LEVEL:: CL_FISCAL),
                FB_err(FB_ERROR_MEMORY_TO_OFD_EXHAUSTED, "Исчерпана память хранения ОФД",           FB_CRITICAL_LEVEL:: CL_FISCAL),
                FB_err(FB_ERROR_TIME_TO_OFD_MORE_30_DAYS, "Время хранения >30 дней",                FB_CRITICAL_LEVEL:: CL_FISCAL),
                FB_err(FB_ERROR_DURATION_SHIFT_IN_FN_MORE_24, "Cмена ФН >24 часов",                 FB_CRITICAL_LEVEL:: CL_FISCAL),
                FB_err(FB_ERROR_DIFFERENT_TIME_MORE_5_MINUTS_IN_FN, "Разница >5 минут разницы ФН",  FB_CRITICAL_LEVEL:: CL_FISCAL),
                FB_err(FB_ERROR_INVALID_MESSAGE_FROM_OFD, "Неверное сообщение от ОФД",              FB_CRITICAL_LEVEL:: CL_WARNING),
                FB_err(FB_ERROR_NO_CONNECTION_EITH_FN, "Нет связи с ФН",                            FB_CRITICAL_LEVEL:: CL_FISCAL),
                FB_err(FB_ERROR_EXHANGE_ERROR_WITH_FN, "Ошибка обмена с ФН",                        FB_CRITICAL_LEVEL:: CL_FISCAL),
                FB_err(FB_ERROR_TOO_LONG_COMMAND_TO_FN, "Слишк.длин. команда для ФН",               FB_CRITICAL_LEVEL:: CL_WARNING),

                FB_err(1001, "Ошибка обмена с ФН",                FB_CRITICAL_LEVEL:: CL_ALL            ), // "При приёме: первый байт не STX";
                FB_err(1002, "Ошибка обмена с ФН",                FB_CRITICAL_LEVEL:: CL_WARNING              ), // "При приёме: не совпадает ID пакета";
                FB_err(1003, "Ошибка обмена с ФН",                FB_CRITICAL_LEVEL:: CL_WARNING              ), // "При приёме: не совпадает номер команды 1-ый байт";
                FB_err(1004, "Ошибка обмена с ФН",                FB_CRITICAL_LEVEL:: CL_WARNING              ), // "При приёме: не совпадает номер команды 2-ой байт";
                FB_err(1005, "Ошибка обмена с ФН",                FB_CRITICAL_LEVEL:: CL_ALL            ), // "При приёме: последний байт не ETX";
                FB_err(1006, "Ошибка обмена с ФН",                FB_CRITICAL_LEVEL:: CL_ALL            ), // "При приёме: ошибка контрольной суммы CRC";
                FB_err(1007, "Ошибка обмена с ФН",                FB_CRITICAL_LEVEL:: CL_ALL            ), // "При отправке данных данные отправились не полностью";
                FB_err(1008, "Ошибка обмена с ФН",                FB_CRITICAL_LEVEL:: CL_ALL            ), // "При приёме: слишком короткий пакет";
                FB_err(1009, "Ошибка обмена с ФН",                FB_CRITICAL_LEVEL:: CL_ALL            ), // "При приёме: потеря связи с ФР";
                FB_err(1010, "Ошибка обмена с ФН",                FB_CRITICAL_LEVEL:: CL_ALL            ), // "При обработке ответа: кол-во параметров не соответствует заявленному";
                FB_err(1011, "Ошибка обмена с ФН",                FB_CRITICAL_LEVEL:: CL_ALL            ), // "ФР не отвечает на команду Ping";

                FB_err(2000, "Ошибка обмена с ФН",                FB_CRITICAL_LEVEL:: CL_ALL            ), // "Порт не открыт";
                FB_err(2001, "Ошибка обмена с ФН",                FB_CRITICAL_LEVEL:: CL_ALL            ), // "Невозможно открыть порт";
                FB_err(2002, "Ошибка обмена с ФН",                FB_CRITICAL_LEVEL:: CL_ALL            ), // "Ошибка порта при открытии";
                FB_err(2003, "Ошибка обмена с ФН",                FB_CRITICAL_LEVEL:: CL_ALL            ), // "Ошибка порта при инициализации";
                FB_err(2010, "Ошибка обмена с ФН",                FB_CRITICAL_LEVEL:: CL_ALL            ), // "Ошибка соединения с БИОС";
                FB_err(2011, "Ошибка обмена с ФН",                FB_CRITICAL_LEVEL:: CL_ALL            ), // "Ошибка создания сокета";

                FB_err(2222, "Ошибка обмена с ФН",                FB_CRITICAL_LEVEL:: CL_ALL            )  // Мало параметров при парсе пакета parseAnswerN<double>()

//                FB_err(1001, "Ошибка Приёма: 1001",                FB_CRITICAL_LEVEL:: CL_ALL            ), // "При приёме: первый байт не STX";
//                FB_err(1002, "Ошибка Приёма: 1002",                FB_CRITICAL_LEVEL:: CL_WARNING              ), // "При приёме: не совпадает ID пакета";
//                FB_err(1003, "Ошибка Приёма: 1003",                FB_CRITICAL_LEVEL:: CL_WARNING              ), // "При приёме: не совпадает номер команды 1-ый байт";
//                FB_err(1004, "Ошибка Приёма: 1004",                FB_CRITICAL_LEVEL:: CL_WARNING              ), // "При приёме: не совпадает номер команды 2-ой байт";
//                FB_err(1005, "Ошибка Приёма: 1005",                FB_CRITICAL_LEVEL:: CL_ALL            ), // "При приёме: последний байт не ETX";
//                FB_err(1006, "Ошибка Приёма: 1006",                FB_CRITICAL_LEVEL:: CL_ALL            ), // "При приёме: ошибка контрольной суммы CRC";
//                FB_err(1007, "Ошибка Приёма: 1007",                FB_CRITICAL_LEVEL:: CL_ALL            ), // "При отправке данных данные отправились не полностью";
//                FB_err(1008, "Ошибка Приёма: 1008",                FB_CRITICAL_LEVEL:: CL_ALL            ), // "При приёме: слишком короткий пакет";
//                FB_err(1009, "Ошибка Приёма: 1009",                FB_CRITICAL_LEVEL:: CL_ALL            ), // "При приёме: потеря связи с ФР";
//                FB_err(1010, "Ошибка Приёма: 1010",                FB_CRITICAL_LEVEL:: CL_ALL            ), // "При обработке ответа: кол-во параметров не соответствует заявленному";
//                FB_err(1011, "Ошибка Приёма: 1011",                FB_CRITICAL_LEVEL:: CL_ALL            ), // "ФР не отвечает на команду Ping";
//
//                FB_err(2000, "Ошибка Приёма: 2000",                FB_CRITICAL_LEVEL:: CL_ALL            ), // "Порт не открыт";
//                FB_err(2001, "Ошибка Приёма: 2001",                FB_CRITICAL_LEVEL:: CL_ALL            ), // "Невозможно открыть порт";
//                FB_err(2002, "Ошибка Приёма: 2002",                FB_CRITICAL_LEVEL:: CL_ALL            ), // "Ошибка порта при открытии";
//                FB_err(2003, "Ошибка Приёма: 2003",                FB_CRITICAL_LEVEL:: CL_ALL            ), // "Ошибка порта при инициализации";
//                FB_err(2010, "Ошибка Приёма: 2010",                FB_CRITICAL_LEVEL:: CL_ALL            ), // "Ошибка соединения с БИОС";
//                FB_err(2011, "Ошибка Приёма: 2011",                FB_CRITICAL_LEVEL:: CL_ALL            ), // "Ошибка создания сокета";
//
//                FB_err(2222, "Ошибка Приёма: 2222",                FB_CRITICAL_LEVEL:: CL_ALL            )  // Мало параметров при парсе пакета parseAnswerN<double>()
        };


// ===================================== Ошибки для Белорусского Биос =================================
const vector <FB_err> FB_errs_RB
        {
/*                     КОД |  РАСШИФРОВКА                          | УРОВЕНЬ КРИТИЧНОСТИ ОШИБКИ                                 */

                //-----------------------------------------------------------------------------------------
                // Ошибки выполнение команд
                //-----------------------------------------------------------------------------------------
                FB_err(0x01, "тек.статус ККТ: Функция невыполнима", FB_CRITICAL_LEVEL:: CL_ALL           ),
                FB_err(0x02, "Неверный номер функции",              FB_CRITICAL_LEVEL:: CL_ALL           ),
                FB_err(0x03, "Некорректный формат/пар-р команды",   FB_CRITICAL_LEVEL:: CL_ALL           ),
                //-----------------------------------------------------------------------------------------
                // Ошибки протокола передачи данных
                //-----------------------------------------------------------------------------------------
                FB_err(0x04, "Переполнение буфера коммуник. порта", FB_CRITICAL_LEVEL:: CL_ALL           ),
                FB_err(0x05, "Таймаут при передаче инф-ции",        FB_CRITICAL_LEVEL:: CL_ALL           ),
                FB_err(0x06, "Неверный пароль в пакете",            FB_CRITICAL_LEVEL:: CL_ALL           ),
                FB_err(0x07, "Ошибка контр.суммы в команде",        FB_CRITICAL_LEVEL:: CL_ALL           ),
                //-----------------------------------------------------------------------------------------
                // Ошибки печатающего устройства
                //-----------------------------------------------------------------------------------------
                FB_err(0x08, "Конец бумаги",                        FB_CRITICAL_LEVEL:: CL_FISC_AND_NOT_FISC ),
                FB_err(0x09, "Принтер не готов",                    FB_CRITICAL_LEVEL:: CL_FISC_AND_NOT_FISC ),
                //-----------------------------------------------------------------------------------------
                // 	Ошибки даты/времени
                //-----------------------------------------------------------------------------------------
                FB_err(0x0A, "Текущая смена больше 24 часов",       FB_CRITICAL_LEVEL:: CL_ALL            ),
                FB_err(0x0B, "Разница во времени >8мин.",           FB_CRITICAL_LEVEL:: CL_FISCAL         ),
                FB_err(0x0C, "дата<даты посл. фиск.оп-ции",         FB_CRITICAL_LEVEL:: CL_FISCAL         ),
                FB_err(0x0D, "Неверный пароль БЭП",                 FB_CRITICAL_LEVEL:: CL_FISC_AND_NOT_FISC ),
                FB_err(0x0E, "Отрицательный результат",             FB_CRITICAL_LEVEL:: CL_FISC_AND_NOT_FISC ),
                FB_err(0x0F, "Необходимо закрыть смену",            FB_CRITICAL_LEVEL:: CL_FISCAL         ),
                FB_err(0x10, "Незакрытые заказы в смене",           FB_CRITICAL_LEVEL:: CL_FISCAL         ),
                FB_err(0x48, "Нет запрошенных данных",              FB_CRITICAL_LEVEL:: CL_FISC_AND_NOT_FISC ),
                FB_err(0x20, "Фатальная ошибка КСА",                FB_CRITICAL_LEVEL:: CL_FISC_AND_NOT_FISC ),
                FB_err(0x21, "Память КСА переполнена",              FB_CRITICAL_LEVEL:: CL_FISCAL         ),
                FB_err(0x80, "Общая ошибка СКНО",                   FB_CRITICAL_LEVEL:: CL_FISC_AND_NOT_FISC ),
                FB_err(0x91, "Нет связи с СКНО",                    FB_CRITICAL_LEVEL:: CL_FISC_AND_NOT_FISC ),
                FB_err(0x92, "Отсутствует СКЗИ",                    FB_CRITICAL_LEVEL:: CL_ALL ),
                FB_err(0x93, "СКНО неисправно" ,                    FB_CRITICAL_LEVEL:: CL_FISC_AND_NOT_FISC ),
                FB_err(0x94, "Ошибка идентиф-ии" ,                  FB_CRITICAL_LEVEL:: CL_ALL ),
                FB_err(0x96, "Сертификат СКЗИ окончен",             FB_CRITICAL_LEVEL:: CL_ALL ),
                FB_err(0x97, "Z отчёты непереданны",                FB_CRITICAL_LEVEL:: CL_ALL ),
                FB_err(0x98, "Память СКНО переполнена",             FB_CRITICAL_LEVEL:: CL_ALL ),
                //-----------------------------------------------------------------------------------------
                FB_err(1001,  "Ошибка контр.суммы в команд",               FB_CRITICAL_LEVEL:: CL_ALL            ), // "При приёме: первый байт не STX";
                FB_err(1002,  "Ошибка контр.суммы в команд",               FB_CRITICAL_LEVEL:: CL_WARNING        ), // "При приёме: не совпадает ID пакета";
                FB_err(1003,  "Ошибка контр.суммы в команд",               FB_CRITICAL_LEVEL:: CL_WARNING        ), // "При приёме: не совпадает номер команды 1-ый байт";
                FB_err(1004,  "Ошибка контр.суммы в команд",               FB_CRITICAL_LEVEL:: CL_WARNING        ), // "При приёме: не совпадает номер команды 2-ой байт";
                FB_err(1005,  "Ошибка контр.суммы в команд",               FB_CRITICAL_LEVEL:: CL_ALL            ), // "При приёме: последний байт не ETX";
                FB_err(1006,  "Ошибка контр.суммы в команд",               FB_CRITICAL_LEVEL:: CL_ALL            ), // "При приёме: ошибка контрольной суммы CRC";
                FB_err(1007,  "Ошибка контр.суммы в команд",               FB_CRITICAL_LEVEL:: CL_ALL            ), // "При отправке данных данные отправились не полностью";
                FB_err(1008,  "Ошибка контр.суммы в команд",               FB_CRITICAL_LEVEL:: CL_ALL            ), // "При приёме: слишком короткий пакет";
                FB_err(1009, "Таймаут при передаче инф-ции",               FB_CRITICAL_LEVEL:: CL_ALL            ), // "При приёме: потеря связи с ФР";
                FB_err(1010,  "Ошибка контр.суммы в команд",               FB_CRITICAL_LEVEL:: CL_ALL            ), // "При обработке ответа: кол-во параметров не соответствует заявленному";
                FB_err(1011, "Таймаут при передаче инф-ции",               FB_CRITICAL_LEVEL:: CL_ALL            ), // "ФР не отвечает на команду Ping";
                FB_err(2000, "Таймаут при передаче инф-ции",               FB_CRITICAL_LEVEL:: CL_ALL            ), // "Порт не открыт";
                FB_err(2001, "Таймаут при передаче инф-ции",               FB_CRITICAL_LEVEL:: CL_ALL            ), // "Невозможно открыть порт";
                FB_err(2002, "Таймаут при передаче инф-ции",               FB_CRITICAL_LEVEL:: CL_ALL            ), // "Ошибка порта при открытии";
                FB_err(2003, "Таймаут при передаче инф-ции",               FB_CRITICAL_LEVEL:: CL_ALL            ), // "Ошибка порта при инициализации";
                FB_err(2010, "Таймаут при передаче инф-ции",               FB_CRITICAL_LEVEL:: CL_ALL            ), // "Ошибка соединения с БИОС";
                FB_err(2011, "Таймаут при передаче инф-ции",               FB_CRITICAL_LEVEL:: CL_ALL            ), // "Ошибка создания сокета";
                FB_err(2222,  "Ошибка контр.суммы в команд",               FB_CRITICAL_LEVEL:: CL_ALL            )  // Мало параметров при парсе пакета parseAnswerN<double>()
        };
//------------------------------------------------------------------
//------------------------------------------------------------------
//------------------------------------------------------------------
//------------------------------------------------------------------
//------------------------------------------------------------------
// Статусы БИОС
//------------------------------------------------------------------
enum BIOS_STATUS
{
    BIOS_STATUS_KKT_NOT_INITED         = 0x0001,             //  0-й бит    Не была вызвана функция ?Начало работы?
    BIOS_STATUS_KKT_NOT_FISCAL         = 0x0002,             //  1-й бит 	 Нефискальный режим
    BIOS_STATUS_KKT_IS_SHIFT_OPEN      = 0x0004,             //  2-й бит 	 Смена открыта
    BIOS_STATUS_KKT_IS_SHIFT_24H       = 0x0008,             //  3-й бит    Смена больше 24 часов
    BIOS_STATUS_KKT_FS_CLOSE           = 0x0010,             //  4-й бит    Архив ФН закрыт
    BIOS_STATUS_KKT_NOT_REGISTERED     = 0x0020,             //  5-й бит    ФН не зарегистрирован (postFiscal =   kkt_notFiscal (???) kkt_notRegistered)
    BIOS_STATUS_KKT_CLOSE_SHIFT_NEED   = 0x0100,             //  8-й бит    Не было завершено закрытие смены, необходимо повторить операцию
    BIOS_STATUS_KKT_CL_ERR             = 0x0200,             //  9-й бит    Ошибка контрольной ленты

    BIOS_STATUS_FATAL_WRONG_SUM_NVR    = 0x0001,             // 1-й бит Неверная контрольная сумма NVR
    BIOS_STATUS_FATAL_WRONG_SUM_KONF   = 0x0002,             // 2-й бит Неверная контрольная сумма в конфигурации
    BIOS_STATUS_FATAL_FS_NOT_ATH       = 0x0020,             // 6-й бит ККТ не авторизовано
    BIOS_STATUS_FATAL_FS_ERR           = 0x0040,             // 7-й бит Фатальная ошибка ФН
    BIOS_STATUS_FATAL_SD_ERR           = 0x0100              // 9-й бит 	SD карта отсутствует или неисправна
};

//-------------------------------------------------
// Шрифты для принтера BIOS
enum FONTS
{
    FONT_NORM               = 0x01,               //  Шрифт 9х17
    FONT_NORM_DBL_H         = 0x11,               //  Шрифт 9х17, двойной высоты
    FONT_NORM_DBL_W         = 0x21,               //  Шрифт 9х17, двойной ширины
    FONT_NORM_DBL_H_W       = 0x31,               //  Шрифт 12х24, двойной высоты, двойной ширины

    FONT_BIG                = 0x00,               //  Шрифт 12х24
    FONT_BIG_DBL_H          = 0x10,               //  Шрифт 12х24, двойной высоты
    FONT_BIG_DBL_W          = 0x20,               //  Шрифт 12х24, двойной ширины
    FONT_BIG_DBL_H_W        = 0x30,               //  Шрифт 12х24, двойной высоты, двойной ширины
};

//-------------------------------------------------
// тип открываемого документа
enum DOC_TYPES
{
    DOC_TYPE_SERVICE              = 0x01,               //  Сервисный документ
    DOC_TYPE_SALE                 = 0x02,               //  Чек на продажу (приход)
    DOC_TYPE_SALE_RET             = 0x03,               //  Чек на возврат (возврат прихода)
    DOC_TYPE_INSERTION            = 0x04,               //   Внесение в кассу
    DOC_TYPE_RESERVE              = 0x05,               //  Инкассация
    DOC_TYPE_CONSUMPTION          = 0x06,               //  Чек на покупку (расход)
    DOC_TYPE_CONSUMPTION_RET      = 0x07                //  Чек на возврат покупки (возврат расхода)
};

//-------------------------------------------------
// тип открываемого документа РБ
enum DOC_TYPES_BY
{
    DOC_TYPE_BY_SERVICE        = 0x01,               //  Сервисный документ
    DOC_TYPE_BY_SALE           = 0x02,               //  Чек на продажу (приход)
    DOC_TYPE_BY_SALE_RET       = 0x03,               //  Чек на возврат (возврат прихода)
    DOC_TYPE_BY_INSERTION      = 0x04,               //  Внесение в кассу
    DOC_TYPE_BY_RESERVE        = 0x05,               //  Инкассация
    DOC_TYPE_BY_ORDER_CREATE   = 0x06,               // Чек оформление заказа
    DOC_TYPE_BY_ORDER_CLOSE    = 0x07,               // Чек закрытия заказа
    DOC_TYPE_BY_ORDER_CANCEL   = 0x08,               // Чек отмены заказа
    DOC_TYPE_BY_ORDER_ADD      = 0x09,               // Чек добавления заказа
    DOC_TYPE_BY_ORDER_COR      = 0x0a,               // Чек коррекции заказа
};

//-------------------------------------------------
// режим формирования документа
enum DOC_FORMATS
{
    DOC_FORMAT_NORMAL        = 0x00,               //  Обычный режим формирования документа
    DOC_FORMAT_PACKET        = 0x10,               //   Пакетный режим формирования документа
    DOC_FORMAT_FAST          = 0x90                //   Пакетный режим формирования документа
};

//-------------------------------------------------
//  режим печати реквизитов
enum DOC_REQ_PRINT
{
    DOC_REQ_PRINT_NORMAL     = 0x00,               //  Обычный режим печати реквизитов
    DOC_REQ_PRINT_DELAYED    = 0x20               //   Режим отложенной печати реквизитов
};

//-------------------------------------------------
//  типы систем налогообложения
enum SNO_TYPES
{
    SNO_OSN          = 0x00,               //  Основная
    SNO_USN          = 0x01,               //  Упрощенная Доход
    SNO_USN_DOH_RASH = 0x02,               //  Упрощенная Доход минус Расход
    SNO_ENVD         = 0x03,               //  Единый налог на вмененный доход
    SNO_ESN          = 0x04,               //  Единый сельскохозяйственный налог
    SNO_PATENT       = 0x05                //  Патентная система налогообложения
};
//-------------------------------------------------
//  Команды PIRIT RB
enum PIRIT_RB_COMMAND
{
    PIRIT_RB_CANCEL_DOC        = 0x35, // Аннулировать документ
    PIRIT_RB_DOC_FROM_KL_PRINT = 0x70, // Распечатать документ из КЛ по номеру
    PIRIT_RB_DOC_FROM_KL_READ  = 0xf0  // Прочесть документ из КЛ по номеру
};

//  Параметры для PIRIT_RECEIPT_DATA
enum PIRIT_RECEIPT_DATA_PARAM
{
    PIRIT_RECEIPT_DATA_CUR_DOC  = 0x01, // Данные по текущему чеку
    PIRIT_RECEIPT_DATA_LAST_DOC = 0x02  // Данные по закрытому чеку
};

//  Параметры для PIRIT_RECEIPT_DATA
enum PIRIT_REG_DOC_TYPE
{
    PIRIT_REG_DOC_TYPE_UNDEFINED = 0x00, // BAD
    PIRIT_REG_DOC_TYPE_REG       = 0x01, // registration
    PIRIT_REG_DOC_TYPE_REREG     = 0x0B  // reregistration
};

//-------------------------------------------------
//  Индексы для типов платежей
enum PIRIT_RECEIPT_INDEX
{
    PIRIT_RECEIPT_CASH      = 0x00,    // НАЛИЧНЫМИ
    PIRIT_RECEIPT_CASHLESS  = 0x01,    // ЭЛЕКТРОННЫМИ
    PIRIT_RECEIPT_USER_0    = 0x02,    // Пользовательские строки наименования платежа
    PIRIT_RECEIPT_USER_1    = 0x03,    // Пользовательские строки наименования платежа
    PIRIT_RECEIPT_USER_2    = 0x04,    // Пользовательские строки наименования платежа
    PIRIT_RECEIPT_USER_3    = 0x05,    // Пользовательские строки наименования платежа
    PIRIT_RECEIPT_USER_4    = 0x06,    // Пользовательские строки наименования платежа
    PIRIT_RECEIPT_USER_5    = 0x07,    // Пользовательские строки наименования платежа
    PIRIT_RECEIPT_USER_6    = 0x08,    // Пользовательские строки наименования платежа
    PIRIT_RECEIPT_USER_7    = 0x09,    // Пользовательские строки наименования платежа
    PIRIT_RECEIPT_USER_8    = 0x0a,    // Пользовательские строки наименования платежа
    PIRIT_RECEIPT_USER_9    = 0x0b,    // Пользовательские строки наименования платежа
    PIRIT_RECEIPT_USER_10   = 0x0c,    // Пользовательские строки наименования платежа
    PIRIT_RECEIPT_ADVANCE   = 0x0d,    // ПРЕДВАРИТЕЛЬНАЯ ОПЛАТА (АВАНС)
    PIRIT_RECEIPT_CREDIT    = 0x0e,    // ПОСЛЕДУЮЩАЯ ОПЛАТА (КРЕДИТ)
    PIRIT_RECEIPT_EXCHANGE  = 0x0f     // ИНАЯ ФОРМА ОПЛАТЫ
};

//-------------------------------------------------
//  Параметры для команды PIRIT_KKT_INFO
enum PIRIT_KKT_INFO_PARAM
{
    PIRIT_KKT_INFO_KKT_PLANT_NUM        = 0x01,     //  Вернуть заводской номер ККТ
    PIRIT_KKT_INFO_ID_FIRMWARE          = 0x02,     //  Вернуть идентификатор прошивки
    PIRIT_KKT_INFO_INN                  = 0x03,     //	Вернуть ИНН
    PIRIT_KKT_INFO_PLANT_NUM            = 0x04,     //  Вернуть регистрационный номер ККТ
    PIRIT_KKT_INFO_DATE_TIME_LAST_FD    = 0x05,     //	Вернуть дату и время последней фискальной операции
    PIRIT_KKT_INFO_DATE_TIME_REG        = 0x06,     //  Вернуть дату регистрации / перерегистрации
    PIRIT_KKT_INFO_CASH_IN_DROWER       = 0x07,     //  Вернуть сумму наличных в денежном ящике
    PIRIT_KKT_INFO_NEXT_DOC             = 0x08,     //  Вернуть номер следующего документа
    PIRIT_KKT_INFO_FS_VALIDITY_DATE     = 0x0E,     //  Вернуть ресурс ключей ФН
    PIRIT_KKT_INFO_DATE_TIME_OPEN_SHIFT = 0x11,     //  Вернуть дату и время открытия смены
    PIRIT_KKT_INFO_CNO_SIGNS_AGENT      = 0x17      //  Вернуть систему налогообложения и режим работы и ФН
};

//-------------------------------------------------
//  Параметры для команды PIRIT_READ_TABLE_SETTINGS
enum PIRIT_TABLE_SETTINGS
{
    PIRIT_TABLE_SETTINGS_KKT        = 0x01,     //  Параметры ПУ
    PIRIT_TABLE_SETTINGS_PURCHASE   = 0x02,     //  Параметры чека
    PIRIT_TABLE_CALC_MASK           = 0x05,     //  Управление расчетами (битовая маска)
    PIRIT_TABLE_NAME_ADDRESS_ORG    = 0x1E,     //  Наименование и адрес организации
    PIRIT_TABLE_AUTOMAT_NUM         = 0x46,     //  Номер автомата
    PIRIT_TABLE_OFD_INN             = 0x47,     //  ИНН ОФД
    PIRIT_TABLE_OFD_IP              = 0x4D,     //  Адрес сервера ОФД для отправки документов
    PIRIT_TABLE_OFD_PORT            = 0x4E,     //  Порт сервера ОФД
    PIRIT_TABLE_OFD_NAME            = 0x51,     //  Наименование ОФД
    PIRIT_REC_SENDER_EMAIL          = 0x52,     //  Электронная почта отправителя чека
    PIRIT_TABLE_CALCULATION_PLACE   = 0x55,     //  Место расчетов
    PIRIT_TABLE_LIC_OPTIONS         = 0x58      //  получить маску локальных лицензий(тэгов, опций)
};

//-------------------------------------------------
//  Параметры для команды PIRIT_TABLE_SETTINGS_KKT
enum PIRIT_SETTINGS_KKT
{
    PIRIT_SET_KKT_PRINT_INTERV = 0x01, // печать с уменьшенным межстрочным интервалом
    PIRIT_SET_KKT_FULL_CUT     = 0x02, // полная отрезка чека
    PIRIT_SET_KKT_PRINT_LOGO   = 0x04, // печатать графический логотип
    PIRIT_SET_KKT_PRINT_QR     = 0x20, // печатать QR код на чеке (всвегда 1)
    PIRIT_SET_KKT_AUTO_SHIFT   = 0x20, // автоматически переоткрывать смену при открытии документа, если смена истекла
    PIRIT_SET_KKT_PRINT_DEPART = 0x40, // печатать отделы на чеках
    PIRIT_SET_KKT_NOT_PRINT    = 0x80  // не печатать документы на чековой ленте
};

//-------------------------------------------------
//  Коды реквизитов для команды PIRIT_PRINT_REQUISIT_OFD
enum PIRIT_REQUISIT_OFD
{
    PIRIT_REQ_OFD_SIGN_AGENT       = 0x0040,      // Признак агента в чеке

    PIRIT_REQ_OFD_TRANS_OPER_ADR   = 0x03ED,    // Адрес оператора перевода
    PIRIT_REQ_OFD_TRANS_OPER_INN   = 0x03F8,    // ИНН оператора перевода
    PIRIT_REQ_OFD_TRANS_OPER_NAME  = 0x0402,    // Наименование оператора перевода
    PIRIT_REQ_OFD_PAYING_AGENT_OP  = 0x0414,    // Операция платежного агента
    PIRIT_REQ_OFD_AGENT_SIGN       = 0x0421,    // Признак агента
    PIRIT_REQ_OFD_AGENT_TEL_NUM    = 0x0431,    // Телефон платежного агента
    PIRIT_REQ_OFD_BILLING_TEL_NUM  = 0x0432,    // Телефон опер. по приему платежей
    PIRIT_REQ_OFD_TRANSFER_TEL_NUM = 0x0433,    // Телефон оператора перевода
    PIRIT_REQ_OFD_PROVIDER_TEL_NUM = 0x0493,    // Телефон поставщика
    PIRIT_REQ_OFD_PROVIDER_INN     = 0x04CA,    // ИНН поставщика
    PIRIT_REQ_OFD_ADD_REC_REQ      = 0x04A8     // дополнительный реквизит чека (БСО)
};

//-------------------------------------------------
//  Параметры для команды PIRIT_GET_INFO_FROM_FN
enum PIRIT_INFO_FN
{
    PIRIT_INFO_FN_VERSION       = 0x0E,     // Вернуть версии ФН и ФФД
    PIRIT_INFO_FN_DATETIME      = 0xCA,     // Вернуть дату и время из ФН
    PIRIT_INFO_FN_SET_DATETIME  = 0xCB      // Установить дату и время ФН
};

//-------------------------------------------------
//  Номер параметров PIRIT_INFO_FN
enum PIRIT_VERSION_INFO
{
    PIRIT_VERSION_FIRMWARE_FN   = 0x01,     // Версия прошивки ФН
    PIRIT_VERSION_FN            = 0x02,     // Версия ФН (0 - отладочный ФН, 1 - серийный ФН)
    PIRIT_VERSION_FFD           = 0x03,     // Версия ФФД
    PIRIT_VERSION_KKT           = 0x04,     // Версия ККТ
    PIRIT_VERSION_FFD_KKT       = 0x05,     // Версия ФФД ККТ
    PIRIT_VERSION_REG_FFD_FN    = 0x06,     // Зарегистрированная версия ФФД ФН
    PIRIT_VERSION_MAX_FFD_FN    = 0x07      // Максимальная версия ФФД ФН
};

//-------------------------------------------------
// Номер параметров PIRIT_INFO_FN_DATETIME
enum PIRIT_DATETIME_INFO
{
    PIRIT_DATETIME_DATE         = 0x01,     // Дата из ФН
    PIRIT_DATETIME_TIME         = 0x02      // Время из ФН
};

//-------------------------------------------------
// Номер параметров для получения сервисной информации
enum PIRIT_SERVICE_INFORMATION
{
    PIRIT_BATTERY_VOLTAGE       = 0x07,     // Напряжение на батарейке
    PIRIT_SKNO_DATETIME         = 0x64      // Дата и время из СКНО
};

enum PIRIT_DREAMKAS_KEY_CMD
{
    PIRIT_DREAMKAS_KEY_SET          = 0x01, // Установить Дримкас-ключ в БИОС
    PIRIT_DREAMKAS_KEY_GET_INFO     = 0x02, // Получить информацию о выбранной платной фиче
    PIRIT_DREAMKAS_KEY_PRINT_ALL    = 0x03, // Распечатать все известные фичи
    PIRIT_DREAMKAS_KEY_GET_ALL      = 0x04  // Получить информацию о всех платных фичах
};

//========================================================================================
// Общие команды
//========================================================================================
void libSetIP      (std::string newIP);
void libSetPort    (int       newPort);
void libSetPasswd (Country newCountry);
void libSetErrors (Country newCountry);

int   commandStart          ();  //Начало работы   ОБЯЗАТЕЛЬНО ПРИ СТАРТЕ!
void  setDebugLevel(int level);  // Уровень отладки

//========================================================================================
// Принтер
//========================================================================================
int   getPrinterStatus(int* result);               //Запрос состояния  печатающего устройства (ПУ)
int   scrollPaper                ();               // Промотка бумаги П2
int   libCutDocument             ();               // Принудительная отрезка документа с предпечатью
int   libPrintServiceData        ();               // Распечатать сервисные данные
int   libPrintServiceDataSKNO_By ();               // Распечатать сервисные данные СКНО (только РБ)

// Печать отчета о текущем состоянии расчетов
int   libCurrentStatusReport(       char* nameCashier);
int   libPrintXReport        (const char* nameCashier);        // Печать Х отчёт
int   libPrintCopyLastZReport                       ();        // Печать копии последнего Z-отчета
int   libPrintCopyReportFiscalization               ();        // Печать копии чека фискализации/перерегистрации

//Печать текста любого с заданными аттрибутами
                int libPrintString(char               *textStr,    // Печатаемый текст 0..72 символа
                                   FONTS                   fnt);   // Аттрибуты из enum FONTS
// Печать штрих-кода
int   libPrintBarCode             (unsigned char       posText,
                                   unsigned char  widthBarCode,
                                   unsigned char heightBarCode,
                                   unsigned char   typeBarCode,
                                   const    char      *barCode);
// Печать графики в документе
int   libPrintPictureInDocument   (int                   width,
                                   int                  height,
                                   int                      sm,
                                   unsigned char         *data);
// Печать загруженной ранее картинки
int   libPrintPreloadedPicture    (int                      sm,
                                   int                  number);
// Распечатать фискальный отчет по сменам
int   libPrintFiscalReportByShifts(unsigned char     typeReport,
                                   int         startShiftNumber,
                                   int           endShiftNumber,
                                   const    char      *password);
// Распечатать фискальный отчет по датам
int   libPrintFiscalReportByDate  (unsigned char     typeReport,
                                   PLDate             startDate,
                                   PLDate               endDate,
                                   const char         *password,
                                   const char         *cashier  );
// Печать реквизита
int   libPrintRequisite           (unsigned char codeReq, unsigned char attributeText,
                                   const char* str1,           const char* str2 = nullptr,
                                   const char* str3 = nullptr, const char* str4 = nullptr);
// Печать реквизита для ОФД
int   libPrintRequsitOFD          (int                  codeReq,
                                   unsigned char  attributeText,
                                   const    char       *reqName,
                                   const    char        *reqStr);


//========================================================================================
// Чеки
//========================================================================================
int   libOpenShift          (const    char  *nameCashier);              // Открыть смену
int   libOpenShift_By                                  ();              // Открыть смену для РБ
/**
 * @brief The CloseDocParam struct входные параметры закрытия документа
 */
struct CloseDocParam
{
    CloseDocParam() : cutPaper(0x00), flags(0) {}

    unsigned char cutPaper;                         // (Целое число) Флаг отрезки
    std::string buyerAddress;                       // (Строка) Адрес покупателя
    uint32_t flags;                                 // (Число) Разные флаги
    std::string clcPlace;                           // (Строка) Место расчётов
    std::string recSendAddress;                     // (Строка) Адрес отправителя чеков
    std::string automaticNumber;                    // (Строка) Номер автомата
    std::pair<std::string, std::string> addUserReq; // (Строка) Название дополнительного реквизита пользователя
                                                    // (Строка) Значение дополнительного реквизита пользователя
    std::string buyerName;                          // (Строка)[0..128] Покупатель
    std::string buyerInn;                           // (Строка)[0..12] ИНН покупателя
};
/**
 * @brief libCloseDocument закрыть документ
 * Если НУЖЕН полный набор ответных параметров
 * Полный набор ответных параметров возвращается только для фискальных чеков прихода, возврата прихода, расхода и возврата расхода.
 * Для остальных документов возвращаются только сквозной номер документа и операционный счётчик.
 * @param[in] cutPaper флаг отрезки
 * @param[in] buyer_address адрес покупателя
 * @param[in] internetSign установить признак расчетов в интернет
 * @return структура ответных данных
 */
CloseDocResult libCloseDocument(const CloseDocParam &closeDocParam);

/**
 * @brief libCloseDocumentLite
 * Если полный набор ответных параметров НЕ НУЖЕН!!!
 * Полный набор ответных параметров возвращается только для фискальных чеков прихода, возврата прихода, расхода и возврата расхода.
 * Для остальных документов возвращаются только сквозной номер документа и операционный счётчик.
 * @param[in] buyer_address флаг отрезки
 * @param[in] buyer_address адрес покупателя
 * @param[in] internetSign установить признак расчетов в интернет
 * @return структура ответных данных
 */
CloseDocResult libCloseDocumentLite   (unsigned         char cutPaper,
                                       string           buyerAddress,
                                       unsigned char    internetSign);


// ЗАКРЫТЬ ДОКУМЕНТ БЕЛОРУССУ ПО КРАСОТЕ
MData libCloseDocument_By(unsigned       char   cutPaper);              //  Флаг отрезки
//--------
int   libEmergencyCloseShift                           ();              // Аварийное закрытие смены

int   libPrintZReport       (const    char   *nameCashier,
                             int                 options);              // Закрыть смену + Z отчёт
int   libPrintZReport_By    (const    char   *nameCashier);             // Закрыть смену + Z отчёт для РБ

int   libCancelDocument                                ();              // Аннулировать документ
int   libCancelDocument_By(int, string, double (&arr)[PIRIT_PARAM_16]); // Аннулировать документ для РБ
int   libPostponeDocument   (const char            *info);              // Отложить документ
/**
 * @brief libGetDocFromKL чтение документа из КЛ
 * @param docNum номер документа
 * @param doc документ из КЛ
 * @return код ошибки
 */
int libGetDocFromKL(const uint32_t &docNum, DocFromKL &doc);
//--------
// Команда на открытие документа(чека) кайфовая
int libOpenDocument       (DOC_TYPES                   type,   //  тип открываемого документа
                           DOC_FORMATS                  fmt,   //  режим формирования документа
                           DOC_REQ_PRINT            doc_req,   //  режим печати реквизитов
                           int              numOfDepartment,   //  Номер отдела
                           string            nameOfOperator,   //  Имя оператора
                           int                     numOfDoc,   //  Обязателен при уст флага "Нумер чеков со стороны внешн. программы"
                           SNO_TYPES                    sno);  //  Система налогообложения

// Команда на открытие документа(чека) кайфовая для РБ
int libOpenDocument_By    (DOC_TYPES_BY                type,   //  тип открываемого документа
                           DOC_FORMATS                  fmt,   //  режим формирования документа
                           DOC_REQ_PRINT            doc_req,   //  режим печати реквизитов
                           int              numOfDepartment,   //  Номер отдела
                           string            nameOfOperator,   //  Имя оператора
                           int                     numOfDoc);  //  Обязателен при уст флага "Нумер чеков со стороны внешн. программы"

// Открыть документ 1
int   libOpenDocument       (unsigned char           type,
                             unsigned char      numDepart,
                             char*            nameCashier,
                             long               docNumber);
// Открыть документ 2
int   libOpenDocumentEx     (unsigned char           type,
                             unsigned char      numDepart,
                             char*            nameCashier,
                             long               docNumber,
                             unsigned char          taxN);
// Запрос данных по чеку
MData libGetReceiptData     (unsigned char    numRequest);
int   libSubTotal();                                                   // Подитог

//  =========== Позиции ============

/**
 * @brief libSetPosAddReq - установить дополнительные реквизиты позиции
 * @param requisite -  код товарной номенклатуры
 * @return результат выполнения
 */
int libSetPosAddReq(string requisite);

// Полная команда "Установить дополнительные реквизиты позиции"
int libSetPosAddReq( const AdditionalReq &ar );

// Добавить товарную позицию
int   libAddPosition        (const char*        goodsName,
                             const char*          barcode,
                             double              quantity,
                             double                 price,
                             unsigned char      taxNumber,
                             int              numGoodsPos,
                             unsigned char      numDepart,
                             unsigned char       coefType,
                             const char         *coefName,
                             double             coefValue);
namespace PiritLib
{
    enum LIMIT
    {
        LIMIT_GOODS_NAME = 256,
        LIMIT_ARTICLE_BARCODE = 18,
        LIMIT_POSITION_NUMBER = 4,
        LIMIT_ORIGIN_COUNTRY_CODE = 3,
        LIMIT_CUSTOM_ENTRY_NUM = 32
    };
    /**
     * @brief The Position struct структура данных позиции
     */
    struct Position
    {
        std::string goodsName;                    // (Строка[0...224]) Название товара
        std::string barcode;                      // (Строка[0..18]) Артикул или штриховой код товара/номер ТРК
        double quantity {.00};                    // (Дробное число) Количество товара в товарной позиции
        double price{.00};                        // (Дробное число) Цена товара по данному артикулу
        unsigned char taxNumber {0x00};           // (Целое число) Номер ставки налога
        int numGoodsPos {0};                      // (Строка[0..4]) Номер товарной позиции
        unsigned char numDepart {0x00};           // (Целое число 1..16) Номер секции
        unsigned char coefType {0x00};            // 0x00
        std::string coefName;                     // 0x00
        double coefValue {.00};                   // 0
        int32_t signMethodCalculation {0x00};     // Целое число) Признак способа расчета  (1 - Предоплата 100%)
        int32_t signCalculationObject {0x00};     // Целое число) Признак предмета расчета (1 - ????)
        std::string originCountryCode;            // код страны происхождения товара
        std::string customEntryNum;               // регистрационный номер таможенной декларации
        double exciseDuty {.00};                  // сумма акциза
        std::string addPosReq;                    // дополнительный реквизит предмета расчёта
    };
    /**
     * @brief libAddPosition добавить товарную позицию
     * @param position позиция
     * @return результат выполнения
     */
    int libAddPosition(const Position &position);
}

// Добавить товарную позицию РБ
int libAddPositionLarge_By   (const char      *goodsName,  // (Строка[0...224])   Название товара
                              const char        *barcode,  // (Строка[0..18])     Артикул или штриховой код + A-не GTIN, B-GTIN, C-услуга
                              double            quantity,  // (Дробное число)     Количество товара в товарной позиции
                              double               price,  // (Дробное число)     Цена товара по данному артикулу
                              unsigned char    taxNumber,  // (Целое число)       Номер ставки налога
                              int            numGoodsPos,  // (Строка[0..4])      Номер товарной позиции
                              unsigned char    numDepart); // (Целое число 1..16) Номер секции

//----------------------------------------------
//                     КЛ
//----------------------------------------------
// Выгрузить документ по номеру из КЛ
//----------------------------------------------
int libKLDocRead( long doc_num, std::vector<std::string> &strs );
//----------------------------------------------
// Распечатать документ по номеру из КЛ
//----------------------------------------------
int libKLDocPrint( long doc_num );
//----------------------------------------------

// Сторнировать товарную позицию
int   libDelPosition        (const char          *goodsName,
                             const char            *barcode,
                             double                quantity,
                             double                   price,
                             unsigned char        taxNumber,
                             int                numGoodsPos,
                             unsigned char        numDepart);

int   libAddDiscount    (unsigned char typeDiscount, const char* nameDiscount, long sum); // Добавить скидку
int   libAddMargin      (unsigned char typeMargin,   const char* nameMargin,   long sum); // Добавить наценку

int   libAddPayment     (unsigned char typePayment,     double sum, const char* infoStr); // Добавить тип оплаты
int   libAddPaymentD    (unsigned char typePayment,     double sum, const char* infoStr); // Добавить тип оплаты

int   libAddPayment_By  (unsigned char typePayment,     double sum, const char* infoStr); // Добавить тип оплаты Белоруссия

unsigned long libSetBuyerAddress(const char *buyerAddress);                   //Установить адрес покупателя
unsigned long libGetBuyerAddress(char *buyerAddress, unsigned long baLength); //Получить адрес покупателя

// формирует чек коррекции с записью в ФН
int   libDoCheckCorrection(const char         *nameCashier,
                           double                     cash,
                           double                 cashless,
                           unsigned char   correctionFlags);
// формирует чек коррекции с записью в ФН
int   libDoCheckCorrectionEx(const char       *nameCashier,
                             double                   cash,
                             double               cashless,
                             double                   sum1,
                             double                   sum2,
                             double                   sum3,
                             unsigned char correctionFlags,
                             const PLDate         *docDate,
                             const char         *docNumber,
                             const char    *correctionName,
                             double                 sum_18,
                             double                 sum_10,
                             double                  sum_0,
                             double                 sum_WT,
                             double             sum_18_118,
                             double             sum_10_110);
//Открыть копию чека
int   libOpenCopyReceipt    (unsigned char            type,
                             unsigned char       numDepart,
                             const char       *nameCashier,
                             int                  numCheck,
                             int                   numCash,
                             PLDate                 mpDate,
                             PLTime                 mpTime);
//Стереть адрес покупателя
void  libCleanBuyerAddress                              ();
//Внесение / изъятие суммы
int   libCashInOut          (const char           *infoStr,
                             double                   sum);
//Зарегистрировать сумму по отделу
int   libRegisterSumToDepart(unsigned char  typeOperation,
                             unsigned char   numberDepart,
                             long                     sum);
//Зарегистрировать сумму налога
int   libRegisterTaxSum     (unsigned char      numberTax,
                             long                     sum);
//Сравнить сумму по чеку
int   libCompareSum         (long                     sum);


//========================================================================================
// Дата и Время
//========================================================================================
int   libSetPiritDateTime(PLDate  mpDate,
                          PLTime  mpTime);  //Запись даты/времени ККТ

int   getPiritDateTime   (int     *cDate,
                          int     *cTime);  //Чтение даты/времени ККТ 1

int   getCurMPTime       (PLDate *mpDate,
                          PLTime *mpTime);  // Получить тек локальные дата и время

//========================================================================================
// Ошибки Протокола пирит
//========================================================================================
MData libGetExErrorInfo    (unsigned char numRequest);       // Запрос дополнительной информации о ошибках П2
/**
 * @brief libIsErrorCritical метод для определения, была ли выполнена операция
 * @param errCode код полученной ошибки
 * @param opType тип операции, в ходе которой получена ошибка
 * @param retMess текст ошибки (возвращаем из метода), необязательно
 * @return результат выполнения
 */
bool libIsErrorCritical(const int errCode, FB_OPERATION_CRITICAL_LEVEL opType, std::string& retMess);
//========================================================================================
// ККТ
//========================================================================================
int   command0x9A                                     ();   //Очищение фискальной памяти
int   libTechnologicalReset(const PLDateTime  *dateTime);   // ! Тех обнуление !
MData libReadSettingsTable (unsigned char        number,
                            int                   index);   // Чтение таблицы настроек

int getStrFromSettingsTable(string &dataTable,uint8_t number, int32_t index, uint16_t numParam = 0);   // Чтение таблицы настроек с возвратом строки
int getIntFromSettingsTable(int &data, uint8_t number, int32_t index, uint16_t numParam = 0);          // Чтение таблицы настроек с возвратом числа или битовой маски

int getStrFromInfoFN(uint8_t number, int32_t index, string &dataFN); // Чтение информации из ФН с возвратом строки
int getIntFromInfoFN(uint8_t number, int32_t index, int &data);      // Чтение информации из ФН с возвратом числа или битовой маски

int getStrServiceInfoSKNO(uint8_t number, uint32_t index, string &dataSKNO); // Чтение информации из СКНО с возвратом строки
int getIntServiceInfoSKNO(uint8_t number, uint32_t index, int &data);        // Чтение информации из СКНО с возвратом числа или битовой маски

int libGetStatusOFD(int &result );  // Проверка доступности сервера ОФД

int   libWriteSettingsTable(unsigned char        number,
                            int                   index,
                            const char            *data);   // Запись таблицы настроек

int   libWriteSettingsTable(unsigned char        number,
                            int                   index,
                            uint8_t data);   // Запись таблицы настроек
/**
 * @brief The SetNVRcmd struct NVR-wrapper
 */
struct SetNVRcmd
{
    SetNVRcmd() = delete;
    SetNVRcmd(int _address, char *_data, uint16_t _size)
    {
        if ((_data != nullptr) && (_size <= MAX_SIZE))
        {
            address = _address;
            size = _size;
            data = new char[size];
            memcpy(data, _data, size);
        }
    }
    ~SetNVRcmd()
    {
        if (data != nullptr)
        {
            delete [] data;
        }
    }

    static const uint16_t MAX_SIZE = 128;       // Максимальная длинна пакета
    static const uint16_t ASCII_BYTE_LEN = 2;   // Размер в байтах ASCII-байта
    static const int TYPE = 3;                  // Тип
    int address = 0xf000;                       // адрес
    uint16_t size = 0;                          // размер данных
    char *data = nullptr;                       // данные
};
/**
 * @brief libSetNVR запись NVR
 * @param setNVRcmd NVR-wrapper
 * @return результат выполнения
 */
int libSetNVR(const SetNVRcmd &setNVRcmd);


int   saveLogoToFile       (char              *fileName);   // Скачать с БИОСа логотипчик
int   libLoadLogo          (int                    size,
                            unsigned char         *data);   // Загрузить логотип в ФР
int   libDeleteLogo                                   ();   // Удалить логотип

// Запрос флагов статуса ККТ
int   getStatusFlags       (int        *fatalStatus,
                            int *currentFlagsStatus,
                            int     *documentStatus);

// Запрос сведений о ККТ, getKKTInfoNum(numParam = 1)
int   getKKTInfo(           unsigned char numRequest,
                            char              *data);
// Запрос ????
int   getKKTInfoNum        (unsigned char numRequest,
                            int             numParam,
                            char             *param);

MData libGetServiceInfo    (unsigned char numRequest);       // Запрос сервисной информации П2

int libGetBatteryVoltage               (int &voltage);       // вернуть напряжение на батарейке

int   libLoadReceiptDesign( int                size,
                            unsigned char*     data);         // Загрузить дизайн чека
// Загрузить(Напечатать?) графическое изображение
int   libLoadPicture      ( int               width,
                            int              height,
                            int                  sm,
                            const char*        name,
                            int              number,
                            unsigned char     *data);
// Версия Драйвера
long long getDriverVersion();

// Создание сообщения по коду ошибки
unsigned long libFormatMessage(int           errorCode,
                               char         *msgBuffer );

//========================================================================================
// РБ. Таблица настроек КСА
//========================================================================================
// Запись значения в таблицу настроек КСА
int libSetSettingsKSA_By(int   number,         // Номер поля
                         int    index,         // Индекс(для массива)
                         char   *data);        // Значение

// Чтение значения из таблицы настроек КСА
int libGetSettingsKSA_By(int   number,          // Номер поля
                         int    index,          // Индекс(для массива)
                         char   *data);         // Значение

//========================================================================================
// РБ. Запрос состояний
//========================================================================================
// РБ. Запрос дополнительной информации о ошибках
int libGetInfoErrorsExt_By( int *errs                                            );

// РБ. Запрос флагов статуса КСА
int libGetStatusFlags_By  ( int *statusFatal, int *statusCurrent, int *statusDoc, int *statusSKNO );

//========================================================================================
// Счётчики
//========================================================================================
// ОБЩИЙ(в завис-ти от requestNumber) сменных счетчиков и регистров
int   getCountersAndRegisters         (int        requestNumber,
                                       int                *data);

// Запрос сменных счетчиков и регистров
MData libGetCountersAndRegisters      (unsigned char numRequest);

// Запрос сменных счетчиков и регистров для  12 типа
int   libGetCountersAndRegisters_12   (int               data[],
                                       int           maxElement,
                                       char                *str);

//Запрос сменных счетчиков и регистров ret errCode
int   libGetCountersAndRegisters_intXd(int         requestNumber,
                                       int                data[],
                                       int            maxElement,
                                       int                 type);


//========================================================================================
// Денежный ящик
//========================================================================================
int   libSetToZeroCashInCashDrawer                  ();   // Обнулить наличные в денежном  щике
int   libOpenCashDrawer            (int pulseDuration);   // Открыть денежный  щик
int   getCashDrawerStatus          (int *drawerStatus);   // Получить статус денежного  щика 1


//========================================================================================
// Бибика
//========================================================================================
int   libBeep(int duration);

//========================================================================================
// ФН
//========================================================================================
// Закрыть ФН
int   libCloseFN (const char *cashierName, uint32_t &fd, uint32_t &fpd, string &dateTime);

// Аварийное закрытие ФН (0xA4)
int libEmegencyCloseFN();

// 	Запрос номера ФД и общей длины ТЛВ-структуры последней рег/перерег (для последующей выгрузки)
int libGetRegistrSTLVProps( int   &numberOfFD,    // Номер ФД, для которого требуется получить ТЛВ
                            int     &lenTotal,    // Длина полученного ТЛВ пакета
                            int     &docType );   // Тип документа

int libGetDocSTLV         ( const int   &numberOfFD,    // Номер ФД, для которого требуется получить ТЛВ
                                  int     &lenTotal,    // Длина полученного ТЛВ пакета
                                  int     &docType );   // Тип документа

int libGetRegSTLV         ( PIRIT_REG_DOC_TYPE &regType );      // Тип документа 1- reg, 11 - rereg
int libGetLastRegData     ( uint32_t& fd, uint32_t& fpd );

// 	Запрос документа из архива в формате TLV, до ошибки !! 48h !!, ПОСЛЕ КОМАНДЫ (78 - 15/20, libGetRegistrSTLVProps!!!!)
int libGetRegistrTLV( char       *tlv,                 // Тело ТЛВ пакета             (компоненты общей СТЛВ)
                      int     &lenTLV ,                // Длина полученного ТЛВ пакета(компоненты общей СТЛВ)
                      const bool   isRegTLV = false);  // true - send command 78/20, false - 78/16

// 	Запрос документа из архива по номеру
int libGetFnDocBin(  int        docNum,   // Номер документа
                     char       **data,   // Тело  документа
                     int      &dataLen,   // Длина документа
                     bool     &isCheck,   // Это квитанция( подтверждён в ОФД )
                     int      &docType);  // Тип выгружаемого документа
/**
 * @brief libSetFnDateTime - установка даты и времени в ФН
 * @param dateTime - дата и время
 * @return результат выполнения команды
 */
int libSetFnDateTime(const DateTime &dateTime);
//========================================================================================
// Получить информацию по последнему чеку
//========================================================================================
int libGetLastRecieptData(  int                &type,   // тип чека (для аннулиров. = 0)
                            string       &cur_op_cnt,   // текущий операц. счетчик
                            int             &rec_num,   // номер чека
                            int             &doc_num,   // номер документа
                            double              &sum,   // сумма чека
                            double         &discount,   // сумма скидки
                            string              &fpd,   // строка ФП (фиск. признак)
                            int                  &fd ); // номер ФД

int libGetLastRecieptData_By(  int                &type,   // тип чека (для аннулиров. = 0)
                               string       &cur_op_cnt,   // текущий операц. счетчик
                               int             &rec_num,   // номер чека
                               int             &doc_num,   // номер документа
                               double              &sum,   // сумма чека
                               double         &discount,   // сумма скидки по чеку
                               double          &markup);   // сумма наценки по чеку

//========================================================================================
// РЕГИСТРАЦИЯ ПЕРЕРЕГИСТРАЦИЯ
//========================================================================================

//Полный сброс ККТ и ФН (суперсекретая команда)
int   libAuthorization(PLDate mpDate, PLTime mpTime, const char *numKKT);

//Фискализация /Перерегистрация
int   libFiscalization(const char    *oldPassword,
                       const char      *regNumber,
                       const char            *INN,
                       const char    *newPassword);

//Фискализаци /Перерегистрация
int   libRegistration(unsigned char          type,
                      const char       *regNumber,
                      const char             *INN,
                      int               systemTax,
                      int                     rej,
                      const char     *cashierName);

//Фискализаци /Перерегистрация
int   libRegistrationEx(unsigned char         type,
                        const char      *regNumber,
                        const char            *INN,
                        int              systemTax,
                        int                    rej,
                        const char    *cashierName,
                        PLDateTime     regDateTime,
                        const char      *userName1,
                        const char      *userName2,
                        const char *addressSettle1,
                        const char *addressSettle2,
                        const char    *placeSettle,
                        const char     *autoNumber,
                        const char        *OFDName,
                        const char         *OFDINN,
                        const char    *senderEmail,
                        const char       *FNSEmail,
                        bool             automatic);


// 	Вернуть номер ФД последней регистрации
int libGetRegFDNumber( int &fdNumber );

// 	Вернуть дату и время регистрации
int libGetRegDateTime( char *regDateTime );


//======================================================================================
// Обработка чисел
//======================================================================================
void  setAmountDecimalPlaces     (int decimalPlaces);
void  setQuantityDecimalPlaces   (int decimalPlaces);
void  setPercentageDecimalPlaces (int decimalPlaces);
int   getAmountDecimalPlaces                      ();
int   getQuantityDecimalPlaces                    ();
int   getPercentageDecimalPlaces                  ();




// =====================================
// Команда Вернуть номер ФН
int libGetFN_Number       (char                      *fnNum);  // Получить номер ФН из БИОС
// Команда Вернуть номер последнего фискального документа
int libLastFiscDoc_Number (char            *fiscalDocNumber);  // Вернуть номер последнего фискального документа
// Команда Вернуть состояние текущей смены
int libGetCurShiftState   (int                    *shiftNum,   // Номер смены
                           int                    *isOpened,   // открыта ли смена
                           int           *numRecieptInShift);  // Номер чека в смене
// Вернуть состояние ФН
int libGetCurFNState(int     *fnState,                         // Состояние ФН
                     int *curDocState,                         // Состояние текущего документа
                     int   *warnFlags);                        // Флаги предупреждения

// Вернуть состояние ФН
int libGetStatusOFD( int            *ofdState,                     // Статус обмена
                     int     *numOfDocsToSend,                     // Количество документов для передачи     в ОФД
                     int       *numFirstToSnd,                     // Номер первого документа для передачи   в ОФД
                     char      *firstDateTime                      // Дата/время первого док-та для передачи в ОФД
                    );

// =====================================



//======================================================================================
//============ КОМАДЫ ДЛЯ ПЕЧАТИ НЕФИСКАЛЬНЫХ ДОКУМЕНТОВ В ПАКЕТНОМ РЕЖИМЕ =============
//======================================================================================

// Команда на открытие документа для печати нефискальных данных в пакетном режиме
int libOpeNotFiscalDoc(const bool header = false);

// Команда на закрытие документа
int libCloseNotFiscalDoc(uint8_t cutPaper); //  Флаг отрезки


//Печать текста с заданными аттрибутами в пакетном режиме
int libPrintNotFiscalStr(char  *textStr,   // Печатаемый текст 0..72 символа
                         FONTS     fnt);   // Аттрибуты из enum FONTS


//======================================================================================


//======================================================================================
//==========================  КОМАНДЫ ДРИМКАС-КЛЮЧ =====================================
//======================================================================================
/**
 * @brief libDreamkasKeySet - записать ключ в punix
 * @param strKey - ключ из Кабинета
 * @return 0 или код ошибки
 */
int libDreamkasKeySet(string &strKey);
/**
 * @brief libDreamkasKeyGetInfo - получить информацию о выбранной услуге
 * @param index - номер устуги
 * @param keyInfo - структура с ответом
 * @return 0 или код ошибки
 */
int libDreamkasKeyGetInfo(const uint8_t &index, DreamkasKey &keyInfo);
/**
 * @brief libDreamkasKeyPrintAll - печать всех услуг
 * @return 0 или код ошибки
 */
int libDreamkasKeyPrintAll();
/**
 * @brief libDreamkasKeyGetInfo - получить информацию о всех услугах
 * @param keyInfoList - вектор структур услуг
 * @return 0 или код ошибки
 */
int libDreamkasKeyGetInfoList(vector<DreamkasKey> &keyInfoList);
/**
 * @brief libDreamkasKeyParseInfoList - парс информации об услуге в структуру
 * @param keyInfoList - вектор структур услуг
 * @param infoStr - строка с данными для парса
 * @return версия
 */
int libDreamkasKeyParseInfoList(vector<DreamkasKey> &keyInfoList, const string &infoStr);
/**
 * @brief libSplitString - сплит строки по разделителю
 * @param divisibleStr - исходная строка
 * @param delimiter - разделитель
 * @return вектор строк
 */
vector<string> libSplitString(string divisibleStr, char delimiter);

/**
 * @brief libRestartKKT - переинит БИОС без перезапуска процесса
 * @return 0 или код ошибки
 */
int libRestartKKT();

//======================================================================================


//======================================================================================
//======================================================================================
//======================================================================================
//===========================  ДАЛЬШЕ НЕ СМОТРЕТЬ  =====================================
//======================================================================================
//======================================================================================

//======================================================================================
// Не надо, но надо
MData libGetStatusFlags  ();                                    //Запрос флагов статуса ККТ
MData libGetKKTInfo (unsigned char numRequest);                 //Запрос сведений о ККТ
MData libGetPiritDateTime();                                    //Чтение даты/времени ККТ 2
MData libGetCashDrawerStatus();                                 // Получить статус денежного  щика 2
MData libReadMemoryBlock(unsigned char type, long startAdress, long numBytes); //Чтение блока пам ти ККТ
MData libGetPrinterStatus();                                    // Запрос состояния печатающего устройства (ПУ)


//======================================================================================
// ЭКЛЗ, устарело

int   libActivizationECT();
int   libCloseArchiveECT();
MData libGetInfoFromECT(unsigned char number, long dataL1, long dataL2);

int   libEnableServiceChannelToECT();                  // Включение сервисного канала к ЭКЛЗ
int   command0x9B();                                   // Очищение ЭКЛЗ
int   libPrintControlTapeFromECT(int shiftNumber);     // Распечатать контрольную ленту из ЭКЛЗ
int   libPrintDocumentFromECT   (int   KPKNumber);     // Распечатать документ из ЭКЛЗ
int   libGetInfoFromECT_NumberFP(char *data);          // Запрос информации из ЭКЛЗ
int   libGetInfoFromECT_FP(int numDoc, char *data);    // Запрос информации из ЭКЛЗ

int   libPrintReportFromECTByShifts(unsigned char typeReport,
                                    int     startShiftNumber,
                                    int       endShiftNumber); //Распечатать отчет по сменам из ЭКЛЗ

int   libPrintReportFromECTByDate(unsigned char typeReport,
                                  PLDate         startDate,
                                  PLDate           endDate);   // Распечатать отчет по датам из ЭКЛЗ

int   libPrintReportActivizationECT();                         // Распечатать отчет по активизации ЭКЛЗ
int   libPrintReportFromECTByShift(int shiftNumber);           // Распечатать отчет по сменам из ЭКЛЗ

int   libPrintDocsFromECTSDByNumberDoc(int startNumber, int endNumber);
int   libPrintDocsFromECTSDByNumberShift(int startNumber, int endNumber);
int   libPrintDocsFromECTSDByDate(PLDate mpDateStart, PLDate mpDateEnd);
int   libGetInfoFromECT_NumberDoc(int *numDoc);

int   libGetInfoFromECT_ShiftNumber(int *shiftNumber);
int   libBLRPrintControlTapeFromECT();
/**
 * @brief The KSA_INFORMATION enum
 *        Запрос сведений о КСА
 */
enum KSA_INFORMATION
{
    KSA_INF_PLANT_NUM = 1,          // заводской номер КСА
    KSA_INF_HARDWARE_ID,            // идентификатор прошивки
    KSA_INF_UNP,                    // УНП
    KSA_INF_REG_NUM,                // регистрационный номер КСА
    KSA_INF_LAST_BEP_OP_DATE,       // дата и время последней  операции БЭП
    KSA_INF_REG_DATE,               // дата регистрации / перерегистрации
    KSA_INF_CASH_IN_DRAWER,         // сумма наличных в денежном ящике
    KSA_INF_NEXT_DOC_NUM,           // номер следующего документа
    KSA_INF_SHIFT_REG_REREG_NUM,    // номер смены регистрации/ перерегистрации
    KSA_INF_NEXT_X_REP_NUM,         // номер следующего X отчета
    KSA_INF_OPERATION_CNT,          // текущий операционный счетчик
    KSA_INF_TOTAL_CNT,              // нарастающий итог
    KSA_INF_SHIFTS_BEP_AVAILABLE,   // количество свободных смен в БЭП
    KSA_INF_HARDWARE_TYPE = 15,     // тип прошивки
    KSA_INF_PAPER_DESIGN,           // размер бумаги текущего дизайна
    KSA_INF_SKNO_CUR_STATE = 30     // текущее состояние СКНО
};
/**
 * @brief INT_HASH_CODE - int hash code
 */
const size_t INT_HASH_CODE = typeid(int).hash_code();
/**
 * @brief valueParse - парс значения параметра
 * @param value      - ссылка на значение
 * @return
 */
template<typename T>
int valueParse( T &value )
{
    int err = -1;
    // RTTI
    if(typeid(value).hash_code() == INT_HASH_CODE)
    {
        err = pirit_io.parseAnswerN<T>( PIRIT_PARAM_1, value);
    }
    else
    {
         return err;
    }

    return err;
}
/**
 * @brief ksaInfGet - запрос сведений о КСА
 * @param param     - номер параметра
 * @param value     - ссылка на значения параметра
 * @return
 */
template<typename T>
int ksaInfGet( KSA_INFORMATION param, T &value )
{
    int err = -1;

    pirit_io.makeFirstPartPacketToSend( PIRIT_KKT_INFO );
    pirit_io.addByte( static_cast<unsigned char>(param) );
    pirit_io.makeEndPartPacket();

    err = pirit_io.connectSock();
    if( err != 0 )
    {
        printf("%s:: Error while connecting with Pilarus!", __PRETTY_FUNCTION__ );
        return err;
    }
    err = pirit_io.sendData();
    if (err == 0)
    {
        err = pirit_io.readData();
    }
    else
    {
        printf("%s::sendData FAILED!!!", __PRETTY_FUNCTION__ );
    }

    pirit_io.disconnectSock();


    if(err == 0)
    {
        err = valueParse(value);
    }

    return err;
}

#endif







