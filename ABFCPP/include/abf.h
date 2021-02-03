#ifndef ABF_H
#define ABF_H

#include "abffiles.h"
#include <iostream>
#include <vector>



class ABF {
public:
	ABF(const char* f, unsigned int n=1024*16);
	~ABF();
	std::vector<float> data(int channel = 0, int sweep = 1, bool m = true);
	void save(const char* fn, float* buff, int channel = 0, int sweep = 1, bool m = true);
	int Channel;
	int Sweep = 1;
	float Interval;

private:
	typedef int(_stdcall* pABF_ReadOpen)(const char* szFileName, int* phFile, UINT fFlags, ABFFileHeader* pFH, UINT* puMaxSamples, DWORD* pdwMaxEpi, int* pnError);
	typedef int(_stdcall* pABF_ReadChannel)(int hFile, ABFFileHeader* pFH, int nChannel, unsigned long dwEpisode, float* pfBuffer, unsigned int* puNumsAMPLES, int* pnError);
	typedef int(_stdcall* pABF_Close)(int hFile, int* pnError);
	typedef int(_stdcall* pABF_GetWaveform)(int nFile, ABFFileHeader* pFH, int nChannel, unsigned long dwEpisode, float* pfBuffer, int* pnError);
	typedef int(_stdcall* pABF_WriteOpen)(const char* szFileName, int* phFile, UINT uFlags, ABFFileHeader* pFH, int* pnError);
	typedef int(_stdcall* pABF_MultiplexWrite)(int hFile, ABFFileHeader* pFH, UINT uFlags, void* pvBuffer, DWORD dwEpiStart, UINT uNumSamples, int* pnError);
	typedef int(_stdcall* pABF_WriteDACFileEpi)(int hFile, ABFFileHeader* pFH, short* pnDACArray, int* pnError);
	typedef int(_stdcall* pABF_UpdateHeader)(int hFile, ABFFileHeader* pFH, int* pnError);

	ABFFileHeader fh;
	const char* fn;
	int hfile;
	int error;
	unsigned int maxsamples;
	unsigned long maxepi;
	float* buffer=NULL;
	HINSTANCE module;

	pABF_ReadOpen ABF_ReadOpen;
	pABF_ReadChannel ABF_ReadChannel;
	pABF_Close ABF_Close;
	pABF_GetWaveform ABF_GetWaveform;
	pABF_WriteOpen ABF_WriteOpen;
	pABF_MultiplexWrite ABF_MultiplexWrite;
	pABF_WriteDACFileEpi ABF_WriteDACFileEpi;
	pABF_UpdateHeader ABF_UpdateHeader;
};


#endif //ABF_H