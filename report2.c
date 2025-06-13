#include "gba1.h"
#include "8x8ascii.h"
#define CURRENT_TIME *((unsigned short *)0x0400010C)

typedef unsigned char  byte;

void display_time(hword val);//タイム描画メソッド
void draw_point(hword x,hword y,hword color);//点描画メソッド
void draw_fill();//画面塗りつぶしメソッド
void locate(hword cx,hword cy);//文字描画位置設定メソッド
void print_ascii(byte asii_num,hword color);//ascii描画メソッド
void prints(byte *str,hword color);//文字列描画メソッド
hword mod(hword dividened, hword divisor);//割り算メソッド
hword div(hword dividened, hword divisor);//剰余メソッド
void init_timer(void);//初期タイマー設定メソッド
void stop_game();//ゲームの実行メソッド


hword *ptr,*ptr2;//ポインタ名
hword BLACK = BGR(0x00,0x00,0x00);//黒色
hword WHITE = BGR(0x1F,0x1F,0x1F);//白色
hword ORENGE = BGR(0x1F,0x0F,0x00);//オレンジ
hword RED = BGR(0x1F,0x00,0x00);//赤色
hword GREEN = BGR(0x00,0x1F,0x00);//緑色
hword CYAN = BGR(0x00,0x1F,0x1F);//シアン
point p;//文字の座標

int main(void) {
    while (1) {
        // ゲームを開始
        stop_game(); 
    }
    return 0;
}

void stop_game(){
    volatile unsigned short difference_time;
    hword key;//キーの状態を保存する変数

    //初期化
    ptr = (hword*)IOBASE;
    *ptr = 0x0F03;

    //タイマー設定
    init_timer();

    //START画面を表示
    locate(0,2);
    prints("______________________________",ORENGE);
    locate(11,4);
    prints("STOP GAME",ORENGE);
    locate(0,5);
    prints("______________________________",ORENGE);
    locate(4,10);
    prints("Stop at exactly 10 sec!",WHITE);
    locate(8,15);
    prints("[ PRESS START ]",RED);
    

    //STARTボタンが押されたときの処理
    while(1){
        //キーの状態を取り出す
        ptr2 = (hword*)KEY_STATUS;
        key = (~*ptr2) & 0x000F;

        if(key==KEY_START){//STARTが押されたら
            draw_fill();//スタート画面を塗りつぶす
            *((hword *)0x04000102) = 0x0080;//タイマースイッチON
            break;
        }

    }

    //ストップボタンが押されたときの処理
    while(1){
        //キーの状態を取り出す
        ptr2 = (hword*)KEY_STATUS;
        key = (~*ptr2) & 0x000F;

        if(key==KEY_A){//Aが押されたら
            *((hword *)0x04000102) = 0x0000;//タイマー0_スイッチOFF
            break;
        }

        //ゲーム中の画面描画
        locate(0,2);
        prints("______________________________",CYAN);
        locate(9,4);
        prints("Stay Focused!",CYAN);
        locate(0,5);
        prints("______________________________",CYAN);
        locate(6,15);
        prints("[ PRESS Z to STOP ]",GREEN);

        //タイムを画面に表示
        locate(11, 10);
        display_time(CURRENT_TIME);
        locate(17,10);
        prints("sec",WHITE);

    }

    locate(6,15);
    prints("                   ",BLACK);

    //結果を表示する
    locate(0,2);
    prints("______________________________",RED);
    locate(9,4);
    prints("   RESULT    ",RED);
    locate(0,5);
    prints("______________________________",RED);
    locate(4,12);
    prints("Differences:",WHITE);
    
    //10秒との差を計算
    if(CURRENT_TIME>1000){
        difference_time = CURRENT_TIME - 1000;
    }else if(CURRENT_TIME <= 1000){
        difference_time = 1000 - CURRENT_TIME;
    }

    //差のタイム表示  
    locate(17,12);
    display_time(difference_time); 
    locate(23,12);
    prints("sec",WHITE);

    
    locate(6, 17);
    prints("[ PRESS X to RETRY ]", ORENGE);

    // リトライが押されたときに処理
    while (1) {
        ptr2 = (hword*)KEY_STATUS;
        key = (~*ptr2) & 0x000F;

        if (key == KEY_B) { // Bボタンが押されたらリトライ
            draw_fill(); // 画面をリセット
            *((hword *)0x0400010C) = 0x0000; 
            init_timer();    // タイマーを再初期化
            return; // メインループに戻る
        }
    }

    while(1);
    return;
}

