#define FASTLED_ALLOW_INTERRUPTS 1
#define FASTLED_INTERRUPT_RETRY_COUNT 1

#include <FastLED.h>
#include <OneButton.h>
#include <EEPROM.h>
#include "config.h"

const uint8_t randomSeedPin = A0;
const uint32_t randomSeedCorrection = 127773UL;
CRGB zonesCol[zonesQuantity];

uint8_t level1_prev = 0;

typedef struct {
	uint8_t cometStepTimer;// = 50;
	uint8_t cometTail;// = 1;// //длина хвоста
	uint8_t cometBrightness; 
	uint8_t cometReverse; 
	uint8_t cometLaunchTimer;
	uint32_t cometColor;
} comet_t;

comet_t cometsSettings[cometCount];

uint8_t level1_chaos = 0;

uint8_t cometCounter[cometCount]; //

OneButton btn_mode = OneButton(BTN_PIN_MODE, true, true);
OneButton btn_saw = OneButton(BTN_PIN_SAW, true, true);

CRGB szvColor;
CRGB leds[NUM_LEDS];
CRGB leds_temp[NUM_LEDS];
CRGB leds_comet[NUM_COMET_LEDS];
uint8_t leds_brightness[NUM_LEDS];
uint16_t sawValues[NUM_LEDS];
uint8_t sin5[NUM_LEDS];
uint8_t leds_chaos[NUM_LEDS];

uint8_t paletteIndex = 0;

uint8_t cometState[cometCount];// = 0;
uint8_t cometOver[cometCount];// = 1;

uint8_t launch1_enable = 0;
uint8_t launch2_enable = 0;
uint8_t launch3_enable = 0;
uint8_t launch4_enable = 0;

uint8_t level2_1_max = 0;
uint8_t level2_1_min = 0;

uint8_t level2_2_max = 0;
uint8_t level2_2_min = 0;

uint8_t group_by_3 = 1;
uint8_t interval_by_3 = 1024;

uint8_t switchOff = 0; //для выключения комет и созвездий

uint8_t rainbowSlice = 4; //кусок радуги 2 - 1/2 круга, 4 - 1/4 круга и т.д.              
uint8_t effectSlice = 4; //кусок массива эффектов, сколько выводить, аналогично радуге      

DEFINE_GRADIENT_PALETTE( space_gp ) {
  0, 255,  255,  255,
  85, 255,  0,  229,
  170, 0,  0,  255,
  255, 0, 238, 255
  };
DEFINE_GRADIENT_PALETTE( summer_gp ) {
  0, 0,  255,  9,
  127, 0,  255,  225,
  255, 187, 255, 0
  };
DEFINE_GRADIENT_PALETTE( autumn_gp ) {
  0, 255,  255,  0,
  63, 255,  136,  0,
  127, 255,  47,  0,
  190, 255, 0, 94,
  255, 255,  255,  0
  }; 
DEFINE_GRADIENT_PALETTE( Rainbow_gp1 ) {
         0,  255,  0,  0, // Red
        32,  171, 85,  0, // Orange
        64,  171,171,  0, // Yellow
        96,    0,255,  0, // Green
       128,    0,171, 85, // Aqua
       160,    0,  0,255, // Blue
       192,   85,  0,171, // Purple
       224,  171,  0, 85, // Pink
       255,  255,  0,  0
	};// and back to Red

const uint8_t sin256[256] PROGMEM ={127,131,134,137,140,143,146,149,152,155,158,162,165,167,170,
173,176,179,182,185,188,190,193,196,198,201,203,206,208,211,213,215,218,220,222,224,
226,228,230,232,233,235,237,238,240,241,243,244,245,246,247,248,249,250,251,252,252,
253,253,254,254,254,254,254,254,254,254,254,254,253,253,252,252,251,250,250,249,248,
247,246,244,243,242,240,239,237,236,234,232,231,229,227,225,223,221,219,216,214,212,
210,207,205,202,200,197,194,192,189,186,183,181,178,175,172,169,166,163,160,157,154,
151,148,145,142,138,135,132,129,126,123,120,117,113,110,107,104,101,98,95,92,89,86,
83,80,77,74,72,69,66,63,61,58,55,53,50,48,45,43,41,39,36,34,32,30,28,26,24,23,21,19,
18,16,15,13,12,11,9,8,7,6,5,5,4,3,3,2,2,1,1,1,1,0,0,0,1,1,1,2,2,3,3,4,5,6,7,8,9,10,
11,12,14,15,17,18,20,22,23,25,27,29,31,33,35,37,40,42,44,47,49,52,54,57,59,62,65,67,
70,73,76,79,82,85,88,90,93,97,100,103,106,109,112,115,118,121,124,127};


