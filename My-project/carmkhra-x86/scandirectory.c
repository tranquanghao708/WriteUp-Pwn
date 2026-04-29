/*
	|-----------------------------------------------------------------|
	|					logic for directory scan					  |
	|-----------------------------------------------------------------|
*/

#include <dirent.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

//import thu muc header ben ngoai vao
#include "scandirectory.h"
#define levele 0

//hàm để duyệt thư mục
ssize_t directory_access(char *path,/*char *reader_binary,*/ int level){ //ssize_t hỗ trợ số âm
	char full[4096];
	DIR *direct = opendir(path);
	if(!direct){ //check kiem tra neu dir ko ton tai thi se loi
	fprintf(stdout,"[ERR]Error Dir '%s': %s\n",path,strerror(errno));

	return -1; //se co register RAX vi dung de tra ve gia tri
}

	struct dirent *entry;
    struct stat st;

    while ((entry = readdir(direct)) != NULL) {
        if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
            continue;

        snprintf(full, sizeof(full), "%s/%s", path, entry->d_name);

        if (lstat(full, &st) == -1) {
            perror("stat");
            continue;
        }

        if (S_ISDIR(st.st_mode)) { //nếu gặp thư mục zô tiếp

            printf("Thư mục: %s\n", full); //in  nếu không thì bỏ

            directory_access(full,/*cần thêm cơ chế đọc nhị phân các file ELF, binary và thêm vào mảng reader_binary để thuận cho việc phân tích nhị phân theo chữ ký lổ hổng*/ level + 1);  // Gọi đệ quy

        } else if (S_ISREG(st.st_mode)) { //if phải khác 0 mới TRUE còn bằng 0 là false, và S_ISREG là giá trị int trả về ở đây nếu nó phát hiên có file thì nó trả số khác 0 còn nếu ko phát hiện file nào thì mới trả 0

				if(!S_ISREG(st.st_mode)){

					fprintf(stderr,"[ERR] đường dẫn '%s' không phải là file", full);
					continue;

				}

				char *ext = strrchr(full,'.'); //đọc định dạng sau dấu chấm và gắn chuỗi vào ext
				//[9/2/2026]cải tiến: thêm cơ chế lọc file chỉ lọc file nhị phân
				if( ext && (strcmp(ext,".txt") || 
					//phan so sanh theo chu ky dinh dang
					strcmp(ext,".md") || 
					strcmp(ext,".png") || 
					strcmp(ext,".jpg") || 
					strcmp(ext,".c") || 
					strcmp(ext,".cpp")|| 
					strcmp(ext,".log") || 
					strcmp(ext,".aux") || 
					strcmp(ext,".mp3") || 
					strcmp(ext,".waw") || 
					strcmp(ext,".mp4")

					)){
					continue;
				}

            printf("File: %s\n", full);//in  nếu không thì bỏ 
            
        }
    }

	closedir(direct);
}

void directory_access_custom(/*char *reader_binary*/){
	printf("[INFO] Type your custom Path (ex: /home/user/ ) ? : ");
	char buffer_custom[1000]; //nằm trong section bss do chỉ khai báo mảng buffer chứ không gán giá trị gì vào nên nó sẽ mặc định có giá trị là 0
	
	//dùng fgets để an toàn hơn scanf
	fgets(buffer_custom,sizeof(buffer_custom),stdin);

	buffer_custom[strcspn(buffer_custom, "\n")] = 0; //xóa newline bởi vì khi ấn enter sẽ bị đính kèm theo newline \n gây bug tiềm ẩn và lỗi logic input, đã fix từ ngày 21/12/2026 và phát hiện bởi strace
	directory_access(buffer_custom, /*reader_binary,*/ levele); //lấy ra và gắn vào reader_binary
}
