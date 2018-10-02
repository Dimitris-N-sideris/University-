#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "column_store.h"

void Create_Field_Names(char **fieldNames);
void showFileOptions();
int readFromFile(HeaderInfo* header_info);


int main(void) {

	int option, error;
	char **fieldNames;
	char dbDirectory[1024];
	HeaderInfo header_info;

	CS_Init();
	if (getcwd(dbDirectory,1024) ==NULL){
		return 1;
	}
	strcat(dbDirectory, "/CS_FILES");
	fieldNames = malloc(Number_of_struct_members*sizeof(char*));
	header_info.column_fds = NULL;
	Create_Field_Names(fieldNames);

	do
	{
		showFileOptions();
		while(scanf("%d",&option) != 1)
		{
			while(getchar() != '\n');
			printf("Invalid choice (1-5), 6 to exit\n");
		}
		switch(option)
		{
			case 1:		//create CS files
			{
				error = CS_CreateFiles(fieldNames, dbDirectory);

				if (error == -1) {
					printf("\nA problem existed in creating files.\n");
					return error;
				}
				else {

					printf("\nI've just created the needed files for the application\n\n");
				}

				break;
			}
			case 2:		//open application's CS files
			{
				error = CS_OpenFile(&header_info, dbDirectory);

				if (error == -1) {
					printf("\nA problem occurred in opening files.\n");
					return error;
				}
				else if (error == -2) {

					printf("\nThe files are already open!!!\n");
				}
				else if (error == -3){

					printf("\nThe files haven't been created!!!\n\n");
				}
				else {

					printf("\nThe files have been opened!!!\n\n");
				}
				break;
			}
			case 3:		//insert data to application's CS files
			{
				error = readFromFile(&header_info);

				if (error == -1) {
					printf("\nA problem occurred in closing files.\n");
					return error;
				}
				else if (error == -2) {
					printf("\nYou haven't opened the CS files.\n\n");
				}
				else {

					printf("\nI just closed all open CS files.\n\n");
				}

				break;
			}
			case 4:		//search in application's CS files "value" records
			{
				int i;
				int numberOfFields;
				char **fields;
				char fieldName[15];
				char fieldValue[20];
				char buffer[25];

				printf("Enter number of fields to be printed.\n");
				scanf("%d", &numberOfFields);
				printf("Enter the appropriate number of field names (id, name, surname, status, dateOfBirth, salary)\n");
				fields = malloc(numberOfFields * sizeof(char*));
 				for(i = 0; i < numberOfFields; i++)
				{
					scanf("%s", buffer);
					fields[i] = malloc(sizeof(char) * strlen(buffer));
					strcpy(fields[i], buffer);
				}

				printf("Enter field's name (id, name, surname, status, dateOfBirth, salary): ");
				scanf("%s", fieldName);
				if(strcmp(fieldName, "name") == 0 || strcmp(fieldName, "surname") == 0 || strcmp(fieldName, "status") == 0
						|| strcmp(fieldName, "dateOfBirth") == 0 || strcmp(fieldName, "NULL") == 0)
				{
					printf("Enter field's value: ");
					scanf("%s", fieldValue);
					CS_GetAllEntries(&header_info, fieldName, fieldValue, fields, numberOfFields);

					for(i = 0; i < numberOfFields; i++)
					{
						free(fields[i]);
					}
					free(fields);

				}
				else if (strcmp(fieldName, "id") == 0 || strcmp(fieldName, "salary") == 0)
				{
					int value;
					printf("Enter field's value: ");
					scanf("%d", &value);
					CS_GetAllEntries(&header_info, fieldName, &value, fields, numberOfFields);

					for(i = 0; i < numberOfFields; i++)
					{
						free(fields[i]);
					}
					free(fields);
				}
				else
				{
					printf("Wrong field!\n");
				}
				break;
			}
			case 5:		//close application's CS files
			{
				error = CS_CloseFile(&header_info);

				if (error == -1) {
					printf("\nA problem occurred in closing files.\n");
					return error;
				}
				else if (error == -2) {
					printf("\nYou haven't opened the CS files.\n\n");
				}
				else {

					printf("\nI just closed all open CS files.\n\n");
				}

				break;
			}

		}
	}
	while (option != 6);	// Exit application.
	int i;
	for(i=0;i < Number_of_struct_members; i++){
		free(fieldNames[i]);
	}
	free(fieldNames);
	return 0;
}

void showFileOptions() {
	printf("1 : Create new CS files.\n"
		"2 : Open CS files.\n"
		"3 : Insert records from file.\n"
		"4 : Get all records (based on the value given).\n"
		"5 : Close CS files.\n"
		"6 : Exit.\n"
		"Give choice: ");
}

int readFromFile(HeaderInfo* header_info) {
	int error;
	Record record;
	FILE* fptr;
	char* pch;
	char* str;

	error = 0;
	fptr = NULL;

	fptr = fopen(Data_File, "r");

	if (fptr == NULL) {
		printf("Can't open data file");
		return -1;
	}

	str = (char*)malloc(sizeof(Record)*sizeof(char));

	if (header_info->column_fds == NULL)
		return -2;

	while (!feof(fptr)) {

		str[0] = '\0';

		if (fgets(str, sizeof(Record), fptr) == NULL)
			break;

		pch = strtok(str, " ,\"");
		record.id = atoi(pch);

		pch = strtok(NULL, " ,\"");
		strncpy(record.name, pch, size_of_struct_member2);

		pch = strtok(NULL, " ,\"");
		strncpy(record.surname, pch, size_of_struct_member3);

		pch = strtok(NULL, " ,\"");
		strncpy(record.status, pch, size_of_struct_member4);

		pch = strtok(NULL, " ,\"");
		strncpy(record.dateOfBirth, pch, size_of_struct_member5);

		pch = strtok(NULL, " ,\"");
		record.salary = atoi(pch);

		error = CS_InsertEntry(header_info, record);

		if (error < 0) {

			printf("Problem occured while inserting record in the block.");
			return -1;
		}
	}
	free(str);
	return 0;
}

void Create_Field_Names(char **fieldNames) {

	int i;
	char* str_of_members, *pch;

	str_of_members = malloc(sizeof(Struct_data_str));
	strcpy(str_of_members, Struct_data_str);

	pch = strtok(str_of_members, " ");

	for (i = 0; i < Number_of_struct_members; i++) {

		fieldNames[i] = malloc(Max_Member_Name*sizeof(char));
		strcpy(fieldNames[i], pch);
		pch = strtok(NULL, " ");
	}
	free(str_of_members);
}
