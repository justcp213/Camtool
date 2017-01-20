/*++

Copyright (c) 2005  Future Technology Devices International Ltd.

Module Name:

    FT2232hMpsseI2c.cpp

Abstract:

    FT2232H and FT4232H Hi-Speed Dual Device and FT2232D Dual Device Class Declaration/Definition.

Environment:

    kernel & user mode

Revision History:

    23/03/05    kra     Created.
    03/06/08    kra     Version 1.3, added 3 new functions ie SetGeneralPurposeHighInputOutputPins, GetGeneralPurposeHighInputOutputPinsInputStates,
                        and GetGeneralPurposeHighInputOutputPins to control the output states ie high(true) or low(false)
                        of the 4 general purpose higher input/output ACBUS 0-3/GPIOH 0-3) pins
    01/08/08    kra     Renamed FT2232cMpsseI2c.cpp to FT2232cMpsseI2c.cpp for FT2232H and FT4232H hi-speed devices
    01/08/08    kra     Added new functions for FT2232H and FT4232H hi-speed devices.
    20/08/08    kra     Added new function SetTCKTDITMSPinsCloseState.
    03/09/08    kra     Added critical sections to every public method, to ensure that only one public method in the
                        DLL will be executed at a time, when a process/application has multiple threads running.

--*/

#define WIO_DEFINED

#include "stdafx.h"

#include <io.h>
#include <stdio.h>
#include <stdlib.h>

#include "FT2232hMpsseI2c.h"

FTC_STATUS FT2232hMpsseI2c::CheckWriteDataToExternalDeviceParameters(PWriteControlByteBuffer pWriteControlBuffer, DWORD dwNumControlBytesToWrite,
                                                                     DWORD dwDataWriteTypes, PWriteDataByteBuffer pWriteDataBuffer, 
                                                                     DWORD dwNumDataBytesToWrite, PFTC_PAGE_WRITE_DATA pPageWriteData)
{
  FTC_STATUS Status = FTC_SUCCESS;
  DWORD dwNumDataBytesToBeWritten = 0;

  if (pWriteControlBuffer != NULL)
  {
    if ((dwNumControlBytesToWrite >= MIN_NUM_CONTROL_BYTES) && (dwNumControlBytesToWrite <= MAX_NUM_CONTROL_BYTES))
    {
      if ((dwDataWriteTypes >= NO_WRITE_TYPE) && (dwDataWriteTypes <= PAGE_WRITE_TYPE))
      {
        if ((dwDataWriteTypes == BYTE_WRITE_TYPE) || (dwDataWriteTypes == PAGE_WRITE_TYPE))
        {
          if (pWriteDataBuffer != NULL)
          {
            if ((dwNumDataBytesToWrite >= MIN_NUM_WRITE_DATA_BYTES) && (dwNumDataBytesToWrite <= MAX_NUM_WRITE_DATA_BYTES))
            {
              if (dwDataWriteTypes == PAGE_WRITE_TYPE)
              {
                if (pPageWriteData == NULL)
                  Status = FTC_NULL_PAGE_WRITE_BUFFER_POINTER;
                else
                {
                  dwNumDataBytesToBeWritten = (pPageWriteData->dwNumPages * pPageWriteData->dwNumBytesPerPage);

                  if (dwNumDataBytesToWrite < dwNumDataBytesToBeWritten)
                    Status = FTC_NUMBER_BYTES_TOO_SMALL_PAGE_WRITE;
                }
              }
            }
            else
              Status = FTC_INVALID_NUMBER_DATA_BYTES_WRITE;
          }
          else
            Status = FTC_NULL_WRITE_DATA_BUFFER_POINTER;
        }
      }
      else
        Status = FTC_INVALID_WRITE_TYPE;
    }
    else
      Status = FTC_INVALID_NUMBER_CONTROL_BYTES;
  }
  else
    Status = FTC_NULL_CONTROL_DATA_BUFFER_POINTER;

  return Status;
}

FTC_STATUS FT2232hMpsseI2c::CheckReadDataFromExternalDeviceParameters(PWriteControlByteBuffer pWriteControlBuffer, DWORD dwNumControlBytesToWrite,
                                                                      DWORD dwDataReadTypes, PReadDataByteBuffer pReadDataBuffer,
                                                                      DWORD dwNumDataBytesToRead)
{
  FTC_STATUS Status = FTC_SUCCESS;

  if ((pWriteControlBuffer != NULL) && (pReadDataBuffer != NULL))
  {
    if ((dwNumControlBytesToWrite >= MIN_NUM_CONTROL_BYTES) && (dwNumControlBytesToWrite <= MAX_NUM_CONTROL_BYTES))
    {
      if ((dwNumDataBytesToRead >= MIN_NUM_READ_DATA_BYTES) && (dwNumDataBytesToRead <= MAX_NUM_READ_DATA_BYTES))
      {
        if ((dwDataReadTypes != BYTE_READ_TYPE) && (dwDataReadTypes != BLOCK_READ_TYPE))
          Status = FTC_INVALID_READ_TYPE;
      }
      else
        Status = FTC_INVALID_NUMBER_DATA_BYTES_READ;
    }
    else
      Status = FTC_INVALID_NUMBER_CONTROL_BYTES;
  }
  else
  {
    if (pWriteControlBuffer == NULL)
      Status = FTC_NULL_CONTROL_DATA_BUFFER_POINTER;
    else
      Status = FTC_NULL_READ_DATA_BUFFER_POINTER;
  }

  return Status;
}

FTC_STATUS FT2232hMpsseI2c::SetTCKTDITMSPinsCloseState(FTC_HANDLE ftHandle, PFTC_CLOSE_FINAL_STATE_PINS pCloseFinalStatePinsData)
{
  FTC_STATUS Status = FTC_SUCCESS;

  if ((pCloseFinalStatePinsData->bTCKPinState != FALSE) ||  
      (pCloseFinalStatePinsData->bTDIPinState != FALSE) ||
      (pCloseFinalStatePinsData->bTMSPinState != FALSE)) {
    FTC_AddByteToOutputBuffer(SET_LOW_BYTE_DATA_BITS_CMD, true);

    if (pCloseFinalStatePinsData->bTCKPinState != FALSE) {
      if (pCloseFinalStatePinsData->bTCKPinActiveState != FALSE)
        dwSavedLowPinsValue = (dwSavedLowPinsValue | '\x01'); // Set TCK pin high
      else
        dwSavedLowPinsValue = (dwSavedLowPinsValue & '\xFE'); // Set TCK pin low

      dwSavedLowPinsDirection = (dwSavedLowPinsDirection | '\x01'); // Ensure TCK pin is set to output
    }

    if (pCloseFinalStatePinsData->bTDIPinState != FALSE) {
      if (pCloseFinalStatePinsData->bTDIPinActiveState != FALSE)
        dwSavedLowPinsValue = (dwSavedLowPinsValue | '\x02'); // Set TDI pin high
      else
        dwSavedLowPinsValue = (dwSavedLowPinsValue & '\xFD'); // Set TDI pin low

      dwSavedLowPinsDirection = (dwSavedLowPinsDirection | '\x02'); // Ensure TDI pin is set to output
    }

    if (pCloseFinalStatePinsData->bTMSPinState != FALSE) {
      if (pCloseFinalStatePinsData->bTMSPinActiveState != FALSE)
        dwSavedLowPinsValue = (dwSavedLowPinsValue | '\x08'); // Set TMS pin high
      else
        dwSavedLowPinsValue = (dwSavedLowPinsValue & '\xF7'); // Set TMS pin low

      dwSavedLowPinsDirection = (dwSavedLowPinsDirection | '\x08'); // Ensure TMS pin is set to output
    }
    
    FTC_AddByteToOutputBuffer(dwSavedLowPinsValue, false);
    FTC_AddByteToOutputBuffer(dwSavedLowPinsDirection, false);

    Status = FTC_SendBytesToDevice(ftHandle);
  }

  return Status;
}

FTC_STATUS FT2232hMpsseI2c::InitDevice(FTC_HANDLE ftHandle, DWORD dwClockDivisor)
{
  FTC_STATUS Status = FTC_SUCCESS;

  if ((dwClockDivisor >= MIN_CLOCK_DIVISOR) && (dwClockDivisor <= MAX_CLOCK_DIVISOR))
  {
      Status = FTC_ResetUSBDevicePurgeUSBInputBuffer(ftHandle);

      if (Status == FTC_SUCCESS)
        Status = FTC_SetDeviceUSBBufferSizes(ftHandle, USB_INPUT_BUFFER_SIZE, USB_OUTPUT_BUFFER_SIZE);

      if (Status == FTC_SUCCESS)
        Status = FTC_SetDeviceSpecialCharacters(ftHandle, false, FT_EVENT_VALUE, false, FT_ERROR_VALUE);

      if (Status == FTC_SUCCESS)
        Status = FTC_SetReadWriteDeviceTimeouts(ftHandle, DEVICE_READ_TIMEOUT_INFINITE, DEVICE_WRITE_TIMEOUT);

      if (Status == FTC_SUCCESS)
        Status = FTC_SetDeviceLatencyTimer(ftHandle, DEVICE_LATENCY_TIMER_VALUE);

      if (Status == FTC_SUCCESS)
        Status = FTC_ResetMPSSEInterface(ftHandle);

      if (Status == FTC_SUCCESS)
        Status = FTC_EnableMPSSEInterface(ftHandle);

      if (Status == FTC_SUCCESS)
        Status = FTC_SynchronizeMPSSEInterface(ftHandle);

      //if (Status == FTC_SUCCESS)
      //  Status = FTC_ResetUSBDevicePurgeUSBInputBuffer(ftHandle);

      if (Status == FTC_SUCCESS)
        Sleep(50); // wait for all the USB stuff to complete

      if (Status == FTC_SUCCESS)
        Status = InitDataInOutClockFrequency(ftHandle, dwClockDivisor);

      if (Status == FTC_SUCCESS)
        Sleep(20);

      if (Status == FTC_SUCCESS)
        Status = FTC_SetDeviceLoopbackState(ftHandle, false);

      if (Status == FTC_SUCCESS)
        Status = EmptyDeviceInputBuffer(ftHandle);

      if (Status == FTC_SUCCESS)
        Sleep(30);
  }
  else
    Status = FTC_INVALID_CLOCK_DIVISOR;

  return Status;
}

FTC_STATUS FT2232hMpsseI2c::SetDataInOutClockFrequency(FTC_HANDLE ftHandle, DWORD dwClockDivisor)
{
  FTC_STATUS Status = FTC_SUCCESS;
  
  // set clk divisor
  FTC_AddByteToOutputBuffer(SET_CLOCK_FREQUENCY_CMD, true);
  FTC_AddByteToOutputBuffer((dwClockDivisor & '\xFF'), false);
  FTC_AddByteToOutputBuffer((dwClockDivisor >> 8), false);

  Status = FTC_SendBytesToDevice(ftHandle);

  //Sleep(0); // give up timeslice

  return Status;
}

FTC_STATUS FT2232hMpsseI2c::InitDataInOutClockFrequency(FTC_HANDLE ftHandle, DWORD dwClockDivisor)
{
  FTC_STATUS Status = FTC_SUCCESS;

  // set SK,DO,CS as out
  FTC_AddByteToOutputBuffer(SET_LOW_BYTE_DATA_BITS_CMD, true);

  // SDA SCL WP high
  dwSavedLowPinsValue = '\x13';
  FTC_AddByteToOutputBuffer(dwSavedLowPinsValue, false);

  // inputs on GPIO12-14
  FTC_AddByteToOutputBuffer(dwSavedLowPinsValue, false);

  // outputs on GPIO21-24
  FTC_AddByteToOutputBuffer(SET_HIGH_BYTE_DATA_BITS_CMD, false);
  FTC_AddByteToOutputBuffer('\x0F', false);
  FTC_AddByteToOutputBuffer('\x0F', false);

  Status = FTC_SendBytesToDevice(ftHandle);
  
  if (Status == FTC_SUCCESS)
  {
    //Sleep(0); // give up timeslice

    SetDataInOutClockFrequency(ftHandle, dwClockDivisor);
  }

  return Status;
}

