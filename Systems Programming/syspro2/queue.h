#ifndef QUEUE_H_GUARD
#define QUEUE_H_GUARD

#include    <stdio.h>
#include    <stdlib.h>
#include 	<unistd.h>
#include    <string.h>
#include    <ctype.h>
#include    <fcntl.h>
#include    <errno.h>
#include    <signal.h>
#include 	<sys/types.h>
#include	<dirent.h>
#include 	<sys/stat.h>


#define PROC "/proc/"


#define CREATEHEADER	1
#define GETMESSGEHEADER 2
#define SHUTDOWNHEADER  3
#define TYPE_FILE		4
#define TYPE_MESS		5
#define DUPLICATE		6
#define SHUTDOWN_SUCC	7
#define GET_MESSAGES	8
#define CREAT_SUCCESS   9
#define LISTHEADER     	10
#define WRITEHEADER		11
#define SENDHEADER		12
#define POSTSUCCESS		13
#define LISTANWSER		14
#define OPENFIFO		15
#define CLOSEFIFO		16
#define SEND_SUCCESS	17
#define WRITE_SUCCESS	18
#define NAME 			25

#define MEMERROR		-55
#define ENDOFFILE		-42
#define NotEnoughArgs 	-1
#define ErrorCrtdir 	-2
#define PipeError		-3
#define ErrorOpen		-4
#define SHUTDOWN_FAIL 	-5
#define WRITE_FAIL		-6
#define SEND_FAIL		-7	
#define LISTFAIL		-8
#define CHANNNOTEXIST   -9
#define COMMANDNOTFOUND -10

#define CHARLIMIT 		1024
#define TIMEOUT			1000
#define PERMS			0711
#define PIPEPERMS		0660
#define ServerID		"_Spid"
#define Client2Server 	"_C2S"
#define Server2Client 	"_S2C"
#define Board2Server 	"_B2S"
#define Server2Board 	"_S2B"
#define newName			"1"

#define CREATECHANNEL   "createchannel"
#define GETMESSAGES 	"getmessages"
#define SHUTDOWN 		"shutdown"
#define EXIT 			"exit"

#define LIST 			"list"
#define WRITE 			"write"
#define SEND 			"send"

typedef struct message_list{
	int 					type;
	char 					message[CHARLIMIT]; 
	struct message_list * 	next;
}message_list;


typedef struct head{
	int 			channel_id;
	char  			channel_name[NAME];
	message_list * 	last;
	message_list * 	first;
	struct head* 	next;
}head;

message_list* create_message(int typ,char* mes);
head*		 find_channel(head * node,int id);

head* 	create_channel(int chan_id,char* chan_name);

int 	add_channel(head **node,int chan_id,char* chan_name);
int 	add_mes(head * node,int id,int typ,char * mes);
int 	write_channels(head * node);
int 	write_mess(message_list* target);
int 	smaller_is(int sizeA,int sizeB);
int 	read_pipe(int filed,char*path,head* start,char* server2post,int *write_board);
int 	write_file(int filed,char*path,char* filename,int sizeofFILE);
int	 	readmsg(int read_fd,char * result);
int	 	readfile(int read_fd,char * filename,char* path);
int 	crtPath(char** target,char * path, char * next);

void    print_info(int answer);
void 	write_channel(int filed,head* start);
void	decodepath(char * path, char *directory, char* board_id);
void	write_message(int filed,head* start,int target,char *path);



#endif