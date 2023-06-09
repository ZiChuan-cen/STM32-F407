#include "bsp_adc.h"


static void Rheostat_ADC_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;	
	RCC_AHB1PeriphClockCmd(RHEOSTAT_ADC_GPIO_CLK, ENABLE);    // 使能 GPIO 时钟		
	// 配置 IO
	GPIO_InitStructure.GPIO_Pin = RHEOSTAT_ADC_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;	    
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;    //不上拉不下拉
	GPIO_Init(RHEOSTAT_ADC_GPIO_PORT, &GPIO_InitStructure);		
}

static void Rheostat_ADC_Mode_Config(void)
{
	ADC_InitTypeDef ADC_InitStructure;
  ADC_CommonInitTypeDef ADC_CommonInitStructure;	
  
	RCC_APB2PeriphClockCmd(RHEOSTAT_ADC_CLK , ENABLE);    // 开启ADC时钟

  /*-------------------ADC Common 结构体 参数 初始化------------------------*/
  	
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;                         // 独立ADC模式  
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;                      // 时钟为fpclk 4分频  
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;          // 禁止DMA直接访问模式  	
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_20Cycles;    // 采样时间间隔  
  ADC_CommonInit(&ADC_CommonInitStructure);
	
	/*-----------------------------------------------------------------------*/
	
  /*-------------------ADC Init 结构体 参数 初始化--------------------------*/
	ADC_StructInit(&ADC_InitStructure);  
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;                  // ADC 分辨率  	
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;                           // 禁止扫描模式，多通道采集才需要  	
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;                      // 连续转换  
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;    // 禁止外部边沿触发
  // 外部触发通道，本例子使用软件触发，此值随便赋值即可
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;  	
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;                  // 数据右对齐  
  ADC_InitStructure.ADC_NbrOfConversion = 1;                              // 转换通道 1个                                    
  ADC_Init(RHEOSTAT_ADC, &ADC_InitStructure);
  /*-----------------------------------------------------------------------*/
	
  // 配置 ADC 通道转换顺序为1，第一个转换，采样时间为3个时钟周期
  ADC_RegularChannelConfig(RHEOSTAT_ADC, RHEOSTAT_ADC_CHANNEL, 1, ADC_SampleTime_56Cycles);
	// ADC 转换结束产生中断，在中断服务程序中读取转换值
	ADC_ITConfig(RHEOSTAT_ADC, ADC_IT_EOC, ENABLE);  
  ADC_Cmd(RHEOSTAT_ADC, ENABLE);    // 使能ADC  
  ADC_SoftwareStartConv(RHEOSTAT_ADC);    //开始adc转换，软件触发
}


// 配置中断优先级
static void Rheostat_ADC_NVIC_Config(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	
  NVIC_InitStructure.NVIC_IRQChannel = Rheostat_ADC_IRQ;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	
  NVIC_Init(&NVIC_InitStructure);
}


void Rheostat_Init(void)
{
	Rheostat_ADC_GPIO_Config();
	Rheostat_ADC_Mode_Config();
	Rheostat_ADC_NVIC_Config();
}