FTC_STATUS FT2232hMpsseI2c::EmptyDeviceInputBuffer(FTC_HANDLE ftHandle)
{
  FTC_STATUS Status = FTC_SUCCESS;
  DWORD dwNumBytesDeviceInputBuffer = 0;
  DWORD dwNumBytesRead = 0;

  // Get the number of bytes in the device input buffer
  Status = (FTC_STATUS)FT_GetQueueStatus((FT_HANDLE)ftHandle, &dwNumBytesDeviceInputBuffer);

  if ((Status == FTC_SUCCESS) && (dwNumBytesDeviceInputBuffer > 0))
     FTC_ReadBytesFromDevice(ftHandle, &InputBuffer, dwNumBytesDeviceInputBuffer, &dwNumBytesRead);

  return Status;
}

FTC_STATUS FT2232hMpsseI2c::SetGeneralPurposeHighInputOutputPins(FTC_HANDLE ftHandle, DWORD dwHighPinsDirection, DWORD dwHighPinsValue)
{
  FTC_STATUS Status = FTC_SUCCESS;

  // output on the general purpose I/O high pins 1-4
  FTC_AddByteToOutputBuffer(SET_HIGH_BYTE_DATA_BITS_CMD, true);

  dwHighPinsValue = (dwHighPinsValue & '\x0F');
  FTC_AddByteToOutputBuffer(dwHighPinsValue, false);

  dwHighPinsDirection = (dwHighPinsDirection & '\x0F');
  FTC_AddByteToOutputBuffer(dwHighPinsDirection, false);

  Status = FTC_SendBytesToDevice(ftHandle);

  if (Status == FTC_SUCCESS) {
    dwSavedHighPinsDirection = dwHighPinsDirection;
    dwSavedHighPinsValue = dwHighPinsValue;
  }

  return Status;
}

FTC_STATUS FT2232hMpsseI2c::SetHiSpeedDeviceGeneralPurposeLowInputOutputPins(FTC_HANDLE ftHandle, PFTC_INPUT_OUTPUT_PINS pLowInputOutputPinsData)
{
  DWORD dwLowPinsDirection = 0;
  DWORD dwLowPinsValue = 0;

  // Only the 3 general purpose lower input/output pins (GPIOL2 – GPIOL4) are available, the GPIOL1 pin
  // cannot be used, it is reserved for I2C write protect and therefore is configured as an output
  dwLowPinsDirection = (dwLowPinsDirection | '\x01'); // Set Write Protect pin(GPIOL1) high ie output
  if (pLowInputOutputPinsData->bPin2InputOutputState != FALSE)
    dwLowPinsDirection = (dwLowPinsDirection | '\x02');
  if (pLowInputOutputPinsData->bPin3InputOutputState != FALSE)
    dwLowPinsDirection = (dwLowPinsDirection | '\x04');
  if (pLowInputOutputPinsData->bPin4InputOutputState != FALSE)
    dwLowPinsDirection = (dwLowPinsDirection | '\x08');

  dwLowPinsValue = (dwLowPinsValue | '\x01'); // Set Write Protect pin(GPIOL1) high ie enable write protect (default)
  if (pLowInputOutputPinsData->bPin2LowHighState != FALSE)
    dwLowPinsValue = (dwLowPinsValue | '\x02');
  if (pLowInputOutputPinsData->bPin3LowHighState != FALSE)
    dwLowPinsValue = (dwLowPinsValue | '\x04');
  if (pLowInputOutputPinsData->bPin4LowHighState != FALSE)
    dwLowPinsValue = (dwLowPinsValue | '\x08');

  // output on the general purpose I/O low pins 1-4
  FTC_AddByteToOutputBuffer(SET_LOW_BYTE_DATA_BITS_CMD, TRUE);

  // shift left by 4 bits ie move general purpose I/O low pins 1-4 from bits 0-3 to bits 4-7
  dwLowPinsValue = ((dwLowPinsValue & '\x0F') << 4);

  dwSavedLowPinsValue = (dwSavedLowPinsValue & '\x0F');
  dwSavedLowPinsValue = (dwSavedLowPinsValue | dwLowPinsValue);
  FTC_AddByteToOutputBuffer(dwSavedLowPinsValue, FALSE);

  // shift left by 4 bits ie move general purpose I/O low pins 1-4 from bits 0-3 to bits 4-7
  dwLowPinsDirection = ((dwLowPinsDirection & '\x0F') << 4);

  dwSavedLowPinsDirection = (dwSavedLowPinsDirection & '\x0F');
  dwSavedLowPinsDirection = (dwSavedLowPinsDirection | dwLowPinsDirection); 
  FTC_AddByteToOutputBuffer(dwSavedLowPinsDirection, FALSE);

  return FTC_SendBytesToDevice(ftHandle);
}

FTC_STATUS FT2232hMpsseI2c::SetHiSpeedDeviceGeneralPurposeHighInputOutputPins(FTC_HANDLE ftHandle, PFTH_INPUT_OUTPUT_PINS pHighInputOutputPinsData)
{
  FTC_STATUS Status = FTC_SUCCESS;
  BOOL bHiSpeedFT2232HTDeviceype = FALSE;
  DWORD dwHighPinsDirection = 0;
  DWORD dwHighPinsValue = 0;

  if ((Status = FTC_IsDeviceHiSpeedFT2232HType(ftHandle, &bHiSpeedFT2232HTDeviceype)) == FTC_SUCCESS)
  {
    // If the device is a FT2232H hi-speed device
    if (bHiSpeedFT2232HTDeviceype == TRUE)
    {
      if (pHighInputOutputPinsData->bPin1InputOutputState != FALSE)
        dwHighPinsDirection = (dwHighPinsDirection | '\x01');
      if (pHighInputOutputPinsData->bPin2InputOutputState != FALSE)
        dwHighPinsDirection = (dwHighPinsDirection | '\x02');
      if (pHighInputOutputPinsData->bPin3InputOutputState != FALSE)
        dwHighPinsDirection = (dwHighPinsDirection | '\x04');
      if (pHighInputOutputPinsData->bPin4InputOutputState != FALSE)
        dwHighPinsDirection = (dwHighPinsDirection | '\x08');
      if (pHighInputOutputPinsData->bPin5InputOutputState != FALSE)
        dwHighPinsDirection = (dwHighPinsDirection | '\x10');
      if (pHighInputOutputPinsData->bPin6InputOutputState != FALSE)
        dwHighPinsDirection = (dwHighPinsDirection | '\x20');
      if (pHighInputOutputPinsData->bPin7InputOutputState != FALSE)
        dwHighPinsDirection = (dwHighPinsDirection | '\x40');
      if (pHighInputOutputPinsData->bPin8InputOutputState != FALSE)
        dwHighPinsDirection = (dwHighPinsDirection | '\x80');

      if (pHighInputOutputPinsData->bPin1LowHighState != FALSE)
        dwHighPinsValue = (dwHighPinsValue | '\x01');
      if (pHighInputOutputPinsData->bPin2LowHighState != FALSE)
        dwHighPinsValue = (dwHighPinsValue | '\x02');
      if (pHighInputOutputPinsData->bPin3LowHighState != FALSE)
        dwHighPinsValue = (dwHighPinsValue | '\x04');
      if (pHighInputOutputPinsData->bPin4LowHighState != FALSE)
        dwHighPinsValue = (dwHighPinsValue | '\x08');
      if (pHighInputOutputPinsData->bPin5LowHighState != FALSE)
        dwHighPinsValue = (dwHighPinsValue | '\x10');
      if (pHighInputOutputPinsData->bPin6LowHighState != FALSE)
        dwHighPinsValue = (dwHighPinsValue | '\x20');
      if (pHighInputOutputPinsData->bPin7LowHighState != FALSE)
        dwHighPinsValue = (dwHighPinsValue | '\x40');
      if (pHighInputOutputPinsData->bPin8LowHighState != FALSE)
        dwHighPinsValue = (dwHighPinsValue | '\x80');

      // output on the general purpose I/O high pins 1-8
      FTC_AddByteToOutputBuffer(SET_HIGH_BYTE_DATA_BITS_CMD, TRUE);

      dwHighPinsValue = (dwHighPinsValue & '\xFF');
      FTC_AddByteToOutputBuffer(dwHighPinsValue, FALSE);

      dwHighPinsDirection = (dwHighPinsDirection & '\xFF');
      FTC_AddByteToOutputBuffer(dwHighPinsDirection, FALSE);

      Status = FTC_SendBytesToDevice(ftHandle);
    }
  }

  return Status;
}

void FT2232hMpsseI2c::GetGeneralPurposeHighInputOutputPinsInputStates(DWORD dwInputStatesReturnedValue, PFTC_LOW_HIGH_PINS pHighPinsInputData)
{
  if ((dwInputStatesReturnedValue & PIN1_HIGH_VALUE) == PIN1_HIGH_VALUE)
    pHighPinsInputData->bPin1LowHighState = true;

  if ((dwInputStatesReturnedValue & PIN2_HIGH_VALUE) == PIN2_HIGH_VALUE)
    pHighPinsInputData->bPin2LowHighState = true;

  if ((dwInputStatesReturnedValue & PIN3_HIGH_VALUE) == PIN3_HIGH_VALUE)
    pHighPinsInputData->bPin3LowHighState = true;

  if ((dwInputStatesReturnedValue & PIN4_HIGH_VALUE) == PIN4_HIGH_VALUE)
    pHighPinsInputData->bPin4LowHighState = true;
}

FTC_STATUS FT2232hMpsseI2c::GetGeneralPurposeHighInputOutputPins(FTC_HANDLE ftHandle, PFTC_LOW_HIGH_PINS pHighPinsInputData)
{
  FTC_STATUS Status = FTC_SUCCESS;
  InputByteBuffer InputBuffer;
  DWORD dwNumBytesRead = 0;
  DWORD dwNumBytesDeviceInputBuffer;

  pHighPinsInputData->bPin1LowHighState = false;
  pHighPinsInputData->bPin2LowHighState = false;
  pHighPinsInputData->bPin3LowHighState = false;
  pHighPinsInputData->bPin4LowHighState = false;

  // Put in this small delay incase the application programmer does a get GPIOs immediately after a set GPIOs
  Sleep(5);

  // Get the number of bytes in the device input buffer
  Status = FT_GetQueueStatus((FT_HANDLE)ftHandle, &dwNumBytesDeviceInputBuffer);

  if (Status == FTC_SUCCESS)
  {
    if (dwNumBytesDeviceInputBuffer > 0)
      Status = FTC_ReadBytesFromDevice(ftHandle, &InputBuffer, dwNumBytesDeviceInputBuffer, &dwNumBytesRead);

    if (Status == FTC_SUCCESS)
    {
      // get the states of the general purpose I/O high pins 1-4
      FTC_AddByteToOutputBuffer(GET_HIGH_BYTE_DATA_BITS_CMD, true);
      FTC_AddByteToOutputBuffer(SEND_ANSWER_BACK_IMMEDIATELY_CMD, false);
      Status = FTC_SendBytesToDevice(ftHandle);

      if (Status == FTC_SUCCESS)
      {
        Status = FTC_GetNumberBytesFromDeviceInputBuffer(ftHandle, &dwNumBytesDeviceInputBuffer);

        if (Status == FTC_SUCCESS)
        {
          Status = FTC_ReadBytesFromDevice(ftHandle, &InputBuffer, dwNumBytesDeviceInputBuffer, &dwNumBytesRead);

          if (Status == FTC_SUCCESS)
            GetGeneralPurposeHighInputOutputPinsInputStates(InputBuffer[0], pHighPinsInputData);
        }
      }
    }
  }

  return Status;
}