uint8_t sin1024[3][1024] = {
  { 135,136,137,138,139,140,141,142,143,144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,176,177,178,179,180,181,182,183,184,185,186,186,187,188,189,190,191,192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,206,207,208,209,210,211,212,213,214,215,216,217,218,219,220,221,222,223,224,225,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,240,241,242,243,244,245,245,246,247,248,249,250,251,252,253,254,255,254,253,253,252,251,250,250,249,248,247,247,246,245,244,243,243,242,241,240,240,239,238,237,236,236,235,234,233,233,232,231,230,230,229,228,227,226,226,225,224,223,223,222,221,220,220,219,218,217,216,216,215,214,213,213,212,211,210,210,209,208,207,206,206,205,204,203,203,202,201,200,199,199,198,197,196,196,195,194,193,193,192,191,189,188,186,184,182,181,179,177,175,174,172,170,168,167,165,163,162,160,158,156,155,153,151,149,148,146,144,143,141,139,137,136,134,132,130,129,127,125,124,122,120,118,117,115,113,111,110,108,106,104,103,101,99,98,96,94,92,91,89,87,85,84,82,80,78,77,75,73,72,70,68,66,65,63,61,59,58,56,56,55,55,54,54,54,53,53,52,52,52,51,51,50,50,49,49,49,48,48,47,47,47,46,46,45,45,45,44,44,43,43,42,42,42,41,41,40,40,40,39,39,38,38,38,37,37,36,36,36,35,35,34,34,34,33,33,32,32,31,31,31,30,30,29,29,29,28,28,27,27,27,26,26,25,25,24,24,24,23,23,22,22,22,21,21,20,20,21,21,22,23,23,24,24,25,26,26,27,28,28,29,30,30,31,32,32,33,34,34,35,35,36,37,37,38,39,39,40,41,41,42,42,43,44,44,45,46,46,47,48,48,49,50,50,51,52,52,53,53,54,55,55,56,57,57,58,59,59,60,60,61,62,62,63,64,64,65,66,68,69,70,71,73,74,75,76,78,79,80,81,83,84,85,86,88,89,90,92,93,94,95,97,98,99,100,102,103,104,105,107,108,109,110,112,113,114,115,117,118,119,121,122,124,125,127,128,130,131,133,134,136,137,139,140,141,143,144,146,147,149,150,152,153,155,156,158,159,161,162,163,165,166,168,169,171,172,174,175,177,178,180,181,183,184,185,185,186,187,187,188,189,190,190,191,192,192,193,194,194,195,196,196,197,198,199,199,200,201,201,202,203,203,204,205,205,206,207,208,208,209,210,210,211,212,212,213,214,215,215,216,217,217,218,219,219,220,221,221,222,223,224,224,225,226,226,227,228,228,229,230,230,231,232,233,233,234,235,235,236,235,235,234,233,233,232,231,231,230,230,229,228,228,227,226,226,225,224,224,223,222,222,221,221,220,219,219,218,217,217,216,215,215,214,213,213,212,212,211,210,210,209,208,208,207,206,206,205,204,204,203,202,202,201,201,200,199,199,198,197,197,196,195,195,194,193,193,192,192,191,190,190,189,188,188,187,186,186,185,184,184,183,183,182,181,181,180,179,179,178,177,175,174,172,171,169,168,166,165,163,162,161,159,158,156,155,153,152,150,149,148,146,145,143,142,140,139,137,136,134,133,132,130,129,127,126,124,123,121,120,118,117,116,114,113,111,110,108,107,105,104,103,101,100,98,97,95,94,92,91,89,88,87,85,84,82,81,79,78,76,75,74,72,71,69,68,66,65,63,62,60,59,58,58,57,57,56,55,55,54,53,53,52,52,51,50,50,49,49,48,47,47,46,45,45,44,44,43,42,42,41,41,40,39,39,38,37,37,36,36,35,34,34,33,33,32,31,31,30,29,29,28,28,27,26,26,25,25,24,23,23,22,21,21,20,20,19,18,18,17,17,16,15,15,14,13,13,12,12,11,12,12,13,14,15,15,16,17,17,18,19,20,20,21,22,22,23,24,24,25,26,27,27,28,29,29,30,31,32,32,33,34,34,35,36,37,37,38,39,39,40,41,42,42,43,44,44,45,46,46,47,48,49,49,50,51,51,52,53,54,54,55,56,56,57,58,59,59,60,61,61,62,63,63,64,65,66,66,67,68,68,69,70,71,71,72,73,74,74,75,76,77,77,78,79,80,80,81,82,83,84,84,85,86,87,87,88,89,90,90,91,92,93,94,94,95,96,97,97,98,99,100,100,101,102,103,104,104,105,106,107,107,108,109,110,110,111,112,113,113,114,115,116,117,117,118,119,120,120,121,122,123,123,124,125,126,127,127,128,129,130,130,131,132,133,133,134,135 },
  { 26,25,25,24,24,23,23,22,22,21,21,20,19,19,18,18,17,17,16,16,15,15,14,14,13,12,12,11,11,10,10,9,9,8,8,7,6,6,5,5,4,4,3,3,2,2,1,2,2,3,3,4,4,5,6,6,7,7,8,9,9,10,10,11,12,12,13,13,14,14,15,16,16,17,17,18,18,19,20,20,21,21,22,23,23,24,24,25,26,26,27,27,28,28,29,30,30,31,31,32,32,33,34,34,35,35,36,37,37,38,38,39,40,40,41,41,42,42,43,44,44,45,45,46,46,47,48,48,49,49,50,52,53,55,57,59,60,62,64,66,67,69,71,72,74,76,78,79,81,83,85,86,88,90,92,93,95,97,98,100,102,104,105,107,109,111,112,114,116,117,119,121,123,124,126,128,130,131,133,135,136,138,140,142,143,145,147,149,150,152,154,155,157,159,161,162,164,166,168,169,171,173,175,176,178,180,181,183,185,187,188,190,192,194,195,197,198,198,199,200,200,201,202,202,203,204,204,205,206,206,207,208,208,209,210,210,211,212,212,213,213,214,215,215,216,217,217,218,219,219,220,221,221,222,223,223,224,225,225,226,227,227,228,229,229,230,231,231,232,233,233,234,235,235,236,237,237,238,239,239,240,240,241,242,242,243,244,244,245,246,246,247,248,248,249,250,250,251,252,252,253,254,254,255,254,254,253,252,252,251,250,250,249,248,248,247,246,246,245,244,244,243,242,242,241,240,240,239,238,238,237,236,236,235,234,234,233,232,232,231,230,230,229,228,228,227,227,226,225,225,224,223,223,222,221,221,220,219,219,218,217,217,216,215,215,214,213,213,212,211,211,210,209,209,208,207,207,206,205,205,204,203,203,202,200,198,196,194,192,191,189,187,185,183,181,179,177,175,173,172,170,168,166,164,162,160,158,156,154,153,151,149,147,145,143,141,139,137,135,134,132,130,128,126,124,122,120,118,116,115,113,111,109,107,105,103,101,99,97,96,94,92,90,88,86,84,82,80,78,77,75,73,71,69,67,65,63,61,59,58,56,54,52,50,48,48,47,47,47,46,46,46,45,45,45,44,44,44,43,43,43,42,42,42,41,41,41,40,40,40,39,39,39,38,38,38,37,37,37,36,36,36,35,35,35,34,34,34,33,33,32,32,31,31,31,30,30,29,29,28,28,28,27,27,26,26,25,25,24,24,24,23,23,22,22,21,21,21,20,20,19,19,18,18,18,17,17,16,16,15,15,16,16,17,17,18,18,19,19,20,20,21,21,22,22,23,23,24,24,25,26,26,27,27,28,28,29,29,30,30,31,31,32,32,33,33,34,35,35,36,36,37,37,38,38,39,39,40,40,41,41,42,42,43,44,44,45,45,46,46,47,47,48,48,49,49,50,50,51,51,52,52,53,54,56,57,59,60,62,63,65,66,68,69,71,72,74,75,77,78,79,81,82,84,85,87,88,90,91,93,94,96,97,99,100,102,103,104,106,107,109,110,112,113,115,116,118,119,121,122,124,125,127,128,129,131,132,134,135,137,138,140,141,143,144,146,147,149,150,152,153,154,156,157,159,160,162,163,165,166,168,169,171,172,174,175,177,178,179,180,180,181,182,183,184,184,185,186,187,188,188,189,190,191,192,192,193,194,195,196,196,197,198,199,200,200,201,202,203,204,204,205,206,207,208,208,209,210,211,211,212,213,214,215,215,216,217,218,219,219,220,221,222,223,223,224,225,226,227,227,228,229,230,231,231,232,233,234,235,235,236,237,238,239,239,240,241,240,240,239,238,238,237,237,236,235,235,234,233,233,232,232,231,230,230,229,228,228,227,227,226,225,225,224,223,223,222,221,221,220,220,219,218,218,217,216,216,215,215,214,213,213,212,211,211,210,210,209,208,208,207,206,206,205,205,204,203,203,202,201,201,200,199,199,198,198,197,196,196,195,194,194,193,193,192,191,191,190,189,189,188,188,187,186,186,185,184,182,181,179,178,177,175,174,173,171,170,168,167,166,164,163,161,160,159,157,156,155,153,152,150,149,148,146,145,143,142,141,139,138,137,135,134,132,131,130,128,127,126,124,123,121,120,119,117,116,114,113,112,110,109,108,106,105,103,102,101,99,98,96,95,94,92,91,90,88,87,85,84,83,81,80,78,77,76,74,73,72,70,69,67,66,65,65,64,63,63,62,61,61,60,60,59,58,58,57,56,56,55,54,54,53,52,52,51,51,50,49,49,48,47,47,46,45,45,44,43,43,42,41,41,40,40,39,38,38,37,36,36,35,34,34,33,32,32,31,31,30,29,29,28,27,27,26 },
  { 219,218,217,217,216,215,214,213,212,212,211,210,209,208,208,207,206,205,204,203,203,202,201,200,199,199,198,197,196,195,195,194,193,192,191,190,190,189,188,187,186,186,185,184,183,182,181,181,180,179,178,176,175,173,172,171,169,168,166,165,164,162,161,160,158,157,155,154,153,151,150,148,147,146,144,143,141,140,139,137,136,134,133,132,130,129,127,126,125,123,122,120,119,118,116,115,114,112,111,109,108,107,105,104,102,101,100,98,97,95,94,93,91,90,88,87,86,84,83,82,80,79,77,76,75,73,72,70,69,68,66,65,63,62,61,61,60,59,59,58,57,56,56,55,54,54,53,52,52,51,50,50,49,48,48,47,46,46,45,44,43,43,42,41,41,40,39,39,38,37,37,36,35,34,34,33,32,32,31,30,30,29,28,28,27,26,26,25,24,24,23,22,21,21,20,19,19,18,17,17,16,15,15,14,13,12,12,11,10,10,9,8,8,7,8,9,9,10,11,12,13,13,14,15,16,16,17,18,19,20,20,21,22,23,24,24,25,26,27,27,28,29,30,31,31,32,33,34,35,35,36,37,38,38,39,40,41,42,42,43,44,45,46,46,47,48,49,49,50,51,52,53,53,54,55,56,57,57,58,59,60,60,61,62,63,64,64,65,66,68,69,71,73,74,76,77,79,81,82,84,86,87,89,91,92,94,96,97,99,100,102,104,105,107,109,110,112,114,115,117,119,120,122,123,125,127,128,130,132,133,135,137,138,140,142,143,145,146,148,150,151,153,155,156,158,160,161,163,165,166,168,169,171,173,174,176,178,179,181,183,184,186,188,189,191,192,194,196,197,199,199,200,200,201,201,202,202,202,203,203,204,204,205,205,205,206,206,207,207,208,208,208,209,209,210,210,211,211,212,212,212,213,213,214,214,215,215,215,216,216,217,217,218,218,218,219,219,220,220,221,221,221,222,222,223,223,224,224,224,225,225,226,226,227,227,228,228,228,229,229,230,230,231,231,231,232,232,233,233,234,234,234,235,235,236,236,237,237,237,237,236,236,236,236,236,235,235,235,235,235,234,234,234,234,234,233,233,233,233,233,232,232,232,232,232,231,231,231,231,231,230,230,230,230,230,229,229,229,229,229,228,228,228,228,228,227,227,227,226,225,224,224,223,222,221,220,219,219,218,217,216,215,214,214,213,212,211,210,209,208,208,207,206,205,204,203,203,202,201,200,199,198,198,197,196,195,194,193,193,192,191,190,189,187,186,184,183,181,180,178,177,175,174,172,171,169,168,166,165,163,162,160,159,157,156,154,153,151,150,148,147,146,144,143,141,140,138,137,135,134,132,131,129,128,126,125,123,122,120,119,117,116,114,113,111,110,108,107,105,104,103,101,100,98,97,95,94,92,91,89,88,86,85,83,82,80,79,77,76,74,73,71,70,68,67,65,64,62,61,60,60,59,58,58,57,56,56,55,54,53,53,52,51,51,50,49,49,48,47,47,46,45,45,44,43,43,42,41,41,40,39,38,38,37,36,36,35,34,34,33,32,32,31,30,30,29,28,28,27,26,26,25,24,23,23,22,21,21,20,19,19,18,17,17,16,15,15,14,13,13,12,11,11,10,9,8,8,7,6,6,5,4,4,3,4,5,5,6,7,8,8,9,10,11,12,12,13,14,15,15,16,17,18,19,19,20,21,22,22,23,24,25,26,26,27,28,29,29,30,31,32,33,33,34,35,36,36,37,38,39,39,40,41,42,43,43,44,45,46,46,47,48,49,50,50,51,52,53,53,54,55,56,57,57,58,59,60,60,61,62,63,64,64,65,66,67,67,68,69,71,73,74,76,78,80,81,83,85,87,89,90,92,94,96,97,99,101,103,105,106,108,110,112,113,115,117,119,121,122,124,126,128,129,131,133,135,137,138,140,142,144,145,147,149,151,153,154,156,158,160,161,163,165,167,169,170,172,174,176,177,179,181,183,185,186,188,190,192,193,195,197,198,198,199,200,200,201,202,202,203,204,204,205,206,206,207,208,208,209,210,211,211,212,213,213,214,215,215,216,217,217,218,219,219,220,221,221,222,223,223,224,225,225,226,227,227,228,229,229,230,231,231,232,233,234,234,235,236,236,237,238,238,239,240,240,241,242,242,243,244,244,245,245,244,244,244,243,243,242,242,242,241,241,241,240,240,240,239,239,238,238,238,237,237,237,236,236,236,235,235,235,234,234,233,233,233,232,232,232,231,231,231,230,230,229,229,229,228,228,228,227,227,227,226,226,226,225,225,224,224,224,223,223,223,222,222,222,221,221,220,220,220,219,219 }  
};

