#define NUM_LEDS 36 // число светодиодов в ленте с эффектами
#define DATA_PIN 2 //пин, соединяющийся с лентой с эффектами
#define BTN_PIN_MODE 5 //пин, выбирающий режим ленты с эффектами
#define BTN_PIN_SAW 4 //пин включающий и выключающий синус на ленту с эффектами
#define DATA_COMET_PIN 3 //пин, соединяющийся с лентой с кометами и созвездиями
#define NUM_COMET_LEDS 109 //число светодиодов в ленте с кометами и созвездиями

#define DATA_PIN2 A0 //пин, соединяющийся с лентой 2
#define DATA_PIN3 A1 //пин, соединяющийся с лентой 3
#define DATA_PIN4 A2 //пин, соединяющийся с лентой 4
#define DATA_PIN5 A3 //пин, соединяющийся с лентой 5

//биты для связи с Ардуино-Радио 
#define BIT0 6
#define BIT1 7
#define BIT2 8
#define BIT3 9
#define BIT4 10
#define ACK 11

// #define SETTABLE // директива, убрать если таблица не требуется

// #ifdef SETTABLE // директива 
// const uint8_t corrPx = 2; //число пикселей, требующих коррекции
// int8_t setTable[corrPx][2] ={ //номера пикселя и - яркости
//   {0, 100}, //править только пары номер/поправка
//   {1,100}
// };
// #endif

typedef struct { //эту структуру не редактировать
  uint8_t qty;
  uint8_t* pixels;
} zonepixels_t;

//--------------------------------СЛОЙ 1 ЛЕНТА 1--------------------------//

uint8_t level1_ms = 100; //шаг времени для динамических эффектов 0..255
uint8_t level1 = 1; //режим работы 1 уровня по умолчанию - белый, 0 выкл  0..1
uint8_t defaultBrightness = 100; // яркость по умолчанию     //0..255
uint8_t defaultBrightnessR = 200; //НОВОЕ яркость по умолчанию красного     //0..255
uint8_t defaultBrightnessG = 255; //яркость по умолчанию зеленого     //0..255
uint8_t defaultBrightnessB = 210; //яркость по умолчанию синего    //0..255

const uint8_t zonesQuantity = 5;// количество зон в ленте с эффектами
//--------------------------------ЗОНЫ-----------------------------------//
uint8_t zone1[] = {0,1,2,27,28,29,30,31,32,33,34,35}; //разбивка светодиодов в ленте с эффектами на зоны, не повторять и не забывать светодиоды
uint8_t zone2[] = {3,4,5,24,25,26};
uint8_t zone3[] = {6,7,8,21,22,23};
uint8_t zone4[] = {9,10,11,18,19,20};
uint8_t zone5[] = {12,13,14,15,16,17};

zonepixels_t zonepixels[] = { //массив зон, удалить лишние/дописать новые зоны при прошивке
  { sizeof(zone1), zone1 },
  { sizeof(zone2), zone2 }, 
  { sizeof(zone3), zone3 },
  { sizeof(zone4), zone4 },
  { sizeof(zone5), zone5 },
};

uint8_t colorCount = 12; //количество моноцветов, включая белый и не включая черный  

uint8_t modeCount = 4; //количество режимов, включая белый и не включая черный  
//

uint32_t monocolorTable[]  = {//таблица моноцветов для режима одного цвета  
  0x000000, //0-й всегда черный
  0xFFFFFF, //1-й всегда бел
  0xFF0000, //2-й красный  //далее как нарисовано на пульте, слева направо, сверху вниз, кроме последнего белого
  0x00FF00, //3-й зеленый 
  0x0000FF, //4-й синий 
  0xFFFF00, //5-й желтый  
  0xFFCC00, //6-й оранжевый
  0xFF9900, //7-й рыжий
  0x663300, //8-й коричневый
  0x00FFFF, //9-й голубой
  0x6600CC, //10-й фиолетовый 
  0xCC0066, //11-й пурпурный
  0x009933, //12-й изумрудный
};


uint8_t defaultSlice = 8; //значение куска радуги по умолчанию при переключении режимов    