void FT2232hMpsseI2c::GetHiSpeedDeviceGeneralPurposeLowInputOutputPinsInputStates(DWORD dwInputStatesReturnedValue, PFTC_LOW_HIGH_PINS pLowPinsInputData)
{
  // Only the 3 general purpose lower input/output pins (GPIOL2 – GPIOL4) are available, the GPIOL1 pin
  // cannot be used, it is reserved for I2C write protect and therefore is configured as an output

  if ((dwInputStatesReturnedValue & PIN2_HIGH_VALUE) == PIN2_HIGH_VALUE)
    pLowPinsInputData->bPin2LowHighState = true;

  if ((dwInputStatesReturnedValue & PIN3_HIGH_VALUE) == PIN3_HIGH_VALUE)
    pLowPinsInputData->bPin3LowHighState = true;

  if ((dwInputStatesReturnedValue & PIN4_HIGH_VALUE) == PIN4_HIGH_VALUE)
    pLowPinsInputData->bPin4LowHighState = true;
}

FTC_STATUS FT2232hMpsseI2c::GetHiSpeedDeviceGeneralPurposeLowInputOutputPins(FTC_HANDLE ftHandle, PFTC_LOW_HIGH_PINS pLowPinsInputData)
{
  FTC_STATUS Status = FTC_SUCCESS;
  InputByteBuffer InputBuffer;
  DWORD dwNumBytesRead = 0;
  DWORD dwNumBytesDeviceInputBuffer;

  pLowPinsInputData->bPin1LowHighState = FALSE;
  pLowPinsInputData->bPin2LowHighState = FALSE;
  pLowPinsInputData->bPin3LowHighState = FALSE;
  pLowPinsInputData->bPin4LowHighState = FALSE;

  // Get the number of bytes in the device input buffer
  if ((Status = FT_GetQueueStatus((FT_HANDLE)ftHandle, &dwNumBytesDeviceInputBuffer)) == FTC_SUCCESS)
  {
    if (dwNumBytesDeviceInputBuffer > 0)
      Status = FTC_ReadBytesFromDevice(ftHandle, &InputBuffer, dwNumBytesDeviceInputBuffer, &dwNumBytesRead);

    if (Status == FTC_SUCCESS)
    {
      // get the states of the general purpose I/O low pins 1-4
      FTC_AddByteToOutputBuffer(GET_LOW_BYTE_DATA_BITS_CMD, TRUE);
      FTC_AddByteToOutputBuffer(SEND_ANSWER_BACK_IMMEDIATELY_CMD, FALSE);
      Status = FTC_SendBytesToDevice(ftHandle);

      if (Status == FTC_SUCCESS)
      {
        if ((Status = FTC_GetNumberBytesFromDeviceInputBuffer(ftHandle, &dwNumBytesDeviceInputBuffer)) == FTC_SUCCESS)
        {
          if ((Status = FTC_ReadBytesFromDevice(ftHandle, &InputBuffer, dwNumBytesDeviceInputBuffer, &dwNumBytesRead)) == FTC_SUCCESS)
            // shift right by 4 bits ie move general purpose I/O low pins 1-4 from bits 4-7 to bits 0-3
            GetHiSpeedDeviceGeneralPurposeLowInputOutputPinsInputStates((InputBuffer[0] >> 4), pLowPinsInputData);
        }
      }
    }
  }

  return Status;
}

void  FT2232hMpsseI2c::GetHiSpeedDeviceGeneralPurposeHighInputOutputPinsInputStates(DWORD dwInputStatesReturnedValue, PFTH_LOW_HIGH_PINS pPinsInputData)
{
  if ((dwInputStatesReturnedValue & PIN1_HIGH_VALUE) == PIN1_HIGH_VALUE)
    pPinsInputData->bPin1LowHighState = TRUE;

  if ((dwInputStatesReturnedValue & PIN2_HIGH_VALUE) == PIN2_HIGH_VALUE)
    pPinsInputData->bPin2LowHighState = TRUE;

  if ((dwInputStatesReturnedValue & PIN3_HIGH_VALUE) == PIN3_HIGH_VALUE)
    pPinsInputData->bPin3LowHighState = TRUE;

  if ((dwInputStatesReturnedValue & PIN4_HIGH_VALUE) == PIN4_HIGH_VALUE)
    pPinsInputData->bPin4LowHighState = TRUE;

  if ((dwInputStatesReturnedValue & PIN5_HIGH_VALUE) == PIN5_HIGH_VALUE)
    pPinsInputData->bPin5LowHighState = TRUE;

  if ((dwInputStatesReturnedValue & PIN6_HIGH_VALUE) == PIN6_HIGH_VALUE)
    pPinsInputData->bPin6LowHighState = TRUE;

  if ((dwInputStatesReturnedValue & PIN7_HIGH_VALUE) == PIN7_HIGH_VALUE)
    pPinsInputData->bPin7LowHighState = TRUE;

  if ((dwInputStatesReturnedValue & PIN8_HIGH_VALUE) == PIN8_HIGH_VALUE)
    pPinsInputData->bPin8LowHighState = TRUE;
}

FTC_STATUS FT2232hMpsseI2c::GetHiSpeedDeviceGeneralPurposeHighInputOutputPins(FTC_HANDLE ftHandle, PFTH_LOW_HIGH_PINS pHighPinsInputData)
{
  FTC_STATUS Status = FTC_SUCCESS;
  InputByteBuffer InputBuffer;
  DWORD dwNumBytesRead = 0;
  DWORD dwNumBytesDeviceInputBuffer;
  BOOL bHiSpeedFT2232HTDeviceype = FALSE;

  pHighPinsInputData->bPin1LowHighState = FALSE;
  pHighPinsInputData->bPin2LowHighState = FALSE;
  pHighPinsInputData->bPin3LowHighState = FALSE;
  pHighPinsInputData->bPin4LowHighState = FALSE;
  pHighPinsInputData->bPin5LowHighState = FALSE;
  pHighPinsInputData->bPin6LowHighState = FALSE;
  pHighPinsInputData->bPin7LowHighState = FALSE;
  pHighPinsInputData->bPin8LowHighState = FALSE;

  // Put in this small delay incase the application programmer does a get GPIOs immediately after a set GPIOs
  Sleep(5);

  if ((Status = FTC_IsDeviceHiSpeedFT2232HType(ftHandle, &bHiSpeedFT2232HTDeviceype)) == FTC_SUCCESS)
  {
    // If the device is a FT2232H hi-speed device
    if (bHiSpeedFT2232HTDeviceype == TRUE)
    {
      // Get the number of bytes in the device input buffer
      if ((Status = FT_GetQueueStatus((FT_HANDLE)ftHandle, &dwNumBytesDeviceInputBuffer)) == FTC_SUCCESS)
      {
        if (dwNumBytesDeviceInputBuffer > 0)
          Status = FTC_ReadBytesFromDevice(ftHandle, &InputBuffer, dwNumBytesDeviceInputBuffer, &dwNumBytesRead);

        if (Status == FTC_SUCCESS)
        {
          // get the states of the general purpose I/O high pins 1-4
          FTC_AddByteToOutputBuffer(GET_HIGH_BYTE_DATA_BITS_CMD, TRUE);
          FTC_AddByteToOutputBuffer(SEND_ANSWER_BACK_IMMEDIATELY_CMD, FALSE);
          Status = FTC_SendBytesToDevice(ftHandle);

          if (Status == FTC_SUCCESS)
          {
            if ((Status = FTC_GetNumberBytesFromDeviceInputBuffer(ftHandle, &dwNumBytesDeviceInputBuffer)) == FTC_SUCCESS)
            {
              if ((Status = FTC_ReadBytesFromDevice(ftHandle, &InputBuffer, dwNumBytesDeviceInputBuffer, &dwNumBytesRead)) == FTC_SUCCESS)
                GetHiSpeedDeviceGeneralPurposeHighInputOutputPinsInputStates(InputBuffer[0], pHighPinsInputData);
            }
          }
        }
      }
    }
  }

  return Status;
}

void  FT2232hMpsseI2c::SetI2CStartCondition(FTC_HANDLE ftHandle, BOOL bEnableDisableWriteProtect)
{
  DWORD dwLoopCntr = 0;
  BOOL bHiSpeedTypeDevice = FALSE;

  if (bEnableDisableWriteProtect == false)
    dwSavedLowPinsValue = (dwSavedLowPinsValue & '\xEF'); // Set Write Protect pin low ie disable write protect
  else
    dwSavedLowPinsValue = (dwSavedLowPinsValue | '\x10'); // Set Write Protect pin high ie enable write protect

  FTC_AddByteToOutputBuffer(SET_LOW_BYTE_DATA_BITS_CMD, false);
  dwSavedLowPinsValue = (dwSavedLowPinsValue | '\x03'); //SCL SDA high
  FTC_AddByteToOutputBuffer(dwSavedLowPinsValue, false);

  dwSavedLowPinsDirection = (dwSavedLowPinsDirection & '\xE0');
  dwSavedLowPinsDirection = (dwSavedLowPinsDirection | '\x13'); // set SCL,SDA,WP as out

  FTC_AddByteToOutputBuffer(dwSavedLowPinsDirection, false); // set SCL,SDA,WP as out

  if (FTC_IsDeviceHiSpeedType(ftHandle) == TRUE) {
    // For hi-speed device ie FT2232H or FT4232H, ensure the minimum period of the start hold time ie 600ns is achieved
    for (dwLoopCntr = 0; dwLoopCntr < START_HI_SPEED_DEVICE_HOLD_SETUP_TIME; dwLoopCntr++)
    {
      FTC_AddByteToOutputBuffer(SET_LOW_BYTE_DATA_BITS_CMD, false);
      FTC_AddByteToOutputBuffer(dwSavedLowPinsValue, false);
      FTC_AddByteToOutputBuffer(dwSavedLowPinsDirection, false); // set SCL,SDA,WP as out
    }
  }

  FTC_AddByteToOutputBuffer(SET_LOW_BYTE_DATA_BITS_CMD, false);
  dwSavedLowPinsValue = (dwSavedLowPinsValue & '\xFD'); //SCL high SDA low
  FTC_AddByteToOutputBuffer(dwSavedLowPinsValue, false);

  FTC_AddByteToOutputBuffer(dwSavedLowPinsDirection, false); // set SCL,SDA,WP as out

  if (FTC_IsDeviceHiSpeedType(ftHandle) == TRUE) {
    // For hi-speed device ie FT2232H or FT4232H, ensure the minimum period of the start setup time ie 600ns is achieved
    for (dwLoopCntr = 0; dwLoopCntr < START_HI_SPEED_DEVICE_HOLD_SETUP_TIME; dwLoopCntr++)
    {
      FTC_AddByteToOutputBuffer(SET_LOW_BYTE_DATA_BITS_CMD, false);
      FTC_AddByteToOutputBuffer(dwSavedLowPinsValue, false);
      FTC_AddByteToOutputBuffer(dwSavedLowPinsDirection, false); // set SCL,SDA,WP as out
    }
  }

  if ((GetCommunicationsMode(ftHandle)&STANDARD_MODE) == STANDARD_MODE)
  {
    // For Standard mode, repeat SDA low command to increase the Start Condition Hold Time to 4 microseconds
    for (dwLoopCntr = 0; dwLoopCntr < 3; dwLoopCntr++)
    {
      FTC_AddByteToOutputBuffer(SET_LOW_BYTE_DATA_BITS_CMD, false);
      dwSavedLowPinsValue = (dwSavedLowPinsValue & '\xFD'); //SCL high SDA low
      FTC_AddByteToOutputBuffer(dwSavedLowPinsValue, false);

      FTC_AddByteToOutputBuffer(dwSavedLowPinsDirection, false); // set SCL,SDA,WP as out
    }
  }

  FTC_AddByteToOutputBuffer(SET_LOW_BYTE_DATA_BITS_CMD, false);
  dwSavedLowPinsValue = (dwSavedLowPinsValue & '\xFC'); //SCL low SDA low
  FTC_AddByteToOutputBuffer(dwSavedLowPinsValue, false);

  FTC_AddByteToOutputBuffer(dwSavedLowPinsDirection, false); // set SCL,SDA,WP as out
}

