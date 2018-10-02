#include "BF.h"
#include "column_store.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


int insertEntry(int FileDesc,int sizeOfEntry,void * entry);

void CS_Init() {

	BF_Init();
}

int CS_CreateFiles(char **fieldNames, char *dbDirectory) {

	int i, filedesc, error;
	void* buffer;
	char *cs_file_id, *new_directory, *cs_file_names;

	//=============================== Initialize the variables needed ===============================

	filedesc = error = 0;
	cs_file_id = malloc((strlen(Column_Store_Identifier) + Max_Member_Name)*sizeof(char));
	cs_file_names = malloc((strlen("CSFile_") + Max_Member_Name)*sizeof(char));

	if (dbDirectory != NULL) {			// If there is given directory.
		new_directory = malloc((strlen(dbDirectory) + Max_Member_Name)*sizeof(char));
	}
	else {								// If there is no given directory, then files will be created in the current working directory.
		new_directory = malloc(Max_Member_Name*sizeof(char));
		new_directory[0] = '\0';
	}

	//===============================================================================================

	//================================== Create Column Store Files ==================================

	for (i = 0; i < Number_of_struct_members; i++) {

		if (dbDirectory != NULL) {

			strcpy(new_directory, dbDirectory);
			strcat(new_directory, "/");
		}

		/*Create the directory path including the new file*/
		strcat(new_directory, "CSFile_");
		strcat(new_directory, fieldNames[i]);
		/*************************************************/

		/*Create the Full indetifier for each file based on their name*/
		strcpy(cs_file_id, Column_Store_Identifier);
		strcat(cs_file_id, "CSFile_");
		strcat(cs_file_id, fieldNames[i]);
		/*************************************************************/

		error = BF_CreateFile(new_directory);		// Create file based on the dbDirectory.

		if (error < 0) {							// In case of an error print appropriate message.

			BF_PrintError("Problem occured while creating file.");
			return -1;
		}

		filedesc = BF_OpenFile(new_directory);		// Open file that is in dbDirectory.

		if (filedesc < 0) {							// In case of an error print appropriate message.						

			BF_PrintError("Problem occured while opening file.");
			return -1;
		}

		error = BF_AllocateBlock(filedesc);			// Allocate the indentification block.

		if (error < 0) {							// In case of an error print appropriate message.

			BF_PrintError("Problem occured while allocating new block.");
			return -1;
		}

		error = BF_ReadBlock(filedesc, 0, &buffer);	// Read the indentification block.

		if (error < 0) {							// In case of an error print appropriate message.

			BF_PrintError("Problem occured while reading block.");
			return -1;
		}

		memcpy(buffer, (void*)cs_file_id, strlen(cs_file_id));	// Write indetification data inside the block.

		error = BF_WriteBlock(filedesc, 0);			// Write block back to file.

		if (error < 0) {							// In case of an error print appropriate message.

			BF_PrintError("Problem occured while writing block.");
			return -1;
		}

		error = BF_CloseFile(filedesc);				// Close file.

		if (error < 0) {							// In case of an error print appropriate message.

			BF_PrintError("Problem occured while closing file.");
			return -1;
		}

		cs_file_id[0] = '\0';
		new_directory[0] = '\0';
	}

	//===============================================================================================

	//================================== Create Header Info File ====================================

	if (dbDirectory != NULL) {

		strcpy(new_directory, dbDirectory);
		strcat(new_directory, "/");
	}

	/*Create the directory path including the new file*/
	strcat(new_directory, Header_Info_File_Name);
	/*************************************************/

	/*Create the Full indetifier for each file based on their name*/
	strcpy(cs_file_id, Header_Info_Identifier);
	/*************************************************************/

	error = BF_CreateFile(new_directory);		// Create file based on the dbDirectory.

	if (error < 0) {							// In case of an error print appropriate message.

		BF_PrintError("Problem occured while creating file.");
		return -1;
	}

	filedesc = BF_OpenFile(new_directory);		// Open file that is in dbDirectory.

	if (filedesc < 0) {							// In case of an error print appropriate message.						

		BF_PrintError("Problem occured while opening file.");
		return -1;
	}

	error = BF_AllocateBlock(filedesc);			// Allocate the indentification block.

	if (error < 0) {							// In case of an error print appropriate message.

		BF_PrintError("Problem occured while allocating new block.");
		return -1;
	}

	error = BF_ReadBlock(filedesc, 0, &buffer);	// Read the indentification block.

	if (error < 0) {							// In case of an error print appropriate message.

		BF_PrintError("Problem occured while reading block.");
		return -1;
	}

	memcpy(buffer, (void*)cs_file_id, strlen(cs_file_id));	// Write indetification data inside the block.

	buffer += strlen(cs_file_id);

	for (i = 0; i < Number_of_struct_members; i++) {

		cs_file_names[0] = '\0';
		strcat(cs_file_names, "CSFile_");
		strcat(cs_file_names, fieldNames[i]);
		strcat(cs_file_names, " ");
		memcpy(buffer, (void*)cs_file_names, strlen(cs_file_names));	// Write indetification data inside the block.
		buffer += strlen("CSFile_");
		buffer += strlen(fieldNames[i]);
		buffer += 1;
	}

	error = BF_WriteBlock(filedesc, 0);			// Write block back to file.

	if (error < 0) {							// In case of an error print appropriate message.

		BF_PrintError("Problem occured while writing block.");
		return -1;
	}

	error = BF_CloseFile(filedesc);				// Close file.

	if (error < 0) {							// In case of an error print appropriate message.

		BF_PrintError("Problem occured while closing file.");
		return -1;
	}

	//===============================================================================================
	free(cs_file_id);
	free(new_directory);
	free(cs_file_names);
	return 0;
}

