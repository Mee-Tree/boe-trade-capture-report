# Кодирование сообщения TradeCaptureReport
## Идея
Помимо обычных заказов некоторые биржи позволяют посылать заявку на сделку, стороны которой договорились о ней вне биржи.
Такие заявки иногда называют ex-pit или trade report. Биржа проверяет детали сделки и если в них нет ошибок, то сделка
регистрируется.

Существуют разные виды реализации таких заявок:
* каждая сторона отправляет на биржу свою часть информации, тогда одна сторона выступает инициатором, а вторая - реактором и должна
  послать свою часть в ответ на уведомление с биржи о получении части инициатора
* инициатор отправляет полную информацию о сделке, но реактор получает уведомление об этом и должен подтвердить своё участие
* инициатор отправляет полную информацию о сделке, а реактор получает уведомление лишь об успешной регистрации сделки

Бывает, что на обеих сторонах сделки находится один и тот же участник биржи. Это требуется, когда, например, банк регистрирует на
бирже сделку между своими клиентами, выступая на обеих сторонах сделки от их имени.

В протоколе BOE предусмотрен запрос TradeCaptureReport для отправки заявки на экспит.

## Задача
Требуется реализовать кодировщик сообщения TradeCaptureReport (см. [protocol specifications](doc/BATS_Europe_BOE2_Specification.pdf)) имеющий следующий интерфейс:
```cpp
std::vector<unsigned char> create_trade_capture_report_request(
  unsigned seq_no,
  const std::string & trade_report_id,
  double volume,
  double price,
  const std::string & party_id,
  Side side,
  Capacity capacity,
  const std::string & contra_party_id,
  Capacity contra_capacity,
  const std::string & symbol,
  bool deferred_publication
);
```

Отображение полей (аргумент функции -> поле протокола):
* seq_no -> SequenceNumber
* trade_report_id -> TradeReportID
* volume -> LastShares
* price -> LastPx
* party_id -> PartyID [наша сторона]
* side -> Side [наша сторона]
* capacity -> Capacity [наша сторона]
* contra_party_id -> PartyID [другая сторона]
* contra_capacity -> Capacity [другая сторона]
* symbol -> Symbol
* deferred_publication -> TradePublishIndicator (false -> "Publish trade", true -> "Deferred publication")

Важно проставить правильное значение поля PartyRole:
* EnteringFirm для "нашей" стороны
* ContraFirm для "другой" стороны

Требуется предусмотреть возможность относительно лёгкого изменения списка используемых полей сообщения.

Полный список используемых в этом задании опциональных полей сообщения TradeCaptureReport:
* Symbol
* Capacity
* PartyRole
* TradePublishIndicator
