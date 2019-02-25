/**
 * main_swhw.c
 * HSC proj
 * Edited from cpu.c and main_sw.c 
 * xormos00 12.12.2018
 * added own part to main()
 */

#include <fitkitlib.h>
#include "../fpga/src_filter/addr_space.h"

#include "../cpu/common.h"

/*******************************************************************************
 * Vypis uzivatelske napovedy (funkce se vola pri vykonavani prikazu "help")
*******************************************************************************/
void print_user_help(void) {
}

/*******************************************************************************
 * Dekodovani uzivatelskych prikazu a jejich vykonavani
*******************************************************************************/
unsigned char decode_user_cmd(char *cmd_ucase, char *cmd) {
   return (CMD_UNKNOWN);
}

/*******************************************************************************
 * Inicializace periferii/komponent po naprogramovani FPGA
*******************************************************************************/
void fpga_initialized() {
}

/*******************************************************************************
 Funkce fpga_read() slouzi pro cteni hodnoty z adresoveho prostoru FPGA.

 Vstupy:
   addr     index 32-bitove polozky v ramci adresoveho prostoru FPGA
 Vystupy:
   navratova hodnota reprezentuje prectenou hodnotu z FPGA
*******************************************************************************/
unsigned long fpga_read(int addr) {

   unsigned long val;
   FPGA_SPI_RW_AN_DN(SPI_FPGA_ENABLE_READ, addr, (unsigned char *)&val, 2, 4);

   return val;
}

/*******************************************************************************
 Funkce fpga_write() slouzi pro zapis hodnoty do adresoveho prostoru FPGA.

 Vstupy:
   addr     index 32-bitove polozky v ramci adresoveho prostoru FPGA
   data     data pro zapis
*******************************************************************************/
void fpga_write(int addr, unsigned long data) {

   FPGA_SPI_RW_AN_DN(SPI_FPGA_ENABLE_WRITE, addr, (unsigned char *)&data, 2, 4);
}

/***************************************************************************
 Funkce otsu() vypocte hodnotu prahu na zaklade histogramu pixelu snimku.

 Vstupy:
    hist    ukazatel na histogram
    n       pocet polozek histogramu
 Vystupy:
    hodnota vypocteneho prahu
***************************************************************************/
int otsu(long *hist, int n) {
    
		long total = 0;
    float sum = 0;
    float sumB = 0;
		float varMax = 0;
		float varBetween;
		float mB, mF;
    long wB = 0;
		long wF = 0;
		long threshold = 0;
    long t;

    for (t=0 ; t<n ; t++) {
        sum += t * hist[t];
        total += hist[t];
    }

    for (t=0 ; t<n ; t++) {
        wB += hist[t];
        if (wB == 0) continue;

        wF = total - wB;
        if (wF == 0) break;

        sumB += (float)(t * hist[t]);

        mB = sumB / wB;
        mF = (sum - sumB) / wF;

        varBetween = (float)wB * (float)wF * (mB - mF) * (mB - mF);
				/* Check if new maximum found */
        if (varBetween > varMax) {
            varMax = varBetween;
            threshold = t+1;
        }
    }
    return threshold;
}

/***************************************************************************
 Procedura histogram_clean() zajistuje vymazani histogramu, ktery je potreba
 zejmena na konci zpracovani kazdeho snimku.

 Vstupy:
   hist     adresa histogramu
***************************************************************************/
void histogram_clean(long *hist) {

   int i;
   for(i=0; i<PIXELS; i++)
      hist[i] = 0;
}

/***************************************************************************
 Pomocna procedura pro tisk vysledku

 Vstupy:
   frame       cislo sminku
   threshold   vypocteny prah
   hist        adresa histogramu
   n           pocet polozek histogramu
***************************************************************************/
void print_results(long frame, long threshold, long *hist, int n) {
    
		int i;
    term_send_str("Frame: "); term_send_num(frame); term_send_crlf();
    term_send_str("Histogram: "); term_send_num(hist[0]);
    for (i = 1; i < n; i++) {
        term_send_str(", "); term_send_num(hist[i]);
    }
    term_send_crlf();
    term_send_str("Threshold: "); term_send_num(threshold); term_send_crlf();
}


/*******************************************************************************
 * Hlavni funkce
*******************************************************************************/
int main(void)
{
   short counter = 0;
   unsigned long mcu_ready;

   initialize_hardware();
   set_led_d6(1);  //rozsvitit LED D6
   set_led_d5(1);  //rozsvitit LED D5
   
  	short new_frame = 0;
  	short prev_frame = 0;
  	short threshold = 0;
    long frame_count = 0;
    long histogram[PIXELS];

   /**************************************************************************/
   /*                      Aktualizovany hlavni program                      */
   /**************************************************************************/
   mcu_ready = 1;
   int i = 0;
   fpga_write(FPGA_MCU_READY, mcu_ready);
   while(fpga_read(FPGA_MCU_READY) != 2);

   term_send_str("Both FPGA and MCU are ready."); 
   term_send_crlf();

   set_led_d5(0);  //zhasnout LED D5

	 while ((frame_count + new_frame) < FRAMES) {
        
				delay_ms(1);  //zpozdeni 1ms
        new_frame = fpga_read(FPGA_FRAME_CNT);
        
				if ((new_frame != prev_frame) && (new_frame % 10) == 0) {
            for (i = 0 ; i < PIXELS ; i++) {
                histogram[i] = fpga_read(FPGA_HISTOGRAM + i);
                fpga_write(FPGA_HISTOGRAM + i, 0);
            }
            threshold = otsu(histogram, PIXELS);
            fpga_write(FPGA_THRESHOLD, FPGA_THRESHOLD);

            if (new_frame % 100 == 0) {
                frame_count += 100;
                print_results(frame_count, threshold, histogram, PIXELS);
            }
            prev_frame = new_frame;
        }
        counter++;
        if (counter == 500) {
            flip_led_d6(); //invertovat LED
            counter = 0;
        }
        terminal_idle();  // obsluha terminalu
    }
}

