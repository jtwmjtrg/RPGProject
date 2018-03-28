#define _CRT_SECURE_NO_WARNINGS
#include "wave.h"

Wave::Wave()
{
	memset(&WaveFormat, 0, sizeof(WAVEFORMATEX));
	WaveData = NULL;
	DataSize = 0;
}
Wave::~Wave() {}

bool Wave::Load(TCHAR* FileName)
{
	// �o�C�i���ǂݍ��݃��[�h�ŊJ��
	FILE* fp;
	if (!(fp = ::_tfopen(FileName, _T("rb"))))
		return false;

	char chunkId[5] = {};
	char tmp[5] = {};
	unsigned int chunkSize = 0;

	// RIFF�`�����N�ǂݍ���
	fread(chunkId, sizeof(char) * 4, 1, fp);
	fread(&chunkSize, sizeof(unsigned int), 1, fp);
	fread(tmp, sizeof(char) * 4, 1, fp);
	if (strcmp(chunkId, "RIFF") || strcmp(tmp, "WAVE")) {
		return false;	// Wave�t�@�C������Ȃ�
	}

	// �q�`�����N�ǂݍ���
	bool fmtchunk = false;
	bool datachunk = false;
	while (true)
	{
		fread(chunkId, sizeof(char) * 4, 1, fp);
		fread(&chunkSize, sizeof(unsigned int), 1, fp);
		if (!strcmp(chunkId, "fmt "))
		{
			if (chunkSize >= sizeof(WAVEFORMATEX))
			{
				fread(&WaveFormat, sizeof(WAVEFORMATEX), 1, fp);
				int diff = chunkSize - sizeof(WAVEFORMATEX);
				fseek(fp, diff, SEEK_CUR);
			}
			else
			{
				memset(&WaveFormat, 0, sizeof(WAVEFORMATEX));
				fread(&WaveFormat, chunkSize, 1, fp);
			}
			fmtchunk = true;
		}
		else if (!strcmp(chunkId, "data"))
		{
			// �f�[�^�T�C�Y�m��
			DataSize = chunkSize;
			WaveData = new byte[chunkSize];
			// �f�[�^�ǂݍ���
			if (fread(WaveData, sizeof(byte), chunkSize, fp) != chunkSize)
			{
				fclose(fp);
				return false;	// �t�@�C�������Ă���
			}
			datachunk = true;
		}
		else
		{
			fseek(fp, chunkSize, SEEK_CUR);
		}

		if (fmtchunk && datachunk)
			break;

		// �t�@�C���T�C�Y�`�F�b�N���s�Ȃ���������������
	}

	fclose(fp);
	return true;
}

void Wave::Delete()
{
	if (WaveData != NULL)
	{
		delete[] WaveData;
	}
}