int recievedTemp = 0; 

uint8_t heartHue = 0; //переменные сердцебиения
uint8_t deltaHue = 5;
uint8_t heartBrightness = 0; //переменные сердцебиения
uint8_t deltaBrightness = 12;
uint8_t heartX = 0;


#define arraySize(_array) ( sizeof(_array) / sizeof(*_array) )

CRGBPalette16 myPal = space_gp;

void copyZonesToPixels() {
	for (uint8_t zoneId = 0x00; zoneId < zonesQuantity; zoneId++) 
		for(uint8_t pixelN = 0; pixelN < zonepixels[zoneId].qty; pixelN++)
			leds_temp[zonepixels[zoneId].pixels[pixelN]] = zonesCol[zoneId];    
}

void shuffleArray(uint8_t * array, int size){
	int last = 0;
	int temp = array[last];
	for (int i=0; i<size; i++){
		int index = random(size);
		array[last] = array[index];
		last = index;
	}
	array[last] = temp;
}

void setup() {  
  Serial.begin(9600);
	pinMode(BIT0,INPUT);
	pinMode(BIT1,INPUT);
	pinMode(BIT2,INPUT);
	pinMode(BIT3,INPUT);
	pinMode(BIT4,INPUT);
	digitalWrite(BIT0, HIGH);
	digitalWrite(BIT1, HIGH);
	digitalWrite(BIT2, HIGH);
	digitalWrite(BIT3, HIGH);
	digitalWrite(BIT4, HIGH);
 
	pinMode(ACK,OUTPUT); 

	FastLED.addLeds<WS2812, DATA_PIN, GRB>(leds, NUM_LEDS);
	FastLED.addLeds<WS2812, DATA_PIN2, GRB>(leds, NUM_LEDS);
	FastLED.addLeds<WS2812, DATA_PIN3, GRB>(leds, NUM_LEDS);
	FastLED.addLeds<WS2812, DATA_PIN4, GRB>(leds, NUM_LEDS);
	FastLED.addLeds<WS2812, DATA_PIN5, GRB>(leds, NUM_LEDS);  
	FastLED.addLeds<WS2812, DATA_COMET_PIN, GRB>(leds_comet, NUM_COMET_LEDS);

  if(NUM_LEDS % 3 == 0)
    group_by_3 = NUM_LEDS / 3;
  else
    group_by_3 = NUM_LEDS / 3 + 1;
  if(NUM_LEDS < 3)
    group_by_3 = 1;
  Serial.print("Group count=");
  Serial.println(group_by_3);
  interval_by_3 = 1024 / group_by_3;
  Serial.print("Interval=");
  Serial.println(interval_by_3);
  uint8_t group_counter = 0;
	for(uint8_t i = 0; i < NUM_LEDS; i++) {
		sawValues[i] = interval_by_3 * group_counter;
    if( i % 3 ==2) 
      group_counter++;
		leds_brightness[i] = 150;
		leds_chaos[i]=i;  
    Serial.print(i);
    Serial.print("-item = ");
    Serial.println(sawValues[i]); 
	}    
	
	shuffleArray(leds_chaos, NUM_LEDS);
	
	btn_mode.attachClick(nextMode);
	btn_mode.attachLongPressStart(onOffMode);
	btn_mode.attachDoubleClick(nextSet);
	btn_saw.attachClick(nextSaw);

	randomSeed(randomSeedCorrection * analogRead(randomSeedPin));

	for(uint8_t i = 0; i < cometCount; i++){
		cometsSettings[i].cometStepTimer = random(cometStepTimerMin, cometStepTimerMax);
		cometsSettings[i].cometTail = random(cometTailMin, cometTailMax);
		cometsSettings[i].cometBrightness = random(cometBrightessMin, cometBrightessMax);
		cometsSettings[i].cometReverse = random(0,255)%2;
		cometsSettings[i].cometLaunchTimer = random(cometLaunchTimerMin, cometLaunchTimerMax);
		cometCounter[i] = 0;
		cometOver[i] = 1;
		cometState[i] = 0;
	} 
	level2_1_max = level2_1_max_percent * defaultBrightness / 100;
	level2_1_min = level2_1_min_percent * defaultBrightness / 100;
	level2_2_max = level2_2_max_percent * defaultBrightness / 100;
	level2_2_min = level2_2_min_percent * defaultBrightness / 100;
  
	if(EEPROM.read(0) <= colorCount+modeCount) 
		level1 = EEPROM.read(0);
	if(EEPROM.read(1) <= 2)
		level2 = EEPROM.read(1);
	if(EEPROM.read(2) <= colorCount+modeCount)   
		level1_prev = EEPROM.read(2);    
	rainbowSlice = defaultSlice;       
	effectSlice = defaultSlice;
}