FTC_STATUS FT2232hMpsseI2c::SetI2CStopCondition(FTC_HANDLE ftHandle)
{
  FTC_STATUS Status = FTC_SUCCESS;
  DWORD dwLoopCntr = 0;

  FTC_ClearOutputBuffer();

  FTC_AddByteToOutputBuffer(SET_LOW_BYTE_DATA_BITS_CMD, false);
  dwSavedLowPinsValue = (dwSavedLowPinsValue | '\x01'); //SCL high SDA low
  dwSavedLowPinsValue = (dwSavedLowPinsValue & '\xFD'); //SCL high SDA low
  FTC_AddByteToOutputBuffer(dwSavedLowPinsValue, false);

  dwSavedLowPinsDirection = (dwSavedLowPinsDirection & '\xE0');
  dwSavedLowPinsDirection = (dwSavedLowPinsDirection | '\x13'); // set SCL,SDA,WP as out
  FTC_AddByteToOutputBuffer(dwSavedLowPinsDirection, false); // set SCL,SDA,WP as out

  if (FTC_IsDeviceHiSpeedType(ftHandle) == TRUE) {
    // For hi-speed device ie FT2232H or FT4232H, ensure the minimum period of the stop setup time ie 600ns is achieved
    for (dwLoopCntr = 0; dwLoopCntr < STOP_HI_SPEED_DEVICE_SETUP_TIME; dwLoopCntr++)
    {
      FTC_AddByteToOutputBuffer(SET_LOW_BYTE_DATA_BITS_CMD, false);
      FTC_AddByteToOutputBuffer(dwSavedLowPinsValue, false);
      FTC_AddByteToOutputBuffer(dwSavedLowPinsDirection, false); // set SCL,SDA,WP as out
    }
  }

  if ((GetCommunicationsMode(ftHandle)&STANDARD_MODE) == STANDARD_MODE)
  {
    // For Standard mode, repeat SDA low command to increase the Stop Condition Setup Time to 4 microseconds
    for (dwLoopCntr = 0; dwLoopCntr < 3; dwLoopCntr++)
    {
      FTC_AddByteToOutputBuffer(SET_LOW_BYTE_DATA_BITS_CMD, false);
      FTC_AddByteToOutputBuffer(dwSavedLowPinsValue, false);  //SCL high SDA low

      FTC_AddByteToOutputBuffer(dwSavedLowPinsDirection, false); // set SCL,SDA,WP as out
    }
  }

  FTC_AddByteToOutputBuffer(SET_LOW_BYTE_DATA_BITS_CMD, false);
  dwSavedLowPinsValue = (dwSavedLowPinsValue | '\x02'); //SCL high SDA high
  FTC_AddByteToOutputBuffer(dwSavedLowPinsValue, false);

  FTC_AddByteToOutputBuffer(dwSavedLowPinsDirection, false); // set SCL,SDA,WP as out

  if (FTC_IsDeviceHiSpeedType(ftHandle) == TRUE) {
    // For hi-speed device ie FT2232H or FT4232H, ensure the bus is free for a time, before a new transmission can start
    for (dwLoopCntr = 0; dwLoopCntr < STOP_HI_SPEED_DEVICE_SETUP_TIME; dwLoopCntr++)
    {
      FTC_AddByteToOutputBuffer(SET_LOW_BYTE_DATA_BITS_CMD, false);
      FTC_AddByteToOutputBuffer(dwSavedLowPinsValue, false);
      FTC_AddByteToOutputBuffer(dwSavedLowPinsDirection, false); // set SCL,SDA,WP as out
    }
  }

  // tristate SDA SCL
  FTC_AddByteToOutputBuffer(SET_LOW_BYTE_DATA_BITS_CMD, false);
  FTC_AddByteToOutputBuffer(dwSavedLowPinsValue, false);

  dwSavedLowPinsDirection = (dwSavedLowPinsDirection & '\xE0');
  dwSavedLowPinsDirection = (dwSavedLowPinsDirection | '\x10'); // set SCL,SDA as input,WP as out

  FTC_AddByteToOutputBuffer(dwSavedLowPinsDirection, false); // set SCL,SDA as input,WP as out

  Status = FTC_SendBytesToDevice(ftHandle); // send off the command

  Sleep(0); // give up timeslice

  return Status;
}

void FT2232hMpsseI2c::SetI2CWriteData(FTC_HANDLE ftHandle, DWORD dwNumBitsToWrite, PI2CWriteByteBuffer pI2CWriteBuffer)
{
  DWORD dwDataBufferIndex = 0;
  DWORD dwNumDataBytes = 0;
  DWORD dwNumRemainingDataBits = 0;

  dwNumDataBytes = (dwNumBitsToWrite / 8);

  if (dwNumDataBytes > 0)
  {
    // Number of whole bytes
    dwNumDataBytes = (dwNumDataBytes - 1);

	if ((GetCommunicationsMode(ftHandle)&STRETCH_DATA_MODE) == STRETCH_DATA_MODE)
	{
		//change data after clock edge
		do
		{
			ClockByte((*pI2CWriteBuffer)[dwDataBufferIndex]);
			dwDataBufferIndex = (dwDataBufferIndex + 1);
		}
		while (dwDataBufferIndex < (dwNumDataBytes + 1));
	}
	else
	{
    // clk data bytes out on -ve clk MSB first
    FTC_AddByteToOutputBuffer(CLK_DATA_BYTES_OUT_ON_NEG_CLK_MSB_FIRST_CMD, false);
    FTC_AddByteToOutputBuffer((dwNumDataBytes & '\xFF'), false);
    FTC_AddByteToOutputBuffer(((dwNumDataBytes / 256) & '\xFF'), false);

    // now add the data bytes to go out
    do
    {
      FTC_AddByteToOutputBuffer((*pI2CWriteBuffer)[dwDataBufferIndex], false);
      dwDataBufferIndex = (dwDataBufferIndex + 1);
    }
    while (dwDataBufferIndex < (dwNumDataBytes + 1));
	}
  }

  dwNumRemainingDataBits = (dwNumBitsToWrite % 8);

  if (dwNumRemainingDataBits > 0)
  {
    // adjust for bit count of 1 less than no of bits
    dwNumRemainingDataBits = (dwNumRemainingDataBits - 1);

    // clk data bits out on -ve clk MSB first
    FTC_AddByteToOutputBuffer(CLK_DATA_BITS_OUT_ON_NEG_CLK_MSB_FIRST_CMD, false);
    FTC_AddByteToOutputBuffer((dwNumRemainingDataBits & '\xFF'), false);
    FTC_AddByteToOutputBuffer((*pI2CWriteBuffer)[dwDataBufferIndex], false);
  }
}

void       FT2232hMpsseI2c::SetI2CReadData(DWORD dwNumBitsToRead)
{
  DWORD dwModNumBitsToRead = 0;
  DWORD dwNumDataBytes = 0;
  DWORD dwNumRemainingDataBits = 0;

  // adjust count value
  dwModNumBitsToRead = (dwNumBitsToRead -1);

  if (dwModNumBitsToRead == 0)
  {
    // clk data bits in -ve clk MSB
    FTC_AddByteToOutputBuffer(CLK_DATA_BITS_IN_ON_NEG_CLK_MSB_FIRST_CMD, false);
    FTC_AddByteToOutputBuffer(0, false);
  }
  else
  {
    //dwNumDataBytes = (dwModNumBitsToRead / 8);
    dwNumDataBytes = (dwNumBitsToRead / 8);

    if (dwNumDataBytes > 0)
    {
      // Number of whole bytes
      dwNumDataBytes = (dwNumDataBytes - 1);

      // clk data bytes in -ve clk MSB
      FTC_AddByteToOutputBuffer(CLK_DATA_BYTES_IN_ON_NEG_CLK_MSB_FIRST_CMD, false);
      FTC_AddByteToOutputBuffer((dwNumDataBytes & '\xFF'), false);
      FTC_AddByteToOutputBuffer(((dwNumDataBytes / 256) & '\xFF'), false);
    }

    //dwNumRemainingDataBits = (dwModNumBitsToRead % 8);
    dwNumRemainingDataBits = (dwNumBitsToRead % 8);

    if (dwNumRemainingDataBits > 0)
    {
      // Do remaining bits
      // clk data bits in -ve clk MSB
      FTC_AddByteToOutputBuffer(CLK_DATA_BITS_IN_ON_NEG_CLK_MSB_FIRST_CMD, false);
      FTC_AddByteToOutputBuffer((dwNumRemainingDataBits & '\xFF'), false);
    }
  }
}

FTC_STATUS FT2232hMpsseI2c::ReadDataBytesFromExternalDevice(FTC_HANDLE ftHandle, PI2CReadByteBuffer pI2CReadBuffer, DWORD dwNumBitsToRead, LPDWORD lpdwNumDataBytesRead, BOOL bWaitForAllBytesRead)
{
  FTC_STATUS Status = FTC_SUCCESS;
  DWORD dwNumDataBytesToRead = 0;
  DWORD dwNumRemainingDataBits = 0;
  DWORD dwNumBytesDeviceInputBuffer = 0;
  DWORD dwNumBytesRead = 0;
  DWORD dwInputBufferIndex = 0;
  DWORD dwTotalNumBytesRead = 0;
  SYSTEMTIME StartTime;

  // This will work out the number of whole bytes to read
  dwNumDataBytesToRead = (dwNumBitsToRead / 8);

  dwNumRemainingDataBits = (dwNumBitsToRead % 8);

  // get remaining bits
  if (dwNumRemainingDataBits > 0)
    dwNumDataBytesToRead = dwNumDataBytesToRead + 1; // increment the number of whole bytes to read, if bits left over

  if (bWaitForAllBytesRead == false)
  {
    // This path is used for getting an acknowledgement from an external device after a byte has been written to it

    // Get the number of bytes in the device input buffer
    Status = (FTC_STATUS)FT_GetQueueStatus((FT_HANDLE)ftHandle, &dwNumBytesDeviceInputBuffer);

    if ((Status == FTC_SUCCESS) && (dwNumBytesDeviceInputBuffer > 0))
    {
      //Sleep(0);  // give up timeslice

      Status = FTC_ReadBytesFromDevice(ftHandle, &InputBuffer, dwNumBytesDeviceInputBuffer, &dwNumBytesRead);

      if ((Status == FTC_SUCCESS) && (dwNumBytesRead > 0))
      {
        for (dwInputBufferIndex = 0; dwInputBufferIndex < dwNumBytesRead; dwInputBufferIndex++)
          (*pI2CReadBuffer)[dwInputBufferIndex] = InputBuffer[dwInputBufferIndex];
      }

      *lpdwNumDataBytesRead = dwNumBytesRead;
    }
  }
  else
  {
    GetLocalTime(&StartTime);

    do
    {
      // Get the number of bytes in the device input buffer
      Status = (FTC_STATUS)FT_GetQueueStatus((FT_HANDLE)ftHandle, &dwNumBytesDeviceInputBuffer);

      if ((Status == FTC_SUCCESS) && (dwNumBytesDeviceInputBuffer > 0))
      {
        Status = FTC_ReadBytesFromDevice(ftHandle, &InputBuffer, dwNumBytesDeviceInputBuffer, &dwNumBytesRead);

        if ((Status == FTC_SUCCESS) && (dwNumBytesRead > 0))
        {
          dwInputBufferIndex = 0;

          do
          {
            (*pI2CReadBuffer)[dwTotalNumBytesRead++] = InputBuffer[dwInputBufferIndex++];
          }
          while ((dwInputBufferIndex < dwNumBytesRead) && (dwTotalNumBytesRead < dwNumDataBytesToRead));
        }
      }

      if ((dwTotalNumBytesRead < dwNumDataBytesToRead) && (Status == FTC_SUCCESS))
      {
        Sleep(1);  // give up timeslice
        if (FTC_Timeout(StartTime, MAX_COMMAND_TIMEOUT_PERIOD))
          Status = FTC_FAILED_TO_COMPLETE_COMMAND;
      }
    }
    while ((dwTotalNumBytesRead < dwNumDataBytesToRead) && (Status == FTC_SUCCESS));

    *lpdwNumDataBytesRead = dwTotalNumBytesRead;
  }

  return Status;
}