int CS_OpenFile(HeaderInfo* header_info, char *dbDirectory) {

	int i, filedesc, error, filedesc_header, identifier_length;
	void* buffer;
	char *cs_file_id, *new_directory, *cs_file_names, *pch, *temp_cs_file_id;

	//=============================== Initialize the variables needed ===============================

	filedesc_header = filedesc = error = identifier_length = 0;
	cs_file_id = malloc((strlen(Column_Store_Identifier) + Max_Member_Name)*sizeof(char));
	temp_cs_file_id = malloc((strlen(Column_Store_Identifier) + Max_Member_Name)*sizeof(char));
	cs_file_names = malloc((Number_of_struct_members*(strlen("CSFile_") + Max_Member_Name))*sizeof(char));

	if (dbDirectory != NULL) {			// If there is given directory.
		new_directory = malloc((strlen(dbDirectory) + Max_Member_Name)*sizeof(char));
	}
	else {								// If there is no given directory, then files will be created in the current working directory.
		new_directory = malloc(Max_Member_Name*sizeof(char));
		new_directory[0] = '\0';
	}

	//===============================================================================================

	//======================== Opening Header Info File and Checking Validity =======================

	if (dbDirectory != NULL) {							// Take path in which you'll find the needed file.

		strcpy(new_directory, dbDirectory);
		strcat(new_directory, "/");
	}

	strcat(new_directory, Header_Info_File_Name);		// Make path for file in directory.

	filedesc_header = BF_OpenFile(new_directory);		// Open file that is in dbDirectory.

	if (filedesc_header == BFE_FILENOTEXISTS)					// Check if we are asked to open non created files.
		return -3;

	if (filedesc_header < 0) {							// In case of an error print appropriate message.						

		BF_PrintError("Problem occured while opening file.");
		return -1;
	}

	error = BF_ReadBlock(filedesc_header, 0, &buffer);	// Read the indentification block.

	if (error < 0) {							// In case of an error print appropriate message.

		BF_PrintError("Problem occured while reading block.");
		return -1;
	}

	memcpy(cs_file_id, buffer, strlen(Header_Info_Identifier)); // Acquire needed data from block.

	if (strncmp(cs_file_id, Header_Info_Identifier, strlen(Header_Info_Identifier)) != 0) {		// Check for header info file's validity.

		printf("Couldn't identify Header Info file");
		return -1;
	}

	//===============================================================================================

	//======================== Opening Column Store File and Checking Validity ======================

	cs_file_id[0] = '\0';
	buffer += strlen(Header_Info_Identifier);

	memcpy(cs_file_names, buffer, Number_of_struct_members*(strlen("CSFile_") + Max_Member_Name));

	header_info->column_fds = malloc(Number_of_struct_members *sizeof(ColumnFds)); // Allocate the needed space for the every column file.

	new_directory[0] = '\0';

	pch = strtok(cs_file_names, " ");

	for (i = 0; i < Number_of_struct_members; i++) {		// For every Column File check.

		temp_cs_file_id[0] = '\0';

		if (dbDirectory != NULL) {							// Take path in which you'll find the needed file.

			strcpy(new_directory, dbDirectory);
			strcat(new_directory, "/");
		}

		/*Create the directory path including the new file*/
		strcat(new_directory, pch);
		/*************************************************/

		filedesc = BF_OpenFile(new_directory);		// Open file that is in dbDirectory.

		if (filedesc == BFE_FILEOPEN)				// Check if we are asked to open same files again.
			return -2;

		if (filedesc == BFE_FILENOTEXISTS)			// Check if we are asked to open non created files.
			return -3;

		if (filedesc < 0) {							// In case of an error print appropriate message.						

			BF_PrintError("Problem occured while opening file.");
			return -1;
		}
		
		error = BF_ReadBlock(filedesc, 0, &buffer);	// Read the indentification block.

		if (error < 0) {							// In case of an error print appropriate message.

			BF_PrintError("Problem occured while reading block.");
			return -1;
		}

		identifier_length = strlen(Column_Store_Identifier) + strlen(pch);	

		memcpy(cs_file_id, buffer, identifier_length);

		strcpy(temp_cs_file_id, Column_Store_Identifier);
		strcat(temp_cs_file_id, pch);

		if (strncmp(temp_cs_file_id, cs_file_id, identifier_length) != 0)  {	// Check for the validity of the file.

			printf("Couldn't identify CS file");
			return -1;
		}
		/*Store needed data*/
		header_info->column_fds[i].fd = filedesc;							
		header_info->column_fds[i].columnName = malloc(strlen(pch)*sizeof(char));
		strcpy(header_info->column_fds[i].columnName, pch);
		/******************/

		pch = strtok(NULL, " ");
		new_directory[0] = '\0';
	}

	//===============================================================================================

	//================================ Closing Header Info File =====================================

	error = BF_CloseFile(filedesc_header);				// Close file.

	if (error < 0) {							// In case of an error print appropriate message.

		BF_PrintError("Problem occured while closing file.");
		return -1;
	}

	//===============================================================================================
	free(cs_file_id);
	free(new_directory);
	free(cs_file_names);
	free(temp_cs_file_id);
 	return 0;
}

