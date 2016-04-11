#include <cstdlib>
#include <iostream>
#include <string>
#include <time.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/file.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>

using namespace std;

int DIGITS=4;    //how many digits of number the game is going to play with 4~10 default is 4
const char* Originquestion;    //宣告問題數字陣列
const char* your_answer;
void Task1(u_long* question);    //隨機產生 DIGITS 個不同的數字 int 
void Task2(u_long* question);
void guess(u_long* your_answer, u_long* question);    //猜數字，由使用者輸入數字 
u_long compare(u_long* your_answer, u_long* question,u_long* A,u_long* B);    //比較是否正確  

int main(int argc, char *argv[])
{
	 pid_t pid,parent, child;
	//int is_correct=0; 
	int count=1;
    //int your_answer[6];    //宣告猜測數字陣列
    //整數值：辨別所猜數字是否正確
    //計算猜了幾次 
	int task=0;
	void *ptr_question;
	void *ptr_answer;
	void *ptr_A;
	void *ptr_B;
	void *ptr_C;
	const size_t region_size = sysconf(_SC_PAGE_SIZE);
	/*shm_open*/
	int fd1 = shm_open("question", O_CREAT|O_TRUNC|O_RDWR, 0666);
	if(fd1 == -1) cout << "shm_open error\n";
	int fd2 = shm_open("your_answer", O_CREAT|O_TRUNC|O_RDWR, 0666);
	if(fd2 == -1) cout << "shm_open error\n";
	int fd3 = shm_open("is_correct", O_CREAT|O_TRUNC|O_RDWR, 0666 );
	if(fd3 == -1) cout << "shm_open error\n";
	int fd4 = shm_open("xxxxxxx", O_CREAT|O_TRUNC|O_RDWR, 0666 );
	if(fd4 == -1) cout << "shm_open error\n";
	int fd5 = shm_open("yyyyyy", O_CREAT|O_TRUNC|O_RDWR, 0666 );
	if(fd5 == -1) cout << "shm_open error\n";
    /*ftruncate*/
	int r = ftruncate(fd1, region_size);
	if(r != 0 ) cout << "assign size error 1\n";
	r = ftruncate(fd2, region_size);
	if(r != 0 ) cout << "assign size error 2\n";
	r = ftruncate(fd3, sizeof(int));
	if(r != 0 ) cout << "assign size error 3\n";
	r = ftruncate(fd4, sizeof(int));
	if(r != 0 ) cout << "assign size error 4\n";
	r = ftruncate(fd5, sizeof(int));
	if(r != 0 ) cout << "assign size error 5\n";
	/*mmap*/
	ptr_question = mmap(NULL, region_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd1, 0);
	if(ptr_question == MAP_FAILED) cout << "mmap error\n";
	close(fd1);
	ptr_answer = mmap(NULL, region_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd2, 0);
	if(ptr_answer == MAP_FAILED) cout << "mmap error\n";
	close(fd2);
	ptr_A = mmap(NULL, sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED, fd3, 0);
	if(ptr_A == MAP_FAILED) cout << "mmap error\n";
	close(fd3);	
	ptr_B = mmap(NULL, sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED, fd4, 0);
	if(ptr_B == MAP_FAILED) cout << "mmap error\n";
	close(fd4);
	ptr_C = mmap(NULL, sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED, fd5, 0);
	if(ptr_C == MAP_FAILED) cout << "mmap error\n";
	close(fd5);
    /*assign to u_long variable*/
	u_long *answer = (u_long *) ptr_answer;
	u_long *question = (u_long *) ptr_question;
	u_long *is_correct = (u_long *) ptr_A;
	u_long *A = (u_long *) ptr_B;
	u_long *B = (u_long *) ptr_C;
	
	*is_correct = 0;    /*initial is_correct flag*/
	
	do{
        cout << "請輸入要猜幾位數？(4 ~ 10)\n";
        cin >> DIGITS;
    }while(DIGITS < 4 || DIGITS > 10);
	 
	do{
		cout << "task1 or task2?(please enter 1 or 2 )\n";
		cin >> task;
    }while(task != 1 && task != 2 );
	
	switch(task)
	 {
		case 1:
			Task1(question);
			break;
		case 2:
			Task2(question);
			break;
	}		
	
    cout << "請輸入" << DIGITS << "個不同的數字，且第一個數字不為零。輸入 enter 則顯示答案 \n";
	cout << "\n*********";
	for(int i=0;i<DIGITS;i++)
		cout << question[i];
	cout << "*******\n";

	cin.ignore();
	/*first, run input request one time*/
	parent = getpid();
	cout << "[" << parent << " User] : ";
	guess(answer, question);
	
    do
  	  {
		pid = fork();
		if(pid == 0)    /*child process*/
		{
			child = getpid();
			*is_correct = compare(answer, question, A, B);  //compare, correct or not
			cout << "\n[" << getpid() << " Answer " << count << "]"  ;
			if(*A != DIGITS)				
				cout << *A << "A" << *B << "B";
			exit(0);
			
		}
		else if(pid > 1)    /*parent process*/
		{		
	        wait(0);
			parent = getpid();
			cout << "\n[" << getpid() << " Main " << count << "]"  ;
			if(*A != DIGITS)
			{
				cout << *A << "A" << *B << "B\n";
				cout << "\n[" << getpid() << " User] : ";
				guess(answer, question);
				count++;
			}
			else
			{
				cout << " done.\n";
				cout << "answer :";
				for(int i=0;i<DIGITS;i++) 
					cout << question[i] ;
				cout << endl;				
			}
		}		
    }while(*is_correct == 0);    /*continue the guessing game if not correct*/

	if(pid>0)
	{
	    /*munmap*/
		r = munmap(ptr_answer, region_size);
    	if (r != 0) cout << "munmap error\n";
		r = munmap(ptr_question, region_size);
    	if (r != 0) cout << "munmap error\n";    	
    	r = munmap(ptr_A, sizeof(int));
    	if (r != 0) cout << "munmap error\n";    	
    	r = munmap(ptr_B, sizeof(int));
    	if (r != 0) cout << "munmap error\n";
    	r = munmap(ptr_C, sizeof(int));
    	if (r != 0) cout << "munmap error\n";
    	/*shm_unlink*/
    	r = shm_unlink("your_answer");
    	if (r != 0) cout << "shm_unlink error\n";
    	r = shm_unlink("question");
    	if (r != 0) cout << "shm_unlink error\n";
    	r = shm_unlink("is_correct");
    	if (r != 0) cout << "shm_unlink error\n";
    	r = shm_unlink("xxxxxxx");
    	if (r != 0) cout << "shm_unlink error\n";
    	r = shm_unlink("yyyyyy");
    	if (r != 0) cout << "shm_unlink error\n";	
	}
	
	return 0;   
}
 