FTC_STATUS FT2232hMpsseI2c::ReadDataAckFromExternalDevice(FTC_HANDLE ftHandle, PI2CReadByteBuffer pI2CReadBuffer, DWORD dwNumBitsToRead, AckTypes AckType, DWORD dwAckTimeoutmSecs)
{
  FTC_STATUS Status = FTC_SUCCESS;
  SYSTEMTIME StartTime;
  DWORD dwNumDataBytesRead = 0;
  BOOL bAckReceived = false;
  DWORD dwAckValue = 0;

  FTC_AddByteToOutputBuffer(SET_LOW_BYTE_DATA_BITS_CMD, false);
  FTC_AddByteToOutputBuffer(dwSavedLowPinsValue, false);

  FTC_AddByteToOutputBuffer(CLK_DATA_BYTES_OUT_ON_NEG_CLK_MSB_FIRST_CMD, false);  // set SCL,WP as out SDA as in

  SetI2CReadData(dwNumBitsToRead);

  FTC_AddByteToOutputBuffer(SEND_ANSWER_BACK_IMMEDIATELY_CMD, false);  // Send immediate

  Status = FTC_SendBytesToDevice(ftHandle); // send off the command

  //Sleep(0); // give up timeslice

  if (Status == FTC_SUCCESS)
  {
    if (AckType == NoAck)
      Status = ReadDataBytesFromExternalDevice(ftHandle, pI2CReadBuffer, dwNumBitsToRead, &dwNumDataBytesRead, true);
    else
    {
      GetLocalTime(&StartTime);

      do
      {
        Status = ReadDataBytesFromExternalDevice(ftHandle, pI2CReadBuffer, dwNumBitsToRead, &dwNumDataBytesRead, false);

        if (Status == FTC_SUCCESS)
        {
          if (dwNumDataBytesRead > 0)
          {
            dwAckValue = ((*pI2CReadBuffer)[0] & ACKNOWLEDGE_BIT);

            if (dwAckValue == ACKNOWLEDGE_VALUE)
              bAckReceived = true;
          }

          if (bAckReceived == false)
          {
            //Sleep(0);  // give up timeslice
            if (FTC_Timeout(StartTime, dwAckTimeoutmSecs))
            {
              if (AckType == ControlAck)
                Status = FTC_CONTROL_ACKNOWLEDGE_TIMEOUT;
              else
                Status = FTC_DATA_ACKNOWLEDGE_TIMEOUT;
            }
          }
        }
      }
      while ((bAckReceived == false) && (Status == FTC_SUCCESS));
    }

    if (Status == FTC_SUCCESS)
    {
      FTC_ClearOutputBuffer();

      FTC_AddByteToOutputBuffer(SET_LOW_BYTE_DATA_BITS_CMD, false);
      FTC_AddByteToOutputBuffer(dwSavedLowPinsValue, false);

      FTC_AddByteToOutputBuffer(CLK_DATA_BITS_OUT_ON_NEG_CLK_MSB_FIRST_CMD, false); // set SCL,SDA,WP as out

      Status = FTC_SendBytesToDevice(ftHandle); // send off the command

      Sleep(0);  // give up timeslice
    }
  }

  return Status;
}

FTC_STATUS FT2232hMpsseI2c::ReadDataByteFromExternalDevice(FTC_HANDLE ftHandle, PI2CReadByteBuffer pI2CReadBuffer)
{
  FTC_STATUS Status = FTC_SUCCESS;
  DWORD dwNumDataBytesRead = 0;

  FTC_AddByteToOutputBuffer(SET_LOW_BYTE_DATA_BITS_CMD, false);
  FTC_AddByteToOutputBuffer(dwSavedLowPinsValue, false);

  FTC_AddByteToOutputBuffer(CLK_DATA_BYTES_OUT_ON_NEG_CLK_MSB_FIRST_CMD, false);  // set SCL,WP as out SDA as in

  SetI2CReadData(8);

  FTC_AddByteToOutputBuffer(SEND_ANSWER_BACK_IMMEDIATELY_CMD, false);  // Send immediate

  Status = FTC_SendBytesToDevice(ftHandle); // send off the command

  //Sleep(0); // give up timeslice

  if (Status == FTC_SUCCESS)
  {
    Status = ReadDataBytesFromExternalDevice(ftHandle, pI2CReadBuffer, 8, &dwNumDataBytesRead, true);

    if (Status == FTC_SUCCESS)
    {
      FTC_ClearOutputBuffer();

      FTC_AddByteToOutputBuffer(SET_LOW_BYTE_DATA_BITS_CMD, false);
      FTC_AddByteToOutputBuffer(dwSavedLowPinsValue, false);

      FTC_AddByteToOutputBuffer(CLK_DATA_BITS_OUT_ON_NEG_CLK_MSB_FIRST_CMD, false); // set SCL,SDA,WP as out

      Status = FTC_SendBytesToDevice(ftHandle); // send off the command
    }
  }

  return Status;
}

FTC_STATUS FT2232hMpsseI2c::SendDataCheckAcknowledge(FTC_HANDLE ftHandle, BYTE DataByte, AckTypes AckType, DWORD dwAckTimeoutmSecs)
{
  FTC_STATUS Status = FTC_SUCCESS;
  I2CWriteByteBuffer I2CWriteBuffer;
  I2CReadByteBuffer I2CReadBuffer;
  BOOL bAckReceived = false;

  I2CWriteBuffer[0] = DataByte;
  SetI2CWriteData(ftHandle, 8, &I2CWriteBuffer);

  if (AckType == NoAck)
  {
    Status = FTC_SendBytesToDevice(ftHandle); // send off the command

    //Sleep(0); // give up timeslice
  }
  else
    Status = ReadDataAckFromExternalDevice(ftHandle, &I2CReadBuffer, NUM_ACKNOWLEDGE_BITS, AckType, dwAckTimeoutmSecs);

  return Status;
}

FTC_STATUS FT2232hMpsseI2c::WriteAddressExternalDevice(FTC_HANDLE ftHandle, BYTE DeviceAddress, AckTypes ControlAckType,
                                                       DWORD dwControlAckTimeoutmSecs, BOOL bEnableDisableWriteProtect)
{
  FTC_STATUS Status = FTC_SUCCESS;
  DWORD dwControlAckCntr = 0;

  do
  {
    if (bEnableDisableWriteProtect == false)
    {
      FTC_ClearOutputBuffer();

      SetI2CStartCondition(ftHandle, bEnableDisableWriteProtect);

      Status = SendDataCheckAcknowledge(ftHandle, DeviceAddress, ControlAckType, dwControlAckTimeoutmSecs);
    }
    else
    {
      FTC_ClearOutputBuffer();

      SetI2CStartCondition(ftHandle, bEnableDisableWriteProtect);

      Status = SendDataCheckAcknowledge(ftHandle, DeviceAddress, ControlAckType, dwControlAckTimeoutmSecs);
    }

    dwControlAckCntr++;
  }
  while ((Status == FTC_CONTROL_ACKNOWLEDGE_TIMEOUT) && (dwControlAckCntr < 100));

  return Status;
}

FTC_STATUS FT2232hMpsseI2c::WriteControlToExternalDevice(FTC_HANDLE ftHandle, PWriteControlByteBuffer pWriteControlBuffer,
                                                         DWORD dwNumControlBytesToWrite, AckTypes ControlAckType,
                                                         DWORD dwControlAckTimeoutmSecs)
{
  FTC_STATUS Status = FTC_SUCCESS;
  DWORD dwControlByteIndex = 1;

  Status = WriteAddressExternalDevice(ftHandle, (*pWriteControlBuffer)[0], ControlAckType,
                                      dwControlAckTimeoutmSecs, false);

  if (Status == FTC_SUCCESS)
  {
    if (dwNumControlBytesToWrite > 1)
    {
      do
      {
        Status = SendDataCheckAcknowledge(ftHandle, (*pWriteControlBuffer)[dwControlByteIndex], ControlAckType, dwControlAckTimeoutmSecs);

        dwControlByteIndex++;
      }
      while ((dwControlByteIndex < dwNumControlBytesToWrite) && (Status == FTC_SUCCESS)); 
    }
  }

  return Status;
}

FTC_STATUS FT2232hMpsseI2c::WriteDataToExternalDevice(FTC_HANDLE ftHandle, PWriteControlByteBuffer pWriteControlBuffer,
                                                      DWORD dwNumControlBytesToWrite, BOOL bControlAcknowledge,
                                                      DWORD dwControlAckTimeoutmSecs, BOOL bStopCondition, 
                                                      DWORD dwDataWriteTypes, PWriteDataByteBuffer pWriteDataBuffer,
                                                      DWORD dwNumDataBytesToWrite, BOOL bDataAcknowledge,
                                                      DWORD dwDataAckTimeoutmSecs, PFTC_PAGE_WRITE_DATA pPageWriteData)
{
  FTC_STATUS Status = FTC_SUCCESS;
  DWORD dwWriteDataBufferIndex = 0;
  AckTypes ControlAckType = NoAck;
  AckTypes DataAckType = NoAck;

  if (bControlAcknowledge != FALSE)
    ControlAckType = ControlAck;

  if (bDataAcknowledge != FALSE)
    DataAckType = DataAck;

  Status = WriteControlToExternalDevice(ftHandle, pWriteControlBuffer, dwNumControlBytesToWrite,
                                        ControlAckType, dwControlAckTimeoutmSecs);

  if (Status == FTC_SUCCESS)
  {
    switch(dwDataWriteTypes) 
    {
      case NO_WRITE_TYPE:
        break;
      case BYTE_WRITE_TYPE:
        Status = SendDataCheckAcknowledge(ftHandle, (*pWriteDataBuffer)[0], DataAckType, dwDataAckTimeoutmSecs);
        break;
      case PAGE_WRITE_TYPE:
        do
        {
          Status = SendDataCheckAcknowledge(ftHandle, (*pWriteDataBuffer)[dwWriteDataBufferIndex], DataAck, dwDataAckTimeoutmSecs);

          dwWriteDataBufferIndex++;
        }
        while ((dwWriteDataBufferIndex < dwNumDataBytesToWrite) && (Status == FTC_SUCCESS));
        break;
    }

    if ((Status == FTC_SUCCESS) && (bStopCondition != false))
    {
      Status = SetI2CStopCondition(ftHandle);

      if (Status == FTC_SUCCESS)
        //Status = WriteAddressExternalDevice(ftHandle, (*pWriteControlBuffer)[0], ControlAckType,
        //                                    dwControlAckTimeoutmSecs, true);  // Enable write protect
		WriteProtectEnable(ftHandle, true);
    }
  }

  return Status;
}

