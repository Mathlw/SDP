turkey

#include<stdio.h>

int main() {
   char name[] = "name";
   int stdNum=0;
   
   printf("�̸��� �Է��ϼ���\n");
   scanf("%s", &name);
   printf("�й��� �Է��ϼ���\n");
   scanf("%d", &stdNum);
   
   printf("�̸� : %s , �й� : %d", name, stdNum);
   system("PAUSE");
   return 0;
}