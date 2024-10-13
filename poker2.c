#include<stdio.h>
#include<stdlib.h>
#include<time.h>

/* カードの並び変数と先頭番号 */
static int  KARD[52], KD;

/* カード表示関数 */
static void hyouji(int kd[])
{
	char   *mk[] = {"★","◇","▲","○"};
	char   *no[] = {"２","３","４","５","６","７","８","９","10","Ｊ","Ｑ","Ｋ","Ａ"};

	printf( "┏━┓┏━┓┏━┓┏━┓┏━┓\n");
	printf( "┃%s┃┃%s┃┃%s┃┃%s┃┃%s┃\n",mk[(kd[0]%4)],mk[(kd[1]%4)],mk[(kd[2]%4)],mk[(kd[3]%4)],mk[(kd[4]%4)]);
	printf( "┃%s┃┃%s┃┃%s┃┃%s┃┃%s┃\n",no[(kd[0]/4)],no[(kd[1]/4)],no[(kd[2]/4)],no[(kd[3]/4)],no[(kd[4]/4)]);
	printf( "┗━┛┗━┛┗━┛┗━┛┗━┛\n");
}

/* カードを整列させる */
static void seiretu(int kd[], int b1, int b2, int b3, int b4, int b5 )
{
	b1 = kd[b1];
	b2 = kd[b2];
	b3 = kd[b3];
	b4 = kd[b4];
	b5 = kd[b5];

	kd[0] = b1;
	kd[1] = b2;
	kd[2] = b3;
	kd[3] = b4;
	kd[4] = b5;
}

/* カードの役を判定するプログラム */
static int hantei(int kd[])
{
	int   p, i, j, c, yk=0;

	/* カードを数値の高い順に並べ替える */
	do{
		j = 0;
		for(i = 0; i < 4; i++ ){
			if (kd[i] < kd[i+1]){
				c = kd[i];  kd[i] = kd[i+1];
				kd[i+1] = c;
				j++;
			}
		}
	}while(j != 0);
	
	/* ４つの隣り合う組のパターンを２進数にする */
	for(p = 0, i = 0; i < 4; i++ ){
		p *= 2;
		if (kd[i]/4 == kd[i+1]/4) p++;
	}

	/* 役の判定 */
	switch(p){
		/* ノーペア，ストレート，フラッシュの判定 */
		case 0b0000:
		{
			if ( kd[0]/4 - kd[4]/4 == 4 )              yk = 4; /* ストレート */
			else if ( kd[0]/4 == 12 && kd[1]/4 == 3 ){
				yk = 4; /* Ａから５までのストレート */
				seiretu(kd,1,2,3,4,0);
			}

			/* カードのマークを調べる */
			for ( i = 0, j = 0; i < 4; i++ ) j += ( kd[i]%4 == kd[i+1]%4 );
			if (j == 4){
				if ( yk == 4 ){
					yk = 8;/* ストレートフラッシュ */
					if(kd[0]/4 == 12 && kd[4]/4 == 8){
						yk = 9; /* ロイヤルストレートフラッシュ */
					}
				}else{
					yk = 5;/* フラッシュ */
				}
			}
			break;
		}
		/* ワンペア */
		case 0b0001: seiretu(kd,3,4,0,1,2); yk = 1; break;
		case 0b0010: seiretu(kd,2,3,0,1,4); yk = 1; break;
		case 0b0100: seiretu(kd,1,2,0,3,4); yk = 1; break;
		case 0b1000:                        yk = 1; break;

		/* ツーペア */
		case 0b0101: seiretu(kd,1,2,3,4,0); yk = 2; break;
		case 0b1001: seiretu(kd,0,1,3,4,2); yk = 2; break;
		case 0b1010:                        yk = 2; break;
		
		/* スリーカード */
		case 0b0011: seiretu(kd,2,3,4,0,1); yk = 3; break;
		case 0b0110: seiretu(kd,1,2,3,0,4); yk = 3; break;
		case 0b1100:                        yk = 3; break;
		
		/* フルハウス */
		case 0b1011: seiretu(kd,2,3,4,0,1); yk = 6; break;
		case 0b1101:                        yk = 6; break;

		/* フォーカード */
		case 0b0111: seiretu(kd,1,2,3,4,0); yk = 7; break;
		case 0b1110:                        yk = 7; break;
	}

	return(yk);
}

/* 乱数を返す関数 */
static int rnd(int n)
{
	return(random() & 0xffff) * n >> 16;
}

/* カードのシャッフルをする関数 */
static void shaffle( void )
{
	int i, n, m, k;
	
	for(i = 0; i < 500; i++ ){
		n = rnd(52);
		m = rnd(52);
		k = KARD[m]; KARD[m] = KARD[n]; KARD[n] = k;
	}
}