FTC_STATUS FT2232hMpsseI2c::ReadDataFromExternalDevice(FTC_HANDLE ftHandle, PWriteControlByteBuffer pWriteControlBuffer,
                                                       DWORD dwNumControlBytesToWrite, BOOL bControlAcknowledge,
                                                       DWORD dwControlAckTimeoutmSecs, DWORD dwDataReadTypes,
                                                       PReadDataByteBuffer pReadDataBuffer, DWORD dwNumDataBytesToRead)
{
  FTC_STATUS Status = FTC_SUCCESS;
  DWORD dwReadDataBufferIndex = 0;
  I2CReadByteBuffer I2CReadBuffer;
  I2CWriteByteBuffer I2CWriteBuffer;
  AckTypes ControlAckType = NoAck;

  if (bControlAcknowledge != FALSE)
    ControlAckType = ControlAck;

  if (dwDataReadTypes == BYTE_READ_TYPE)
  {
    // Set Read/Write bit to write ie 0
    (*pWriteControlBuffer)[0] = ((*pWriteControlBuffer)[0] & '\xFE');

    Status = WriteControlToExternalDevice(ftHandle, pWriteControlBuffer, dwNumControlBytesToWrite,
                                          ControlAckType, dwControlAckTimeoutmSecs);

    if (Status == FTC_SUCCESS)
    {
      // Set Read/Write bit to read ie 1
      (*pWriteControlBuffer)[0] = ((*pWriteControlBuffer)[0] | '\x01');

      Status = WriteAddressExternalDevice(ftHandle, (*pWriteControlBuffer)[0], ControlAckType,
                                          dwControlAckTimeoutmSecs, true);

      if (Status == FTC_SUCCESS)
      {
        Status = ReadDataAckFromExternalDevice(ftHandle, &I2CReadBuffer, 8, NoAck, 0);

        if (Status == FTC_SUCCESS)
        {
          (*pReadDataBuffer)[dwReadDataBufferIndex] = I2CReadBuffer[0];

          Status = SetI2CStopCondition(ftHandle);
        }
      }
    }
  }
  else
  {
    // Set Read/Write bit to write ie 0
    (*pWriteControlBuffer)[0] = ((*pWriteControlBuffer)[0] & '\xFE');

    Status = WriteControlToExternalDevice(ftHandle, pWriteControlBuffer, dwNumControlBytesToWrite,
                                          ControlAckType, dwControlAckTimeoutmSecs);

    if (Status == FTC_SUCCESS)
    {
      // Set Read/Write bit to read ie 1
      (*pWriteControlBuffer)[0] = ((*pWriteControlBuffer)[0] | '\x01');

      Status = WriteAddressExternalDevice(ftHandle, (*pWriteControlBuffer)[0], ControlAckType,
                                            dwControlAckTimeoutmSecs, true);
      if (Status == FTC_SUCCESS)
      {
        FTC_ClearOutputBuffer();

        // Write Ack to external device
        I2CWriteBuffer[0] = 0;

        do
        {
          Status = ReadDataByteFromExternalDevice(ftHandle, &I2CReadBuffer);

          if (Status == FTC_SUCCESS)
          {
            (*pReadDataBuffer)[dwReadDataBufferIndex] = I2CReadBuffer[0];

            dwReadDataBufferIndex++;

            if (dwReadDataBufferIndex < dwNumDataBytesToRead)
            {
              // Write Ack to external device
              SetI2CWriteData(ftHandle, 1, &I2CWriteBuffer);

              Status = FTC_SendBytesToDevice(ftHandle); // send off the command
            }
          }
        }
        while ((dwReadDataBufferIndex < dwNumDataBytesToRead) && (Status == FTC_SUCCESS));
      }
    }

    if (Status == FTC_SUCCESS)
      Status = SetI2CStopCondition(ftHandle);
  }

  return Status;
}

void FT2232hMpsseI2c::CreateDeviceDataRecord(FTC_HANDLE ftHandle)
{
  DWORD dwDeviceIndex = 0;
  BOOL bDeviceDataRecordCreated = false;

  for (dwDeviceIndex = 0; ((dwDeviceIndex < MAX_NUM_DEVICES) && !bDeviceDataRecordCreated); dwDeviceIndex++)
  {
    if (OpenedDevicesDataRecords[dwDeviceIndex].hDevice == 0)
    {
      bDeviceDataRecordCreated = true;

      OpenedDevicesDataRecords[dwDeviceIndex].hDevice = ftHandle;
      OpenedDevicesDataRecords[dwDeviceIndex].dwCommsMode = FAST_MODE;
    }
  }

  if (bDeviceDataRecordCreated == TRUE)
    dwNumOpenedDevices = dwNumOpenedDevices + 1; 
}

INT FT2232hMpsseI2c::GetDeviceDataRecordIndex(FTC_HANDLE ftHandle)
{
  DWORD dwDeviceIndex = 0;
  BOOLEAN bDeviceHandleFound = false;
  INT iDeviceDataRecordIndex = -1;

  if (ftHandle != 0)
  {
    for (dwDeviceIndex = 0; ((dwDeviceIndex < MAX_NUM_DEVICES) && !bDeviceHandleFound); dwDeviceIndex++)
    {
      if (OpenedDevicesDataRecords[dwDeviceIndex].hDevice == ftHandle)
      {
        bDeviceHandleFound = true;

        iDeviceDataRecordIndex = dwDeviceIndex;
      }
    }
  }
  else
  {
    // This code is executed if there is only one device connected to the system, this code is here just in case
    // that a device was unplugged from the system, while the system was still running
    for (dwDeviceIndex = 0; ((dwDeviceIndex < MAX_NUM_DEVICES) && !bDeviceHandleFound); dwDeviceIndex++)
    {
      if (OpenedDevicesDataRecords[dwDeviceIndex].hDevice != 0)
      {
        bDeviceHandleFound = true;

        iDeviceDataRecordIndex = dwDeviceIndex;
      }
    }
  }

  return iDeviceDataRecordIndex;
}

void FT2232hMpsseI2c::DeleteDeviceDataRecord(FTC_HANDLE ftHandle)
{
  DWORD dwDeviceIndex = 0;
  BOOLEAN bDeviceHandleFound = false;

  for (dwDeviceIndex = 0; ((dwDeviceIndex < MAX_NUM_DEVICES) && !bDeviceHandleFound); dwDeviceIndex++)
  {
    if (OpenedDevicesDataRecords[dwDeviceIndex].hDevice == ftHandle)
    {
      bDeviceHandleFound = true;

      OpenedDevicesDataRecords[dwDeviceIndex].hDevice = 0;
    }
  }

  if ((dwNumOpenedDevices > 0) && bDeviceHandleFound)
    dwNumOpenedDevices = dwNumOpenedDevices - 1;
}

DWORD FT2232hMpsseI2c::GetCommunicationsMode(FTC_HANDLE ftHandle)
{
  DWORD dwCommsMode = FAST_MODE;
  INT iDeviceDataRecordIndex;

  iDeviceDataRecordIndex = GetDeviceDataRecordIndex(ftHandle);

  if (iDeviceDataRecordIndex != -1)
    dwCommsMode = OpenedDevicesDataRecords[iDeviceDataRecordIndex].dwCommsMode;

  return dwCommsMode;
}


FT2232hMpsseI2c::FT2232hMpsseI2c(void)
{
  DWORD dwDeviceIndex = 0;

  dwSavedLowPinsValue = 0;

  dwNumOpenedDevices = 0;

  for (dwDeviceIndex = 0; (dwDeviceIndex < MAX_NUM_DEVICES); dwDeviceIndex++)
    OpenedDevicesDataRecords[dwDeviceIndex].hDevice = 0;

  InitializeCriticalSection(&threadAccess);
}

FT2232hMpsseI2c::~FT2232hMpsseI2c(void)
{
  DeleteCriticalSection(&threadAccess);
}

FTC_STATUS FT2232hMpsseI2c::I2C_GetNumDevices(LPDWORD lpdwNumDevices)
{
  FTC_STATUS Status = FTC_SUCCESS;
  FT2232CDeviceIndexes FT2232CIndexes;

  EnterCriticalSection(&threadAccess);

  *lpdwNumDevices = 0;

  Status = FTC_GetNumNotOpenedDevices(lpdwNumDevices, &FT2232CIndexes);

  LeaveCriticalSection(&threadAccess);

  return Status;
}

FTC_STATUS FT2232hMpsseI2c::I2C_GetNumHiSpeedDevices(LPDWORD lpdwNumHiSpeedDevices)
{
  FTC_STATUS Status = FTC_SUCCESS;
  HiSpeedDeviceIndexes HiSpeedIndexes;

  EnterCriticalSection(&threadAccess);

  *lpdwNumHiSpeedDevices = 0;

  Status = FTC_GetNumHiSpeedDevices(lpdwNumHiSpeedDevices, &HiSpeedIndexes);

  LeaveCriticalSection(&threadAccess);

  return Status;
}

FTC_STATUS FT2232hMpsseI2c::I2C_GetDeviceNameLocationID(DWORD dwDeviceNameIndex, LPSTR lpDeviceNameBuffer, DWORD dwBufferSize, LPDWORD lpdwLocationID)
{
  FTC_STATUS Status = FTC_SUCCESS;

  EnterCriticalSection(&threadAccess);

  Status = FTC_GetDeviceNameLocationID(dwDeviceNameIndex, lpDeviceNameBuffer, dwBufferSize, lpdwLocationID);

  LeaveCriticalSection(&threadAccess);

  return Status;
}

FTC_STATUS FT2232hMpsseI2c::I2C_GetHiSpeedDeviceNameLocationIDChannel(DWORD dwDeviceNameIndex, LPSTR lpDeviceNameBuffer, DWORD dwBufferSize, LPDWORD lpdwLocationID, LPSTR lpChannel, DWORD dwChannelBufferSize, LPDWORD lpdwHiSpeedDeviceType)
{
  FTC_STATUS Status = FTC_SUCCESS;
  DWORD dwDeviceType = 0;

  EnterCriticalSection(&threadAccess);

  *lpdwHiSpeedDeviceType = 0;

  Status = FTC_GetHiSpeedDeviceNameLocationIDChannel(dwDeviceNameIndex, lpDeviceNameBuffer, dwBufferSize, lpdwLocationID, lpChannel, dwChannelBufferSize, &dwDeviceType);

  if (Status == FTC_SUCCESS)
  {
    if ((dwDeviceType == FT_DEVICE_2232H) || (dwDeviceType == FT_DEVICE_4232H))
    {
      if (dwDeviceType == FT_DEVICE_2232H)
        *lpdwHiSpeedDeviceType = FT2232H_DEVICE_TYPE;
      else
        *lpdwHiSpeedDeviceType = FT4232H_DEVICE_TYPE;
    }
    else
      Status = FTC_DEVICE_NOT_FOUND;
  }

  LeaveCriticalSection(&threadAccess);

  return Status;
}

FTC_STATUS FT2232hMpsseI2c::I2C_OpenDevice(FTC_HANDLE *pftHandle)
{
  FTC_STATUS Status = FTC_SUCCESS;

  EnterCriticalSection(&threadAccess);

  Status = FTC_OpenDevice(pftHandle);
  
  if (Status == FTC_SUCCESS)
    CreateDeviceDataRecord(*pftHandle);

  LeaveCriticalSection(&threadAccess);

  return Status;
}

FTC_STATUS FT2232hMpsseI2c::I2C_OpenSpecifiedDevice(LPSTR lpDeviceName, DWORD dwLocationID, FTC_HANDLE *pftHandle)
{
  FTC_STATUS Status = FTC_SUCCESS;

  EnterCriticalSection(&threadAccess);

  Status = FTC_OpenSpecifiedDevice(lpDeviceName, dwLocationID, pftHandle);
  
  if (Status == FTC_SUCCESS)
    CreateDeviceDataRecord(*pftHandle);

  LeaveCriticalSection(&threadAccess);

  return Status;
}

FTC_STATUS FT2232hMpsseI2c::I2C_OpenSpecifiedHiSpeedDevice(LPSTR lpDeviceName, DWORD dwLocationID, LPSTR lpChannel, FTC_HANDLE *pftHandle)
{
  FTC_STATUS Status = FTC_SUCCESS;

  EnterCriticalSection(&threadAccess);

  Status = FTC_OpenSpecifiedHiSpeedDevice(lpDeviceName, dwLocationID, lpChannel, pftHandle);

  if (Status == FTC_SUCCESS)
    CreateDeviceDataRecord(*pftHandle);

  LeaveCriticalSection(&threadAccess);

  return Status;
}