/* 隨機產生 DIGITS 個不同的數字 int */
void Task1(u_long* question)
{
    int is_repeat;    /* 為了檢查是否產生重複的數字 */ 
    srand( time(NULL) );    //以時間做為亂數種子 

    question[0] = rand() % 9 + 1 ;    /* 第一個數字會是 1-9 */
 
    	/* 產生其餘數字 */
    for(int i = 1; i < DIGITS; i++){
        /* 產生新的數字直到沒有重複的情形發生 */
        do{
            is_repeat = 0;    /* 一開始假設沒有產生重複數字的情況 */
            question[i] = rand() % 10 ;    /* 其他數字會是 0-9. */
            for(int j = 0; j < i; j++)
            {
                if(question[i] == question[j])
                {
                    is_repeat = 1;    /* 重複發生 */
                    break;
                }
            }
        }while(is_repeat == 1);    //重複發生就繼續做 
    }
}

void Task2(u_long* question)
{
	srand(time(NULL));
	question[0] = rand() % 9 + 1 ;
	for(int i=1;i<DIGITS ;i++)
		question[i] = rand() % 10 ;
}

/* 取得使用者輸入的數字並存在 your_answer 陣列裡 */ 
/* 當使用者輸入 0 時，則秀出答案 */
void guess(u_long* your_answer, u_long* question)
{
    int i;    //宣告整數變數 
	string value;
    getline(cin,value,'\n');    //輸入數字 
    if(value == "" )    //when input is an "Enter", print out the answer
    {    
 	    for(int i=0 ;i<DIGITS;i++)
			your_answer[i] = question[i];
		return;
    }
    else     /* 取得數字 */
    {
	    cout  << "\n-------";
	    for(int i=0;i<value.length();i++)
	    {
		    your_answer[i] = value[i] - '0';
		    cout << your_answer[i];
	    }
	    cout << "-------\n";
    }
	return;
}
/* 比較 question 與使用者所輸入的數字
   如果使用者猜對了 return 1;
   否則 return 0;
*/ 
u_long compare(u_long* your_answer, u_long* question,u_long* A,u_long* B)
{
     int a_count = 0, b_count = 0, i, j;
	 bool* TruePtrA = new bool[DIGITS]; 
	 bool* TruePtrB = new bool[DIGITS];
  	for(i=0;i<DIGITS;i++)////A
	{	
		if( your_answer[i] == question[i])
		{
			a_count++;
			TruePtrA[i] = true;
			TruePtrB[i] = true;
		}
	}
	for(i=0;i<DIGITS;i++) ////B
	{
		for(j=0;j<DIGITS;j++)
		{
			if(TruePtrA[i] == true)
				break;
			if( i != j && your_answer[j] == question[i] && TruePtrB[j] != true)
			{
				b_count++;
				TruePtrB[j] = true;
				break;
			}
		}
	}

	delete[] TruePtrA;
	delete[] TruePtrB;   
 
	*A = a_count;
	*B = b_count;

    if(a_count == DIGITS)
        return 1;
    else
        return 0;
}
