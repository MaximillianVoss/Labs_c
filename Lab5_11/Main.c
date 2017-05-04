#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<locale.h>
#include<Windows.h>
#pragma warning (disable:4996)
#pragma region Gloals
//5 define'ов: имя - значение;
char values[10][80];
//номера строк define
int lineN[10];
//строка текста
char line[255];
//количество строк
size_t linesCount;
//количество define's
size_t dCount;
#pragma endregion
#pragma region File
char * ReadFromFile(char *filename, int index)
{
	FILE *fp = fopen(filename, "rt");
	if (fp == NULL)
	{
		puts("Открыть файл не удалось\n");
		exit(1);
	}
	else
	{
		int i = 0;
		while (fgets(line, 255, fp))
		{
			if (i != index)
			{
				strcpy(line, "");
				i++;
			}
			else
			{
				fclose(fp);
				return line;
			}
		}
		fclose(fp);
	}
}
int GetLinesCount(char*filename)
{
	FILE *fp;
	fp = fopen(filename, "rt");
	if (fp == NULL)
	{
		puts("Открыть файл не удалось\n");
		exit(1);
	}
	else
	{
		char line[255];
		int i = 0;
		while (fgets(line, 255, fp))
			i++;
		fclose(fp);
		return i;
	}
}
void WriteToFile(char*filename, char *line)
{
	FILE *fp = fopen(filename, "a+");
	if (fp == NULL)
	{
		puts("Открыть файл не удалось\n");
		exit(1);
	}
	else
	{
		fputs(line, fp);
		fclose(fp);
	}
}
#pragma endregion
#pragma region Defines
char** GetDefineValues(char* str, int counter)
{
	if (strstr(str, "#define") != NULL)
	{
		//буффер
		static char buffer[255];
		strcpy(buffer, "");
		char *temp = strstr(str, " ");
		while (*temp == ' ')
			temp++;
		while (*temp != ' ')
		{
			char symbol[2];
			strncpy(symbol, temp, 1);
			symbol[1] = '\0';
			strcat(buffer, symbol);
			temp++;
		}
		strcpy(values[counter], buffer);
		while (*temp == ' ')
			temp++;
		strcpy(buffer, "");
		while (*temp != ' ' && *temp != '\n')
		{
			static char symbol[1];
			strncpy(symbol, temp, 1);
			strcat(buffer, symbol);
			temp++;
		}
		strcpy(values[counter + 1], buffer);
		return values;
	}
	return NULL;
}
void GetDefines(char*filename)
{
	int length = GetLinesCount(filename);
	int j = 0, k = 0;
	for (int i = 0;i < length;i++)
	{
		char*line = ReadFromFile(filename, i);
		if (strstr(line, "#define") != NULL)
		{
			GetDefineValues(line, j);
			lineN[k++] = i;
			j += 2;
			dCount++;
		}
	}
}
#pragma endregion
#pragma region Replace
char*ReplaceStr(char *str, char *orig, char *rep)
{
	static char buffer[1024];
	strcpy(buffer, "");
	char *p;
	int i = 0;

	if (!(p = strstr(str + i, orig)))
	{
		return str;
	}

	while (str[i])
	{
		if (!(p = strstr(str + i, orig)))
		{
			strcat(buffer, str + i);
			break;
		}
		strncpy(buffer + strlen(buffer), str + i, (p - str) - i);
		buffer[p - str] = '\0';
		strcat(buffer, rep);
		i = (p - str) + strlen(orig);
	}
	return buffer;
}
void Replace(char*infilename, char*outfilename)
{
	int length = GetLinesCount(infilename);
	FILE *fp = fopen(outfilename, "w");
	fclose(fp);
	for (int j = 0;j < length;j++)
	{
		int flag = 1;
		for (int k = 0;k < dCount;k++)
			if (lineN[k] == j)
				flag = 0;
		char*line = ReadFromFile(infilename, j);
		if (flag)
		{
			for (int i = 0;i < dCount * 2;i += 2)
			{
				char *name = values[i], *value = values[i + 1];
				strcpy(line, ReplaceStr(line, name, value));
			}
			printf("%s\n", line);
			WriteToFile(outfilename, line);
		}
		else
		{
			printf("%s\n", line);
			WriteToFile(outfilename, line);
		}
	}
}
#pragma endregion

int main()
{
	setlocale(LC_ALL, "rus");
	GetDefines("in.txt");
	Replace("in.txt", "out.txt");
	system("pause");
}