int CS_CloseFile(HeaderInfo* header_info) {

	int i, filedesc, error;

	filedesc = error = 0;

	if (header_info->column_fds == NULL) {
		return -2;
	}

	//================================ Closing Header Info File =====================================

	for (i = 0; i < Number_of_struct_members; i++) {// For every open CS file.

		filedesc = header_info->column_fds[i].fd;

		error = BF_CloseFile(filedesc);				// Close file.

		if (error < 0) {							// In case of an error print appropriate message.

			BF_PrintError("Problem occured while closing file.");
			return -1;
		}

	}
	for(i=0;i < Number_of_struct_members; i++){
		free(header_info->column_fds[i].columnName);
	}
	free(header_info->column_fds);
	header_info->column_fds = NULL;
	//===============================================================================================

  return 0;
}

int CS_InsertEntry(HeaderInfo* header_info, Record record) {
  int i;

  for(i=0;i<Number_of_struct_members;i++){
       
        int nextFileNum=header_info->column_fds[i].fd;

        switch(i){  // kalese thn insertEntry analoga me to data member
        case 0:
                if(insertEntry(nextFileNum,size_of_struct_member1,&(record.id))){
                    return -2;  
                }
                break;
            case 1:
                if(insertEntry(nextFileNum,size_of_struct_member2,record.name)){
                    return -3;  
                }
                break;
            case 2:
                if(insertEntry(nextFileNum,size_of_struct_member3,record.surname)){
                    return -4;  
                }
                break;
            case 3:
                if(insertEntry(nextFileNum,size_of_struct_member4,record.status)){
                    return -5;  
                }
                break;
            case 4:
                if(insertEntry(nextFileNum,size_of_struct_member5,record.dateOfBirth)){
                    return -6;  
                }
                break;
            case 5:
                if(insertEntry(nextFileNum,size_of_struct_member6,&(record.salary))){
                    return -7;  
                }
                break;
            default:
                printf("Number_of_struct_members bigger than usual\n");
        }

  }
  return 0;
} 

int insertEntry(int FileDesc,int sizeOfEntry,void * entry){
  
    int blockid=BF_GetBlockCounter(FileDesc);
   
    if(blockid==1){     // first block is only for info
        if(BF_AllocateBlock(FileDesc)<0){
            printf("Couldn't allocate block\n");
            return -5;
        }
        blockid++;
    }
    void *block;
    int  bytesused= 0;

    if(BF_ReadBlock(FileDesc,blockid-1,&block)<0){
        printf("Error reading block\n");
        return -6;
    }
   
    // Yparxei enas ari8mos sthn arxh kathe block pou 
    // deixnei posa bytes sto block xrhsimopoiountai

    memcpy(&bytesused,block,sizeof(int));    

    if((bytesused+sizeOfEntry)>BLOCK_SIZE){ // an dn xwraei sto block ftia3e kainourgio
        if(BF_AllocateBlock(FileDesc)<0){
            printf("Couldn't allocate block\n");
            return -5;
        }
        blockid++; 
        if(BF_ReadBlock(FileDesc,blockid-1,&block)<0){
            printf("Error reading block\n");
            return -6;
        }
        memcpy(&bytesused,block,sizeof(int));
    }
    char * temp=(char*)block;
   
    //  gia thn prwth eisagwgh gia na agnoh8ei o int
    if(bytesused==0){       
        bytesused+=sizeof(int);
    }
    
    memcpy(&temp[bytesused],entry,sizeOfEntry);
    bytesused+=sizeOfEntry;
    memcpy(&temp[0],&bytesused,sizeof(int));
    
    if(BF_WriteBlock(FileDesc,blockid-1)<0){     // write data in disk
        printf("couldn't write\n");
        return -7;
    }
    return 0;
}


