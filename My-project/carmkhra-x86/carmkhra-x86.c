/*
	____________________________________________________________________________
	|		                 ******PROJRECT******								|
	|	-> day chinh la cong cu carmkhra-x86 quet lo hong, du an dau tien cua toi			|
	|	-> se duoc phat trien 1 minh											|
	|		                 ******TAC GIA*******								|
	|----------------------------TRAN QUANG HAO----------------------------------

	ngay bat dau che tao: 12/1/2026

*/
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define levele 0

#include "scandirectory.h"

void tittle(){
		printf("_________________________________________________________________________\n|\t\t\t\tCarmKhra-X86\t\t\t\t|\n_________________________________________________________________________\n\n\n\n");
} //chương trình sẽ có thể mở stack frame cho hàm tittle

void help(){
	printf( "HELP:\n\n\t"
			"SYNTAX: carmkhra-x86 [option] [thunar]\n\t"
			"-H , --Human-Display: show ascii display out terminal\n\t"
			"-h : show help panel\n\t"
			"-C [directory], --chosen [directory] : chose target directory need scan\n\t"
	);

	printf("\n");
	return;
} //bang tro giup


//hàm checkroot để kiểm tra có quyền super user
ssize_t check_root(){ //chắc chắn có stack frame để có thể return address

	// [8/2/2026] cải tiến thêm RUID và EUID checker để kiểm tra root, vì RUID là thông số UID thực tế và EUID là thông số UID của quyền hạn thực tế
	const int euid = geteuid(); // cái này chắc chắn sẽ nằm trong .rodata vì có const
	const int ruid = getuid(); 
	if(euid != 0 || ruid != 0){
		fprintf(stderr,"[ERR] System non root [RUID = %d , EUID = %d], error code: %s\n",ruid,euid,strerror(errno));
		exit(1); //[8/2/2026] cải tiến thay return thành exit, vì return chỉ là trả về giá trị cho hàm và thoát ở mức hàm chứ không phải thoát cả chương trình
	}

	FILE *open = fopen("/usr/bin/ls","r+"); 
	
	if(open == NULL){ //đã fix lỗi từ ngay 21/1/2026 vì lỗi segfault và đã phát hiện qua gdb
		fprintf(stderr,"[ERR] System non root [Failed access reading in Path /usr/bin/ls], error code: %s\n",strerror(errno));
		exit(1); //[8/2/2026] cải tiến thay return thành exit, vì return chỉ là trả về giá trị cho hàm và thoát ở mức hàm chứ không phải thoát cả chương trình
	}

	fclose(open); //call fclose
	
	return 0;
}

int display_human(){
	//compiler co the se lay canary tu tls de them vao nham chong buffer overflow
	tittle();

	char bufferquestion[10]; //[9/2/2026] QUES: vi sao khi thay 10 thanh 1, no lai ko hien shell de cho user nhap thay vao do no tu dong hien default du chua nhap gi vao het
	printf("[INFO] You need chosen argument :\n\t[1] Scan vuln in folder here\n\t[2] Scan vuln in whole Operation System, need root\n\t[3] Scan vuln in folder custom\n\n[QUES] What you chose?, chose 1-3? : ");

	fgets(bufferquestion,sizeof(bufferquestion),stdin); //dung fgets thay cho scanf de tang tinh bao mat

	switch (bufferquestion[0]){
		case '1': printf("[INFO]starting execute argument 1\n");directory_access(".",/*buffer_binary,*/levele); break;
		case '2': printf("[INFO]starting execute argument 2\n");check_root();directory_access("/",/*buffer_binary,*/levele);break;
		case '3': printf("[INFO]starting execute argument 3\n");directory_access_custom(/*buffer_binary*/);break;
		default:  printf("[ERR] you need chose one argument, or argument invaild. EXIT\n");return 0;
}}

int main(int argc, char *argv[]){
	

	//dieu kien kiem tra khi argument dung cu phap
	if(strcmp(argv[1], "-h") == 0 ){
		help();
	}else if(strcmp(argv[1], "-H") == 0){
		display_human();
	}else if(strcmp(argv[1], "-C") == 0){
		directory_access(argv[2],/*buffer_binary,*/levele);
	}else if(strcmp(argv[1], "--Human-Display") == 0){
		display_human();
	}else if(strcmp(argv[1], "--chosen") == 0){
		directory_access(argv[2],/*buffer_binary,*/levele);
	}

	//dieu kien kiem tra khi argument sai cu phap
	if(strcmp(argv[1], "-h") != 0 ){
		help();
	}else if(strcmp(argv[1], "-H") != 0){
		help();
	}else if(strcmp(argv[1], "-C") != 0){
		help();
	}else if(strcmp(argv[1], "--Human-Display") != 0){
		help();
	}else if(strcmp(argv[1], "--chosen") != 0){
		help();
 	} //bug: vi sao khi nhap sai hoac tham chi dung syntax thi cai nay van la TRUE

	char *buffer_binary = malloc(1000); //cho cat tru info binary vao

	free(buffer_binary);
	return 0;
}