void onOffMode(){
	if(level1){
		level1_prev = level1;  
		level1 = 0;
		switchOff = 1;
		if(level1_prev)
			EEPROM.update(2, level1_prev);  
	}else{    
		level1 = level1_prev;
		switchOff = 0;
		if((level1 ==0)&&(level1_prev == 0))
			level1 =1;    	
	}  
	EEPROM.update(0, level1);   
}


void nextSet() {
	if(level1) {
		if(level1 == 1) 
			level1 = 2; 
		else
			if((level1 >= 2)&&(level1 <= colorCount)) {
				level1 = colorCount + 1;
				rainbowSlice = defaultSlice;       
				effectSlice = defaultSlice;
			}else
				if((level1 >= colorCount + 1 )&&(level1 <= colorCount + modeCount))
					level1 = 1;    
		EEPROM.update(0, level1);   
	} 
}

void nextMode(){
	if(level1) {
		if((level1 > 1) &&(level1 <= colorCount)) { //если выбраны полные цвета
			level1++;     
			if(level1 == colorCount + 1) 
				level1 = 2;				
		}
		if((level1 > colorCount) && (level1 <= colorCount + modeCount)) {//если выбраны эффекты
			level1++;    
			if(level1 == colorCount + modeCount + 1) 
				level1 = colorCount + 1;		
			rainbowSlice = defaultSlice;       
			effectSlice = defaultSlice;
		}
		EEPROM.update(0, level1);
	}
}


