/*++

Copyright (c) 2005  Future Technology Devices International Ltd.

Module Name:

    ftci2c.cpp

Abstract:

    API DLL for FT2232H and FT4232H Hi-Speed Dual Device and FT2232D Dual Device setup to simulate the
    Inter-Integrated Circuit(I2C) synchronous serial protocol.
	  Defines the entry point for the DLL application.

Environment:

    kernel & user mode

Revision History:

    23/03/05    kra   Created.
    03/06/08    kra   Added two new functions ie I2C_SetGPIOs and I2C_GetGPIOs to control the output
                      states ie high(true) or low(false) of the 4 general purpose higher input/output
                      ACBUS 0-3/GPIOH 0-3) pins
    01/08/08    kra   Added new functions for FT2232H and FT4232H hi-speed devices.
    20/08/08    kra   Added new function I2C_CloseDevice.
	
--*/

#include "stdafx.h"
#include <windows.h>
#include "ftci2c.h"
#include "ft2232hmpssei2c.h"
#include "FTD2XX.H"
//#include "stdafx.h"
#include <stdlib.h>



static FT2232hMpsseI2c *pFT2232hMpsseI2c = NULL;


	BOOL APIENTRY DllMain(HANDLE hModule,DWORD  ul_reason_for_call,LPVOID lpReserved)
	{
		switch (ul_reason_for_call)
		{
		case DLL_PROCESS_ATTACH:
			if (pFT2232hMpsseI2c == NULL)
				pFT2232hMpsseI2c = new FT2232hMpsseI2c();
			break;
		case DLL_THREAD_ATTACH:
			break;
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			delete pFT2232hMpsseI2c;
			break;
		}

		return TRUE;
	}

	//---------------------------------------------------------------------------

	extern "C" __declspec(dllexport)
		FTC_STATUS WINAPI I2C_GetNumDevices(LPDWORD lpdwNumDevices)
	{
		return pFT2232hMpsseI2c->I2C_GetNumDevices(lpdwNumDevices);
	}

	extern "C" __declspec(dllexport)
		FTC_STATUS WINAPI I2C_GetNumHiSpeedDevices(LPDWORD lpdwNumHiSpeedDevices)
	{
		return pFT2232hMpsseI2c->I2C_GetNumHiSpeedDevices(lpdwNumHiSpeedDevices);
	}

	extern "C" __declspec(dllexport)
		FTC_STATUS WINAPI I2C_GetDeviceNameLocID(DWORD dwDeviceNameIndex, LPSTR lpDeviceNameBuffer, DWORD dwBufferSize, LPDWORD lpdwLocationID)
	{
		return pFT2232hMpsseI2c->I2C_GetDeviceNameLocationID(dwDeviceNameIndex, lpDeviceNameBuffer, dwBufferSize, lpdwLocationID);
	}

	extern "C" __declspec(dllexport)
		FTC_STATUS WINAPI I2C_GetHiSpeedDeviceNameLocIDChannel(DWORD dwDeviceNameIndex, LPSTR lpDeviceNameBuffer, DWORD dwBufferSize, LPDWORD lpdwLocationID, LPSTR lpChannel, DWORD dwChannelBufferSize, LPDWORD lpdwHiSpeedDeviceType)
	{
		return pFT2232hMpsseI2c->I2C_GetHiSpeedDeviceNameLocationIDChannel(dwDeviceNameIndex, lpDeviceNameBuffer, dwBufferSize, lpdwLocationID, lpChannel, dwChannelBufferSize, lpdwHiSpeedDeviceType);
	}

	extern "C" __declspec(dllexport)
		FTC_STATUS WINAPI I2C_Open(FTC_HANDLE *pftHandle)
	{
		return pFT2232hMpsseI2c->I2C_OpenDevice(pftHandle);
	}

	extern "C" __declspec(dllexport)
		FTC_STATUS WINAPI I2C_OpenEx(LPSTR lpDeviceName, DWORD dwLocationID, FTC_HANDLE *pftHandle)
	{
		return pFT2232hMpsseI2c->I2C_OpenSpecifiedDevice(lpDeviceName, dwLocationID, pftHandle);
	}

	extern "C" __declspec(dllexport)
		FTC_STATUS WINAPI I2C_OpenHiSpeedDevice(LPSTR lpDeviceName, DWORD dwLocationID, LPSTR lpChannel, FTC_HANDLE *pftHandle)
	{
		return pFT2232hMpsseI2c->I2C_OpenSpecifiedHiSpeedDevice(lpDeviceName, dwLocationID, lpChannel, pftHandle);
	}

	extern "C" __declspec(dllexport)
		FTC_STATUS WINAPI I2C_GetHiSpeedDeviceType(FTC_HANDLE ftHandle, LPDWORD lpdwHiSpeedDeviceType)
	{
		return pFT2232hMpsseI2c->I2C_GetHiSpeedDeviceType(ftHandle, lpdwHiSpeedDeviceType);
	}

	extern "C" __declspec(dllexport)
		FTC_STATUS WINAPI I2C_Close(FTC_HANDLE ftHandle)
	{
		return pFT2232hMpsseI2c->I2C_CloseDevice(ftHandle);
	}

	extern "C" __declspec(dllexport)
		FTC_STATUS WINAPI I2C_CloseDevice(FTC_HANDLE ftHandle, PFTC_CLOSE_FINAL_STATE_PINS pCloseFinalStatePinsData)
	{
		return pFT2232hMpsseI2c->I2C_CloseDevice(ftHandle, pCloseFinalStatePinsData);
	}

	extern "C" __declspec(dllexport)
		FTC_STATUS WINAPI I2C_InitDevice(FTC_HANDLE ftHandle, DWORD dwClockDivisor)
	{
		return pFT2232hMpsseI2c->I2C_InitDevice(ftHandle, dwClockDivisor);
	}

	extern "C" __declspec(dllexport)
		FTC_STATUS WINAPI I2C_TurnOnDivideByFiveClockingHiSpeedDevice(FTC_HANDLE ftHandle)
	{
		return pFT2232hMpsseI2c->I2C_TurnOnDivideByFiveClockingHiSpeedDevice(ftHandle);
	}

	extern "C" __declspec(dllexport)
		FTC_STATUS WINAPI I2C_TurnOffDivideByFiveClockingHiSpeedDevice(FTC_HANDLE ftHandle)
	{
		return pFT2232hMpsseI2c->I2C_TurnOffDivideByFiveClockingHiSpeedDevice(ftHandle);
	}

	extern "C" __declspec(dllexport)
		FTC_STATUS WINAPI I2C_TurnOnThreePhaseDataClockingHiSpeedDevice(FTC_HANDLE ftHandle)
	{
		return pFT2232hMpsseI2c->I2C_TurnOnThreePhaseDataClockingHiSpeedDevice(ftHandle);
	}

	extern "C" __declspec(dllexport)
		FTC_STATUS WINAPI I2C_TurnOffThreePhaseDataClockingHiSpeedDevice(FTC_HANDLE ftHandle)
	{
		return pFT2232hMpsseI2c->I2C_TurnOffThreePhaseDataClockingHiSpeedDevice(ftHandle);
	}

	extern "C" __declspec(dllexport)
		FTC_STATUS WINAPI I2C_SetDeviceLatencyTimer(FTC_HANDLE ftHandle, BYTE timerValue)
	{
		return pFT2232hMpsseI2c->I2C_SetDeviceLatencyTimer(ftHandle, timerValue);
	}

	extern "C" __declspec(dllexport)
		FTC_STATUS WINAPI I2C_GetDeviceLatencyTimer(FTC_HANDLE ftHandle, LPBYTE lpTimerValue)
	{
		return pFT2232hMpsseI2c->I2C_GetDeviceLatencyTimer(ftHandle, lpTimerValue);
	}

	extern "C" __declspec(dllexport)
		FTC_STATUS WINAPI I2C_GetClock(DWORD dwClockDivisor, LPDWORD lpdwClockFrequencyHz)
	{
		return pFT2232hMpsseI2c->I2C_GetClock(dwClockDivisor, lpdwClockFrequencyHz);
	}

	extern "C" __declspec(dllexport)
		FTC_STATUS WINAPI I2C_GetHiSpeedDeviceClock(DWORD dwClockDivisor, LPDWORD lpdwClockFrequencyHz)
	{
		return pFT2232hMpsseI2c->I2C_GetHiSpeedDeviceClock(dwClockDivisor, lpdwClockFrequencyHz);
	}

	extern "C" __declspec(dllexport)
		FTC_STATUS WINAPI I2C_SetClock(FTC_HANDLE ftHandle, DWORD dwClockDivisor, LPDWORD lpdwClockFrequencyHz)
	{
		return pFT2232hMpsseI2c->I2C_SetClock(ftHandle, dwClockDivisor, lpdwClockFrequencyHz);
	}

	extern "C" __declspec(dllexport)
		FTC_STATUS WINAPI I2C_SetLoopback(FTC_HANDLE ftHandle, BOOL bLoopbackState)
	{
		return pFT2232hMpsseI2c->I2C_SetDeviceLoopbackState(ftHandle, bLoopbackState);
	}

	extern "C" __declspec(dllexport)
		FTC_STATUS WINAPI I2C_SetMode(FTC_HANDLE ftHandle, DWORD dwCommsMode)
	{
		return pFT2232hMpsseI2c->I2C_SetCommunicationsMode(ftHandle, dwCommsMode);
	}

	extern "C" __declspec(dllexport)
		FTC_STATUS WINAPI I2C_SetGPIOs(FTC_HANDLE ftHandle, PFTC_INPUT_OUTPUT_PINS pHighInputOutputPinsData)
	{
		return pFT2232hMpsseI2c->I2C_SetGeneralPurposeHighInputOutputPins(ftHandle, pHighInputOutputPinsData);
	}

	extern "C" __declspec(dllexport)
		FTC_STATUS WINAPI I2C_SetHiSpeedDeviceGPIOs(FTC_HANDLE ftHandle, BOOL bControlLowInputOutputPins,
			PFTC_INPUT_OUTPUT_PINS pLowInputOutputPinsData,
			BOOL bControlHighInputOutputPins,
			PFTH_INPUT_OUTPUT_PINS pHighInputOutputPinsData)
	{
		return pFT2232hMpsseI2c->I2C_SetHiSpeedDeviceGeneralPurposeInputOutputPins(ftHandle, bControlLowInputOutputPins,
			pLowInputOutputPinsData, bControlHighInputOutputPins,
			pHighInputOutputPinsData);
	}

	extern "C" __declspec(dllexport)
		FTC_STATUS WINAPI I2C_GetGPIOs(FTC_HANDLE ftHandle, PFTC_LOW_HIGH_PINS pHighPinsInputData)
	{
		return pFT2232hMpsseI2c->I2C_GetGeneralPurposeHighInputOutputPins(ftHandle, pHighPinsInputData);
	}

	extern "C" __declspec(dllexport)
		FTC_STATUS WINAPI I2C_GetHiSpeedDeviceGPIOs(FTC_HANDLE ftHandle, BOOL bControlLowInputOutputPins,
			PFTC_LOW_HIGH_PINS pLowPinsInputData,
			BOOL bControlHighInputOutputPins,
			PFTH_LOW_HIGH_PINS pHighPinsInputData)
	{
		return pFT2232hMpsseI2c->I2C_GetHiSpeedDeviceGeneralPurposeInputOutputPins(ftHandle, bControlLowInputOutputPins,
			pLowPinsInputData, bControlHighInputOutputPins,
			pHighPinsInputData);
	}

	extern "C" __declspec(dllexport)
		FTC_STATUS WINAPI I2C_Write(FTC_HANDLE ftHandle, PWriteControlByteBuffer pWriteControlBuffer,
			DWORD dwNumControlBytesToWrite, BOOL bControlAcknowledge, DWORD dwControlAckTimeoutmSecs,
			BOOL bStopCondition, DWORD dwDataWriteTypes, PWriteDataByteBuffer pWriteDataBuffer, DWORD dwNumDataBytesToWrite,
			BOOL bDataAcknowledge, DWORD dwDataAckTimeoutmSecs, PFTC_PAGE_WRITE_DATA pPageWriteData)
	{
		return pFT2232hMpsseI2c->I2C_WriteDataToExternalDevice(ftHandle, pWriteControlBuffer, dwNumControlBytesToWrite,
			bControlAcknowledge, dwControlAckTimeoutmSecs, bStopCondition,
			dwDataWriteTypes, pWriteDataBuffer, dwNumDataBytesToWrite,
			bDataAcknowledge, dwDataAckTimeoutmSecs, pPageWriteData);
	}

	extern "C" __declspec(dllexport)
		FTC_STATUS WINAPI I2C_Read(FTC_HANDLE ftHandle, PWriteControlByteBuffer pWriteControlBuffer,
			DWORD dwNumControlBytesToWrite, BOOL bControlAcknowledge, DWORD dwControlAckTimeoutmSecs,
			DWORD dwDataReadTypes, PReadDataByteBuffer pReadDataBuffer, DWORD dwNumDataBytesToRead)
	{
		return pFT2232hMpsseI2c->I2C_ReadDataFromExternalDevice(ftHandle, pWriteControlBuffer, dwNumControlBytesToWrite,
			bControlAcknowledge, dwControlAckTimeoutmSecs, dwDataReadTypes,
			pReadDataBuffer, dwNumDataBytesToRead);
	}

	extern "C" __declspec(dllexport)
		FTC_STATUS WINAPI I2C_GetDllVersion(LPSTR lpDllVersionBuffer, DWORD dwBufferSize)
	{
		return pFT2232hMpsseI2c->I2C_GetDllVersion(lpDllVersionBuffer, dwBufferSize);
	}

	extern "C" __declspec(dllexport)
		FTC_STATUS WINAPI I2C_GetErrorCodeString(LPSTR lpLanguage, FTC_STATUS StatusCode, LPSTR lpErrorMessageBuffer, DWORD dwBufferSize)
	{
		return pFT2232hMpsseI2c->I2C_GetErrorCodeString(lpLanguage, StatusCode, lpErrorMessageBuffer, dwBufferSize);
	}

	extern "C" __declspec(dllexport)
		int WINAPI MeineTestFunktion(void)
	{
		//hat Funktioniert

		return pFT2232hMpsseI2c->MeineTestFunktion();

	}

	extern "C" __declspec(dllexport)
		FTC_STATUS WINAPI ftdi_Open(int devicenumber, FT_HANDLE *pHandle) {

		return FT_Open(devicenumber, pHandle);
	}

	extern "C" __declspec(dllexport)
		FTC_STATUS WINAPI ftdi_ListDevices(int devicenumber, FT_HANDLE *pHandle) {

		return FT_Open(devicenumber, pHandle);
	}

	extern "C" __declspec(dllexport)
		FTC_STATUS WINAPI ftdi_GetDriverVersion(FT_HANDLE *pHandle, LPDWORD lpdwVersion) {

		return FT_GetDriverVersion(pHandle, lpdwVersion);
	}

	extern "C" __declspec(dllexport)
		FTC_STATUS WINAPI ftdi_Close(FT_HANDLE ftHandle) {

		return FT_Close(ftHandle);
	}

	extern "C" __declspec(dllexport)
		FTC_STATUS WINAPI ftdi_OpenEx(PVOID pArg1, FT_HANDLE * ftHandle) {

		return FT_OpenEx(pArg1, 2, ftHandle);
	}

	extern "C" __declspec(dllexport)
		FTC_STATUS WINAPI ftdi_GetDeviceInfoList(FT_DEVICE_LIST_INFO_NODE *pDest, LPDWORD numdevs) {

		return FT_GetDeviceInfoList(pDest, numdevs);

	}



//---------------------------------------------------------------------------