void CS_GetAllEntries(HeaderInfo* header_info, char *fieldName, void *value,
                      char **fieldNames, int n)
{
	int i, j, printValue = 0, valueIndex = 0, numberOfFiles, numberOfBlocks = 0, numberOfResults = 0;

	for(i = 0; i < n ; i++)	//briskei to fieldName mesa sto pinaka fieldNames
	{						//an to brei to printValue pairnei thn timh 1
		if(strcmp(fieldNames[i], fieldName) == 0)//ara 8a ektupw8ei to pedio me to opoio sugkrinontai oi eggrafes
		{
			printValue = 1;
			valueIndex = i;
		}
	}

	if(printValue == 0)
		numberOfFiles = n + 1;	//ari8mos twn pediwn pou 8a diabastoun
	else
		numberOfFiles = n;


	char *columnName;
	char column[30];
	ScanInfo scan_info[numberOfFiles];	//pinakas pou krataei tis aparaithtes plhrofories gia ola ta pedia pou 8a e3etastoun

	for(i = 0; i < n; i++)	//apo8hkeuei tis aparaithtes plhrofories gia ka8e arxeio sto
	{						//pinaka scan_info
		for (j = 0; j < 6 ; j++)	//briskei to fd gia ka8e field
		{
			//to columnName sto header_info exei thn morfh CSFILE_columnname
			//afairei to CSFILE_ apo to string gia na ginei h sugkrish
			strcpy(column, header_info->column_fds[j].columnName);
			columnName = strtok(column, "_");
			columnName = strtok(NULL, "_");

			if(strcmp(columnName, fieldNames[i]) == 0)
			{
				numberOfBlocks++;
				scan_info[i].fd = header_info->column_fds[j].fd;
				scan_info[i].currentBlock = 1;
				BF_ReadBlock(scan_info[i].fd, scan_info[i].currentBlock, &scan_info[i].block);
				scan_info[i].block += sizeof(int);
				scan_info[i].remainingBytes = 1024 - sizeof(int);

				//apo8hkeuei to mege8os tou pediou
				if(strcmp(columnName, "id") == 0 || strcmp(columnName, "salary") == 0)
				{
					scan_info[i].fieldSize = sizeof(int);
				}
				else if (strcmp(columnName, "name") == 0)
				{
					scan_info[i].fieldSize = size_of_struct_member2 * sizeof(char);
				}
				else if (strcmp(columnName, "surname") == 0)
				{
					scan_info[i].fieldSize = size_of_struct_member3 * sizeof(char);
				}
				else if (strcmp(columnName, "status") == 0)
				{
					scan_info[i].fieldSize = size_of_struct_member4 * sizeof(char);
				}
				else if (strcmp(columnName, "dateOfBirth") == 0)
				{
					scan_info[i].fieldSize = size_of_struct_member5 * sizeof(char);
				}
				break;
			}
		}
	}


	//an to fieldName kai to value einai NULL ektupwse oles tis eggrafes
	if(strcmp(fieldName, "NULL") == 0 || strcmp(value, "NULL") == 0)
	{
		char zeroBlock[scan_info[0].fieldSize];
		memset(zeroBlock, 0, scan_info[0].fieldSize);	//zero record

		while(memcmp(scan_info[0].block, zeroBlock, scan_info[0].fieldSize) != 0)
		{
			numberOfResults++;
			PrintColumns(fieldNames, n, &scan_info[0]);

			for(i = 0; i < n; i++)
			{
				scan_info[i].remainingBytes -= scan_info[i].fieldSize;
				scan_info[i].block += scan_info[i].fieldSize;
				if(scan_info[i].remainingBytes - scan_info[i].fieldSize < 0)
				{
					scan_info[i].currentBlock++;
					scan_info[i].remainingBytes = 1024 - sizeof(int);
					BF_ReadBlock(scan_info[i].fd, scan_info[i].currentBlock, &scan_info[i].block);
					scan_info[i].block += sizeof(int);
					numberOfBlocks++;
				}
			}
		}

		printf("Number of results: %d\n", numberOfResults);
		printf("Number of blocks: %d\n", numberOfBlocks);

		return;
	}


	//pros8ese sto telos tou pinaka scan_info to pedio fieldName
	if(printValue == 0)
	{
		valueIndex = numberOfFiles - 1;
		scan_info[valueIndex].currentBlock = 1;
		for (j = 0; j < 6 ; j++)	//briskei to fd gia to pedio fieldName
		{
			strcpy(column, header_info->column_fds[j].columnName);
			columnName = strtok(column, "_");
			columnName = strtok(NULL, "_");

			if(strcmp(columnName, fieldName) == 0)
			{
				numberOfBlocks++;
				scan_info[valueIndex].fd = header_info->column_fds[j].fd;
				scan_info[valueIndex].currentBlock = 1;
				BF_ReadBlock(scan_info[valueIndex].fd, scan_info[valueIndex].currentBlock, &scan_info[valueIndex].block);
				scan_info[valueIndex].block += sizeof(int);
				scan_info[valueIndex].remainingBytes = 1024 - sizeof(int);
				if(strcmp(columnName, "id") == 0 || strcmp(columnName, "salary") == 0)
				{
					scan_info[i].fieldSize = sizeof(int);
				}
				else if (strcmp(columnName, "name") == 0)
				{
					scan_info[i].fieldSize = size_of_struct_member2 * sizeof(char);
				}
				else if (strcmp(columnName, "surname") == 0)
				{
					scan_info[i].fieldSize = size_of_struct_member3 * sizeof(char);
				}
				else if (strcmp(columnName, "status") == 0)
				{
					scan_info[i].fieldSize = size_of_struct_member4 * sizeof(char);
				}
				else if (strcmp(columnName, "dateOfBirth") == 0)
				{
					scan_info[i].fieldSize = size_of_struct_member5 * sizeof(char);
				}
				break;
			}
		}
	}

	int (*Compare_Function)(void *, void *);

	if(strcmp(fieldName, "id") == 0 || strcmp(fieldName, "salary") == 0)
	{
		Compare_Function = CompareInt;
	}
	else
	{
		Compare_Function = CompareString;
	}

	char zeroBlock[scan_info[valueIndex].fieldSize];
	memset(zeroBlock, 0, scan_info[valueIndex].fieldSize);	//zero record




	printf("========================= Results =========================\n");
	while(memcmp(scan_info[valueIndex].block, zeroBlock, scan_info[valueIndex].fieldSize) != 0)
	{
		if(Compare_Function(scan_info[valueIndex].block, value) == 1)
		{
			numberOfResults++;
			PrintColumns(fieldNames, n, &scan_info[0]);
		}

		for(i = 0; i < numberOfFiles; i++)
		{
			scan_info[i].remainingBytes -= scan_info[i].fieldSize;
			scan_info[i].block += scan_info[i].fieldSize;
			if(scan_info[i].remainingBytes - scan_info[i].fieldSize < 0)
			{
				scan_info[i].currentBlock++;
				scan_info[i].remainingBytes = 1024 - sizeof(int);
				BF_ReadBlock(scan_info[i].fd, scan_info[i].currentBlock, &scan_info[i].block);
				scan_info[i].block += sizeof(int);
				numberOfBlocks++;
			}
		}
	}
	printf("===========================================================\n");
	printf("Number of results: %d\n", numberOfResults);
	printf("Number of blocks: %d\n", numberOfBlocks);


}

void PrintColumns(char **fieldNames, int n, ScanInfo *scan_info)
{
	int i;
	for(i = 0; i < n; i++)
	{
		if(strcmp(fieldNames[i], "id") == 0 || strcmp(fieldNames[i], "salary") == 0)
		{
			printf("%s: %d |  ", fieldNames[i], *(int *)scan_info[i].block);
		}
		else
		{
			char string[20];
			strncpy(string, scan_info[i].block, scan_info[i].fieldSize);
			printf("%s: %s  |  ", fieldNames[i], string);
		}
	}
	printf("\n");
}


int CompareInt(void *value1, void * value2)
{
	if(memcmp(value1, value2, sizeof(int)) == 0)
		return 1;
	else
		return 0;
}

int CompareString(void * value1, void *value2)
{
	if(memcmp(value1, value2, strlen(value1)) == 0)
		return 1;
	else
		return 0;
}
