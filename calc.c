#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include<conio.h>
#include<math.h>
#define A 8
#define B 10000000
#define C 100000000
#define D 10000
typedef struct number{
	int*a;
	unsigned int b,c,d;
}Number;
typedef struct pass{
	Number*a,b,c;
}Pass;
int flag;
int DG=8;						/*Digit Grouping		*/
int GD=0;						/*Group Decimals		*/
int PF=0;						/*Print on File			*/
int PS=1;						/*Print on Screen		*/
int DL=1000;					/*Decimal Limit			*/
int LA=0;						/*Limit All operations	*/
int TL=0;						/*Threads Limit			*/
char GC=',';					/*Group Character		*/
char CC='.';					/*Comma Character		*/
char SV[52];					/*Saved Variables		*/
char RF[21]="Results.txt";		/*Results Filename		*/
char SF[9]="Settings";			/*Settings Filename		*/
char VF[10]="Variables";		/*Variables Filename	*/
Number PR;						/*Previous Results		*/
Number newNumber(int*a,int b,int c,int d)
{
	Number temp;
	int i;
	temp.b=b;
	temp.c=c;
	b+=c;
	temp.a=malloc(sizeof(int)*b);
	for(i=0;i<b;i++)
	temp.a[i]=a[i];
	temp.d=d;
	return temp;
}
char onoff(int a)
{
	if(a)
	return 'x';
	return ' ';
}
void swi(int*a)
{
	if(*a)
	*a=0;
	else
	*a=1;
}
int readNum()
{
	long long i=0;
	char a,b=0;
	while((a=getch())!=13)
	if(a==8 && (i || b=='0'))
	{
		i/=10;
		printf("%c%c%c",8,32,8);
		b=a;
	}
	else if(a<'0' || a>'9')
	continue;
	else if(b!='0' || a!='0' || i)
	{
		if(b=='0' && i/10==0)
		printf("%c%c%c",8,32,8);
		if(i*10+a-'0'>2000000000)
		return 2000000000;
		i=i*10+a-'0';
		putchar(a);
		b=a;
	}
	return i;
}
void read(char*a,int b)
{
	int i;
	for(i=0;i<b;i++)
	{
		a[i]=getch();
		if(a[i]=='/' || a[i]=='\\' || a[i]==':' || a[i]=='*' || a[i]=='?' || a[i]=='"' || a[i]=='<' || a[i]=='>' || a[i]=='|' || a[i]=='\t')
		i--;
		else if(a[i]==8)
		{
			if(i>0)
			{
				i-=2;
				printf("%c%c%c",8,32,8);
			}
			else
			i--;
		}
		else
		putchar(a[i]);
		if(a[i]==13)
		break;
	}
	a[i]=0;
}
void settings()
{
	char a=0;
	FILE*file;
	while(a!='0')
	{
		system("cls");
		printf("1. Digit grouping size (0 to disable)	[%d]\n",DG);
		printf("2. Group decimals			[%c]\n",onoff(GD));
		printf("3. Save output on file			[%c]\n",onoff(PF));
		printf("4. Show results on screen		[%c]\n",onoff(PS));
		printf("5. Grouping character			[%c]\n   Comma character			[%c]\n",GC,CC);
		printf("6. Output file name			[%s]\n",RF);
		printf("7. Decimals calculated (x8)		[%d]\n",DL);
		printf("8. Decimal limit for all operations	[%c]\n",onoff(LA));
		printf("9. Threads number (0 for default)	[%d]\n",TL);
		printf("0. Save and exit\n");
		a=getch();
		if(a=='1')
		{
			printf("Enter grouping size: ");
			DG=readNum();
			if(DG==0)
			GD=0;
		}
		else if(a=='2' && DG)
		swi(&GD);
		else if(a=='3')
		swi(&PF);
		else if(a=='4')
		swi(&PS);
		else if(a=='5')
		if(GC=='.')
		{
			GC=',';
			CC='.';
		}
		else
		{
			GC='.';
			CC=',';
		}
		if(a=='6')
		{
			printf("Enter desired filename:\n");
			do
			{
				read(RF,20);
			}while(RF[0]==0);
		}
		else if(a=='7')
		{
			printf("Enter desired decimal size:\n");
			DL=readNum();
		}
		else if(a=='8')
		swi(&LA);
		else if(a=='9')
		{
			printf("Enter the desired number of threads to be used:\n");
			TL=readNum();
		}
	}
	file=fopen(SF,"wb");
	fwrite(&DG,sizeof(int),1,file);
	fputc(GD,file);
	fputc(PF,file);
	fputc(PS,file);
	fwrite(&DL,sizeof(int),1,file);
	fputc(LA,file);
	fputc(GC,file);
	fputc(CC,file);
	fwrite(&TL,sizeof(int),1,file);
	fprintf(file,"%s",RF);
	fclose(file);
}
void scanSettings()
{
	FILE*file=fopen(SF,"r");
	if(file)
	{
		fread(&DG,sizeof(int),1,file);
		GD=fgetc(file);
		PF=fgetc(file);
		PS=fgetc(file);
		fread(&DL,sizeof(int),1,file);
		LA=fgetc(file);
		GC=fgetc(file);
		CC=fgetc(file);
		fread(&TL,sizeof(int),1,file);
		fscanf(file,"%s",&RF);
		fclose(file);
	}
}
int nextPrime(int a)
{
	int b,i,j;
	if(a==2)
	return 3;
	for(i=a+2;1;i+=2)
	{
		b=sqrt(i);
		for(j=3;j<=b;j++)
		if(i%j==0)
		break;
		if(j>b)
		return i;
	}
}
int isParenthesis(char a)
{
	if(a=='(' || a==')')
	return 1;
	return 0;
}
int isOperand(char a)
{
	if(a=='+' || a=='-' || a=='*' || a=='/' || a=='^' || a=='r' || a==':')
	return 1;
	return 0;
}
int isComma(char a)
{
	if(a=='.' || a==',')
	return 1;
	return 0;
}
int isNumeric(char a)
{
	if(a>='0' && a<='9')
	return 1;
	return 0;
}
int isNeg(char*a,int b)
{
	if(a[b]=='-' && (b==0 || a[b-1]=='(' || a[b-1]==':' || a[b-1]=='^'))
	return 1;
	return 0;
}
int isSaved(char a)
{
	int i;
	for(i=0;SV[i];i++)
	if(SV[i]==a)
	return 1;
	return 0;
}
int arraySize(int a,int b)
{
	if((b-a)%A)
	return (b-a)/A+1;
	return (b-a)/A;
}
int arrayPlace(int a)
{
	int i,b=1;
	a=A-a%A;
	for(i=1;i<a;i++)
	b*=10;
	return b;
}
void push(char**a,int head,char b)
{
	*a=(char*)realloc(*a,head+1);
	(*a)[head]=b;
}
char pop(char**a,int head)
{
	char b=(*a)[head];
	*a=(char*)realloc(*a,head);
	return b;
}
int priority(char a)
{
	if(a==':')
	return -1;
	if(a=='+' || a=='-')
	return 0;
	if(a=='*' || a=='/')
	return 1;
	if(a=='^' || a=='r')
	return 2;
}
void printChar(char a,FILE*b)
{
	if(PS)
	putchar(a);
	if(PF)
	fputc(a,b);
}
char print(Number a)
{
	int i,j=a.b+a.c-1,k,l,m=1;
	FILE*file=fopen(RF,"a");
	printChar(a.d,file);
	if(DG==0 || DG==A)
	{
		if(PS)
		printf("%d",a.a[0]);
		if(PF)
		fprintf(file,"%d",a.a[0]);
		for(i=1;i<a.b;i++)
		{
			if(DG)
			printChar(GC,file);
			if(PS)
			printf("%08d",a.a[i]);
			if(PF)
			fprintf(file,"%08d",a.a[i]);
		}
		if(a.c)
		printChar(CC,file);
		for(i=a.b;i<j;i++)
		{
			if(PS)
			printf("%08d",a.a[i]);
			if(PF)
			fprintf(file,"%08d",a.a[i]);
			if(DG && GD)
			printChar(GC,file);
		}
		if(a.c)
		{
			k=A;
			for(j=1;!(a.a[i]/j%10);j*=10)
			k--;
			for(j=B;j && k--;j/=10)
			printChar(a.a[i]/j%10+48,file);
		}
		if(GD && DG && a.c%A==0)
		{
			if(PS)
			printf("%c%c%c",8,32,8);
			if(PF)
			fprintf(file,"%c%c%c",8,32,8);
		}
	}
	else
	{
		j=8*a.b;
		for(i=B;i>1 && a.a[0]/i==0;i/=10)
		j--;
		k=DG*(j%DG>0)-j%DG;
		l=A*(j%A>0)-j%A;
		for(i=1;i<A-l;i++)
		m*=10;
		for(i=0;i<j;i++)
		{
			if(m==0)
			m=B;
			if(i && (i+k)%DG==0)
			printChar(GC,file);
			printChar(a.a[(i+l)/A]/m%10+48,file);
			m/=10;
		}
		if(a.c)
		{
			printChar(CC,file);
			i+=l;
			j+=l+A*a.c;
			for(k=1;a.a[a.b+a.c-1]/k%10==0;k*=10)
			j--;
			k=DG*(i%DG>0)-i%DG;
			for(;i<j;i++)
			{
				if(m==0)
				m=B;
				if(GD && i/A+(i%A>0)>a.b && (i+k)%DG==0)
				printChar(GC,file);
				printChar(a.a[i/A]/m%10+48,file);
				m/=10;
			}
		}
	}
	printChar('\n',file);
	if(PF)
	fclose(file);
}
void searchVar(char a,FILE*b)
{
	int c,d;
	char e;
	fread(&e,sizeof(char),1,b);
	while(e!=a)
	{
		fread(&e,sizeof(char),1,b);
		fread(&c,sizeof(int),1,b);
		fread(&d,sizeof(int),1,b);
		fseek(b,(c+d)*sizeof(int),SEEK_CUR);
		fread(&e,sizeof(char),1,b);
	}
}
void delVar(char a)
{
	int*b=NULL,c,d;
	char*e=tmpnam(NULL),f,g;
	FILE*file=fopen(VF,"rb"),*temp=fopen(e,"wb");
	while(fread(&f,sizeof(char),1,file))
	{
		fread(&g,sizeof(char),1,file);
		fread(&c,sizeof(int),1,file);
		fread(&d,sizeof(int),1,file);
		if(f!=a)
		{
			b=realloc(b,sizeof(int)*(c+d));
			fread(b,sizeof(int),c+d,file);
			fwrite(&f,sizeof(char),1,temp);
			fwrite(&g,sizeof(char),1,temp);
			fwrite(&c,sizeof(int),1,temp);
			fwrite(&d,sizeof(int),1,temp);
			fwrite(b,sizeof(int),c+d,temp);
		}
		else
		fseek(file,sizeof(int)*(c+d),SEEK_CUR);
	}
	free(b);
	fclose(file);
	fclose(temp);
	remove(VF);
	rename(e,VF);
}
Number readVar(char a)
{
	FILE*file;
	Number temp;
	file=fopen(VF,"rb");
	searchVar(a,file);
	fread(&a,sizeof(char),1,file);
	temp.d=a;
	fread(&(temp.c),sizeof(int),1,file);
	fread(&(temp.b),sizeof(int),1,file);
	temp.a=malloc(sizeof(int)*(temp.b+temp.c));
	fread(temp.a,sizeof(int),temp.b+temp.c,file);
	fclose(file);
	return temp;
}
void readSV()
{
	int i=0,a,b;
	char c;
	FILE*file=fopen(VF,"rb");
	if(file)
	{
		while(fread(&c,sizeof(char),1,file))
		{
			SV[i++]=c;
			fread(&c,sizeof(char),1,file);
			fread(&a,sizeof(int),1,file);
			fread(&b,sizeof(int),1,file);
			fseek(file,sizeof(int)*(a+b),SEEK_CUR);
		}
		fclose(file);
	}
	for(;i<52;i++)
	SV[i]=0;
}
void fix(Number*a)
{
	int i=0,j=a->b,k;
	while(!a->a[i] && j>1)
	{
		i++;
		j--;
	}
	if(!j)
	{
		a->a=(int*)realloc(a->a,sizeof(int)*(a->c+1));
		for(i=a->c;i>0;i--)
		a->a[i]=a->a[i-1];
		a->a[0]=0;
		a->b=1;
	}
	else if(j!=a->b)
	{
		k=a->b-j;
		for(i=0;i<a->c+j;i++)
		a->a[i]=a->a[i+k];
		a->a=(int*)realloc(a->a,sizeof(int)*(a->c+j));
		a->b=j;
	}
	if(LA && a->c>DL)
	{
		a->c=DL;
		a->a=(int*)realloc(a->a,sizeof(int)*(a->b+a->c));
	}
	i=a->b+a->c-1;
	j=a->c;
	while(!a->a[i] && j)
	{
		i--;
		j--;
	}
	if(j!=a->c)
	{
		a->a=(int*)realloc(a->a,sizeof(int)*(a->b+j));
		a->c=j;
	}
	if(a->b==1 && a->c==0 && a->a[0]==0)
	a->d='+';
}
int toNumber(Number*a,char*b,int c,int d)
{
	int temp=d,k=0,i,start;
	if(c)
	a->d='-';
	else
	a->d='+';
	for(i=d;isNumeric(b[i]) || isComma(b[i]);i++)
	if(isComma(b[i]))
	{
		k=arraySize(d,i);
		a->b=k;
		start=((i-d)%A&&1)*A-(i-d)%A;
		d=i+1;
	}
	k+=arraySize(d,i);
	a->a=(int*)calloc(k,sizeof(int));
	a->c=arraySize(d,i);
	if(d==temp)
	{
		a->b=k;
		a->c=0;
		start=((i-d)%A&&1)*A-(i-d)%A;
	}
	k=0;
	for(d=temp;d<i;d++)
	if(!isComma(b[d]))
	a->a[(start+k)/A]+=arrayPlace(start+k++)*(b[d]-48);
	fix(a);
	return d;
}
Number*postfix(char**d)
{
	int i=0,j,k,bCounter=0,start,head=0,ptr=0,flag;
	char*c=NULL,*a=*d,*stack=NULL;
	Number*b=NULL;
	while(a[i])
	{
		if(flag=isNeg(a,i))
		i++;
		else if(i && (a[i]=='(' && (a[i-1]=='!' || isNumeric(a[i-1]) || a[i-1]==')' || isSaved(a[i-1])) ||
		isSaved(a[i]) && (isSaved(a[i-1]) || isNumeric(a[i-1]) || a[i-1]==')' || a[i-1]=='!') ||
		isNumeric(a[i]) && (isSaved(a[i-1]) || a[i-1]==')' || a[i-1]=='!')))
		a[--i]='*';
		if(isNumeric(a[i]) || isSaved(a[i]))
		{
			b=(Number*)realloc(b,sizeof(Number)*(bCounter+1));
			if(isNumeric(a[i]))
			i=toNumber(b+bCounter++,a,flag,i);
			else
			{
				b[bCounter++]=readVar(a[i++]);
				if(flag)
				if(b[bCounter-1].d=='+')
				b[bCounter-1].d='-';
				else
				b[bCounter-1].d='+';
			}
			push(&c,ptr++,48);
		}
		else if(a[i]=='!')
		push(&c,ptr++,a[i++]);
		else if(flag && a[i]=='(')
		{
			a=(char*)realloc(a,sizeof(char)*(strlen(a)+4));
			for(j=strlen(a);j>i-2;j--)
			a[j+2]=a[j];
			a[j+2]='0';
			a[j+1]='(';
			j+=5;
			for(k=1;k;j++)
			if(a[j]=='(')
			k++;
			else if(a[j]==')')
			k--;
			for(k=strlen(a);k>=j;k--)
			a[k+1]=a[k];
			a[k+1]=')';
			i--;
		}
		else if(isOperand(a[i]) || isParenthesis(a[i]))
		{
            if(head==0 || a[i]=='(' || a[i]!=')' && (stack[head-1]=='(' || priority(a[i])>priority(stack[head-1]) || priority(a[i])==2))
			push(&stack,head++,a[i++]);
			else if(a[i]!=')' && stack[head-1]!='(' && priority(a[i])<=priority(stack[head-1]))
			{
				while(head>0 && stack[head-1]!='(' && priority(a[i])<=priority(stack[head-1]))
				push(&c,ptr++,pop(&stack,--head));
				push(&stack,head++,a[i++]);
			}
			else if(a[i]==')')
			{
				while(stack[head-1]!='(')
				push(&c,ptr++,pop(&stack,--head));
				pop(&stack,--head);
				i++;
			}
		}
	}
	while(head)
	push(&c,ptr++,pop(&stack,--head));
	push(&c,ptr,0);
	free(a);
	free(stack);
	*d=c;
	return b;
}
void printInput(char*a)
{
	int i=0,j,k;
	system("cls");
	printf("Press '\\' to enter cmd mode.\n");
	printf("Enter the numerical representation you wish calculated:\n");
	if(a==NULL)
	return;
	while(a[i])
	{
		while(a[i] && !isNumeric(a[i]))
		putchar(a[i++]);
		for(j=i;isNumeric(a[j]);j++);
		k=0;
		for(j-=i;j>0;j--)
		{
			if(DG && k && j%DG==0)
			putchar(GC);
			putchar(a[i++]);
			k=1;
		}
		if(isComma(a[i]))
		{
			j=0;
			putchar(CC);
			i++;
			while(isNumeric(a[i]))
			{
				if(GD && j && j%DG==0)
				putchar(GC);
				j++;
				putchar(a[i++]);
			}
		}
	}
}
void filePrintInput(char*a)
{
	int i=0,j,k;
	FILE*file=fopen(RF,"a");
	while(a[i])
	{
		while(a[i] && !isNumeric(a[i]))
		fputc(a[i++],file);
		for(j=i;isNumeric(a[j]);j++);
		k=0;
		for(j-=i;j>0;j--)
		{
			if(DG && k && j%DG==0)
			fputc(GC,file);
			fputc(a[i++],file);
			k=1;
		}
		if(isComma(a[i]))
		{
			j=0;
			fputc(CC,file);
			i++;
			while(isNumeric(a[i]))
			{
				if(GD && j && j%DG==0)
				fputc(GC,file);
				j++;
				fputc(a[i++],file);
			}
		}
	}
	fprintf(file,"=\n");
	fclose(file);
}
void cmd(char*a,int b)
{
	int i,j,k;
	char c[6],d=0,e;
	Number temp;
	FILE*file;
	system("cls");
	if(a)
	printf("\"as x\"	to save the last number typed or the last result as x (or whatever).\n");
	printf("\"del x\"	to delete x or \"del\" to delete all.\n");
	printf("\"pr x\"	to print x or \"pr\" to print all.\n");
	printf("\"set\"	to enter the settings screen.\n");
	printf("\nHit enter with none of these commands to switch to normal mode.\n");
	read(c,6);
	printf("\n");
	if(a && (c[0]=='a' && c[1]=='s' && c[2]==' ' && (c[3]>64 && c[3]<91 || c[3]!='r' && c[3]>96 && c[3]<123) && c[4]==0 && (b && (isNumeric(a[b-1]) || isSaved(a[b-1])) || PR.b)))
	{
		if(isSaved(c[3]))
		{
			printf("This will replace the existing variable '%c'. Are you sure? (y/n)",c[3]);
			do
			{
				d=getch();
			}while(d!='y' && d!='Y' && d!='n' && d!='N');
			if(d=='y' || d=='Y')
			delVar(c[3]);
		}
		if(!isSaved(c[3]) || d=='y' || d=='Y')
		{
			if(isNumeric(a[--b]))
			{
				while(b && (isNumeric(a[b-1]) || isComma(a[b-1])))
				b--;
				toNumber(&temp,a,isNeg(a,b-1),b);
			}
			else if(isSaved(a[b]))
			temp=readVar(a[b]);
			else if(PR.b)
			temp=PR;
			file=fopen(VF,"ab");
			fwrite(c+3,sizeof(char),1,file);
			fwrite(&(temp.d),sizeof(char),1,file);
			fwrite(&(temp.c),sizeof(int),1,file);
			fwrite(&(temp.b),sizeof(int),1,file);
			fwrite(temp.a,sizeof(int),temp.b+temp.c,file);
			fclose(file);
			readSV();
		}
	}
	else if(c[0]=='d' && c[1]=='e' && c[2]=='l' && c[3]==' ' && (c[4]>64 && c[4]<91 || c[4]!='r' && c[4]>96 && c[4]<123) && c[5]==0 && isSaved(c[4]))
	{
		delVar(c[4]);
		readSV();
	}
	else if(c[0]=='d' && c[1]=='e' && c[2]=='l' && c[3]==0)
	{
		remove(VF);
		readSV();
	}
	else if(c[0]=='p' && c[1]=='r' && c[2]==' ' && (c[3]>64 && c[3]<91 || c[3]!='r' && c[3]>96 && c[3]<123) && c[4]==0 && isSaved(c[3]))
	{
		temp=readVar(c[3]);
		printf("%c = ",c[3]);
		print(temp);
		free(temp.a);
		getch();
	}
	else if(c[0]=='p' && c[1]=='r' && c[2]==0)
	{
		for(k=0;SV[k];k++);
		for(i=0;i<k;i++)
		{
			e=127;
			for(j=0;j<k;j++)
			if(SV[j]<e && SV[j]>d)
			e=SV[j];
			temp=readVar(e);
			printf("%c = ",e);
			print(temp);
			free(temp.a);
			d=e;
		}
		getch();
	}
	else if(c[0]=='s' && c[1]=='e' && c[2]=='t' && c[3]==0)
	settings();
	else
	{
		printf("No command was given. Press any key to return to normal mode.\n");
		getch();
	}
}
char*input()
{
	char*a=malloc(sizeof(char)),b=getch(),flag=0,pr;
	int i=0,j,count=0;
	while(b=='=' || b==13)
	b=getch();
	if(b=='\\')
	cmd(a,i);
	a[0]=0;
	printInput(a);
	while(b!='=' && b!=13)
	{
		if(b==8 && i>0)
		{
			if(a[--i]=='(')
			count--;
			else if(a[i]==')')
			count++;
			if(i && !isNumeric(a[i]) && !isSaved(a[i]) && a[i-1]=='0' && (i==1 || i>1 && !isNumeric(a[i-2]) && !isSaved(a[i-2])))
			flag=2;
			else if(isComma(a[i]) || a[i]=='0' && (i==0 || !isNumeric(a[i-1])))
			flag=0;
			else if(!isNumeric(a[i]) && i>0 && isNumeric(a[i-1]))
			{
				for(j=i;j>0 && isNumeric(a[--j]););
				if(isComma(a[j]))
				flag=1;
			}
			a=(char*)realloc(a,i+2);
			a[i]=0;
			printInput(a);
		}
		if(i==0)
		pr=0;
		else
		pr=a[i-1];
		if(isNumeric(b) || isSaved(b) && !isComma(pr) || b=='(' && !isComma(pr) || 
		b==')' && count>0 && (isNumeric(pr) || pr==')' || pr=='!') || 
		isComma(b) && isNumeric(pr) && flag!=1 || 
		isOperand(b) && i>0 && !isOperand(pr) && pr!='(' && !isComma(pr) || 
		b=='-' && (pr=='(' || i==0 || pr==':' || pr=='^') ||
		b=='!' && (isNumeric(pr) || isSaved(pr) || pr==')' || pr=='!'))
		{
			if(flag==2 && (isNumeric(b) || isSaved(b)))
			{
				i--;
				if(b!='0')
				flag=0;
				a[i]=0;
				printInput(a);
			}
			else
			a=(char*)realloc(a,i+2);
			a[i++]=b;
			a[i]=0;
			if(b=='(')
			count++;
			else if(b==')')
			count--;
			if(isComma(b))
			{
				flag=1;
				a[i-1]=CC;
			}
			else if(!isNumeric(b))
			flag=0;
			else if(b=='0' && !isNumeric(pr) && !isComma(pr))
			flag=2;
			if(DG)
			printInput(a);
			else
			putchar(a[i-1]);
		}
		do
		{
			b=getch();
			if(b=='\\')
			{
				cmd(a,i);
				printInput(a);
			}
		}while(b=='\\' || (b=='=' || b==13) && (count>0 || i==0 || isOperand(a[i-1]) || isComma(a[i-1])));
	}
	printf("=\n");
	if(PF)
	filePrintInput(a);
	return a;
}
int equality2(int a,int b)
{
	if(a>b)
	return 1;
	if(a<b)
	return 2;
	return 0;
}
int equality(Number a,Number b)
{
	int i;
	if(equality2(a.b,b.b))
	return equality2(a.b,b.b);
	for(i=0;i<a.b+a.c && i<b.b+b.c;i++)
	if(equality2(a.a[i],b.a[i]))
	return equality2(a.a[i],b.a[i]);
	if(equality2(a.c,b.c))
	return equality2(a.c,b.c);
	return 0;
}
int absolute(int a)
{
	if(a<0)
	return -a;
	return 0;
}
int most(int a,int b)
{
	if(a>b)
	return a;
	return b;
}
int least(int a,int b)
{
	if(a>b)
	return b;
	return a;
}
void carry(Number*a)
{
	int b=a->c-a->b,i;
	for(i=b;i>0;i--)
	if(a->a[i]>=C||a->a[i]<0)
	{
		b=a->a[i]/C;
		if(b<=0 && a->a[i]%C)
		b--;
		a->a[i]-=b*C;
		a->a[i-1]+=b;
	}
}
void*FinalAdd(Pass*a)
{
	int i,j=0,b=a->c.c-a->b.b,c=a->c.b-a->b.b,d=a->b.c-a->b.b;
	a->a->a=realloc(a->a->a,sizeof(int)*(b+2));
	a->a->c=a->c.c;
	for(i=c;i<=d;i++)
	a->a->a[i]+=a->c.a[j++];
	for(i=d+1;i<=b;i++)
	a->a->a[i]=a->c.a[j++];
	carry(a->a);
	free(a->c.a);
	pthread_exit(NULL);
}
void finalAdd(Number*a,int b)
{
	int i,j,k=1,l,count;
	pthread_t threads[b/2];
	Pass c[b/2];
	for(i=b;i/2;i=i/2+i%2)
	{
		l=k;
		k*=2;
		count=0;
		for(j=0;j+l<b;j+=k)
		{
			c[count].a=a+j;
			c[count].b=a[j];
			c[count].c=a[j+l];
			pthread_create(threads+count++,NULL,(void*)FinalAdd,c+count);
		}
		for(j=0;j<count;j++)
		pthread_join(threads[j],NULL);
	}
}
void*Add(Pass*a)
{
	int b1,b2,c1,c2,i;
	if(a->b.b>a->c.b)
	{
		b1=0;
		c1=a->b.b-a->c.b;
	}
	else
	{
		b1=a->c.b-a->b.b;
		c1=0;
	}
	b2=b1-a->a->b;
	c2=c1-a->a->b;
	b1=least(a->a->c-b1+1,a->b.b+a->b.c);
	c1=least(a->a->c-c1+1,a->c.b+a->c.c);
	for(i=absolute(b2);i<b1;i++)
	a->a->a[i+b2]=a->b.a[i];
	if(a->a->d=='a')
	for(i=absolute(c2);i<c1;i++)
	a->a->a[i+c2]+=a->c.a[i];
	else
	for(i=absolute(c2);i<c1;i++)
	a->a->a[i+c2]-=a->c.a[i];
	pthread_exit(NULL);
}
Number add(Number a,Number b,char c,char d)
{
	int i,g=0,h,threadsNo=pthread_num_processors_np(),size,residue;
	if(TL)
	threadsNo=TL;
	size=most(a.b,b.b);
	size+=most(a.c,b.c);
	if(threadsNo>size)
	threadsNo=size;
	Number e[threadsNo];
	Pass f[threadsNo];
	pthread_t threads[threadsNo];
	residue=size%threadsNo;
	for(i=0;i<threadsNo;i++)
	{
		h=size/threadsNo+(0<residue--);
		e[i].a=calloc(h+1,sizeof(int));
		e[i].b=g;
		g+=h;
		e[i].c=g-1;
		e[i].d=d;
		f[i].a=e+i;
		f[i].b=a;
		f[i].c=b;
		pthread_create(threads+i,NULL,(void*)Add,f+i);
	}
	for(i=0;i<threadsNo;i++)
	pthread_join(threads[i],NULL);
	finalAdd(e,threadsNo);
	e[0].b=most(a.b,b.b);
	e[0].c=most(a.c,b.c);
	if(e[0].a[0]/C)
	{
		g=e[0].b+e[0].c;
		e[0].a=realloc(e[0].a,sizeof(int)*(g+1));
		for(i=g;i>0;)
		e[0].a[i]=e[0].a[--i];
		e[0].a[0]/=C;
		e[0].a[1]%=C;
		e[0].b++;
	}
	e[0].d=c;
	fix(e);
	return e[0];
}
void*Mul(Pass*a)
{
	int*b=a->b.a+a->a->b,c=a->a->c-a->a->b,d=a->c.b+a->c.c,e,i,j,f;
	short x0,x1,*y0=malloc(sizeof(short)*d),*y1=malloc(sizeof(short)*d);    /*  y0 and y1 will not change length, but for big values of d, the program closes if they are static... */
	a->a->c+=d;
	for(i=0;i<d;i++)
	{
		y0[i]=a->c.a[i]/D;
		y1[i]=a->c.a[i]%D;
	}
	for(i=0;i<=c;i++)
	{
		x0=*b/D;
		x1=*b%D;
		e=i;
		for(j=0;j<d;j++)
		{
			f=x1*y0[j]+y1[j]*x0;
			a->a->a[e++]+=x0*y0[j]+f/D;
			a->a->a[e]+=x1*y1[j]+f%D*D;
		}
		if(i%10==9)
		carry(a->a);
		b++;
	}
	free(y0);
	free(y1);
	carry(a->a);
}
Number mul(Number a,Number b)
{
	int i,e=0,f,threadsNo=pthread_num_processors_np(),size,residue;
	if(TL)
	threadsNo=TL;
	if(b.b+b.c>a.b+a.c)
	{
		Number temp=b;
		b=a;
		a=temp;
	}
	size=a.b+a.c;
	if(threadsNo>size)
	threadsNo=size;
	Number c[threadsNo];
	Pass d[threadsNo];
	pthread_t threads[threadsNo];
	residue=size%threadsNo;
	for(i=0;i<threadsNo;i++)
	{
		f=size/threadsNo+(0<residue--);
		c[i].a=calloc(f+b.b+b.c,sizeof(int));
		c[i].b=e;
		e+=f;
		c[i].c=e-1;
		d[i].a=c+i;
		d[i].b=a;
		d[i].c=b;
		pthread_create(threads+i,NULL,(void*)Mul,d+i);
	}
	for(i=0;i<threadsNo;i++)
	pthread_join(threads[i],NULL);
	finalAdd(c,threadsNo);
	c[0].b=a.b+b.b;
	c[0].c=a.c+b.c;
	if(a.d==b.d)
	c[0].d='+';
	else
	c[0].d='-';
	e=c[0].b+c[0].c-1;
	fix(c);
	return c[0];
}
void*Fac(Number*a)
{
	int b,f,g,i,temp=a->a[0]-a->c,lim=1;
	short k,l,m,n;
	while(temp>0)
	{
		b=0;
		k=temp%D;
		l=temp/D;
		for(i=0;i<lim || a->a[i];i++)
		{
			m=a->a[i]%D;
			n=a->a[i]/D;
			f=l*m+n*k;
			g=l*n+f/D;
			f=k*m+f%D*D;
			a->a[i]=f+b;
			b=g;
		}
		a->a[i]+=b;
		lim=i;
		if(a->a[a->b])
		{
			a->a=realloc(a->a,sizeof(int)*(++a->b+1));
			a->a[a->b]=0;
		}
		temp-=a->c;
	}
	for(i=a->b;!a->a[i];i--);
	a->b=i+1;
	a->c=0;
	a->d='+';
	for(b=0;i>b;b++)
	{
		temp=a->a[i];
		a->a[i--]=a->a[b];
		a->a[b]=temp;
	}
	a->a=realloc(a->a,sizeof(int)*a->b);
}
Number fac(Number a)
{
	Number b;
	double d=0;
	int i,e=0,threadsNo=pthread_num_processors_np();
	if(TL)
	threadsNo=TL;
	if(a.b>1 || a.d!='+')
	{
		if(a.d!='+')
		printf("Factorial of a negative");
		else if(a.b>1)
		printf("Factorial of very big number");
		printf(".\nCalculations will continue with the same number.\n");
		b=newNumber(a.a,a.b,a.c,a.d);
		return b;
	}
	if(a.c)
	printf("Decimal digits in factorial will be ignored.\n");
	if(a.d=='+' && a.a[0]==0)
	{
		b.a=malloc(sizeof(int));
		b.a[0]=1;
		b.b=1;
		b.c=0;
		b.d='+';
		return b;
	}
	if(threadsNo>a.a[0]/2)
	threadsNo=a.a[0]/2+(a.a[0]<2);
	Number c[threadsNo];
	pthread_t threads[threadsNo];
	for(i=1;i<=a.a[0];i++)
	d+=log10(i);
	c[0].b=d/8/threadsNo+1;
	for(i=0;i<threadsNo;i++)
	{
		c[i].b=c[0].b;
		c[i].a=calloc(c[i].b+1,sizeof(int));
		c[i].a[0]=a.a[0]-i;
		c[i].c=threadsNo;
		pthread_create(threads+i,NULL,(void*)Fac,c+i);
	}
	for(i=0;i<threadsNo;i++)
	pthread_join(threads[i],NULL);
	for(i=1;i<threadsNo;i++)
	{
		b=mul(c[0],c[i]);
		free(c[0].a);
		free(c[i].a);
		c[0]=b;
	}
	return c[0];
}
int divLess(Number b,Number a)
{
	int ib,ia,jb,ja;
	for(ib=0;b.a[ib]==0;ib++);
	for(ia=0;a.a[ia]==0;ia++);
	for(jb=B;b.a[ib]/jb==0;jb/=10);
	for(ja=B;a.a[ia]/ja==0;ja/=10);
	do
	{
		if(b.a[ib]/jb%10<a.a[ia]/ja%10)
		return 1;
		if(b.a[ib]/jb%10>a.a[ia]/ja%10)
		return 0;
		if((jb/=10)==0)
		{
			jb=B;
			ib++;
		}
		if((ja/=10)==0)
		{
			ja=B;
			ia++;
		}
	}while(ib<b.b+b.c && ia<a.b+a.c);
	if(ib<b.b+b.c)
	return 0;
	while(jb!=B && jb)
	{
		if(b.a[ib]/jb%10)
		return 0;
		jb/=10;
	}
	return 1;
}
int less(int a,int b)
{
	if(a<b)
	return a;
	return b;
}
Number divi(Number a,Number b)
{
	int i,j=0,k,i1=0,i2,ia=0,ib=0,ic=0,intpart;
	long long i3,da,di;
	short*ai=NULL,*bi,*ci,flag=0;
	Number result;
	if((b.a[0]==0 || b.a[0]==1 && b.b==1) && b.c==0 || a.a[0]==0 && a.c==0)
	{
		if(b.a[0]==0 && b.c==0)
		printf("Zero in denominator.\nCalculations will continue with the numerator.\n");
		result=newNumber(a.a,a.b,a.c,a.d);
		return result;
	}
	/*	fix a	*/
	for(i=0;a.a[i]==0;i++);
	if(a.a[i]/D==0)
	{
		ai=malloc(sizeof(short));
		ai[ia++]=a.a[i++]%D;
	}
	while(i<a.b+a.c)
	{
		ai=realloc(ai,sizeof(short)*(2+ia));
		ai[ia++]=a.a[i]/D;
		ai[ia++]=a.a[i++]%D;
	}
	while(ai[--ia-1]==0 && ai[ia]==0);
	if(ai[ia]==0)
	ai=realloc(ai,sizeof(short)*ia--);
	/*	fix b	*/
	for(i=0;b.a[i]==0;i++);
	for(i1=B;b.a[i]/i1==0;i1/=10);
	if(i1>=D)
	{
		i1=i1*10;
		i2=i1/D;
		bi=malloc(sizeof(short)*2);
		bi[ib++]=b.a[i]%i1/i2;
	}
	else
	{
		i2=i1*10;
		i1=i2*D;
		bi=malloc(sizeof(short));
	}
	i3=C/i1;
	bi[ib]=b.a[i++]%i2*i3;
	while(i<b.b+b.c)
	{
		bi[ib++]+=b.a[i]/i1;
		bi=realloc(bi,sizeof(short)*(ib+2));
		bi[ib++]=b.a[i]%i1/i2;
		bi[ib]=b.a[i++]%i2*i3;
	}
	while(bi[ib-1]==0 && bi[ib]==0)
	ib--;
	if(bi[ib]==0)
	bi=realloc(bi,sizeof(short)*ib--);
	/*	find integer part size	*/
	for(i1=0;a.a[i1]==0;i1++);
	for(i2=0;b.a[i2]==0;i2++);
	for(k=B;a.a[i1]/k==0;k/=10)
	j--;
	for(k=B;b.a[i2]/k==0;k/=10)
	j++;
	j+=divLess(b,a);
	if(i1 && !i2)
	{
		i1--;
		k=-b.b-i1;
	}
	else if(!i1 && i2)
	{
		i2--;
		k=a.b+i2;
	}
	else if(i1)
	k=i2-i1;
	else
	k=a.b-b.b;
	if(j<=0)
	{
		j+=8;
		k--;
	}
	if(k<=0)
	{
		result.b=1;
		if(k==0)
		intpart=0;
		else
		intpart=k+1;
	}
	else
	{
		result.b=k+1;
		intpart=k;
	}
	intpart=intpart*2-(j<5)+4;
	/*	calc	*/
	i=0;
	da=ai[i++]*D;
	if(i<=ia)
	da+=ai[i++];
	ci=malloc(sizeof(short));
	ci[ic]=da/bi[0];
	if(ci[ic++]!=da/bi[0])
	{
		ci=realloc(ci,sizeof(short)*2);
		ci[ic-1]=da/bi[0]/D;
		ci[ic++]=da/bi[0]%D;
	}
	if((i<=ia || di) && ic<DL*2+intpart)
	{
		while((i<=ia || di) && ic<DL*2+intpart)
		{
			di=da%bi[0];
			if(i<=ia)
			da=ai[i]+di*D;
			else
			da=di*D;
			i2=less(ic,ib);
			i3=0;
			for(i1=2;i1<=i2;i1++)
			i3+=ci[ic-i1]*bi[i1];
			if(ib)
			da-=i3+ci[ic-1]*bi[1];
			while(da<0)
			{
				ci[ic-1]--;
				di+=bi[0];
				if(i<=ia)
				da=ai[i]+di*D;
				else
				da=di*D;
				if(ib)
				da-=i3+ci[ic-1]*bi[1];
			}
			flag=1;
			if(++ic<DL*2+intpart)
			{
				flag=0;
				ci=realloc(ci,sizeof(short)*(ic));
				ci[ic-1]=da/bi[0];
			}
			i++;
		}
		if(flag)
		ic--;
	}
	free(ai);
	free(bi);
	for(i=0;i<ic;i++)
	if(ci[i]<0 && i+1<ic && ci[i+1]>=D)
	{
		ci[i]++;
		ci[i+1]-=D;
	}
	else if(ci[i]<0)
	{
		ci[i-1]--;
		ci[i]+=D;
	}
	/* Create Number */
	if(k<=0)
	{
		result.a=calloc(1-k+(ic+1)/2+1,sizeof(int));
		i=-k;
	}
	else
	{
		result.a=malloc(sizeof(int)*((ic+1)/2+1));
		i=0;
	}
	i2=1;
	for(i1=1;i1<j;i1++)
	i2*=10;
	for(i1=D;ci[0]/i1==0;i1/=10);
	j=0;
	i3=i2/i1;
	if(i3)
	{
		result.a[i]=ci[j++]*i3;
		if(i3>=D && j<ic)
		{
			i2=i3/D;
			result.a[i]+=ci[j++]*i2;
		}
		else
		{
			i2=i3;
			i3*=D;
		}
		i1=D/i2;
		if(j<ic)
		result.a[i]+=ci[j]/i1;
	}
	else
	{
		i3=i2*D/i1*D;
		i1=i1/i2;
		result.a[i]=ci[j]/i1;
		i2=i3/D;
	}
	i++;
	while(j<ic)
	{
		result.a[i]=ci[j++]%i1*i3;
		if(j<ic)
		result.a[i]+=ci[j++]*i2;
		if(j<ic)
		result.a[i]+=ci[j]/i1;
		i++;
	}
	if(i>result.b+DL)
	i=result.b+DL;
	if(i<result.b)
	{
		result.a=realloc(result.a,sizeof(int)*result.b);
		while(i<result.b)
		result.a[i++]=0;
	}
	else
	result.a=realloc(result.a,sizeof(int)*i);
	result.c=i-result.b;
	free(ci);
	if(a.d==b.d)
	result.d='+';
	else
	result.d='-';
	fix(&result);
	return result;
}
Number powe(Number a,Number b)
{
	int i,j,c=b.a[0],d=0,e=b.d;
	Number temp;
	if(b.b>1 || b.b==1 && (c==0 || c==1))
	{
		if(b.b==1 && c==0)
		{
			int f=1;
			b=newNumber(&f,1,0,'+');
		}
		else
		{
			if(b.b>1)
			printf("Very big exponent.\nCalculations will continue with the base.\n");
			b=newNumber(a.a,a.b,a.c,a.d);
		}
		return b;
	}
	if(b.c)
	printf("Decimal digits in exponent will be ignored.\n");
	for(i=nextPrime(0);i<=c;i=nextPrime(i))
	while(c%i==0)
	{
		c/=i;
		b=a;
		for(j=1;j<i;j++)
		{
			temp=mul(a,b);
			if(j>1){
			free(a.a);}
			a=temp;		
		}
		if(d){
		free(b.a);}
		d=1;
	}
	if(e=='-')
	{
		int f=1;
		b=newNumber(&f,1,0,'+');
		temp=divi(b,a);
		free(b.a);
		free(a.a);
		return temp;
	}
	return a;
}
Number root(Number a,Number b)
{
	int c=1;
	Number n0,n1,x0,x1,temp1,temp2,temp3;
	if(a.b>1 || b.d=='-' && a.a[0]%2==0)
	{
		if(a.b>1)
		printf("Very large root.\n");
		else
		printf("Even root for a negative base.\n");
		printf("Calculations will continue with the base.\n");
		int i;
		x1=b;
		x1.a=malloc(sizeof(int)*(b.b+b.c));
		for(i=0;i<b.b+b.c;i++)
		x1.a[i]=b.a[i];
		return x1;
	}
	if(a.c)
	printf("Decimal digits in root will be ignored.\n");
	x0=newNumber(&c,1,0,'+');
	x1.a=NULL;
	n0=newNumber(a.a,1,0,'+');
	c=a.a[0]-1;
	n1=newNumber(&c,1,0,'+');
	do
	{
		if(x1.a)
		{
			free(x0.a);
			x0=x1;
		}
		temp1=mul(n1,x0);
		temp2=powe(x0,n1);
		temp3=divi(b,temp2);
		free(temp2.a);
		temp2=add(temp1,temp3,b.d,'a');
		free(temp1.a);
		free(temp3.a);
		x1=divi(temp2,n0);
		free(temp2.a);
	}while(equality(x0,x1));
	free(x0.a);
	if(a.d=='-')
	{
		c=1;
		b=newNumber(&c,1,0,'+');
		a=divi(b,x1);
		free(b.a);
		free(x1.a);
		return a;
	}
	return x1;
}
Number comb(Number a,Number b)
{
	if(equality(a,b)==2 || a.d!='+' || b.d!='+' || a.b>1 || b.b>1)
	{
		if(a.d!='+' || b.d!='+')
		printf("Negative number in binomial coefficient");
		else if(equality(a,b)==2)
		printf("Second number larger than first in binomial coefficient");
		else
		printf("Very big number for binomial coefficient");
		printf(".\nCalculations will continue with the first number.\n");
		Number temp=newNumber(a.a,a.b,a.c,a.d);
		return temp;
	}
	Number temp1=add(a,b,'+','s'),temp2=fac(b),temp3=fac(temp1);
	free(temp1.a);
	temp1=mul(temp2,temp3);
	free(temp2.a);
	free(temp3.a);
	temp3=fac(a);
	temp2=divi(temp3,temp1);
	free(temp3.a);
	free(temp1.a);
	return temp2;
}
Number process(char*a,Number*b)
{
	int num,ch,i,j;
	Number temp;
	while(a[1])
	{
		for(i=1;a[i]=='0';i++);
		if(a[i]=='!')
		temp=fac(b[i-1]);
		else if(a[i]=='*')
		temp=mul(b[i-2],b[i-1]);
		else if(a[i]=='/')
		temp=divi(b[i-2],b[i-1]);
		else if(a[i]=='^')
		temp=powe(b[i-2],b[i-1]);
		else if(a[i]=='r')
		temp=root(b[i-2],b[i-1]);
		else if(a[i]==':')
		temp=comb(b[i-2],b[i-1]);
		else if(b[i-2].d=='+' && (a[i]=='+' && b[i-1].d=='+' || a[i]=='-' && b[i-1].d=='-'))
		temp=add(b[i-2],b[i-1],'+','a');
		else if(b[i-2].d=='-' && (a[i]=='+' && b[i-1].d=='-' || a[i]=='-' && b[i-1].d=='+'))
		temp=add(b[i-2],b[i-1],'-','a');
		else if(equality(b[i-2],b[i-1])==1 && b[i-2].d=='-' && (a[i]=='+' && b[i-1].d=='+' || a[i]=='-' && b[i-1].d=='-'))
		temp=add(b[i-2],b[i-1],'-','s');
		else if(equality(b[i-2],b[i-1])==2 && b[i-2].d=='+' && (a[i]=='+' && b[i-1].d=='-' || a[i]=='-' && b[i-1].d=='+'))
		temp=add(b[i-1],b[i-2],'-','s');
		else if(equality(b[i-2],b[i-1])==1 && b[i-2].d=='+' && (a[i]=='+' && b[i-1].d=='-' || a[i]=='-' && b[i-1].d=='+'))
		temp=add(b[i-2],b[i-1],'+','s');
		else
		temp=add(b[i-1],b[i-2],'+','s');
		free(b[i-1].a);
		num=0;
		for(ch=0;a[ch];ch++)
		if(a[ch]=='0')
		num++;
		if(a[i]!='!')
		{
			free(b[i-2].a);
			b[i-2]=temp;
			for(j=i;j<ch;j++)
			{
				a[j-1]=a[j+1];
				if(j<num)
				b[j-1]=b[j];
			}
			a=(char*)realloc(a,ch-1);
			b=(Number*)realloc(b,sizeof(Number)*(num-1));
		}
		else
		{
			b[i-1]=temp;
			for(j=i;j<ch;j++)
			a[j]=a[j+1];
			a=(char*)realloc(a,ch);
		}
	}
	temp=*b;
	free(a);
	free(b);
	fix(&temp);
	return temp;
}
void*busy(char*a)
{
	while(flag)
	{
		sleep(100);
		if(kbhit() && getch()=='\\')
		{
			cmd(NULL,0);
			system("cls");
			printf("Press '\\' to enter cmd mode.\n");
			printf("%s",a);
			printf("=\n");
		}
	}
}
int main()
{
	int i,j;
	char*a,*temp;
    double start,stop;
	Number*b;
	pthread_t thread;
	FILE*file;
	PR.a=NULL;
	PR.b=0;
	readSV();
	scanSettings();
	printInput(NULL);
	while(1)
	{
		a=input();
		temp=malloc(sizeof(char)*(strlen(a)+1));
		for(i=0;a[i];i++)
		temp[i]=a[i];
		temp[i]=0;
		flag=1;
		pthread_create(&thread,NULL,(void*)busy,temp);
		free(PR.a);
		start=(double)clock()/CLOCKS_PER_SEC;
		b=postfix(&a);
		PR=process(a,b);
    	stop=(double)clock()/CLOCKS_PER_SEC;
    	flag=0;
    	pthread_join(thread,NULL);
    	free(temp);
		print(PR);
		j=B;
		for(i=0;i<8 && PR.a[0]/j==0;i++)
		j/=10;
		i=PR.b*8-i;
    	printf("%d integer digits\n%.3f sec\n\n",i,stop-start);
    	if(PF)
    	{
			file=fopen(RF,"a");
	    	fprintf(file,"%d integer digits\n%.3f sec\n\n",i,stop-start);
	    	fclose(file);
		}
	}
}