//--------------------------------СЛОЙ 2-------------------------------------//
uint8_t level2_ms1_1 = 11; //шаг времени, режим 1 медленный                                    
uint8_t level2_ms1_2 = 12;                                                                    
uint8_t level2_ms1_3 = 13;                                                                      
uint8_t level2_ms1_4 = 14;                                                                     
uint8_t level2_ms1_5 = 15;                                                                    

uint8_t level2_ms2_1 = 7; //шаг времени, режим 2 быстрый                                      
uint8_t level2_ms2_2 = 8; //шаг времени, режим 2 быстрый                                      
uint8_t level2_ms2_3 = 9; //шаг времени, режим 2 быстрый                                      
uint8_t level2_ms2_4 = 10; //шаг времени, режим 2 быстрый                                      
uint8_t level2_ms2_5 = 11; //шаг времени, режим 2 быстрый                                       

uint8_t level2 = 0; //режим работы 2 уровня по умолчанию 1 - ВКЛ, 0 - ВЫКЛ
uint8_t level2_1_max_percent = 100; //верхний уровень синуса медленного режим 1 //0..100
uint8_t level2_1_min_percent = 40; // нижний уровень синуса медленного режим 1  //0..100
uint8_t level2_2_max_percent = 100; //верхний уровень синуса быстрого режим 2  //0..100
uint8_t level2_2_min_percent = 20; // нижний уровень синуса быстрого режим 2   //0..100
  

//--------------------------------СОЗВЕЗДИЯ ЛЕНТА 2--------------------------//

uint8_t defaulSZVBrightness = 255; //яркость созвездий

uint32_t SZVColor = 0xFFFFFF; // цвет созвездий
uint8_t defaulSZVBrightnessR = 255; //НОВОЕ яркость созвездий по умолчанию Красный //0..255
uint8_t defaulSZVBrightnessG = 255; //яркость созвездий по умолчанию Зеленый //0..255
uint8_t defaulSZVBrightnessB = 255; //яркость созвездий по умолчанию Синий //0..255
const uint8_t sozvezdiaQuantity = 1; //количество созвездий

 uint8_t sozvezdie1[] = {20,21,22,23,24,25,26,27,28}; //разбивка светодиодов в ленте на созвездия

 zonepixels_t sozvezdiya[] = {   //массив созвездий, удалить лишние/дописать новые созвездия при прошивке
  {sizeof(sozvezdie1), sozvezdie1 },
};

//--------------------------------КОМЕТЫ ЛЕНТА 2--------------------------//
uint8_t cometStepTimerMin = 50; //минимум и максимум задержки шага перемещения кометы
uint8_t cometStepTimerMax = 125;
 
uint8_t cometTailMin = 2; //минимум и максимум длины хвоста
uint8_t cometTailMax = 10;

uint8_t cometBrightessMin = 200; //минимум и максимум яркости 0..255
uint8_t cometBrightessMax = 255;

uint8_t cometLaunchTimerMin = 15; //задержка между запусками комет в секундах
uint8_t cometLaunchTimerMax = 30;

const uint8_t cometCount = 5;//число комет

uint32_t colorTable[] = { //таблица с цветами для комет
  0xFFFFFF,
  0xFF0000,
  0x00FF00,
  0x0000FF,
  0xFF1493,
  0xFF4500,
  0xFFFF00,
  0xEEE8AA,
  0x9370DB,
  0xFF00FF,
  0x7FFFD4,
  0x87CEFA,
  0x00BFFF,
  0x800080,
};

uint8_t comet1[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19}; //разбивка светодиодов на кометы
uint8_t comet2[] = {29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48};
uint8_t comet3[] = {49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68};
uint8_t comet4[] = {69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88}; //разбивка светодиодов на кометы
uint8_t comet5[] = {89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108}; //разбивка светодиодов на кометы

zonepixels_t comets[] = { //массив комет, удалить лишние/дописать новые кометы при прошивке
  {sizeof(comet1), comet1 },
  {sizeof(comet2), comet2 },
  {sizeof(comet3), comet3 },
  {sizeof(comet4), comet4 },
  {sizeof(comet5), comet5 }
};

//начальный сдвиг по времени для комет в секундах (время между первым запуском
uint8_t shift1 = 1; //от запуска 1й до запуска 2й
uint8_t shift2 = 2; //от запуска 1й до запуска 3й
uint8_t shift3 = 3; //от запуска 1й до запуска 4й
uint8_t shift4 = 4; //от запуска 1й до запуска 5й