//タイムを描画するメソッド
void display_time(hword val) {
    byte char_data[] = "0123456789";
    byte buf[7];  // "XX:XX\0" 表示のための配列
    hword ten_seconds, one_second, one_tenth;

    // 10秒、1秒、0.1秒に分割
    one_tenth = mod(val, 10);          
    val = div(val, 10);               
    one_second = mod(val, 10);         
    ten_seconds = div(val, 10); 

    // バッファに各値を格納（ASCII文字に変換）
    buf[0] = char_data[mod(ten_seconds / 10, 10)];  
    buf[1] = char_data[mod(ten_seconds, 10)];       
    buf[2] = ':';                                  
    buf[3] = char_data[mod(one_second, 10)];        
    buf[4] = char_data[mod(one_tenth, 10)];         
    buf[5] = '\0';

    // 表示
    prints(buf, WHITE);
}

//点描画メソッド
void draw_point(hword x,hword y,hword color){
    ptr = (hword*)VRAM;
    ptr = ptr + x + y*240;
    *ptr = color;
}

//画面塗りつぶしメソッド
void draw_fill(){
    hword x,y;
    for(y=0; y<160; y++){
        for(x=0; x<240; x++){
            draw_point(x,y,BLACK);
        }
    }
}

//文字描画開始位置設定メソッド
void locate(hword cx,hword cy){
    if(cx<30){
        p.x = cx << 3;
    }else{
        p.x = 30-1;
    }

    if(cy<20){
        p.y = cy << 3;
    }else{
        p.y = 20-1;
    }
}

//ASCII描画メソッド
void print_ascii(byte ascii_num,hword color){
    hword tx,ty;
    byte cbit;

    for(ty=0; ty<8; ty++){
        cbit = 0x80;

        for(tx=0; tx<8; tx++){
            if((char8x8[ascii_num][ty] & cbit) == cbit){
                draw_point((p.x+tx),(p.y+ty),color);
            }else{
                draw_point((p.x+tx),(p.y+ty),BLACK);
            }
            cbit = cbit >> 1;
        }
    }
}

//文字列描画メソッド
void prints(byte *str,hword color){
    while(*str){
        print_ascii(*str,color);
        *str ++;
        p.x += 8;
    }
}

//割り算メソッド
hword div(hword dividened, hword divisor){
	
	hword quotient = 0;			
	
	while(1){
		if (dividened >= divisor){
			dividened = dividened - divisor;
			quotient++;
		} else {
			break;
		}
	}
	return quotient;
}


//剰余メソッド
hword mod(hword dividened, hword divisor){
	
	hword quotient = 0;			

	while(1){
		if (dividened >= divisor){
			dividened = dividened - divisor;
			quotient++;
		} else {
			break;
		}
	}
	return dividened;
}

//初期タイマー設定メソッド(0.1秒カウントアップ)
void init_timer(void) {
    // タイマーを完全停止
    *((hword *)0x04000102) = 0x0000;  
    *((hword *)0x04000106) = 0x0000;  
    *((hword *)0x0400010A) = 0x0000;  
    *((hword *)0x0400010E) = 0x0000;

    //タイマーを設定
	*((hword *)0x04000100) = 0xFFFF - 1678 + 1;
	*((hword *)0x04000104) = 0xFFFF - 10 + 1;
	*((hword *)0x04000108) = 0xFFFF - 10 + 1;
	*((hword *)0x0400010C) = 0x0000;

    //タイマーを再起動
	*((hword *)0x04000102) = 0x0000;	
	*((hword *)0x04000106) = 0x0084;	
	*((hword *)0x0400010A) = 0x0084;	
	*((hword *)0x0400010E) = 0x0084;	
}