FTC_STATUS FT2232hMpsseI2c::I2C_GetHiSpeedDeviceType(FTC_HANDLE ftHandle, LPDWORD lpdwHiSpeedDeviceType)
{
  FTC_STATUS Status = FTC_SUCCESS;
  BOOL bHiSpeedFT2232HTDeviceType = FALSE;

  EnterCriticalSection(&threadAccess);

  *lpdwHiSpeedDeviceType = 0;

  Status = FTC_GetHiSpeedDeviceType(ftHandle, &bHiSpeedFT2232HTDeviceType);

  if (Status == FTC_SUCCESS)
  {
    // Is the device a FT2232H hi-speed device
    if (bHiSpeedFT2232HTDeviceType == TRUE)
      *lpdwHiSpeedDeviceType = FT2232H_DEVICE_TYPE;
    else
      *lpdwHiSpeedDeviceType = FT4232H_DEVICE_TYPE;
  }

  LeaveCriticalSection(&threadAccess);

  return Status;
}

FTC_STATUS FT2232hMpsseI2c::I2C_CloseDevice(FTC_HANDLE ftHandle)
{
  FTC_STATUS Status = FTC_SUCCESS;

  EnterCriticalSection(&threadAccess);

  Status = FTC_CloseDevice(ftHandle);

  DeleteDeviceDataRecord(ftHandle);

  LeaveCriticalSection(&threadAccess);

  return Status;
}

FTC_STATUS WINAPI FT2232hMpsseI2c::I2C_CloseDevice(FTC_HANDLE ftHandle, PFTC_CLOSE_FINAL_STATE_PINS pCloseFinalStatePinsData)
{
  FTC_STATUS Status = FTC_SUCCESS;

  EnterCriticalSection(&threadAccess);

  if ((Status = FTC_IsDeviceHandleValid(ftHandle)) == FTC_SUCCESS)
  {
    if (pCloseFinalStatePinsData != NULL)
    {
      if ((Status = SetTCKTDITMSPinsCloseState(ftHandle, pCloseFinalStatePinsData)) == FTC_SUCCESS)
        Status = I2C_CloseDevice(ftHandle);
    }
    else
      Status = FTC_NULL_CLOSE_FINAL_STATE_BUFFER_POINTER;
  }

  LeaveCriticalSection(&threadAccess);

  return Status;
}

FTC_STATUS FT2232hMpsseI2c::I2C_InitDevice(FTC_HANDLE ftHandle, DWORD dwClockDivisor)
{
  FTC_STATUS Status = FTC_SUCCESS;

  EnterCriticalSection(&threadAccess);

  if ((Status = FTC_IsHiSpeedDeviceHandleValid(ftHandle)) == FTC_SUCCESS)
  {
    if ((Status = FTC_InitHiSpeedDevice(ftHandle)) == FTC_SUCCESS)
    {
      Status = InitDevice(ftHandle, dwClockDivisor);
    }
  }

  if (Status == FTC_INVALID_HANDLE)
  {
    if ((Status = FT2232c::FTC_IsDeviceHandleValid(ftHandle)) == FTC_SUCCESS)
    {
      Status = InitDevice(ftHandle, dwClockDivisor);
    }
  }

  LeaveCriticalSection(&threadAccess);

  return Status;
}

FTC_STATUS FT2232hMpsseI2c::I2C_TurnOnDivideByFiveClockingHiSpeedDevice(FTC_HANDLE ftHandle)
{
  FTC_STATUS Status = FTC_SUCCESS;

  EnterCriticalSection(&threadAccess);

  Status = FTC_TurnOnDivideByFiveClockingHiSpeedDevice(ftHandle);

  LeaveCriticalSection(&threadAccess);

  return Status;
}

FTC_STATUS FT2232hMpsseI2c::I2C_TurnOffDivideByFiveClockingHiSpeedDevice(FTC_HANDLE ftHandle)
{
  FTC_STATUS Status = FTC_SUCCESS;

  EnterCriticalSection(&threadAccess);

  Status = FTC_TurnOffDivideByFiveClockingHiSpeedDevice(ftHandle);

  LeaveCriticalSection(&threadAccess);

  return Status;
}

FTC_STATUS FT2232hMpsseI2c::I2C_TurnOnThreePhaseDataClockingHiSpeedDevice(FTC_HANDLE ftHandle)
{
  FTC_STATUS Status = FTC_SUCCESS;

  EnterCriticalSection(&threadAccess);

  Status = FTC_TurnOnAdaptiveClockingHiSpeedDevice(ftHandle);

  LeaveCriticalSection(&threadAccess);

  return Status;
}

FTC_STATUS FT2232hMpsseI2c::I2C_TurnOffThreePhaseDataClockingHiSpeedDevice(FTC_HANDLE ftHandle)
{
  FTC_STATUS Status = FTC_SUCCESS;

  EnterCriticalSection(&threadAccess);

  Status = FTC_TurnOffAdaptiveClockingHiSpeedDevice(ftHandle);

  LeaveCriticalSection(&threadAccess);

  return Status;
}

FTC_STATUS FT2232hMpsseI2c::I2C_SetDeviceLatencyTimer(FTC_HANDLE ftHandle, BYTE LatencyTimermSec)
{
  FTC_STATUS Status = FTC_SUCCESS;

  EnterCriticalSection(&threadAccess);

  Status = FTC_SetDeviceLatencyTimer(ftHandle, LatencyTimermSec);

  LeaveCriticalSection(&threadAccess);

  return Status;
}

FTC_STATUS FT2232hMpsseI2c::I2C_GetDeviceLatencyTimer(FTC_HANDLE ftHandle, LPBYTE lpLatencyTimermSec)
{
  FTC_STATUS Status = FTC_SUCCESS;

  EnterCriticalSection(&threadAccess);

  Status = FTC_GetDeviceLatencyTimer(ftHandle, lpLatencyTimermSec);

  LeaveCriticalSection(&threadAccess);

  return Status;
}

FTC_STATUS FT2232hMpsseI2c::I2C_GetClock(DWORD dwClockDivisor, LPDWORD lpdwClockFrequencyHz)
{
  FTC_STATUS Status = FTC_SUCCESS;

  EnterCriticalSection(&threadAccess);

  if ((dwClockDivisor >= MIN_CLOCK_DIVISOR) && (dwClockDivisor <= MAX_CLOCK_DIVISOR))
    FTC_GetClockFrequencyValues(dwClockDivisor, lpdwClockFrequencyHz);
  else
    Status = FTC_INVALID_CLOCK_DIVISOR;

  LeaveCriticalSection(&threadAccess);

  return Status;
}

FTC_STATUS FT2232hMpsseI2c::I2C_GetHiSpeedDeviceClock(DWORD dwClockDivisor, LPDWORD lpdwClockFrequencyHz)
{
  FTC_STATUS Status = FTC_SUCCESS;

  EnterCriticalSection(&threadAccess);

  if ((dwClockDivisor >= MIN_CLOCK_DIVISOR) && (dwClockDivisor <= MAX_CLOCK_DIVISOR))
    FTC_GetHiSpeedDeviceClockFrequencyValues(dwClockDivisor, lpdwClockFrequencyHz);
  else
    Status = FTC_INVALID_CLOCK_DIVISOR;

  LeaveCriticalSection(&threadAccess);

  return Status;
}

FTC_STATUS FT2232hMpsseI2c::I2C_SetClock(FTC_HANDLE ftHandle, DWORD dwClockDivisor, LPDWORD lpdwClockFrequencyHz)
{
  FTC_STATUS Status = FTC_SUCCESS;

  EnterCriticalSection(&threadAccess);

  if ((dwClockDivisor >= MIN_CLOCK_DIVISOR) && (dwClockDivisor <= MAX_CLOCK_DIVISOR))
  {
    if ((Status = FTC_IsHiSpeedDeviceHandleValid(ftHandle)) == FTC_SUCCESS)
    {
      FTC_GetHiSpeedDeviceClockFrequencyValues(ftHandle, dwClockDivisor, lpdwClockFrequencyHz);
    }

    if (Status == FTC_INVALID_HANDLE)
    {
      if ((Status = FT2232c::FTC_IsDeviceHandleValid(ftHandle)) == FTC_SUCCESS)
      {
        FTC_GetClockFrequencyValues(dwClockDivisor, lpdwClockFrequencyHz);
      }
    }

    if (Status == FTC_SUCCESS)
    {
      Status = SetDataInOutClockFrequency(ftHandle, dwClockDivisor);
    }
  }
  else
    Status = FTC_INVALID_CLOCK_DIVISOR;

  LeaveCriticalSection(&threadAccess);

  return Status;
}

FTC_STATUS FT2232hMpsseI2c::I2C_SetDeviceLoopbackState(FTC_HANDLE ftHandle, BOOL bLoopbackState)
{
  FTC_STATUS Status = FTC_SUCCESS;

  EnterCriticalSection(&threadAccess);

  if ((Status = FTC_IsDeviceHandleValid(ftHandle)) == FTC_SUCCESS)
    Status = FTC_SetDeviceLoopbackState(ftHandle, bLoopbackState);

  LeaveCriticalSection(&threadAccess);

  return Status;
}

FTC_STATUS WINAPI FT2232hMpsseI2c::I2C_SetCommunicationsMode(FTC_HANDLE ftHandle, DWORD dwCommsMode)
{
  FTC_STATUS Status = FTC_SUCCESS;
  INT iDeviceDataRecordIndex;

  EnterCriticalSection(&threadAccess);

  if ((Status = FTC_IsDeviceHandleValid(ftHandle)) == FTC_SUCCESS)
  {
    if ((dwCommsMode == STANDARD_MODE) || (dwCommsMode == FAST_MODE) || (dwCommsMode == STRETCH_DATA_MODE))
    {
      iDeviceDataRecordIndex = GetDeviceDataRecordIndex(ftHandle);

      if (iDeviceDataRecordIndex != -1)
        OpenedDevicesDataRecords[iDeviceDataRecordIndex].dwCommsMode = dwCommsMode; 
    }
    else
      Status = FTC_INVALID_COMMS_MODE;
  }

  LeaveCriticalSection(&threadAccess);

  return Status;
}

FTC_STATUS FT2232hMpsseI2c::I2C_SetGeneralPurposeHighInputOutputPins(FTC_HANDLE ftHandle, PFTC_INPUT_OUTPUT_PINS pHighInputOutputPinsData)
{
  FTC_STATUS Status = FTC_SUCCESS;
  DWORD dwHighPinsDirection = 0;
  DWORD dwHighPinsValue = 0;

  EnterCriticalSection(&threadAccess);

  Status = FTC_IsDeviceHandleValid(ftHandle);

  if (Status == FTC_SUCCESS)
  {
    if (pHighInputOutputPinsData != NULL)
    {
      if (pHighInputOutputPinsData->bPin1InputOutputState != false)
        dwHighPinsDirection = (dwHighPinsDirection | '\x01');
      if (pHighInputOutputPinsData->bPin2InputOutputState != false)
        dwHighPinsDirection = (dwHighPinsDirection | '\x02');
      if (pHighInputOutputPinsData->bPin3InputOutputState != false)
        dwHighPinsDirection = (dwHighPinsDirection | '\x04');
      if (pHighInputOutputPinsData->bPin4InputOutputState != false)
        dwHighPinsDirection = (dwHighPinsDirection | '\x08');
      if (pHighInputOutputPinsData->bPin1LowHighState != false)
        dwHighPinsValue = (dwHighPinsValue | '\x01');
      if (pHighInputOutputPinsData->bPin2LowHighState != false)
        dwHighPinsValue = (dwHighPinsValue | '\x02');
      if (pHighInputOutputPinsData->bPin3LowHighState != false)
        dwHighPinsValue = (dwHighPinsValue | '\x04');
      if (pHighInputOutputPinsData->bPin4LowHighState != false)
        dwHighPinsValue = (dwHighPinsValue | '\x08');

      Status = SetGeneralPurposeHighInputOutputPins(ftHandle, dwHighPinsDirection, dwHighPinsValue);
    }
    else
    {
      Status = FTC_NULL_INPUT_OUTPUT_BUFFER_POINTER;
    }
  }

  LeaveCriticalSection(&threadAccess);

  return Status;
}