void nextSaw(){
	level2 = (level2 + 1) % 3;
	EEPROM.update(1, level2);
}

void loop() {  
	EVERY_N_MILLISECONDS(100) {
		recievedTemp = 0;
		if(digitalRead(BIT0))
			recievedTemp += 1;
		if(digitalRead(BIT1))
			recievedTemp += 2;  
		if(digitalRead(BIT2))
			recievedTemp += 4; 
		if(digitalRead(BIT3))
			recievedTemp += 8;
		if(digitalRead(BIT4))
			recievedTemp += 16;
		if(recievedTemp == 0)
			digitalWrite(ACK, LOW);
		else{
			digitalWrite(ACK, HIGH);         
		}
		recievedTemp += 6912;
	}  

	if((recievedTemp == 6913)&&( level1 == 0 )){ //ON
		level1 = level1_prev;
		switchOff = 0;
	} 
	if((recievedTemp >= 6914)&&(recievedTemp <= 6933)) {
		if(level1) {
			switch(recievedTemp){
				case 6914: //AUTO
					nextSaw();
					break; 
				case 6915: //OFF
					level1_prev = level1;  
					level1 = 0;
					switchOff = 1;    
					break;
				case 6917: //M+
					if(level1 <= colorCount)
						level1 = colorCount + 1;
					else 
						if(level1 > colorCount){
							level1++;
							if(level1 > colorCount + modeCount)
								level1 = colorCount + 1;							
						}					
					rainbowSlice = defaultSlice;       
					effectSlice = defaultSlice;  
					EEPROM.update(0, level1);   
					break;      
				case 6920: // M-
					if(level1 <= colorCount) 
						level1 = colorCount + 1;
					else{                    
						level1--;
						if(level1 == colorCount) 
							level1 = colorCount + modeCount;          
					}      
					rainbowSlice = defaultSlice;       
					effectSlice = defaultSlice;          
					EEPROM.update(0, level1);       
					break;
				case 6922:
				case 6923:
				case 6924:
				case 6925:
				case 6926:
				case 6927:
				case 6928:
				case 6929:
				case 6930:
				case 6931:
				case 6932:
					level1 = recievedTemp - 6920;
					EEPROM.update(0, level1);
					break;
				case 6933:
					level1 = 1;
					EEPROM.update(0, level1);
					break;
				case 6916: //slice+  
					if(rainbowSlice < 32) 
						rainbowSlice *= 2;					
					if(effectSlice < 32) 
						effectSlice *= 2;      
					break;
				case 6919:
					if(rainbowSlice > 2) 
						rainbowSlice /= 2;					
					if(effectSlice > 2) 
						effectSlice /= 2;
					break; 
				case 6918:   //br+
					if(defaultBrightness < 230)
						defaultBrightness += 20;
					else
						defaultBrightness = 255;               
					break;
				case 6921:   //br-
					if(defaultBrightness > 50)
						defaultBrightness -= 20;            
					break;
        default:
          recievedTemp=0;
          break;
			}			
		}
	}  
 recievedTemp = 0;

  // level 1
	if(level1 < colorCount + 1) 
		for(uint8_t pixelNo = 0; pixelNo < NUM_LEDS; pixelNo++) 
			leds_temp[pixelNo] = CRGB(monocolorTable[level1]);		  
	else {  
		if (level1 == 13) {
			myPal = Rainbow_gp1;
			fill_palette(zonesCol, zonesQuantity, paletteIndex, 255 / zonesQuantity / effectSlice, myPal, 255, LINEARBLEND);
			EVERY_N_MILLISECONDS(level1_ms){
				paletteIndex++;
			}
		}
		if(level1 == 14) {
			myPal = space_gp;
			fill_palette(zonesCol, zonesQuantity, paletteIndex, 255 / zonesQuantity / effectSlice, myPal, 255, LINEARBLEND);
			EVERY_N_MILLISECONDS(level1_ms){
				paletteIndex++;
			}
		}
		if(level1 == 15) {
			myPal = summer_gp;
			fill_palette(zonesCol, zonesQuantity, paletteIndex, 255 / zonesQuantity / effectSlice, myPal, 255, LINEARBLEND);
			EVERY_N_MILLISECONDS(level1_ms){
				paletteIndex++;
			}
		}   
		if(level1 == 16) {
			myPal = autumn_gp;
			fill_palette(zonesCol, zonesQuantity, paletteIndex, 255 / zonesQuantity / effectSlice, myPal, 255, LINEARBLEND);
			EVERY_N_MILLISECONDS(level1_ms){
				paletteIndex++;
			}
		} 
		if( level1 == 17)  { //режим сердцебиения
			for(uint8_t pixelNo = 0; pixelNo < NUM_LEDS; pixelNo++) {
				leds_temp[pixelNo] = CHSV(heartHue,255, heartBrightness);
			}  
			EVERY_N_MILLISECONDS(level1_ms * 25){               
				heartHue += deltaHue;
			}
			EVERY_N_MILLISECONDS(level1_ms){   
				if(heartX < 5) {            
					heartBrightness += deltaBrightness;
				} else {
					if((heartX < 10)||((heartX <20) && (heartX >= 15))) {
						heartBrightness += deltaBrightness * 2;          
					} else {
						if((heartX < 15) || ((heartX < 25) && (heartX >= 20))) {
							heartBrightness -= deltaBrightness * 2;    
						} else {
							heartBrightness -= deltaBrightness ;    
						}
					}
				}
				if(heartX < 30)
					heartX++;
				else {
					heartX = 0;   
					heartBrightness = 0;
				}		
			}    
		} else {
			if(level1 == 18) { //chaos rainbow
        if(!level1_chaos)
          shuffleArray(leds_chaos, NUM_LEDS);
        level1_chaos = 1;        
				myPal = Rainbow_gp1;
				fill_palette(leds_temp, NUM_LEDS, paletteIndex, 255 / NUM_LEDS , myPal, 255, LINEARBLEND);
				EVERY_N_MILLISECONDS(level1_ms){
					paletteIndex++;
				}
			} else {
        level1_chaos = 0;			
				copyZonesToPixels(); 
			}
		}
	}
  
	if(level2) {  //УРОВЕНЬ2
		if(level2 == 1) {
			EVERY_N_MILLISECONDS(level2_ms1_1) {    
				for(uint8_t i = 0; i < NUM_LEDS; i++){                          
					leds_brightness[i] = (level2_1_max-level2_1_min)*sin1024[i % 3][sawValues[i]]/256+level2_1_min;                         
					sawValues[i]++; 	
          if(sawValues[i] == 1024)
            sawValues[i]=0;
				}		       
			}                     
		}
		if(level2 == 2) {
			EVERY_N_MILLISECONDS(level2_ms2_1) {    
				for(uint8_t i = 0; i < NUM_LEDS; i++){                         
					leds_brightness[i] = (level2_2_max-level2_2_min)*sin1024[i % 3][sawValues[i]]/256+level2_2_min;                         
					sawValues[i]++;  
          if(sawValues[i] == 1024)
            sawValues[i]=0;
				}  
			}         
		}    
	} else 
		for(uint8_t i = 0; i < NUM_LEDS; i++)                   
			leds_brightness[i] = defaultBrightness;                  
		
	for(uint8_t i = 0; i < NUM_LEDS; i++) {  
    uint8_t j = i;
		if(level1 == 18) {
			i = leds_chaos[i];
		}
		leds_temp[i].nscale8(leds_brightness[i]);//subtractFromRGB(leds_brightness[i]);      
		leds_temp[i].r=scale8(leds_temp[i].r, defaultBrightnessR);
		leds_temp[i].g=scale8(leds_temp[i].g, defaultBrightnessG);
		leds_temp[i].b=scale8(leds_temp[i].b, defaultBrightnessB);               
		leds[j] = CRGB(leds_temp[i]);       
    i = j;
	}   

	#if defined (SETTABLE)
		if(level1) 
			for(uint8_t i=0; i< corrPx; i++)
				leds[setTable[i][0]].subtractFromRGB(setTable[i][1]);
	#endif 

  //зажигаем созвездия

	if(switchOff == 0) {
		for(uint8_t i = 0; i < sozvezdiaQuantity; i++) {
			for(uint8_t j = 0; j < sozvezdiya[i].qty; j++) {        
				leds_comet[sozvezdiya[i].pixels[j]] = CRGB(SZVColor);   
				leds_comet[sozvezdiya[i].pixels[j]].r=scale8(leds_comet[sozvezdiya[i].pixels[j]].r, defaulSZVBrightnessR);
				leds_comet[sozvezdiya[i].pixels[j]].g=scale8(leds_comet[sozvezdiya[i].pixels[j]].g, defaulSZVBrightnessG);
				leds_comet[sozvezdiya[i].pixels[j]].b=scale8(leds_comet[sozvezdiya[i].pixels[j]].b, defaulSZVBrightnessB);
			}   
		}
	}else{
		for(uint8_t i = 0; i < sozvezdiaQuantity; i++) {
			for(uint8_t j = 0; j < sozvezdiya[i].qty; j++) {
				leds_comet[sozvezdiya[i].pixels[j]] = CRGB::Black;   
			}   
		}    
	}

	//запускаем кометы
	if(switchOff == 0) {
		if(launch1_enable == 0){
			EVERY_N_SECONDS(shift1) {
				launch1_enable = 1; 
			}
		}
		if(launch2_enable == 0){
			EVERY_N_SECONDS(shift2) {
				launch2_enable = 1; 
			}
		}
		if(launch3_enable == 0){
			EVERY_N_SECONDS(shift3) {
				launch3_enable = 1; 
			}
		}
    if(launch4_enable == 0){
      EVERY_N_SECONDS(shift4) {
        launch4_enable = 1; 
      }
    }   
    
		if(cometCount > 0) {
			if(cometOver[0] == 1) {
				EVERY_N_SECONDS(cometsSettings[0].cometLaunchTimer) {
					if(cometState[0] == 0) {
						cometState[0] = 1;          
						cometOver[0] = 0;
					} else {  
						cometState[0] = 0;
						cometCounter[0] = 0;
            
						randomSeed(randomSeedCorrection * analogRead(randomSeedPin));
						cometsSettings[0].cometStepTimer = random(cometStepTimerMin, cometStepTimerMax);
						cometsSettings[0].cometTail = random(cometTailMin, cometTailMax);
						cometsSettings[0].cometBrightness = random(cometBrightessMin, cometBrightessMax);
						cometsSettings[0].cometReverse = random(0,255)%2;     
						cometsSettings[0].cometColor = colorTable[random(arraySize(colorTable))];      
						cometsSettings[0].cometLaunchTimer = random(cometLaunchTimerMin, cometLaunchTimerMax);
					}      
					for(uint8_t i = 0; i < comets[0].qty; i++) {
						leds_comet[comets[0].pixels[i]] = CRGB::Black;   
					}        
				}
			}  
        
			if(cometState[0]){
				EVERY_N_MILLISECONDS(cometsSettings[0].cometStepTimer) { 
					for(uint8_t i = 0; i < comets[0].qty; i++) {
						leds_comet[comets[0].pixels[i]] = CRGB::Black;   
					}           
					if(cometCounter[0] < comets[0].qty + cometsSettings[0].cometTail) {
						if(cometsSettings[0].cometReverse) {
							for(int8_t i = comets[0].qty - cometCounter[0] - 1;  ((i < comets[0].qty) && (i <= comets[0].qty - cometCounter[0] - 1 + cometsSettings[0].cometTail)); i++) {        
								if(i >= 0) {
									leds_comet[comets[0].pixels[i]] = cometsSettings[0].cometColor;
									leds_comet[comets[0].pixels[i]].nscale8(cometsSettings[0].cometBrightness/(i - (comets[0].qty - cometCounter[0] - 1) + 1));
								}
							}      
						}else{
							for(int8_t i = cometCounter[0];  ((i >= 0)&&(cometCounter[0] - i <= cometsSettings[0].cometTail)); i--) {   
								if(i<comets[0].qty) {     
									leds_comet[comets[0].pixels[i]] = cometsSettings[0].cometColor;                  
									leds_comet[comets[0].pixels[i]].nscale8(cometsSettings[0].cometBrightness/(cometCounter[0] - i + 1));
								}
							}          
						}
						cometCounter[0]++;
					}else{
						for(uint8_t i = 0; i < comets[0].qty; i++) {
							leds_comet[comets[0].pixels[i]] = CRGB::Black;   
						}      
						cometOver[0] = 1;       
					}            
				}      
			}
		}
  
		if((cometCount > 1)&&(launch1_enable)) {
			if(cometOver[1] == 1) {
				EVERY_N_SECONDS(cometsSettings[1].cometLaunchTimer) {
					if(cometState[1] == 0) {
						cometState[1] = 1;          
						cometOver[1] = 0;
					} else {  
						randomSeed(randomSeedCorrection * analogRead(randomSeedPin));
						cometState[1] = 0;
						cometCounter[1] = 0;
						cometsSettings[1].cometStepTimer = random(cometStepTimerMin, cometStepTimerMax);
						cometsSettings[1].cometTail = random(cometTailMin, cometTailMax);
						cometsSettings[1].cometBrightness = random(cometBrightessMin, cometBrightessMax);
						cometsSettings[1].cometReverse = random(0,255)%2;           
						cometsSettings[1].cometColor = colorTable[random(arraySize(colorTable))];   
						cometsSettings[1].cometLaunchTimer = random(cometLaunchTimerMin, cometLaunchTimerMax);
					}      
					for(uint8_t i = 0; i < comets[1].qty; i++) {
						leds_comet[comets[1].pixels[i]] = CRGB::Black;   
					}        
				}
			}  
			if(cometState[1]){
				EVERY_N_MILLISECONDS(cometsSettings[1].cometStepTimer) { 
					for(uint8_t i = 0; i < comets[1].qty; i++) {
						leds_comet[comets[1].pixels[i]] = CRGB::Black;   
					}           
					if(cometCounter[1] < comets[1].qty + cometsSettings[1].cometTail) {
						if(cometsSettings[1].cometReverse) {
							for(int8_t i = comets[1].qty - cometCounter[1] - 1;  ((i < comets[1].qty) && (i <= comets[1].qty - cometCounter[1] - 1 + cometsSettings[1].cometTail)); i++) {        
								if(i >= 0) {
									leds_comet[comets[1].pixels[i]] = cometsSettings[1].cometColor;
									leds_comet[comets[1].pixels[i]].nscale8(cometsSettings[1].cometBrightness/(i - (comets[1].qty - cometCounter[1] - 1) + 1));
								}
							}      
						}else{
							for(int8_t i = cometCounter[1];  ((i >= 0)&&(cometCounter[1] - i <= cometsSettings[1].cometTail)); i--) {   
								if(i<comets[1].qty) {     
									leds_comet[comets[1].pixels[i]] = cometsSettings[1].cometColor;                  
									leds_comet[comets[1].pixels[i]].nscale8(cometsSettings[1].cometBrightness/(cometCounter[1] - i + 1));
								}
							}          
						}
						cometCounter[1]++;
					}else{
						for(uint8_t i = 0; i < comets[1].qty; i++) {
							leds_comet[comets[1].pixels[i]] = CRGB::Black;   
						}      
						cometOver[1] = 1;       
					}            
				}      
			}
      
			if((cometCount > 2)&&(launch2_enable)) {
				if(cometOver[2] == 1) {
					EVERY_N_SECONDS(cometsSettings[2].cometLaunchTimer) {
						if(cometState[2] == 0) {
							cometState[2] = 1;          
							cometOver[2] = 0;
						} else {  
							randomSeed(randomSeedCorrection * analogRead(randomSeedPin));
							cometState[2] = 0;
							cometCounter[2] = 0;
							cometsSettings[2].cometStepTimer = random(cometStepTimerMin, cometStepTimerMax);
							cometsSettings[2].cometTail = random(cometTailMin, cometTailMax);
							cometsSettings[2].cometBrightness = random(cometBrightessMin, cometBrightessMax);
							cometsSettings[2].cometReverse = random(0,255)%2;           
							cometsSettings[2].cometColor = colorTable[random(arraySize(colorTable))];   
							cometsSettings[2].cometLaunchTimer = random(cometLaunchTimerMin, cometLaunchTimerMax);
						}      
						for(uint8_t i = 0; i < comets[2].qty; i++) {
							leds_comet[comets[2].pixels[i]] = CRGB::Black;   
						}        
					}
				}  
				if(cometState[2]){
					EVERY_N_MILLISECONDS(cometsSettings[2].cometStepTimer) { 
						for(uint8_t i = 0; i < comets[2].qty; i++) {
							leds_comet[comets[2].pixels[i]] = CRGB::Black;   
						}           
						if(cometCounter[2] < comets[2].qty + cometsSettings[2].cometTail) {
							if(cometsSettings[2].cometReverse) {
								for(int8_t i = comets[2].qty - cometCounter[2] - 1;  ((i < comets[2].qty) && (i <= comets[2].qty - cometCounter[2] - 1 + cometsSettings[2].cometTail)); i++) {        
									if(i >= 0) {
										leds_comet[comets[2].pixels[i]] = cometsSettings[2].cometColor;
										leds_comet[comets[2].pixels[i]].nscale8(cometsSettings[2].cometBrightness/(i - (comets[2].qty - cometCounter[2] - 1) + 1));
									}
								}      
							}else{
								for(int8_t i = cometCounter[2];  ((i >= 0)&&(cometCounter[2] - i <= cometsSettings[2].cometTail)); i--) {   
									if(i<comets[2].qty) {     
										leds_comet[comets[2].pixels[i]] = cometsSettings[2].cometColor;                  
										leds_comet[comets[2].pixels[i]].nscale8(cometsSettings[2].cometBrightness/(cometCounter[2] - i + 1));
									}
								}          
							}
							cometCounter[2]++;
						}else{
							for(uint8_t i = 0; i < comets[2].qty; i++) {
								leds_comet[comets[2].pixels[i]] = CRGB::Black;   
							}      
							cometOver[2] = 1;       
						}            
					}      
				}
			}
			if((cometCount > 3)&&(launch3_enable)) {
				if(cometOver[3] == 1) {
					EVERY_N_SECONDS(cometsSettings[3].cometLaunchTimer) {
						if(cometState[3] == 0) {
							cometState[3] = 1;          
							cometOver[3] = 0;
						} else {  
							randomSeed(randomSeedCorrection * analogRead(randomSeedPin));
							cometState[3] = 0;
							cometCounter[3] = 0;
							cometsSettings[3].cometStepTimer = random(cometStepTimerMin, cometStepTimerMax);
							cometsSettings[3].cometTail = random(cometTailMin, cometTailMax);
							cometsSettings[3].cometBrightness = random(cometBrightessMin, cometBrightessMax);
							cometsSettings[3].cometReverse = random(0,255)%2;           
							cometsSettings[3].cometColor = colorTable[random(arraySize(colorTable))];   
							cometsSettings[3].cometLaunchTimer = random(cometLaunchTimerMin, cometLaunchTimerMax);
						}      
						for(uint8_t i = 0; i < comets[3].qty; i++) {
							leds_comet[comets[3].pixels[i]] = CRGB::Black;   
						}        
					}
				}  
				if(cometState[3]){
					EVERY_N_MILLISECONDS(cometsSettings[3].cometStepTimer) { 
						for(uint8_t i = 0; i < comets[3].qty; i++) {
							leds_comet[comets[3].pixels[i]] = CRGB::Black;   
						}           
						if(cometCounter[3] < comets[3].qty + cometsSettings[3].cometTail) {
							if(cometsSettings[3].cometReverse) {
								for(int8_t i = comets[3].qty - cometCounter[3] - 1;  ((i < comets[3].qty) && (i <= comets[3].qty - cometCounter[3] - 1 + cometsSettings[3].cometTail)); i++) {        
									if(i >= 0) {
										leds_comet[comets[3].pixels[i]] = cometsSettings[3].cometColor;
										leds_comet[comets[3].pixels[i]].nscale8(cometsSettings[3].cometBrightness/(i - (comets[3].qty - cometCounter[3] - 1) + 1));
									}
								}      
							}else{
								for(int8_t i = cometCounter[3];  ((i >= 0)&&(cometCounter[3] - i <= cometsSettings[3].cometTail)); i--) {   
									if(i<comets[3].qty) {     
										leds_comet[comets[3].pixels[i]] = cometsSettings[3].cometColor;                  
										leds_comet[comets[3].pixels[i]].nscale8(cometsSettings[3].cometBrightness/(cometCounter[3] - i + 1));
									}
								}          
							}
							cometCounter[3]++;
						}else{
							for(uint8_t i = 0; i < comets[3].qty; i++) {
								leds_comet[comets[3].pixels[i]] = CRGB::Black;   
							}      
							cometOver[3] = 1;       
						}            
					}      
				}
			}      
      if((cometCount > 4)&&(launch4_enable)) {//5я
        if(cometOver[4] == 1) {
          EVERY_N_SECONDS(cometsSettings[4].cometLaunchTimer) {
            if(cometState[4] == 0) {
              cometState[4] = 1;          
              cometOver[4] = 0;
            } else {  
              randomSeed(randomSeedCorrection * analogRead(randomSeedPin));
              cometState[4] = 0;
              cometCounter[4] = 0;
              cometsSettings[4].cometStepTimer = random(cometStepTimerMin, cometStepTimerMax);
              cometsSettings[4].cometTail = random(cometTailMin, cometTailMax);
              cometsSettings[4].cometBrightness = random(cometBrightessMin, cometBrightessMax);
              cometsSettings[4].cometReverse = random(0,255)%2;           
              cometsSettings[4].cometColor = colorTable[random(arraySize(colorTable))];   
              cometsSettings[4].cometLaunchTimer = random(cometLaunchTimerMin, cometLaunchTimerMax);
            }      
            for(uint8_t i = 0; i < comets[4].qty; i++) {
              leds_comet[comets[4].pixels[i]] = CRGB::Black;   
            }        
          }
        }  
        if(cometState[4]){
          EVERY_N_MILLISECONDS(cometsSettings[4].cometStepTimer) { 
            for(uint8_t i = 0; i < comets[4].qty; i++) {
              leds_comet[comets[4].pixels[i]] = CRGB::Black;   
            }           
            if(cometCounter[4] < comets[4].qty + cometsSettings[4].cometTail) {
              if(cometsSettings[4].cometReverse) {
                for(int8_t i = comets[4].qty - cometCounter[4] - 1;  ((i < comets[4].qty) && (i <= comets[4].qty - cometCounter[4] - 1 + cometsSettings[4].cometTail)); i++) {        
                  if(i >= 0) {
                    leds_comet[comets[4].pixels[i]] = cometsSettings[4].cometColor;
                    leds_comet[comets[4].pixels[i]].nscale8(cometsSettings[4].cometBrightness/(i - (comets[4].qty - cometCounter[4] - 1) + 1));
                  }
                }      
              }else{
                for(int8_t i = cometCounter[4];  ((i >= 0)&&(cometCounter[4] - i <= cometsSettings[4].cometTail)); i--) {   
                  if(i<comets[4].qty) {     
                    leds_comet[comets[4].pixels[i]] = cometsSettings[4].cometColor;                  
                    leds_comet[comets[4].pixels[i]].nscale8(cometsSettings[4].cometBrightness/(cometCounter[4] - i + 1));
                  }
                }          
              }
              cometCounter[4]++;
            }else{
              for(uint8_t i = 0; i < comets[4].qty; i++) {
                leds_comet[comets[4].pixels[i]] = CRGB::Black;   
              }      
              cometOver[4] = 1;       
            }            
          }      
        }
      }         
		}
	}else{
		if(cometCount > 0) {
			for(uint8_t i = 0; i < comets[0].qty; i++) {
				leds_comet[comets[0].pixels[i]] = CRGB::Black;   
			}  
		}
		if(cometCount > 1){
			for(uint8_t i = 0; i < comets[1].qty; i++) {
				leds_comet[comets[1].pixels[i]] = CRGB::Black;   
			}            
		}
		if(cometCount > 2) {
			for(uint8_t i = 0; i < comets[2].qty; i++) {
				leds_comet[comets[2].pixels[i]] = CRGB::Black;   
			}      
		}
		if(cometCount > 3) {
			for(uint8_t i = 0; i < comets[3].qty; i++) {
				leds_comet[comets[3].pixels[i]] = CRGB::Black;   
			}
		}   
    if(cometCount > 4) {
      for(uint8_t i = 0; i < comets[4].qty; i++) {
        leds_comet[comets[4].pixels[i]] = CRGB::Black;   
      }
    }              
	}
	FastLED.show();  
	btn_mode.tick();
	btn_saw.tick();
}
