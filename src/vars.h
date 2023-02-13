/*
 * vars.h
 */

 // logging vars
 byte logFlags;
// #define secFlag    1
// #define minFlag    2
// #define hourFlag   4
// #define dayFlag    8
// #define weekFlag   16
// #define monthFlag  32
// #define yearFlag   64

bool hourFlag = false;
bool dayFlag = false;
bool weekFlag = false;
bool monthFlag = false;
bool yearFlag=false;
int EverSoOften = 0;

#define SET_FLAG(n, f) n = n | f;   //((n) |= (f)) 
#define CLR_FLAG(n, f) n = n & !f;  //((n) &= ~(f)) 
//#define TGL_FLAG(n, f) ((n) ^= (f)) 
#define CHK_FLAG(n, f) (n & f) //((n) & (f))

int checkMinute = 0; // holds when doHourly was called 
struct logvars{
  char hourE1[12];  // delivered tariff 1
  char hourE2[12];  // delivered tariff 2
  char hourR1[12];  // return tariff 1
  char hourR2[12];  // return tariff 2
  char hourTE[12];   // total tariff 1
  char hourTR[12];   // total tariff 2
  char hourG[12];   // Gas

  char dayE1[12];
  char dayE2[12];
  char dayR1[12];
  char dayR2[12];
  char dayTE[12];
  char dayTR[12];
  char dayG[12];

  char weekE1[12];
  char weekE2[12];
  char weekR1[12];
  char weekR2[12];
  char weekTE[12];
  char weekTR[12];
  char weekG[12];

  char monthE1[12];
  char monthE2[12];
  char monthR1[12];
  char monthR2[12];
  char monthTE[12];
  char monthTR[12];
  char monthG[12];

  char yearE1[12];
  char yearE2[12];
  char yearR1[12];
  char yearR2[12];
  char yearTE[12];
  char yearTR[12];
  char yearG[12];
} log_data = {};
