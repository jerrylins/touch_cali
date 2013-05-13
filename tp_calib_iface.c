/*
 * =====================================================================================
 *
 *       Filename:  tp_calibration.c
 *
 *    Description:  calibration interface
 *
 *        Version:  1.0
 *        Created:  Friday, May 03, 2013 04:50:13 HKT
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zhangyunxi 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/kobject.h>

#include "tp_calibration.h"
#define FLAG_SCREEN_DATA   0x01
#define FLAG_UNCALI_DATA   0x02
static int CALI_FLAG=0;

int extern screen_x[5];
int extern screen_y[5];
int extern uncali_x_default[5] ;
int extern uncali_y_default[5] ;
int extern uncali_x[5] ;
int extern uncali_y[5] ;
 extern UHPOINT screen_cali[5]; 
 extern UHPOINT uncali_cali[5]; 
 extern UHPOINT screen_dis[5]; 
 extern MATRIX  matrix; 



struct kobject *cali_kobj;

static int get_cali_data(const char* buf,UHPOINT* p )
{
	int ret,cmd;
	ret=sscanf(buf,"%d %d %d %d %d %d %d %d %d %d %d",&cmd,&p[0].x,&p[0].y,&p[1].x,&p[1].y,
			                                          &p[2].x,&p[2].y,&p[3].x,&p[3].y,
													  &p[4].x,&p[4].y);
	printk("cali para:%d   %d %d   %d %d   %d %d   %d %d   %d %d\n",cmd,p[0].x,p[0].y,p[1].x,p[1].y, 
			                                          p[2].x,p[2].y,p[3].x,p[3].y,
													  p[4].x,p[4].y);

   if(((cmd==CMD_SCREEN_DATA)||(cmd==CMD_SCREEN_DATA))&&(ret==11) )
	 return 1;

 return 0;
}

static ssize_t cali_show(struct kobject *kobj, struct kobj_attribute *attr,
			     char *buf)
{
    UHPOINT* p=screen_dis;
    UHPOINT* n=screen_cali;
	return sprintf(buf, "cali dis:%04d %04d   %04d %04d   %04d %04d   %04d %04d   %04d %04d\n\
cali pos:%04d %04d   %04d %04d   %04d %04d   %04d %04d   %04d %04d\n",\
p[0].x,p[0].y,p[1].x,p[1].y,p[2].x,p[2].y,p[3].x,p[3].y, p[4].x,p[4].y,\
n[0].x,n[0].y,n[1].x,n[1].y,n[2].x,n[2].y,n[3].x,n[3].y, n[4].x,n[4].y);
}

static setdata()
{
;
}
static ssize_t cali_store(struct kobject *kobj, struct kobj_attribute *attr,
			      const char *buf, size_t n)
{
	int cmd;	

	sscanf(buf, "%d",&cmd);
	
	printk("\nCalibration get ccmd %d\n",cmd);
	
	switch(cmd)
	{
		case CMD_SCREEN_DATA:
			                 if(get_cali_data(buf,screen_dis))
								 CALI_FLAG&=FLAG_SCREEN_DATA;
							  break;
		case CMD_UNCALI_DATA:
			                 if( get_cali_data(buf,screen_cali))
								 CALI_FLAG|=FLAG_UNCALI_DATA;   
							// if(CALI_FLAG!=(FLAG_SCREEN_DATA|FLAG_UNCALI_DATA)) 
							
								 break;
		case CMD_CALI_START:
							setCalibrationMatrix(&screen_dis,&screen_cali,&matrix);
							printk("matrix An=%d\n\tBn=%d\n\tCn=%d\n\tDn=%d\n\tEn=%d\n",\
									matrix.An,matrix.Bn,matrix.Cn,matrix.Dn,matrix.En);
							break;


		default:
			  
				 CALI_FLAG=0;   
               printk("calibration get a wrong cmd line\n");

	}
    printk("\ncalibration  flag=%d\n",CALI_FLAG);
	return n;
}

CALI_ATTR(cali);

static struct attribute * g[] = {
	&cali_attr.attr,
	NULL,
};
static struct attribute_group attr_group = {
	.attrs = g,
};
static int __init cali_init(void)
{

    printk("calibration cali_init\n");
	cali_kobj = kobject_create_and_add("ts_cali", NULL);
	if (!cali_kobj)
		return -ENOMEM;
	return sysfs_create_group(cali_kobj, &attr_group);
} 
module_init(cali_init);
