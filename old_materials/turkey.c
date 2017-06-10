turkey

#include<stdio.h>

int main() {
   char name[] = "name";
   int stdNum=0;
   
   printf("이름을 입력하세요\n");
   scanf("%s", &name);
   printf("학번을 입력하세요\n");
   scanf("%d", &stdNum);
   
   printf("이름 : %s , 학번 : %d", name, stdNum);
   system("PAUSE");
   return 0;
}