int main( void )
{
	char *yaku[] = { "ノーペア","ワンペア","ツーペア","スリーカード","ストレート","フラッシュ",
					"フルハウス","フォーカード","ストレートフラッシュ","ロイヤルストレートフラッシュ" };
	int  kd[5], cpu_kd[5], yk, cpu_yk, i, win = 0, lose = 0, draw = 0, kekka_yaku[10];
	char ny[6];
	time_t tm;

	for ( i = 0; i < 52; i++ ) KARD[i] = i; /* カードを初期化する */
	for ( i = 0; i < 10; i++ ) kekka_yaku[i] = 0; /* 役の結果を初期化 */

	time( &tm ); srandom( tm ); /* 乱数の基数を時間変数に設定 */

	printf("=============================\n");
	printf("＄　ＰＯＫＥＲ　 ＧＡＭＥ　＄\n");
	printf("=============================\n");

	do{
		shaffle(); /* カードのシャッフルをする */
		KD = 0;    /* カードは先頭から */

		kd[0] = KARD[KD++]; /* カードを５枚配る */
		kd[1] = KARD[KD++];
		kd[2] = KARD[KD++];
		kd[3] = KARD[KD++];
		kd[4] = KARD[KD++];

		cpu_kd[0] = KARD[KD++]; /* CPUにカードを５枚配る */
		cpu_kd[1] = KARD[KD++];
		cpu_kd[2] = KARD[KD++];
		cpu_kd[3] = KARD[KD++];
		cpu_kd[4] = KARD[KD++];

		printf("カードが配られました\n");

		/* 最初のカードの表示 */
		cpu_yk = hantei( cpu_kd );
		printf( "┏━┓┏━┓┏━┓┏━┓┏━┓\n");
		printf( "┃* ┃┃* ┃┃* ┃┃* ┃┃* ┃\n");
		printf( "┃ *┃┃ *┃┃ *┃┃ *┃┃ *┃\n");
		printf( "┗━┛┗━┛┗━┛┗━┛┗━┛\n");

		yk = hantei( kd );
		hyouji( kd );

		printf(" -１-  -２-  -３-  -４-  -５- \n");
		printf("現在%sです。\n",yaku[yk]);
		printf("交換するカードの番号を続けて入力して下さい （しない時は０）\n");

		/* カードの交換をする */
		for(i = 0;i < 6;i++) ny[i] = 0;
		scanf("%s",ny);
		if (ny[0] != '0'){ 
			for( i = 0; i < 5; i++ ){
				if( '6' > ny[i] && ny[i] > '0' ) kd[ny[i]-0x31] = KARD[KD++];
			}
		}

		/* CPUのカード交換（適当）*/
		{
			int nr = rnd(5);
			int c[] = {0,1,2,3,4};
			char change[6];

			/* 5回手札をシャッフル */
			for(i = 0;i < nr;i++){
				int c1 = rnd(5);
				int c2 = rnd(5);
				int cc;
				
				cc = c[c1];
				c[c1] = c[c2];
				c[c2] = cc;
			}

			/* 相手は1－5枚で手札を左からランダムに交換 */
			nr = rnd(5)+1;
			printf("相手の交換は%d枚です\n",nr);
			for(i = 0; i < nr; i++){
				change[i] = '0' + c[i];
			}
			change[nr] = '\0';

			for( i = 0; i < 5; i++ ){
				if( '6' > change[i] && change[i] > '0' ) cpu_kd[change[i]-'0'] = KARD[KD++];
			}
		}

		printf("カードが交換されました\n");
#if 0
		/* カードをロイヤルストレートフラッシュに無理やりして引き分けのチェック用 */
		cpu_kd[0] = 12*4;
		cpu_kd[1] = 11*4;
		cpu_kd[2] = 10*4;
		cpu_kd[3] = 9*4;
		cpu_kd[4] = 8*4;
		kd[0] = 12*4+1;
		kd[1] = 11*4+1;
		kd[2] = 10*4+1;
		kd[3] = 9*4+1;
		kd[4] = 8*4+1;
#endif

		/* CPUのカードの結果表示 */
		cpu_yk = hantei( cpu_kd );
		hyouji( cpu_kd );

		/* 自分のカードの結果表示 */
		yk = hantei( kd );
		hyouji( kd );

		/* 結果表示 */
		printf("あなたの役は%sです\n",yaku[yk]);
		printf("相手の役は%sです\n",yaku[cpu_yk]);
		kekka_yaku[yk]++;

		if(yk < cpu_yk) {
			printf("あなたの負けです\n");
			lose++;
		}else if(yk > cpu_yk){
			printf("あなたの勝ちです\n");
			win++;
		}else{
#if 0
			printf("引き分けです\n");
			draw++;
#else /* 2024/10/13 カードの力関係でのスカウター判定を追加 */
			unsigned long power = 0, cpu_power = 0;
			int i;
			for(i = 0;i < 5;i++){
				power |= ((kd[i]/4+1) << ((4-i)*4));
				cpu_power |= ((cpu_kd[i]/4+1) << ((4-i)*4));
			}
			if(power > cpu_power){
				printf("引き分けですが、カードの力関係であなたの勝ちです\n");
				printf("あなた %dパワー 相手 %dパワー\n",power, cpu_power);
				win++;
			}else if(power < cpu_power){
				printf("引き分けですが、カードの力関係であなたの負けです\n");
				printf("あなた %dパワー 相手 %dパワー\n",power, cpu_power);
				lose++;
			}else{
				printf("完全にカードが一致していて引き分けです\n");
				printf("あなた %dパワー 相手 %dパワー\n",power, cpu_power);
				draw++;
			}
#endif
		}

		printf("やめますか？('y'es / 'n'o ) ");
		scanf("%s",ny);
		printf("\n");
	}while(ny[0] != 'y');
	
	printf("%d試合中 %d勝、%d敗、%d引き分けです。\n", win+lose+draw, win, lose, draw);
	for( i = 0; i < 10; i++ ){
		printf("%s %d回\n", yaku[i], kekka_yaku[i]);
	}
	
	return 0;
}
















