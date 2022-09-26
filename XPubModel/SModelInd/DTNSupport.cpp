#include <assert.h>
#include "../SSTLdef/STLdef.h"
#include "DTNSupport.h"





CDTNSupport::CDTNSupport()
{
}
CDTNSupport::~CDTNSupport()
{
}

void CDTNSupport::operator =(const CDTNSupport& cSupp)
{
  SetLongDateFormat(cSupp.GetLongDateFormat());
  SetYearWithCentury(cSupp.GetYearWithCentury());
  SetDateSeparator(cSupp.GetDateSeparator());
  SetTimeSeparator(cSupp.GetTimeSeparator());
  SetUseSymbolsFromSystem(cSupp.GetUseSymbolsFromSystem());
  SetDecimalSymbol(cSupp.GetDecimalSymbol());
  SetDigitGroupingSymbol(cSupp.GetDigitGroupingSymbol());
  SetUseCountOfDigitsAfterDecimal(cSupp.GetUseCountOfDigitsAfterDecimal());
  SetCountOfDigitsAfterDecimal(cSupp.GetCountOfDigitsAfterDecimal());
}
