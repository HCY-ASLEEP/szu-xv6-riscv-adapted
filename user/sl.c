#include "include/unistd.h"
#include "include/stdio.h"
#include "include/stdlib.h"
#include "include/string.h"

#define D51STR1  "        (@@@@)                                                                                   "
#define D51STR2  "        (@@)                                                                                     "
#define D51STR3  "        @                                                                                        "
#define D51STR4  "      ====        ________                 __________      _______________________________       "
#define D51STR5  "  _D _|  |_______/        \\\\__I_I_____===__|_________|    /                               \\\\___A "
#define D51STR6  "   |(_)---  |   H\\\\________/ |   |        =|___ ___|     |   ||   ||   ||====   \\\\    //        |"
#define D51STR7  "   /     |  |   H  |  |      |   |         ||_| |_||     |   ||___||   ||        \\\\  //         |"
#define D51STR8  "  |      |  |   H  |__---------------------| [___] |     |   ||   ||   ||          ||           |"
#define D51STR9  "  | ________|___H__/__|_____/[][]~\\\\_______|       |     |   ||   ||   ||====      ||           |"
#define D51STRa  "  |/ |   |-----------I_____I [][] []  D    |=======|_____|______________________________________|"
#define D51STRb  "__/ =| o |=-~~\\\\  /~~\\\\  /~~\\\\  /~~\\\\ ____Y___________|__|______________________________________|"
#define D51STRc  " |/-=|___|=O=====O=====O=====O   |_____/~\\\\___/              |_D__D__D__D__| |D__D__| |D__D__D|  "
#define D51STRd  "  \\\\_/      \\\\__/  \\\\__/  \\\\__/  \\\\__/      \\\\_/               \\\\_/   \\\\_/     \\\\_/     \\\\_/     "

#define DELAY  900

const char* D51STR[13]={
  D51STR1,
  D51STR2,
  D51STR3,
  D51STR4,
  D51STR5,
  D51STR6,
  D51STR7,
  D51STR8,
  D51STR9,
  D51STRa,
  D51STRb,
  D51STRc,
  D51STRd,
}; 

void init();
void show();
void stringSlice(const char* source, int start, int end, char* destination); 
void uselessSort(); 
void delay(int level); 
void faded(); 

void init(){
  printf("\033[2J");
}
void show(){
 faded(); 
}

void showPart(int beginIndex, int endIndex){
  
  char result[13][100]; 
   for(int i=0;i<13;i++){
     stringSlice(D51STR[i],beginIndex,endIndex,result[i]); 
   }
   
   // clear screen
  printf("\033[2J");
  printf("\033[1;1H");
  for(int i=0; i<13;i++){
    printf("%s",result[i]);
    printf("\n"); 
  }
   
  fflush(stdout); 
}

void faded(){
  int showMaxLen=strlen(D51STR1);   
  for(int i=0; i<showMaxLen; i++){
    showPart(i,showMaxLen-1);
    delay(110); 
  }
}

void bubbleSort(int arr[], int size) {
    for (int i = 0; i < size - 1; i++) {
        for (int j = 0; j < size - i - 1; j++) {
            if (arr[j] < arr[j + 1]) {
                // 交换两个元素
                int temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}
void uselessSort(){

    int arr[DELAY];

    // 初始化数组为顺序数组
    for (int i = 0; i < DELAY; i++) {
        arr[i] = i + 1;
    }
    
    bubbleSort(arr, DELAY);
}

void delay(int level){
  for(int i=0;i<level;i++){
    uselessSort(); 
  }
}

void stringSlice(const char* source, int start, int end, char* destination) {
    int length = strlen(source);
    int i, j = 0;

    if (start < 0 || end > length || start > end) {
        printf("Invalid slice range.\n");
        return;
    }

    for (i = start; i <= end; i++) {
        destination[j] = source[i];
        j++;
    }

    destination[j] = '\0';  // 添加字符串结尾的'\0'字符
}

int main(){
  while(1){
    init();
    show(); 
    printf("(enter \'q\' to exit...)\n");
    printf("> ");
    // fflush(stdout);
    while(1){
      char c = getchar();
      if (c=='q')
        exit(0); 
    }
    return 0; 
  }
}
