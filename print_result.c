#include <stdio.h>
#include <stdlib.h>

int main()
{
	FILE *fp;
	long int max,curr,i=0,genre_tag=1;
	fp = fopen("result.txt","r+");
	fscanf(fp,"%ld",&curr);
	max = curr;
	for(i=1;i<10;i++)
	{
		fscanf(fp,"%ld",&curr);
		if(curr > max)
		{
			max = curr;
			genre_tag = i+1;
		}
	}
	switch(genre_tag)
	{
		case 1:
			printf("Predicted genre: Classical\n");
			break;
		case 2:
			printf("Predicted genre: Country\n");
			break;
		case 3:
			printf("Predicted genre: Jazz\n");
			break;
		case 4:
			printf("Predicted genre: Rock\n");
			break;
		case 5:
			printf("Predicted genre: Hiphop\n");
			break;
		case 6:
			printf("Predicted genre: Pop\n");
			break;
		case 7:
			printf("Predicted genre: Disco\n");
			break;
		case 8:
			printf("Predicted genre: Blues\n");
			break;
		case 9:
			printf("Predicted genre: Reggae\n");
			break;
		case 10:
			printf("Predicted genre: Metal\n");
			break;
	}
	/*FILE *fp1 = fopen("matrix.txt","a+");
	fprintf(fp1, "%ld\n",genre_tag );
	fclose(fp1);*/
	return 0;
}