FTC_STATUS FT2232hMpsseI2c::I2C_SetHiSpeedDeviceGeneralPurposeInputOutputPins(FTC_HANDLE ftHandle, BOOL bControlLowInputOutputPins,
                                                                              PFTC_INPUT_OUTPUT_PINS pLowInputOutputPinsData,
                                                                              BOOL bControlHighInputOutputPins,
                                                                              PFTH_INPUT_OUTPUT_PINS pHighInputOutputPinsData)
{
  FTC_STATUS Status = FTC_SUCCESS;

  EnterCriticalSection(&threadAccess);

  if ((Status = FTC_IsHiSpeedDeviceHandleValid(ftHandle)) == FTC_SUCCESS)
  {
    if ((pLowInputOutputPinsData != NULL) && (pHighInputOutputPinsData != NULL))
    {
      if (bControlLowInputOutputPins != FALSE)
        Status = SetHiSpeedDeviceGeneralPurposeLowInputOutputPins(ftHandle, pLowInputOutputPinsData);
      
      if (bControlHighInputOutputPins != FALSE)
        Status = SetHiSpeedDeviceGeneralPurposeHighInputOutputPins(ftHandle, pHighInputOutputPinsData);
    }
    else
      Status = FTC_NULL_INPUT_OUTPUT_BUFFER_POINTER;
  }

  LeaveCriticalSection(&threadAccess);

  return Status;
}

FTC_STATUS FT2232hMpsseI2c::I2C_GetGeneralPurposeHighInputOutputPins(FTC_HANDLE ftHandle, PFTC_LOW_HIGH_PINS pHighPinsInputData)
{
  FTC_STATUS Status = FTC_SUCCESS;

  EnterCriticalSection(&threadAccess);

  Status = FTC_IsDeviceHandleValid(ftHandle);

  if (Status == FTC_SUCCESS)
  {
    if (pHighPinsInputData != NULL)
      Status = GetGeneralPurposeHighInputOutputPins(ftHandle, pHighPinsInputData);
    else
      Status = FTC_NULL_INPUT_BUFFER_POINTER;
  }

  LeaveCriticalSection(&threadAccess);

  return Status;
}

FTC_STATUS FT2232hMpsseI2c::I2C_GetHiSpeedDeviceGeneralPurposeInputOutputPins(FTC_HANDLE ftHandle, BOOL bControlLowInputOutputPins,
                                                                              PFTC_LOW_HIGH_PINS pLowPinsInputData,
                                                                              BOOL bControlHighInputOutputPins,
                                                                              PFTH_LOW_HIGH_PINS pHighPinsInputData)
{
  FTC_STATUS Status = FTC_SUCCESS;

  EnterCriticalSection(&threadAccess);

  if ((Status = FTC_IsHiSpeedDeviceHandleValid(ftHandle)) == FTC_SUCCESS)
  {
    if ((pLowPinsInputData != NULL) && (pHighPinsInputData != NULL))
    {
      // Put in this small delay incase the application programmer does a get GPIOs immediately after a set GPIOs
      Sleep(5);

      if (bControlLowInputOutputPins != FALSE)
        Status = GetHiSpeedDeviceGeneralPurposeLowInputOutputPins(ftHandle, pLowPinsInputData);

      if (bControlHighInputOutputPins != FALSE)
        Status = GetHiSpeedDeviceGeneralPurposeHighInputOutputPins(ftHandle, pHighPinsInputData);
    }
    else
      Status = FTC_NULL_INPUT_OUTPUT_BUFFER_POINTER;
  }

  LeaveCriticalSection(&threadAccess);

  return Status;
}

FTC_STATUS FT2232hMpsseI2c::I2C_WriteDataToExternalDevice(FTC_HANDLE ftHandle, PWriteControlByteBuffer pWriteControlBuffer,
                                                          DWORD dwNumControlBytesToWrite, BOOL bControlAcknowledge,
                                                          DWORD dwControlAckTimeoutmSecs, BOOL bStopCondition, 
                                                          DWORD dwDataWriteTypes, PWriteDataByteBuffer pWriteDataBuffer,
                                                          DWORD dwNumDataBytesToWrite, BOOL bDataAcknowledge,
                                                          DWORD dwDataAckTimeoutmSecs, PFTC_PAGE_WRITE_DATA pPageWriteData)
{
  FTC_STATUS Status = FTC_SUCCESS;

  EnterCriticalSection(&threadAccess);

  Status = FTC_IsDeviceHandleValid(ftHandle);

  if (Status == FTC_SUCCESS)
  {
    Status = CheckWriteDataToExternalDeviceParameters(pWriteControlBuffer, dwNumControlBytesToWrite, dwDataWriteTypes, 
                                                      pWriteDataBuffer, dwNumDataBytesToWrite, pPageWriteData);

    if (Status == FTC_SUCCESS)
      Status = WriteDataToExternalDevice(ftHandle, pWriteControlBuffer, dwNumControlBytesToWrite,
                                         bControlAcknowledge, dwControlAckTimeoutmSecs, bStopCondition,
                                         dwDataWriteTypes, pWriteDataBuffer, dwNumDataBytesToWrite,
                                         bDataAcknowledge, dwDataAckTimeoutmSecs, pPageWriteData);
  }

  LeaveCriticalSection(&threadAccess);

  return Status;
}

FTC_STATUS FT2232hMpsseI2c::I2C_ReadDataFromExternalDevice(FTC_HANDLE ftHandle, PWriteControlByteBuffer pWriteControlBuffer,
                                                           DWORD dwNumControlBytesToWrite, BOOL bControlAcknowledge,
                                                           DWORD dwControlAckTimeoutmSecs, DWORD dwDataReadTypes,
                                                           PReadDataByteBuffer pReadDataBuffer, DWORD dwNumDataBytesToRead)
{
  EnterCriticalSection(&threadAccess);

  FTC_STATUS Status = FTC_SUCCESS;

  Status = FTC_IsDeviceHandleValid(ftHandle);

  if (Status == FTC_SUCCESS)
  {
    Status = CheckReadDataFromExternalDeviceParameters(pWriteControlBuffer, dwNumControlBytesToWrite, dwDataReadTypes, 
                                                       pReadDataBuffer, dwNumDataBytesToRead);

    if (Status == FTC_SUCCESS)
      Status = ReadDataFromExternalDevice(ftHandle, pWriteControlBuffer, dwNumControlBytesToWrite,
                                          bControlAcknowledge, dwControlAckTimeoutmSecs, dwDataReadTypes,
                                          pReadDataBuffer, dwNumDataBytesToRead);
  }

  LeaveCriticalSection(&threadAccess);

  return Status;
}

FTC_STATUS FT2232hMpsseI2c::I2C_GetDllVersion(LPSTR lpDllVersionBuffer, DWORD dwBufferSize)
{
  FTC_STATUS Status = FTC_SUCCESS;

  EnterCriticalSection(&threadAccess);

  if (lpDllVersionBuffer != NULL)
  {
    if (dwBufferSize > strlen(DLL_VERSION_NUM))
      strcpy(lpDllVersionBuffer, DLL_VERSION_NUM);
    else
      Status = FTC_DLL_VERSION_BUFFER_TOO_SMALL;
  }
  else
    Status = FTC_NULL_DLL_VERSION_BUFFER_POINTER;

  LeaveCriticalSection(&threadAccess);

  return Status;
}

FTC_STATUS FT2232hMpsseI2c::I2C_GetErrorCodeString(LPSTR lpLanguage, FTC_STATUS StatusCode,
                                                     LPSTR lpErrorMessageBuffer, DWORD dwBufferSize)
{
  FTC_STATUS Status = FTC_SUCCESS;
  CHAR szErrorMsg[MAX_ERROR_MSG_SIZE];
  INT iCharCntr = 0;

  EnterCriticalSection(&threadAccess);

  if ((lpLanguage != NULL) && (lpErrorMessageBuffer != NULL))
  {
    for (iCharCntr = 0; (iCharCntr < MAX_ERROR_MSG_SIZE); iCharCntr++)
      szErrorMsg[iCharCntr] = '\0';

    if (((StatusCode >= FTC_SUCCESS) && (StatusCode <= FTC_INSUFFICIENT_RESOURCES)) ||
        ((StatusCode >= FTC_FAILED_TO_COMPLETE_COMMAND) && (StatusCode <= FTC_INVALID_STATUS_CODE)))
    {
      if (strcmp(lpLanguage, ENGLISH) == 0)
      {
        if ((StatusCode >= FTC_SUCCESS) && (StatusCode <= FTC_INSUFFICIENT_RESOURCES))
          strcpy(szErrorMsg, EN_Common_Errors[StatusCode]);
        else
          strcpy(szErrorMsg, EN_New_Errors[(StatusCode - FTC_FAILED_TO_COMPLETE_COMMAND)]);
      }
      else
      {
        strcpy(szErrorMsg, EN_New_Errors[FTC_INVALID_LANGUAGE_CODE - FTC_FAILED_TO_COMPLETE_COMMAND]);

        Status = FTC_INVALID_LANGUAGE_CODE;
      }
    }
    else
    {
      sprintf(szErrorMsg, "%s%d", EN_New_Errors[FTC_INVALID_STATUS_CODE - FTC_FAILED_TO_COMPLETE_COMMAND], StatusCode);

      Status = FTC_INVALID_STATUS_CODE;
    }

    if (dwBufferSize > strlen(szErrorMsg))
      strcpy(lpErrorMessageBuffer, szErrorMsg);
    else
      Status = FTC_ERROR_MESSAGE_BUFFER_TOO_SMALL;
  }
  else
  {
    if (lpLanguage == NULL)
      Status = FTC_NULL_LANGUAGE_CODE_BUFFER_POINTER;
    else
      Status = FTC_NULL_ERROR_MESSAGE_BUFFER_POINTER;
  }

  LeaveCriticalSection(&threadAccess);

  return Status;
}


// fix - Andrew 2/11/05

void FT2232hMpsseI2c::RepeatBlock(char byte, DWORD num)
{
	DWORD i=0;

	for(i=0; i<num; i++)
	{
		FTC_AddByteToOutputBuffer((DWORD)0x80, false);
		FTC_AddByteToOutputBuffer((DWORD)byte, false); 
		FTC_AddByteToOutputBuffer((DWORD)0x03, false); 
	}
}

void FT2232hMpsseI2c::ClockByte(unsigned char byte)
{
	int i=0;

	//option 2
	for(i=0; i<8; i++)
	{
		FTC_AddByteToOutputBuffer((DWORD)0x13, false); //latch data on -ve edge msb first no read
		FTC_AddByteToOutputBuffer((DWORD)0x00, false);

		if(byte&0x80)
			FTC_AddByteToOutputBuffer((DWORD)0xff, false); //write address
		else
			FTC_AddByteToOutputBuffer((DWORD)0x00, false);
		
		RepeatBlock((byte>>6)&0x02, 1);
		byte <<= 1;
	}
}

void FT2232hMpsseI2c::WriteProtectEnable(FTC_HANDLE ftHandle, bool b)
{
	FTC_STATUS Status;

	if(b)
	{
		// tristate SDA SCL
		FTC_AddByteToOutputBuffer(SET_LOW_BYTE_DATA_BITS_CMD, false);
		FTC_AddByteToOutputBuffer(dwSavedLowPinsValue & 0xEF , false); //WP off

		FTC_AddByteToOutputBuffer('\x13', false); // set SCL,SDA as input,WP as out

	}else
	{
		// tristate SDA SCL
		FTC_AddByteToOutputBuffer(SET_LOW_BYTE_DATA_BITS_CMD, false);
		FTC_AddByteToOutputBuffer(dwSavedLowPinsValue | 0x10, false); //WP On

		FTC_AddByteToOutputBuffer('\x10', false); // set SCL,SDA as input,WP as out
	}

	Status = FTC_SendBytesToDevice(ftHandle); // send off the command
	Sleep(0); // give up timeslice	

}

int FT2232hMpsseI2c::MeineTestFunktion(void) {

	int x = 2103;
	return x;

}

