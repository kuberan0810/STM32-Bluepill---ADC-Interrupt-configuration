unsigned long long previousmillis =0;
int AD_val=0;
float voltage=0;


void setup() 
{
  Serial.begin(115200);

//Setup for PC13 [LED] as Digital Output & PB5 [ SWITCH] as Digital Input
     
 // PC13 as Digital Output Push-pull 
  GPIOC_BASE->CRH |= ((1<<20)|(1<<21)); 
  GPIOC_BASE->CRH &= ~((1<<22)| (1<<23));

 // PB5 as  Digital Input Push-pull 
  GPIOB_BASE->CRL &= ~((1<<20)|(1<<21)| (1<<22) ); 
  GPIOB_BASE->CRL |= (1<<23);

// Setting up ADC1 for reading data from PA7..............  
// 9 - ADC1 , 10 -ADC2 , 15- ADC3 
  
  
//ADC1 Clock Enable
  RCC_BASE->APB2ENR |= (1<<9);

// ADC1 is being  Reset
  RCC_BASE->APB2RSTR |=(1<<9);

// ADC1 is being  Set
  RCC_BASE->APB2RSTR &=~(1<<9);
  

// PA7 as Analog Input ( for reading Analog input)
  GPIOA_BASE->CRL &= ~((1<<28)|(1<<29)|(1<<30)|(1<<31)); 
 
// Regular Channel single conversion sequenced channel 7 to 1st conversion
  ADC1_BASE->SQR1 = 0x00000000; // Regular channel single conversion
  ADC1_BASE->SQR2 = 0x00000000; // Clear register
  ADC1_BASE->SQR3 = (7<<0);  // channel 7 as 1st conversion  

// Sampling time of conversion of ADC1
  ADC1_BASE->SMPR1   = 0x00000000;           // Clear register no channel used in this register                    
  ADC1_BASE->SMPR2   =  5 << 21;             // Channel 7 sample time is 55.5 cyc 
    
//Enable EOC interrupt & Nested vector Number 18
//adc_attach_interrupt(ADC1,0,SEN);

  ADC1_BASE->CR1 |= (1<<5); // Enable EOC interrupt
  NVIC_BASE->ISER[0] |=(1<<18); 
  ADC1->handlers[0] = __irq_adc;

// setting for the Scan mode ( in scan mode the input selected through SQRx is converted automatically 
   ADC1_BASE->CR1 |= (1<<8);
   
// Select external trigger and set software trigger out of all eight possibility available for ext trigger
  ADC1_BASE->CR2 |= (1<<20); // Enable external trigger
  ADC1_BASE->CR2 |= (7<<17); // SWSTART
    
//Continous conversion mode and ADC on with right data alignment
  ADC1_BASE->CR2 |= (1<<1);  // continous conversion (CONT)
  ADC1_BASE->CR2 |= (1<<0);  // Adc set on (ADCON)
  ADC1_BASE->CR2 &= ~(1<<11);// Right alligned data receive
  
// Calibration of ADC
// Iitialize, wait for inilization,start calibration,wait for calibration to finish & start conversion
  ADC1_BASE->CR2    |=  1 <<  3;             // Initialize calibration registers   
  while (ADC1_BASE->CR2 & (1 << 3));         // Wait for initialization to finish  
  ADC1_BASE->CR2    |=  1 <<  2;             // Start calibration                  
  while (ADC1_BASE->CR2 & (1 << 2));         // Wait for calibration to finish     
  ADC1_BASE->CR2    |=  1 << 22;             // Start first conversion              
}



void loop() 
{

// Switch on/off at PB5 and LED glow/off at PC13 code

if (((GPIOB_BASE->IDR)>>5)&1)
     GPIOC_BASE->ODR |= (1<<13);
else 
     GPIOC_BASE->ODR &= ~(1<<13);
 
  voltage = (float(AD_val)/4096) * 3.3; //formulae to convert the ADC value to voltage
  roll_graph(10);
}


// Local function to plot graph
void roll_graph(unsigned int delayy1)
{
if ((millis()-previousmillis)>=delayy1)
    { 
    Serial.print(0);
    Serial.print(" ");
    Serial.println(voltage);
    previousmillis = millis();
    }
  }  


void __irq_adc()                                              
{                    
// Reading ADC for the Analog data at PA7
      AD_val = ADC1_BASE->DR & 0x0FFF;       // Read AD converted value ( by masking according to right aligned 12 bit answer as set earlier)           
    }

