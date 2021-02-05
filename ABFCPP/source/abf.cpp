#include "abf.h"

ABF::ABF(std::string f, unsigned int n) {
	fn = f;
	error = 0;
	hfile = 1;
	maxsamples = n;
	maxepi = 0;
	module = LoadLibraryA("../ABFCPP/bin/ABFFIO.dll");
	ABF_ReadOpen = (pABF_ReadOpen)GetProcAddress(module, "ABF_ReadOpen");
	ABF_Close = (pABF_Close)GetProcAddress(module, "ABF_Close");
	ABF_ReadChannel = (pABF_ReadChannel)GetProcAddress(module, "ABF_ReadChannel");
	ABF_GetWaveform = (pABF_GetWaveform)GetProcAddress(module, "ABF_GetWaveform");
	ABF_WriteOpen = (pABF_WriteOpen)GetProcAddress(module, "ABF_WriteOpen");
	ABF_MultiplexWrite = (pABF_MultiplexWrite)GetProcAddress(module, "ABF_MultiplexWrite");
	ABF_WriteDACFileEpi = (pABF_WriteDACFileEpi)GetProcAddress(module, "ABF_WriteDACFileEpi");
	ABF_UpdateHeader = (pABF_UpdateHeader)GetProcAddress(module, "ABF_UpdateHeader");
	//ABF_SynchCountFromEpisode = (pABF_SynchCountFromEpisode)GetProcAddress(module, "ABF_SynchCountFromEpisode");
	ABF_MultiplexRead = (pABF_MultiplexRead)GetProcAddress(module, "ABF_MultiplexRead");
	ABF_ReadDACFileEpi = (pABF_ReadDACFileEpi)GetProcAddress(module, "ABF_ReadDACFileEpi");
	ABF_ReadOpen(fn.c_str(), &hfile, ABF_DATAFILE, &fh, &maxsamples, &maxepi, &error);
	if (fh.nOperationMode == 3) {
		buffer = new float[maxsamples * maxepi];
	}
	else if (fh.nOperationMode == 5) {
		buffer = new float[maxsamples];
	}
	Channel = fh.nADCNumChannels;
	if (fh.nOperationMode == 5) {
		Sweep = fh.lActualEpisodes;
	}
	Interval = fh.fADCSequenceInterval;
	ABF_Close(hfile, &error);
}

ABF::~ABF(){
	if (buffer == NULL) {
		return;
	}
	ABF_Close(hfile, &error);
	FreeLibrary(module);
	delete[] buffer;
	buffer = NULL;
}

std::vector<float> ABF::data(int c, int s, bool m) {
	ABF_ReadOpen(fn.c_str(), &hfile, ABF_DATAFILE, &fh, &maxsamples, &maxepi, &error);
	std::vector<float> t;
	if (buffer == NULL) {
		return t;
	}
	//ABF_SynchCountFromEpisode(hfile, &fh, s, &synstart, &error);
	if (fh.nOperationMode == 3 && m) {
		float* res = buffer;
		unsigned int numsamples = maxsamples;
		for (unsigned int i = 1; i <= maxepi; i++) {
			ABF_ReadChannel(hfile, &fh, c, i, res, &numsamples, &error);
			res += numsamples;
		}
		t = std::vector<float>(buffer, res);
	}
	else if (fh.nOperationMode == 5 && m) {
		ABF_ReadChannel(hfile, &fh, c, s, buffer, &maxsamples, &error);
		t = std::vector<float>(buffer, buffer + maxsamples);
	}
	else if (fh.nOperationMode == 3 && !m) {
		float* res = buffer;
		for (unsigned int i = 1; i <= maxepi; i++) {
			ABF_GetWaveform(hfile, &fh, c, i, res, &error);
			res += maxsamples;
		};
		t = std::vector<float>(buffer, buffer + fh.lActualAcqLength/fh.nADCNumChannels);
	}
	else if (fh.nOperationMode == 5 && !m) {
		ABF_GetWaveform(hfile, &fh, c, s, buffer, &error);
		t = std::vector<float>(buffer, buffer + maxsamples);
	}
	ABF_Close(hfile, &error);
	return t;
}

void ABF::save(std::vector<unsigned int>& start, std::vector<unsigned int>& end) {
	if (fh.nOperationMode != 3) {
		return;
	}
	int phfile = 0;
	std::string fnout = "_new.abf";
	fnout.insert(0, fn, 0, fn.size() - 4);
	unsigned int numsamples = maxsamples;
	ABF_ReadOpen(fn.c_str(), &hfile, ABF_DATAFILE, &fh, &numsamples, &maxepi, &error);
	ABF_WriteOpen(fnout.c_str(), &phfile, ABF_DATAFILE, &fh, &error);
	if (fh.nDataFormat == 0) {
		short* pnBuffer = new short[maxsamples * maxepi];
		short* res = pnBuffer;
		for (unsigned int i = 1; i <= 1; i++) {
			ABF_MultiplexRead(hfile, &fh, i, pnBuffer, &numsamples, &error);
			res += numsamples;
		}
		int i = 0;
		int j = 0;
		int flag = start[j];
		while(i < res - pnBuffer) {
			if (flag >= end[j]) {
				j++;
				if (i >= start.size()) {
					break;
				}
				flag = start[j];
			}
			if (flag > i) {
				pnBuffer[i] = pnBuffer[flag];
			}
			flag++;
			i++;
		}
		ABF_MultiplexWrite(phfile, &fh, ABF_DATAFILE, pnBuffer, 0, i, &error);
		delete[] pnBuffer;
	}
	else if (fh.nDataFormat == 1) {
		float* res = buffer;
		for (unsigned int i = 1; i <= maxepi; i++) {
			ABF_MultiplexRead(hfile, &fh, i, buffer, &numsamples, &error);
			res += numsamples;
		}
		int i = 0;
		int j = 0;
		int flag = start[j];
		while (i < res - buffer) {
			if (flag >= end[j]) {
				j++;
				if (i >= start.size()) {
					break;
				}
				flag = start[j];
			}
			if (flag > i) {
				buffer[i] = buffer[flag];
			}
			flag++;
			i++;
		}
		ABF_MultiplexWrite(phfile, &fh, ABF_DATAFILE, buffer, 0, i, &error);
	}
	ABF_UpdateHeader(phfile, &fh, &error);
	ABF_Close(hfile, &error);
	ABF_Close(phfile, &error);
	return;
}
