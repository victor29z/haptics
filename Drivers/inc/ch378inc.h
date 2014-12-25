/********************************** (C) COPYRIGHT *******************************
* File Name          : CH378INC.C
* Author             : MJX
* Version            : V1.10
* Date               : 2013/05/30
* Description        : C Define for CH378
*					   Website: http://wch.cn
*					   Email:   tech@wch.cn 		
*******************************************************************************/



#ifndef __CH378INC_H__
#define __CH378INC_H__

#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************************************************************************/
/* �������ͺͳ������� */

#ifndef		TRUE
#define		TRUE	1
#define		FALSE	0
#endif
#ifndef		NULL
#define		NULL	0
#endif

#ifndef UINT8
typedef unsigned char              UINT8;
#endif
#ifndef UINT16
typedef unsigned short             UINT16;
#endif
#ifndef UINT32
typedef unsigned long              UINT32;
#endif
#ifndef PUINT8
typedef unsigned char              *PUINT8;
#endif
#ifndef PUINT16
typedef unsigned short             *PUINT16;
#endif
#ifndef PUINT32
typedef unsigned long              *PUINT32;
#endif
#ifndef UINT8V
typedef unsigned char volatile     UINT8V;
#endif
#ifndef PUINT8V
typedef unsigned char volatile     *PUINT8V;
#endif

/* ********************************************************************************************************************* */
/* ������˵��: 
   (1)�������������CH376оƬ, �����������ݻ���������ݵĿ��ֲܾ���ͬ;
   (2)��һ���������˳�����:
          a��һ��������(���ڴ��ڷ�ʽ,������֮ǰ����Ҫ����ͬ����);
          b�����ɸ���������(������0��);
          c�������ж�֪ͨ�������ɸ��������(������0��), ��ѡһ, ���ж�֪ͨ��һ��û���������, �����������һ���������ж�;
             ��CMD01_WR_REQ_DATA��������, ˳�����: һ��������, һ���������, ���ɸ���������   
   (3)����������������: CMDxy_NAME
        ���е�x��y��������, x˵�������������ݸ���(�ֽ���), y˵������������ݸ���(�ֽ���), y�����H��˵�������ж�֪ͨ,
        ��Щ�����ܹ�ʵ��0������ֽڵ����ݿ��д, ���ݿ鱾����ֽ���δ����������x��y֮�� */

/* ********************************************************************************************************************* */
/* ��Ҫ����(�ֲ�һ), ����, ���������ڲ�������ʱ��������ж�֪ͨ */

#define	CMD01_GET_IC_VER		   0x01							 /* ��ȡоƬ���̼��汾 */
/* ���: �汾��( λ7Ϊ0, λ6Ϊ1, λ5~λ0Ϊ�汾�� ) */
/*       CH378���ذ汾�ŵ�ֵΪ040H���汾��Ϊ00H */

#define	CMD31_SET_BAUDRATE		   0x02							 /* ���ڷ�ʽ: ���ô���ͨѶ������(�ϵ���߸�λ���Ĭ�ϲ�����Ϊ9600bps,��SCK/SDI/SDO����ѡ��) */
/* ����: 3���ֽڲ�����ϵ��,���ֽ���ǰ */
/* ���: ����״̬( CMD_RET_SUCCESS��CMD_RET_ABORT, ����ֵ˵������δ��� ) */

#define	CMD10_ENTER_SLEEP		   0x03							 /* ����˯��״̬ */
/* ����: 1���ֽڲ���, �������Ϊ0x11,��ʹ��оƬ�����˯��״̬,����֮��USB���ܿ��Լ���ʹ��;
                      �������Ϊ0x22,��ʹ��оƬ�������˯��״̬,����֮�������������USB�豸,��Ҫȫ�����³�ʼ��; */

#define	CMD00_RESET_ALL			   0x05							 /* ִ��Ӳ����λ */

#define	CMD11_CHECK_EXIST	       0x06							 /* ����ͨѶ�ӿں͹���״̬ */
/* ����: �������� */
/* ���: �������ݵİ�λȡ�� */

#define	CMD20_SET_SDO_INT		   0x0B						 	 /* SPI�ӿڷ�ʽ: ����SPI��SDO���ŵ��жϷ�ʽ */
/* ����: ����16H, �жϷ�ʽ */
/*           00H=��ֹSDO���������ж����,��SCSƬѡ��Чʱ��̬�����ֹ, 01H=SDO������SCSƬѡ��Чʱ�����ж�������� */

#define	CMD14_GET_FILE_SIZE		   0x0C							 /* �����ļ�ģʽ: ��ȡ��ǰ�ļ����� */
/* ����: ����68H */
/* ���: ��ǰ�ļ�����(�ܳ���32λ,���ֽ���ǰ) */

#define	CMD11_SET_USB_MODE		   0x15							 /* ����USB����ģʽ */
/* ����: ģʽ���� */
/*       00H=δ���õ��豸��ʽ, 01H=�����õ��豸��ʽ����ʹ���ⲿ�̼�ģʽ, 02H=�����õ��豸��ʽ����ʹ�����ù̼�ģʽ */
/*       03H=SD������ģʽ/δ���õ�����ģʽ,���ڹ���ʹ�ȡSD���е��ļ� */
/*       04H=SD������ģʽ/�����õ�����ģʽ,���ڹ���ʹ�ȡSD���е��ļ� */
/*       05H=δ���õ�������ʽ */
/*       06H=USB����ģʽ/δ���õ�����ģʽ,���ڹ���ʹ�ȡUSB�洢�豸�е��ļ� */
/*       07H=USB����ģʽ/�����õ�����ģʽ,���ڹ���ʹ�ȡUSB�洢�豸�е��ļ� */
/* ���: ����״̬( CMD_RET_SUCCESS��CMD_RET_ABORT, ����ֵ˵������δ��� ) */

#define	CMD01_GET_STATUS		   0x22							 /* ��ȡ�ж�״̬��ȡ���ж����� */
/* ���: �ж�״̬ */

#define	CMD40_RD_HOST_OFS_DATA	   0x26							 /* ����ģʽ: ��ȡ�ڲ�ָ��������ָ��ƫ�Ƶ�ַ���ݿ�(�ڲ�ָ���Զ�����) */
/* ����: ƫ�Ƶ�ַ( 2���ֽ�,���ֽ���ǰ,���Ϊ20480 )����ȡ����(2���ֽ�,���ֽ���ǰ,���Ϊ20480) */
/* ���: ������ */
/* ע��: �������������,��ֻ����ǰ��ʵ�ʻ��������� */

#define	CMD20_RD_HOST_CUR_DATA     0x27							 /* ����ģʽ: ��ȡ�ڲ�ָ����������ǰƫ�Ƶ�ַ���ݿ�(�ڲ�ָ���Զ�����) */
/* ����: ��ȡ����(2���ֽ�,���ֽ���ǰ,���Ϊ20480) */
/* ���: ������ */
/* ע��: �������������,��ֻ����ǰ��ʵ�ʻ��������� */

#define	CMD00_RD_HOST_REQ_DATA	   0x28							 /* ����ģʽ: ��ȡ�ڲ�ָ����������ǰƫ�Ƶ�ַ�����ȡ�����ݿ�(�ڲ�ָ���Զ�����) */
/* ���: ��ȡ����(2���ֽ�,���ֽ���ǰ,���Ϊ20480) */
/* ���: ������ */
/* ע��: �������������,��ֻ����ǰ��ʵ�ʻ��������� */

#define	CMD40_WR_HOST_OFS_DATA	   0x2D							 /* ����ģʽ: ���ڲ�ָ��������ָ��ƫ�Ƶ�ַд�����ݿ�(�ڲ�ָ���Զ�����) */
/* ����: ƫ�Ƶ�ַ( 2���ֽ�,���ֽ���ǰ,���Ϊ20480 )�����볤��(2���ֽ�,���ֽ���ǰ,���Ϊ20480)�������� */
/* ע��: �������������,��д�����ݻ��Զ��ӻ�������ʼ��ַ��ʼ���� */

#define	CMD20_WR_HOST_CUR_DATA	   0x2E							 /* ����ģʽ: ���ڲ�ָ����������ǰƫ�Ƶ�ַд�����ݿ�(�ڲ�ָ���Զ�����) */
/* ����: ���볤��(2���ֽ�,���ֽ���ǰ,���Ϊ20480)�������� */
/* ע��: �������������,��д�����ݻ��Զ��ӻ�������ʼ��ַ��ʼ���� */

#define	CMD10_SET_FILE_NAME		   0x2F							 /* �����ļ�ģʽ: ���ý�Ҫ�������ļ����ļ�����·���� */
/* ����: ��0�������ַ���(��������0���ڳ��Ȳ�����MAX_FILE_NAME_LEN���ַ�) */


/* ********************************************************************************************************************* */
/* ��Ҫ����(�ֲ�һ), ����, �������������ڲ�������ʱ�����ж�֪ͨ, ��������û��������� */

#define	CMD0H_DISK_CONNECT		   0x30							 /* �����ļ�ģʽ: �������Ƿ����� */
/* ����ж� */

#define	CMD0H_DISK_MOUNT		   0x31							 /* �����ļ�ģʽ: ��ʼ�����̲����Դ����Ƿ���� */
/* ����ж� */

#define	CMD0H_FILE_OPEN			   0x32							 /* �����ļ�ģʽ: ���ļ�����Ŀ¼(�ļ���),����ö���ļ���Ŀ¼(�ļ���) */
/* ����ж� */

#define	CMD0H_FILE_ENUM_GO		   0x33							 /* �����ļ�ģʽ: ����ö���ļ���Ŀ¼(�ļ���) */
/* ����ж� */

#define	CMD0H_FILE_CREATE		   0x34							 /* �����ļ�ģʽ: �½��ļ�,����ļ��Ѿ�������ô��ɾ�� */
/* ����ж� */

#define	CMD0H_FILE_ERASE		   0x35							 /* �����ļ�ģʽ: ɾ���ļ�,����Ѿ�����ֱ��ɾ��,��������ļ����ȴ���ɾ��,��Ŀ¼�����ȴ� */
/* ����ж� */

#define	CMD1H_FILE_CLOSE		   0x36							 /* �����ļ�ģʽ: �رյ�ǰ�Ѿ��򿪵��ļ�����Ŀ¼(�ļ���) */
/* ����: �Ƿ���������ļ����� */
/*       00H=��ֹ���³���, 01H=������³��� */
/* ����ж� */

#define	CMD1H_DIR_INFO_READ		   0x37							 /* �����ļ�ģʽ: ��ȡ�ļ���Ŀ¼��Ϣ */
/* ����: ָ����Ҫ��ȡ��Ŀ¼��Ϣ�ṹ�������ڵ������� */
/*       �����ŷ�ΧΪ00H~0FH, ������0FFH��Ϊ��ǰ�Ѿ��򿪵��ļ� */
/* ����ж� */

#define	CMD1H_DIR_INFO_SAVE		   0x38							 /* �����ļ�ģʽ: �����ļ���Ŀ¼��Ϣ */
/* ����: ָ����Ҫд���Ŀ¼��Ϣ�ṹ�������ڵ������� */
/*       �����ŷ�ΧΪ00H~0FH, ������0FFH��Ϊ��ǰ�Ѿ��򿪵��ļ� */
/* ����ж� */

#define	CMD4H_BYTE_LOCATE	       0x39							 /* �����ļ�ģʽ: ���ֽ�Ϊ��λ�ƶ���ǰ�ļ�ָ�� */
/* ����: ƫ���ֽ���(�ܳ���32λ,���ֽ���ǰ) */
/* ����ж� */

#define	CMD2H_BYTE_READ			   0x3A						 	 /* �����ļ�ģʽ: ���ֽ�Ϊ��λ�ӵ�ǰλ�ö�ȡ���ݿ� */
/* ����: �����ȡ���ֽ���(�ܳ���16λ,���ֽ���ǰ,���Ϊ20480) */
/* ����ж� */
/* ע��: ���͸�������ȡ��ʵ���������ڲ���������,��Ҫͨ��CMD40_RD_HOST_OFS_DATA��CMD20_RD_HOST_CUR_DATA�����ȡ */

#define	CMD2H_BYTE_WRITE		   0x3C							 /* �����ļ�ģʽ: ���ֽ�Ϊ��λ��ǰλ��д�����ݿ� */
/* ����: ����д����ֽ���(�ܳ���16λ,���ֽ���ǰ,���Ϊ20480) */
/* ����ж� */
/* ע��: ���ø�����ǰ��Ҫͨ��CMD40_WR_HOST_OFS_DATA��CMD20_WR_HOST_CUR_DATA���Ҫд������Ԥ����䵽�ڲ��������� */

#define	CMD0H_DISK_CAPACITY		   0x3E							 /* �����ļ�ģʽ: ��ѯ������������ */
/* ����ж� */

#define	CMD0H_DISK_QUERY		   0x3F							 /* �����ļ�ģʽ: ��ѯ���̿ռ���Ϣ */
/* ����ж� */

#define	CMD0H_DIR_CREATE		   0x40							 /* �����ļ�ģʽ: �½�Ŀ¼(�ļ���)����,���Ŀ¼�Ѿ�������ôֱ�Ӵ� */
/* ����ж� */

#define	CMD4H_SEC_LOCATE		   0x4A						   	 /* �����ļ�ģʽ: ������Ϊ��λ�ƶ���ǰ�ļ�ָ�� */
/* ����: ƫ��������(�ܳ���32λ,���ֽ���ǰ) */
/* ����ж� */

#define	CMD1H_SEC_READ			   0x4B							 /* �����ļ�ģʽ: ������Ϊ��λ�ӵ�ǰλ�ö�ȡ���ݿ� */
/* ����: �����ȡ�������� */
/* ����ж� */
/* ע��: ���ڲ���������С����,����������д���������Ϊ40 */

#define	CMD1H_SEC_WRITE			   0x4C							 /* �����ļ�ģʽ: ������Ϊ��λ�ڵ�ǰλ��д�����ݿ� */
/* ����: ����д��������� */
/* ����ж� */
/* ע��: ���ڲ���������С����,����������д���������Ϊ40 */

#define	CMD0H_DISK_BOC_CMD		   0x50							 /* ������ʽ/��֧��SD��: ��USB�洢��ִ��BulkOnly����Э������� */
/* ����ж� */

#define	CMD5H_DISK_READ			   0x54							 /* ������ʽ: ��USB�洢����SD������������ */
/* ����: LBA����������ַ(�ܳ���32λ, ���ֽ���ǰ), ������(01H~28H) */
/* ����ж� */
/* ע��: ���ڲ���������С����,����������д���������Ϊ40 */

#define	CMD5H_DISK_WRITE		   0x56							 /* ������ʽ: ��USB�洢����SD��д�������� */
/* ����: LBA����������ַ(�ܳ���32λ, ���ֽ���ǰ), ������(01H~FFH) */
/* ����ж� */
/* ע��: ���ڲ���������С����,����������д���������Ϊ40 */
/* ע��: ���ø�����ǰ��Ҫͨ��CMD40_WR_HOST_OFS_DATA��CMD20_WR_HOST_CUR_DATA���Ҫд������Ԥ����䵽�ڲ��������� */

#define	CMD0H_FILE_QUERY		   0x55							 /* �����ļ�ģʽ: ��ѯ��ǰ�ļ�����Ϣ */
/* ����ж� */
/* ע��: �������ݸ�ʽ: 4���ֽ��ļ����ȡ�2���ֽ��ļ����ڡ�2���ֽ��ļ�ʱ�䡢1���ֽ��ļ����� */

#define	CMD0H_FILE_MODIFY		   0x57							 /* �����ļ�ģʽ: ��ѯ�����޸ĵ�ǰ�ļ�����Ϣ */
/* ����ж� */
/* ע��: �������ݸ�ʽ: 4���ֽ��ļ����ȡ�2���ֽ��ļ����ڡ�2���ֽ��ļ�ʱ�䡢1���ֽ��ļ����� */


/* ********************************************************************************************************************* */
/* ��Ҫ����(�ֲ��), �ǳ���, ���������ڲ�������ʱ��������ж�֪ͨ */

#define	CMD20_BULK_WR_TEST		   0x07							 /* ����д���ݲ��� */
/* ����: 2���ֽں������ݳ���,N���ֽں������� */

#define	CMD20_BULK_RD_TEST		   0x08							 /* ���������ݲ��� */
/* ����: 2���ֽں������ݳ��� */
/* ���: N���ֽں������� */

#define	CMD11_READ_VAR8			   0x0A							 /* ��ȡָ����8λCH378ϵͳ���� */
/* ����: ������ַ */
/* ���: ��ǰ��ַ��Ӧ��8λ���� */

#define	CMD11_GET_DEV_RATE		   0x0A							 /* ������ʽ: ��ȡ��ǰ���ӵ�USB�豸�������������� */
/* ����: ����07H */
/* ���: ������������ */
/*           00��ʾ�ٶ�δ֪; 01��ʾ1.5Mbps����USB�豸; 02��ʾ12Mbpsȫ��USB�豸; 03��ʾ480Mbps����USB�豸 */

#define	CMD20_WRITE_VAR8		   0x0B							 /* ����ָ����8λCH378ϵͳ���� */
/* ����: ������ַ, ���� */

#define	CMD14_READ_VAR32		   0x0C							 /* ��ȡָ����32λCH378ϵͳ���� */
/* ����: ������ַ */
/* ���: ��ǰ��ַ��Ӧ��32λ����(�ܳ���32λ,���ֽ���ǰ) */

#define	CMD50_WRITE_VAR32		   0x0D							 /* ����ָ����32λCH378ϵͳ���� */
/* ����: ������ַ, ����(�ܳ���32λ,���ֽ���ǰ) */

#define	CMD50_SET_FILE_SIZE		   0x0D							 /* �����ļ�ģʽ: ���õ�ǰ�ļ����� */
/* ����: ����68H, ��ǰ�ļ�����(�ܳ���32λ,���ֽ���ǰ) */
/* ע�⣺������ֻ����ʱ�ı䵱ǰ�򿪵��ļ����ļ����� */

#define	CMD02_GET_REAL_LEN		   0x0E							 /* ���ٷ�����һ������ִ����Ϻ����󳤶�����Ӧ��ʵ�ʳ��� */
/* ���: 4���ֽ�ʵ������, ���ֽ���ǰ */
/* ע��: �����������ʹ���ֽڶ�λ���ֽڶ�д��������λ��������д����ʱʹ��,�Լӿ�ִ���ٶ�  */

#define	CMD01_TEST_CONNECT		   0x16							 /* ������ʽ: ���USB�豸/SD������״̬ */
/* ���: ����״̬( USB_INT_CONNECT��USB_INT_DISCONNECT��USB_INT_USB_READY, ����ֵ˵������δ��� ) */

#define	CMD00_DIRTY_BUFFER		   0x25							 /* �����ļ�ģʽ: ����ڲ��Ĵ��̺��ļ������� */

/* ********************************************************************************************************************* */
/* ��Ҫ����(�ֲ��), �ǳ���, �������������ڲ�������ʱ�����ж�֪ͨ, ��������û��������� */

#define	CMD1H_CLR_STALL			   0x41							 /* ������ʽ/��֧��SD��: ���ƴ���-����˵���� */
/* ����: �˵�� */
/* ����ж� */

#define	CMD1H_SET_ADDRESS		   0x45							 /* ������ʽ/��֧��SD��: ���ƴ���-����USB��ַ */
/* ����: ��ֵַ */
/* ����ж� */

#define	CMD1H_GET_DESCR			   0x46							 /* ������ʽ/��֧��SD��: ���ƴ���-��ȡ������ */
/* ����: ���������� */
/* ����ж� */

#define	CMD1H_SET_CONFIG		   0x49							 /* ������ʽ/��֧��SD��: ���ƴ���-����USB���� */
/* ����: ����ֵ */
/* ����ж� */

#define	CMD0H_AUTO_SETUP		   0x4D							 /* ������ʽ/��֧��SD��: �Զ�����USB�豸 */
/* ����ж� */

#define	CMD0H_ISSUE_CTRL_TRAN	   0x4E							 /* ������ʽ/��֧��SD��: ִ�п��ƴ��� */
/* ����ж� */
/* ע��: ���������ǰ,����ͨ��CMD40_WR_HOST_OFS_DATA���8���ֽڵ�SETUP��д�뵽�ڲ���������,�����Ҫͨ��OUT�׶��´����ݰ�,
         ����Ҫ���������ݰ�������8���ֽڵ�SETUP��д�뵽�ڲ��������� */

#define	CMD0H_DISK_INIT			   0x51							 /* ������ʽ/��֧��SD��: ��ʼ��USB�洢�� */
/* ����ж� */

#define	CMD0H_DISK_RESET		   0x52							 /* ������ʽ/��֧��SD��: ���ƴ���-��λUSB�洢�� */
/* ����ж� */

#define	CMD0H_DISK_SIZE			   0x53							 /* ������ʽ/��֧��SD��: ��ȡUSB�洢�������� */
/* ����ж� */

#define	CMD0H_DISK_INQUIRY		   0x58						 	 /* ������ʽ/��֧��SD��: ��ѯUSB�洢������ */
/* ����ж� */

#define	CMD0H_DISK_READY		   0x59							 /* ������ʽ/��֧��SD��: ���USB�洢������ */
/* ����ж� */

#define	CMD0H_DISK_R_SENSE		   0x5A							 /* ������ʽ/��֧��SD��: ���USB�洢������ */
/* ����ж� */

#define	CMD0H_DISK_MAX_LUN		   0x5D							 /* ������ʽ/��֧��SD��: ���ƴ���-��ȡUSB�洢������߼���Ԫ�� */
/* ����ж� */

#define	CMD10_SET_LONG_FILE_NAME   0x60							 /* �����ļ�ģʽ: ���ý�Ҫ�����ĳ��ļ����ļ���(�������ļ���) */
/* ���볤��(2���ֽ�,���ֽ���ǰ,���Ϊ520)��������0�������ַ���(���ļ������Ȳ�����520���ֽ�) */

#define	CMD10_GET_LONG_FILE_NAME   0x61							 /* �����ļ�ģʽ: ���������ļ���·��(�������ļ����ļ���)�õ���Ӧ�ĳ��ļ��� */
/* ����ж� */
/* ���: ��ȡ����(2���ֽ�,���ֽ���ǰ,���Ϊ520)�����ļ��� */
/* ע�⣺���������ǰ,������ͨ��CMD10_SET_FILE_NAME������ļ�������·��������CH378 */

#define	CMD0H_LONG_FILE_CREATE	   0x62							 /* �����ļ�ģʽ: �½����ļ����ļ�,����ļ��Ѿ�������ô��ɾ�� */
/* ����ж� */
/* ע�⣺���������ǰ,������ͨ��CMD10_SET_LONG_FILE_NAME�����CMD10_SET_FILE_NAME����ֱ𽫳��ļ���,�Լ����ļ�������·��������CH378 */

#define	CMD0H_LONG_DIR_CREATE	   0x63							 /* �����ļ�ģʽ: �½����ļ���Ŀ¼(�ļ���)����,���Ŀ¼�Ѿ�������ôֱ�Ӵ� */
/* ����ж� */
/* ע�⣺���������ǰ,������ͨ��CMD10_SET_LONG_FILE_NAME�����CMD10_SET_FILE_NAME����ֱ𽫳��ļ���,�Լ����ļ�������·��������CH378 */

#define	CMD0H_AUTO_DEMO			   0x64							 /* оƬ�Զ���ʾ,��U�̻���SD�����Զ�������Ϊ��оƬ��ʾ.TXT�����ļ�����д�뵱ǰ��Ϣ */
/* ����ж� */

/* ********************************************************************************************************************* */
/* USB�豸ģʽ������� */

#define	CMD40_SET_USB_ID		   0x11							 /* ����USB����VID�Ͳ�ƷPID */
/* ����: 4���ֽ�VID\PID����,�ֱ�Ϊ����ID���ֽ�, ����ID���ֽ�, ��ƷID���ֽ�, ��ƷID���ֽ� */
/* ע�������������CMD11_SET_USB_MODE����ǰ���� */

#define	CMD10_SET_USB_SPEED		   0x13							 /* ����USB�ٶ� */
/* ����: 1���ֽ�USB�ٶ�ֵ,0x00Ϊȫ��,0x02Ϊ����,����ֵ��Ч */
/* ע�������������CMD11_SET_USB_MODE����ǰ���� */

#define	CMD01_GET_USB_SPEED		   0x14							 /* ��ȡUSB�ٶ� */
/* ���: 1���ֽڵ�USB�ٶ�ֵ, 0x00Ϊȫ��,0x02Ϊ����,����ֵ��Ч */

#define	CMD70_INIT_ENDPx		   0x17							 /* ����USB�˵�(�˵�����x��Χ: 1--4) */
/* ����: 1���ֽ�USB�˵�������: 1--4, 1���ֽڶ˵��, 1���ֽڶ˵�����, 1���ֽڶ˵㷽��, 2���ֽ�������С(���ֽ���ǰ),1���ֽڵ��ж�״̬  */
/* ע�������������CMD11_SET_USB_MODE����ǰ���� */

#define	CMD20_SET_INDEXx_IN		   0x18							 /* ����USB IN�˵�����x(x��Χ: 0--4)�Ľ����� */
/* ����: 1���ֽ�������, 1���ֽڹ�����ʽ */
/*           λ7Ϊ1��λ�ö˵��ͬ��λ��
             λ6Ϊ1�����øö˵�������Ӧ��ʽΪ��STALL��
			 λ5Ϊ1������ö˵�֮ǰ��STALL���ԣ�
			 ���������ʵ�������ϴ�,��оƬ�Զ�Ӧ��NAK */

#define	CMD20_SET_INDEXx_OUT	   0x19						 	 /* ����USB OUT�˵�����x(x��Χ: 0--4)�ķ����� */
/* ����: 1���ֽ�������, 1���ֽڹ�����ʽ */
/*           λ7Ϊ1��λ�ö˵��ͬ��λ��
             λ6Ϊ1�����øö˵�������Ӧ��ʽΪ��STALL��
			 λ5Ϊ1������ö˵�֮ǰ��STALL���ԣ�
			 ����������յ��´����ݰ�,��оƬ�Զ�Ӧ��ACK */

#define	CMD10_UNLOCK_USB		   0x23							 /* �ͷŵ�ǰUSB������ */
/* ����: 1���ֽ�USB�˵�������: 0--4 */

#define	CMD1x_RD_USB_DATA		   0x29							 /* ��ָ��USB�жϵĶ˵㻺������ȡ���ݿ�,���ͷŻ����� */
/* ����: 1���ֽ�USB�˵�������: 0--4 */
/* ���: 2���ֽڵ����ݳ���, ������ */

#define	CMD30_WR_USB_DATA	       0x2A							 /* �����֮���USB�˵�ķ��ͻ�����д�����ݿ� */
/* ����: 1���ֽ�USB�˵�������: 1--4; 2���ֽڵ����ݳ���; N���ֽڵ������� */

#define	CMD10_WR_USB_DATA0	       0x2B							 /* ��USB���ƶ˵�ķ��ͻ�����д�����ݿ� */
/* ����: 1���ֽڵ����ݳ���; N���ֽڵ������� */


/* ********************************************************************************************************************* */
/* USB�豸ģʽ�ٶȶ��� */
#define USB_DEV_SPEED_FS		   0x00							 /* ��ǰΪȫ���豸 */
#define USB_DEV_SPEED_HS		   0x02							 /* ��ǰΪ�����豸 */

/* �˵��������� */
#define ENDP_INDEX_0       		   0x00							 /* �˵�����0�����ƶ˵� */ 	
#define ENDP_INDEX_1       		   0x01							 /* �˵�����1���Զ���˵� */
#define ENDP_INDEX_2       		   0x02							 /* �˵�����2���Զ���˵� */
#define ENDP_INDEX_3       		   0x03							 /* �˵�����3���Զ���˵� */
#define ENDP_INDEX_4       		   0x04							 /* �˵�����4���Զ���˵� */	

/* �˵㴫�䷽�� */
#define ENDP_DIR_IN		    	   0x00							 /* �ϴ��˵� */
#define ENDP_DIR_OUT			   0x01							 /* �´��˵� */

/* �˵����� */
#define ENDP_TYPE_RESERVED		   0x00							 /* ���� */
#define ENDP_TYPE_ISO			   0x01					 		 /* ͬ������ */
#define ENDP_TYPE_BULK			   0x02							 /* �������� */
#define ENDP_TYPE_INTERRUPT		   0x03							 /* �жϴ��� */


/* ********************************************************************************************************************* */
/* оƬ˯��ģʽ���� */
#ifndef	HALF_SLEEP_MODE
#define	HALF_SLEEP_MODE	   		   0x11							 /* ��˯��״̬,����֮��USB���ܿ��Լ���ʹ��  */
#define	FULL_SLEEP_MODE			   0x22							 /* ���˯��״̬,����֮�������������USB�豸,��Ҫȫ�����³�ʼ�� */
#endif


/* ********************************************************************************************************************* */
/* ���ڷ�ʽ, ״̬�˿�(������˿�)��λ���� */
#ifndef	PARA_STATE_INTB
#define	PARA_STATE_INTB			   0x80							 /* ���ڷ�ʽ״̬�˿ڵ�λ7: �жϱ�־,����Ч */
#define	PARA_STATE_BUSY			   0x10							 /* ���ڷ�ʽ״̬�˿ڵ�λ4: æ��־,����Ч */
#endif


/* ********************************************************************************************************************* */
/* ���ڷ�ʽ, ��������ǰ������ͬ���� */
#ifndef	SER_CMD_TIMEOUT
#define	SER_CMD_TIMEOUT		       32						     /* �������ʱʱ��, ��λΪmS, ͬ����֮�估ͬ������������֮��ļ��Ӧ�þ�����, ��ʱ��Ĵ���ʽΪ���� */
#define	SER_SYNC_CODE1			   0x57							 /* ���������ĵ�1������ͬ���� */
#define	SER_SYNC_CODE2			   0xAB				 	 	     /* ���������ĵ�2������ͬ���� */
#define	SER_TIMEOUT_RET			   0x03FFFA				 	 	 /* ���ڽ��ճ�ʱ���� */
#endif


/* ********************************************************************************************************************* */
/* ����״̬ */
#ifndef	CMD_RET_SUCCESS
#define	CMD_RET_SUCCESS			   0x51						  	 /* ��������ɹ� */
#define	CMD_RET_ABORT			   0x5F						     /* �������ʧ�� */
#define	CMD_PARAME_ERR			   0x5E						     /* ����������� */
#define	CMD_IDENTIFY_ERR		   0x5D						     /* �޸�������� */
#define	CMD_TIMEOUT_ERR		   	   0x5C						     /* ���ʱ���� */
#endif


/* ********************************************************************************************************************* */
/* CH378�ж�״̬ */
#ifndef	USB_INT_WAKE_UP
#define	USB_INT_WAKE_UP		       0xE0						  	 /* CH378�����ж�״̬ */
#endif


/* ********************************************************************************************************************* */
/* USB�ж�״̬ */

/* ����״̬����0XH����USB�豸��ʽ */
/*   ���ù̼�ģʽ��ֻ��Ҫ����: USB_INT_INDEX1_IN, USB_INT_INDEX2_OUT, USB_INT_INDEX3_IN, USB_INT_INDEX4_OUT */
/*   λ7-λ5Ϊ000 */
/*   λ4-λ3ָʾ��ǰ����, 00=OUT, 10=IN, 11=SETUP */
/*   λ2-λ0ָʾ��ǰ�˵�����, 000=0�Ŷ˵�, 001=1�Ŷ˵�, 010=2�Ŷ˵�,011=3�Ŷ˵�,100=4�Ŷ˵�, 101=USB����, 110=USB���߸�λ */
/*   �˵�����Ϊ0ʱ��Ϊ�˵�0(���ƶ˵�), �˵�����Ϊ1-4ʱΪ�����˵�, �˵�š��˵����͡��˵�������С���˵㷽����ڶ˵�����ʱ���� */

#define	USB_INT_EP0_SETUP	       0x18							 /* USB�˵�0��SETUP */
#define	USB_INT_EP0_OUT		       0x00							 /* USB�˵�0��OUT */
#define	USB_INT_EP0_IN		       0x10							 /* USB�˵�0��IN */
#define	USB_INT_INDEX1_OUT	       0x01							 /* USB 1�Ŷ˵��OUT */
#define	USB_INT_INDEX1_IN	       0x11							 /* USB 1�Ŷ˵��IN */
#define	USB_INT_INDEX2_OUT	       0x02							 /* USB 2�Ŷ˵��OUT */
#define	USB_INT_INDEX2_IN	       0x12							 /* USB 2�Ŷ˵��IN */
#define	USB_INT_INDEX3_OUT	       0x03							 /* USB 3�Ŷ˵��OUT */
#define	USB_INT_INDEX3_IN	       0x13							 /* USB 3�Ŷ˵��IN */
#define	USB_INT_INDEX4_OUT	       0x04							 /* USB 4�Ŷ˵��OUT */
#define	USB_INT_INDEX4_IN	       0x14							 /* USB 4�Ŷ˵��IN */
#define	USB_INT_BUS_SUSP	       0x05							 /* USB���߹��� */
#define	USB_INT_BUS_RESET	       0x06							 /* USB���߸�λ */


/* ����״̬����1XH����USB������ʽ�Ĳ���״̬���� */
#ifndef	USB_INT_SUCCESS
#define	USB_INT_SUCCESS			   0x14							 /* USB������ߴ�������ɹ� */
#define	USB_INT_CONNECT			   0x15							 /* ��⵽USB�豸�����¼�, �����������ӻ��߶Ͽ����������� */
#define	USB_INT_DISCONNECT		   0x16						 	 /* ��⵽USB�豸�Ͽ��¼� */
#define	USB_INT_BUF_OVER		   0x17						 	 /* USB��������������������̫�໺������� */
#define	USB_INT_USB_READY		   0x18							 /* USB�豸�Ѿ�����ʼ��(�Ѿ�����USB��ַ) */
#define	USB_INT_DISK_READ		   0x1D							 /* USB�洢���������ݶ��� */
#define	USB_INT_DISK_WRITE		   0x1E							 /* USB�洢����������д�� */
#define	USB_INT_DISK_ERR		   0x1F							 /* USB�洢������ʧ�� */
#endif


/* ����״̬�������������ļ�ģʽ�µ��ļ�ϵͳ������ */
#ifndef ERR_SUCCESS
#define ERR_SUCCESS				   0x00			 				 /* �����ɹ� */
#endif
#define	ERR_PARAMETER_ERROR		   0x03							 /* ��������, ��Χ��� */
#ifndef	ERR_DISK_DISCON
#define	ERR_DISK_DISCON			   0x82							 /* ������δ����,���ܴ����Ѿ��Ͽ� */
#define	ERR_LARGE_SECTOR	       0x84							 /* ���̵�����̫��,ֻ֧��ÿ����512�ֽ� */
#define	ERR_TYPE_ERROR		       0x92							 /* ���̷������Ͳ�֧��,ֻ֧��FAT12/FAT16/BigDOS/FAT32,��Ҫ�ɴ��̹��������·��� */
#define	ERR_BPB_ERROR			   0xA1						 	 /* ������δ��ʽ��,���߲�������,��Ҫ��WINDOWS����Ĭ�ϲ������¸�ʽ�� */
#define	ERR_DISK_FULL			   0xB1							 /* �����ļ�̫��,ʣ��ռ�̫�ٻ����Ѿ�û��,��Ҫ�������� */
#define	ERR_FDT_OVER			   0xB2							 /* Ŀ¼(�ļ���)���ļ�̫��,û�п��е�Ŀ¼��,FAT12/FAT16��Ŀ¼�µ��ļ���Ӧ������512��,��Ҫ�������� */
#define	ERR_FILE_CLOSE			   0xB4							 /* �ļ��Ѿ��ر�,�����Ҫʹ��,Ӧ�����´��ļ� */
#define	ERR_OPEN_DIR			   0x41							 /* ָ��·����Ŀ¼(�ļ���)���� */
#define	ERR_MISS_FILE			   0x42							 /* ָ��·�����ļ�û���ҵ�,�������ļ����ƴ��� */
#define	ERR_FOUND_NAME			   0x43							 /* ��������ƥ����ļ���,������Ҫ���Ŀ¼(�ļ���)��ʵ�ʽ��ȴ�����ļ� */

/* ���³��ļ������������ļ�ϵͳ�ӳ��� */
#define ERR_NO_NAME                0X44        					 /* �˶��ļ���û�г��ļ��������ĳ��ļ� */
#define ERR_BUF_OVER               0X45        					 /* ���ļ���������� */
#define ERR_LONG_NAME              0X46        					 /* ����ĳ��ļ��� */

/* �����ļ�ϵͳ�����������ļ�ϵͳ�ӳ��� */
#define	ERR_MISS_DIR			   0xB3							 /* ָ��·����ĳ����Ŀ¼(�ļ���)û���ҵ�,������Ŀ¼���ƴ��� */
#define	ERR_LONG_BUF_OVER		   0x48							 /* ���ļ���������� */
#define	ERR_LONG_NAME_ERR		   0x49							 /* ���ļ���û�ж�Ӧ�ĳ��ļ������߳��ļ������� */
#define	ERR_NAME_EXIST			   0x4A						 	 /* ͬ���Ķ��ļ��Ѿ�����,����������������һ�����ļ��� */
#endif

/* ********************************************************************************************************************* */
/* ����ΪSD���Ͳ������ش����� */
#define SD_ERR_NO_CARD		 	   0x60					 		 /* ��û����ȫ���뵽������ */
#define SD_ERR_USER_PARAM      	   0x61					 		 /* �û�ʹ��API����ʱ����ڲ����д��� */
#define SD_ERR_CARD_PARAM		   0x62					 		 /* ���в����д����뱾ģ�鲻���ݣ� */
#define	SD_ERR_VOL_NOTSUSP         0x63					 		 /* ����֧��3.3V���� */
#define SD_ERR_OVER_CARDRANGE	   0x64					 		 /* ����������������Χ */
#define SD_ERR_UNKNOWN_CARD        0x65					 		 /* �޷�ʶ���� */
#define SD_ERR_WP_PROTECT          0x66            		 		 /* д���� */
#define SD_CARD_BUSY               0x67            		 		 /* �ڶ�ȡOCR�Ĵ���ʱ��⵽BIT31Ϊ0��ʾæ */
#define SD_BLOCK_ERROR             0x68            		 		 /* ���Сһ��Ҫ���ڵ���512,�����޷����� */

/* SD������ܷ��صĴ����� */
#define SD_ERR_CMD_RESPTYPE	 	   0x69					 		 /* �������ʹ��� */
#define SD_ERR_CMD_TIMEOUT     	   0x6A					 		 /* ��������Ӧ��ʱ */
#define SD_ERR_CMD_RESP		 	   0x6B					 		 /* ��������Ӧ���� */
#define SD_Power_Up_Busy           0x6C					 		 /* �ϵ�æ */

/* ������������ */
#define SD_ERR_DATA_CRC16      	   0x6D							 /* ������CRC16У�鲻ͨ�� */
#define SD_ERR_DATA_START_TOK	   0x6E					 		 /* ���������ʱ,���ݿ�ʼ���Ʋ���ȷ */
#define	SD_ERR_DATA_RESP		   0x6F					 		 /* д�������ʱ,��������Ӧ���Ʋ���ȷ */

/* �ȴ������� */
#define SD_ERR_TIMEOUT_WAIT    	   0x70					 		 /* д�������ʱ,������ʱ���� */
#define SD_ERR_TIMEOUT_READ    	   0x71					 		 /* ��������ʱ���� */
#define	SD_ERR_TIMEOUT_WRITE	   0x72					 		 /* д������ʱ���� */
#define SD_ERR_TIMEOUT_ERASE   	   0x73					 		 /* ����������ʱ���� */
#define	SD_ERR_TIMEOUT_WAITIDLE    0x74					 		 /* ��ʼ����ʱ,�ȴ����˳�����״̬��ʱ���� */

/* д�������ܷ��صĴ����� */
#define	SD_ERR_WRITE_BLK		   0x75					 		 /* д�����ݴ��� */
#define	SD_ERR_WRITE_BLKNUMS       0x76				 	 		 /* д���ʱ,��Ҫд��Ŀ�����ȷд��Ŀ�����һ�� */
#define SD_ERR_WRITE_PROTECT	   0x77					 		 /* ����ǵ�д�������ش���д����λ�� */


/* ********************************************************************************************************************* */
/* ����״̬�������������ļ�ģʽ�µĴ��̼��ļ�״̬, VAR_DISK_STATUS */
#ifndef	DEF_DISK_UNKNOWN
#define	DEF_DISK_UNKNOWN		   0x00							 /* ��δ��ʼ��,δ֪״̬ */
#define	DEF_DISK_DISCONN		   0x01							 /* ����û�����ӻ����Ѿ��Ͽ� */
#define	DEF_DISK_CONNECT		   0x02							 /* �����Ѿ�����,������δ��ʼ�������޷�ʶ��ô��� */
#define	DEF_DISK_MOUNTED		   0x03							 /* �����Ѿ���ʼ���ɹ�,������δ�����ļ�ϵͳ�����ļ�ϵͳ��֧�� */
#define	DEF_DISK_READY			   0x10						 	 /* �Ѿ��������̵��ļ�ϵͳ�����ܹ�֧�� */
#define	DEF_DISK_OPEN_ROOT		   0x12							 /* �Ѿ��򿪸�Ŀ¼,ʹ�ú����ر�,ע��FAT12/FAT16��Ŀ¼�ǹ̶����� */
#define	DEF_DISK_OPEN_DIR		   0x13							 /* �Ѿ�����Ŀ¼(�ļ���) */
#define	DEF_DISK_OPEN_FILE		   0x14							 /* �Ѿ����ļ� */
#endif


/* ********************************************************************************************************************* */
/* �ļ�ϵͳ���ö��� */
#ifndef	DEF_SECTOR_SIZE
#define	DEF_SECTOR_SIZE		       512							 /* U�̻���SD��Ĭ�ϵ����������Ĵ�С */
#endif

#ifndef	DEF_WILDCARD_CHAR
#define	DEF_WILDCARD_CHAR	       0x2A							 /* ·������ͨ��� '*' */
#define	DEF_SEPAR_CHAR1			   0x5C							 /* ·�����ķָ��� '\' */
#define	DEF_SEPAR_CHAR2			   0x2F							 /* ·�����ķָ��� '/' */
#define	DEF_FILE_YEAR			   2004							 /* Ĭ���ļ�����: 2004�� */
#define	DEF_FILE_MONTH			   1							 /* Ĭ���ļ�����: 1�� */
#define	DEF_FILE_DATE			   1							 /* Ĭ���ļ�����: 1�� */
#endif

#ifndef	ATTR_DIRECTORY

/* FAT���������ļ�Ŀ¼��Ϣ */
typedef struct _FAT_DIR_INFO 
{
	UINT8	DIR_Name[11];										 /* 00H,�ļ���,��11�ֽ�,���㴦��ո� */
	UINT8	DIR_Attr;											 /* 0BH,�ļ�����,�ο������˵�� */
	UINT8	DIR_NTRes;											 /* 0CH */
	UINT8	DIR_CrtTimeTenth;									 /* 0DH,�ļ�������ʱ��,��0.1�뵥λ���� */
	UINT16	DIR_CrtTime;										 /* 0EH,�ļ�������ʱ�� */
	UINT16	DIR_CrtDate;										 /* 10H,�ļ����������� */
	UINT16	DIR_LstAccDate;										 /* 12H,���һ�δ�ȡ���������� */
	UINT16	DIR_FstClusHI;										 /* 14H */
	UINT16	DIR_WrtTime;										 /* 16H,�ļ��޸�ʱ��,�ο�ǰ��ĺ�MAKE_FILE_TIME */
	UINT16	DIR_WrtDate;										 /* 18H,�ļ��޸�����,�ο�ǰ��ĺ�MAKE_FILE_DATE */
	UINT16	DIR_FstClusLO;										 /* 1AH */
	UINT32	DIR_FileSize;										 /* 1CH,�ļ����� */
} FAT_DIR_INFO, *P_FAT_DIR_INFO;								 /* 20H */


/* �ļ����� */
#define ATTR_READ_ONLY			   0x01							 /* �ļ�Ϊֻ������ */
#define ATTR_HIDDEN				   0x02					 	  	 /* �ļ�Ϊ�������� */
#define ATTR_SYSTEM				   0x04							 /* �ļ�Ϊϵͳ���� */
#define ATTR_VOLUME_ID			   0x08							 /* ��� */
#define ATTR_DIRECTORY			   0x10							 /* ��Ŀ¼(�ļ���) */
#define ATTR_ARCHIVE			   0x20							 /* �ļ�Ϊ�浵���� */
#define ATTR_LONG_NAME			   ( ATTR_READ_ONLY | ATTR_HIDDEN | ATTR_SYSTEM | ATTR_VOLUME_ID )	/* ���ļ������� */
#define ATTR_LONG_NAME_MASK		   ( ATTR_LONG_NAME | ATTR_DIRECTORY | ATTR_ARCHIVE )
/* �ļ����� UINT8 */
/* bit0 bit1 bit2 bit3 bit4 bit5 bit6 bit7 */
/*  ֻ   ��   ϵ   ��   Ŀ   ��   δ����   */
/*  ��   ��   ͳ   ��   ¼   ��            */

/* �ļ�ʱ�� UINT16 */
/* Time = (Hour<<11) + (Minute<<5) + (Second>>1) */
#define MAKE_FILE_TIME( h, m, s )  ( (h<<11) + (m<<5) + (s>>1) ) /* ����ָ��ʱ������ļ�ʱ������ */

/* �ļ����� UINT16 */
/* Date = ((Year-1980)<<9) + (Month<<5) + Day */
#define MAKE_FILE_DATE( y, m, d )  ( ((y-1980)<<9) + (m<<5) + d )/* ����ָ�������յ��ļ��������� */

#define LONE_NAME_MAX_CHAR		   (255*2)						 /* ���ļ�������ַ���/�ֽ��� */
#define LONG_NAME_PER_DIR		   (13*2)						 /* ���ļ�����ÿ���ļ�Ŀ¼��Ϣ�ṹ�е��ַ���/�ֽ��� */

#endif

/* ********************************************************************************************************************* */
/* SCSI�����������������ṹ */

#ifndef	SPC_CMD_INQUIRY12

/* SCSI������ */
#define SPC_CMD_INQUIRY			   0x12
#define SPC_CMD_READ_CAPACITY	   0x25
#define SPC_CMD_READ10			   0x28
#define SPC_CMD_WRITE10			   0x2A
#define SPC_CMD_TEST_READY		   0x00
#define SPC_CMD_REQUEST_SENSE	   0x03
#define SPC_CMD_MODESENSE6		   0x1A
#define SPC_CMD_MODESENSE10		   0x5A
#define SPC_CMD_START_STOP		   0x1B

/* BulkOnlyЭ�������� */
typedef struct _BULK_ONLY_CBW 
{
	UINT32	CBW_Sig;
	UINT32	CBW_Tag;
	UINT8	CBW_DataLen0;										 /* 08H,����: ���ݴ��䳤��,����������������Чֵ��0��48,���������������ЧֵΪ0��33 */
	UINT8	CBW_DataLen1;
	UINT16	CBW_DataLen2;
	UINT8	CBW_Flag;											 /* 0CH,����: ���䷽��ȱ�־,λ7Ϊ1����������,λΪ0��������ݻ���û������ */
	UINT8	CBW_LUN;
	UINT8	CBW_CB_Len;											 /* 0EH,����: �����ĳ���,��Чֵ��1��16 */
	UINT8	CBW_CB_Buf[16];										 /* 0FH,����: �����,�û��������Ϊ16���ֽ� */
} BULK_ONLY_CBW, *P_BULK_ONLY_CBW;								 /* BulkOnlyЭ��������, ����CBW�ṹ */

/* INQUIRY����ķ������� */
typedef struct _INQUIRY_DATA 
{
	UINT8	DeviceType;											 /* 00H, �豸���� */
	UINT8	RemovableMedia;										 /* 01H, λ7Ϊ1˵�����ƶ��洢 */
	UINT8	Versions;											 /* 02H, Э��汾 */
	UINT8	DataFormatAndEtc;									 /* 03H, ָ���������ݸ�ʽ */
	UINT8	AdditionalLength;									 /* 04H, �������ݵĳ��� */
	UINT8	Reserved1;
	UINT8	Reserved2;
	UINT8	MiscFlag;											 /* 07H, һЩ���Ʊ�־ */
	UINT8	VendorIdStr[8];										 /* 08H, ������Ϣ */
	UINT8	ProductIdStr[16];									 /* 10H, ��Ʒ��Ϣ */
	UINT8	ProductRevStr[4];									 /* 20H, ��Ʒ�汾 */
} INQUIRY_DATA, *P_INQUIRY_DATA;								 /* 24H */

/* REQUEST SENSE����ķ������� */
typedef struct _SENSE_DATA 
{
	UINT8	ErrorCode;											 /* 00H, ������뼰��Чλ */
	UINT8	SegmentNumber;
	UINT8	SenseKeyAndEtc;										 /* 02H, ������ */
	UINT8	Information0;
	UINT8	Information1;
	UINT8	Information2;
	UINT8	Information3;
	UINT8	AdditSenseLen;										 /* 07H, �������ݵĳ��� */
	UINT8	CmdSpecInfo[4];
	UINT8	AdditSenseCode;										 /* 0CH, ���Ӽ��� */
	UINT8	AddSenCodeQual;										 /* 0DH, ��ϸ�ĸ��Ӽ��� */
	UINT8	FieldReplaUnit;
	UINT8	SenseKeySpec[3];
} SENSE_DATA, *P_SENSE_DATA;									 /* 12H */

#endif

/* ********************************************************************************************************************* */
/* �����ļ�ģʽ�µ��������������ṹ */

#ifndef	MAX_FILE_NAME_LEN
#define MAX_PATH_LEN			128								 /* ���·������,������б�ָܷ�����С���������Լ�·��������00H */
#define MAX_FILE_NAME_LEN		128								 /* ���·������,������б�ָܷ�����С���������Լ�·��������00H */

/* ������������ݺ�������� */
typedef union _CH378_CMD_DATA 
{
	struct 
	{
		UINT8  mBuffer[ MAX_PATH_LEN ];
	} Default;
	
	struct 
	{
		UINT32 mTotalSector;									 /* ����: ��ǰ�߼��̵���������(�ܳ���32λ,���ֽ���ǰ) */
		UINT32 mFreeSector;										 /* ����: ��ǰ�߼��̵�ʣ��������(�ܳ���32λ,���ֽ���ǰ) */
		UINT8  mDiskFat;										 /* ����: ��ǰ�߼��̵�FAT����,1-FAT12,2-FAT16,3-FAT32 */
	} DiskQuery;												 /* CMD0H_DISK_QUERY, ��ѯ������Ϣ */

	struct 
	{
		UINT32 mDiskSizeSec;									 /* ����: ����������̵���������(�ܳ���32λ,���ֽ���ǰ) */
	} DiskCapacity;												 /* CMD0H_DISK_CAPACITY: ��ѯ������������ */

	struct 
	{
		UINT8  mPathName[ MAX_PATH_LEN ];						 /* �������: ·��: [�̷�,ð��,б��,Ŀ¼�������ļ�������չ��...,������00H], �����̷���ð�ſ���ʡ��, ����"C:\DIR1.EXT\DIR2\FILENAME.EXT",00H */
	} Open;														 /* CMD0H_FILE_OPEN, ���ļ� */

	struct 
	{
		UINT8  mUpdateFileSz;									 /* �������: �Ƿ���������ļ�����, 0���ֹ���³��� */
	} CLose;												 	 /* CMD1H_FILE_CLOSE: �رյ�ǰ�Ѿ��򿪵��ļ� */

	struct 
	{
		UINT8  mPathName[ MAX_PATH_LEN ];						 /* �������: ·��: [�̷�,ð��,б��,Ŀ¼�������ļ�������չ��...,������00H], �����̷���ð�ſ���ʡ��, ����"C:\DIR1.EXT\DIR2\FILENAME.EXT",00H */
	} Create;													 /* CMD0H_FILE_CREATE, �½��ļ�����,����ļ��Ѿ���������ɾ�������½� */

	struct 
	{
		UINT8  mPathName[ MAX_PATH_LEN ];						 /* �������: ·��: [�̷�,ð��,б��,Ŀ¼�������ļ�������չ��...,������00H], �����̷���ð�ſ���ʡ��, ����"C:\DIR1.EXT\DIR2\FILENAME.EXT",00H */
	} Erase;													 /* CMD0H_FILE_ERASE, ɾ���ļ����ر� */

	struct 
	{
		UINT32 mFileSize;										 /* �������: �µ��ļ�����,Ϊ0FFFFFFFFH���޸�, ����: ԭ���� */
		UINT16 mFileDate;										 /* �������: �µ��ļ�����,Ϊ0FFFFH���޸�, ����: ԭ���� */
		UINT16 mFileTime;										 /* �������: �µ��ļ�ʱ��,Ϊ0FFFFH���޸�, ����: ԭʱ�� */
		UINT8  mFileAttr;										 /* �������: �µ��ļ�����,Ϊ0FFH���޸�, ����: ԭ���� */
	} Modify;													 /* CMD0H_FILE_QUERY, ��ѯ��ǰ�ļ�����Ϣ; CMD0H_FILE_MODIFY, ��ѯ�����޸ĵ�ǰ�ļ�����Ϣ */

	union 
	{
		UINT32 mSectorOffset;									 /* �������: ƫ��������,������Ϊ��λ��ƫ����(�ܳ���32λ,���ֽ���ǰ) */
		UINT32 mSectorLba;										 /* ����: ��ǰ�ļ�ָ���Ӧ�ľ�������������,0FFFFFFFFH���ѵ��ļ�β(�ܳ���32λ,���ֽ���ǰ) */
	} SectorLocate;												 /* CMD4H_SEC_LOCATE: ������Ϊ��λ�ƶ���ǰ�ļ�ָ�� */

	struct 
	{
		UINT8  mSectorCount;									 /* �������: �����ȡ��������(����: �����ȡ��������) */
		UINT8  mReserved1;
		UINT8  mLbaCount;
		UINT8  mReserved2;
		PUINT8 mDataBuffer;										 /* ����: ��������ǰ��ַ */
	} SectorRead;												 /* CMD1H_SEC_READ: ������Ϊ��λ�ӵ�ǰλ�ö�ȡ���ݿ� */

	struct 
	{
		UINT8  mSectorCount;									 /* �������: ����д���������(����: ����д���������) */																 /*  */
		UINT8  mReserved1;
		UINT8  mLbaCount;
		UINT8  mReserved2;
		PUINT8 mDataBuffer;										 /* ����: ��������ǰ��ַ */
	} SectorWrite;												 /* CMD1H_SEC_WRITE: ������Ϊ��λ�ڵ�ǰλ��д�����ݿ� */

	union 
	{
		UINT32 mByteOffset;										 /* �������: ƫ���ֽ���,���ֽ�Ϊ��λ��ƫ����(�ܳ���32λ,���ֽ���ǰ) */
		UINT32 mSectorLba;										 /* ����: ��ǰ�ļ�ָ���Ӧ�ľ�������������,0FFFFFFFFH���ѵ��ļ�β(�ܳ���32λ,���ֽ���ǰ) */
	} ByteLocate;												 /* CMD4H_BYTE_LOCATE: ���ֽ�Ϊ��λ�ƶ���ǰ�ļ�ָ�� */

	struct 
	{
		UINT16 mByteCount;										 /* �������: �����ȡ���ֽ���(�ܳ���16λ,���ֽ���ǰ) */
	} ByteRead;													 /* CMD2H_BYTE_READ: ���ֽ�Ϊ��λ�ӵ�ǰλ�ö�ȡ���ݿ� */

	struct 
	{
		UINT16 mByteCount;										 /* �������: ����д����ֽ���(�ܳ���16λ,���ֽ���ǰ) */
	} ByteWrite;												 /* CMD2H_BYTE_WRITE: ���ֽ�Ϊ��λ��ǰλ��д�����ݿ� */
	
	struct 
	{
		UINT32 mStartSector;									 /* �������: LBA������ַ(�ܳ���32λ,���ֽ���ǰ) */
		UINT8  mSectorCount;									 /* �������: �����ȡ�������� */
	} DiskRead;													 /* CMD5H_DISK_READ: ��USB�洢�������ݿ�(������Ϊ��λ) */

	struct 
	{
		UINT32 mStartSector;									 /* �������: LBA������ַ(�ܳ���32λ,���ֽ���ǰ) */
		UINT8  mSectorCount;									 /* �������: ����д��������� */
	} DiskWrite;												 /* CMD5H_DISK_WRITE: ��USB�洢��д���ݿ�(������Ϊ��λ) */

	INQUIRY_DATA DiskMountInq;									 /* ����: INQUIRY����ķ������� */
																 /* CMD0H_DISK_MOUNT: ��ʼ�����̲����Դ����Ƿ����,�״�ִ��ʱ */

	FAT_DIR_INFO OpenDirInfo;									 /* ����: ö�ٵ����ļ�Ŀ¼��Ϣ */
																 /* CMD0H_FILE_OPEN: ö���ļ���Ŀ¼(�ļ���) */

	FAT_DIR_INFO EnumDirInfo;									 /* ����: ö�ٵ����ļ�Ŀ¼��Ϣ */
																 /* CMD0H_FILE_ENUM_GO: ����ö���ļ���Ŀ¼(�ļ���) */

	BULK_ONLY_CBW DiskBocCbw;									 /* �������: CBW����ṹ */
																 /* CMD0H_DISK_BOC_CMD: ��USB�洢��ִ��BulkOnly����Э������� */
	struct 
	{
		UINT8  mMaxLogicUnit;									 /* ����: USB�洢��������߼���Ԫ�� */
	} DiskMaxLun;												 /* CMD0H_DISK_MAX_LUN: ���ƴ���-��ȡUSB�洢������߼���Ԫ�� */

	INQUIRY_DATA DiskInitInq;									 /* ����: INQUIRY����ķ������� */
																 /* CMD0H_DISK_INIT: ��ʼ��USB�洢�� */

	INQUIRY_DATA DiskInqData;									 /* ����: INQUIRY����ķ������� */
																 /* CMD0H_DISK_INQUIRY: ��ѯUSB�洢������ */

	SENSE_DATA ReqSenseData;									 /* ����: REQUEST SENSE����ķ������� */
																 /* CMD0H_DISK_R_SENSE: ���USB�洢������ */
}CH378_CMD_DATA, *P_CH378_CMD_DATA;

#endif

/* ********************************************************************************************************************* */
/* �����ļ�ģʽ�µ��ļ�ϵͳ�����ĵ�ַ */

#ifndef	VAR_FILE_SIZE

/* 8λ/���ֽڱ��� */
#define	VAR8_DEV_CONNECTSTATUS	   0x02                          /* ��ǰ�豸����״̬: 0--δ����, 1--����δ��ʼ��, 2--��ʼ���ɹ� */
#define	VAR8_USB_DEV_SPEED	       0x03                          /* ��ǰ���ӵ�USB�豸�ٶ�: 0--δ֪, 1--����, 2--ȫ��, 3--���� */ 	
#define	VAR8_CMD_INT_STATUS		   0x0B							 /* CH378��ǰ����ִ���ж�״̬(��Բ����жϵ�����) */
#define	VAR8_LAST_CMD     		   0x0C							 /* CH378���յ������һ�������� */
#define	VAR8_CMD_OP_STATUS		   0x0E							 /* CH378��ǰ�����ִ��״̬ */
#define	VAR8_SDO_INT_ENABLE        0x16							 /* SPIģʽ�Ƿ�ʹ��SDO�����ж�֪ͨ��־(0:��ֹ, 1:ʹ��) */
#define	VAR8_DISK_STATUS		   0x21							 /* �����ļ�ģʽ�µĴ��̼��ļ�״̬ */
#define	VAR8_DISK_FAT			   0x22							 /* �߼��̵�FAT��־:1=FAT12,2=FAT16,3=FAT32 */
#define	VAR8_SEC_PER_CLUS		   0x23							 /* �߼��̵�ÿ�������� */
#define	VAR8_DISK_SEC_LEN		   0x24							 /* �߼��̵�ÿ��������С(һ��Ϊ512�ֽ�,������U�̿���Ϊ1024,2048,4096��) */
																 /* 9 = 512, 10 = 1024, 11 = 2048, 12 = 4096 */
/* 32λ/4�ֽڱ��� */
#define	VAR32_DISK_CAP			   0x51							 /* �߼��̵������� */
#define	VAR32_DISK_SEC_SIZE	   	   0x52							 /* �߼��̵�ÿ��������С(һ��Ϊ512�ֽ�,������U�̿���Ϊ1024,2048,4096��) */
#define	VAR32_SYSTEM_PARA	   	   0x55							 /* CH378ϵͳ���� */
#define  RB_WORK_MODE_SET0   	   ( 1 << 0 )					 /* λ1--0: ��ǰоƬ�ӿ�ģʽ( 00:δ֪; 01:8λ����; 02:SPI�ӿ�; 03:�첽���� ) */ 	
#define  RB_WORK_MODE_SET1   	   ( 1 << 1 )
#define  RB_AUTO_CHECK_CONNECT	   ( 1 << 2 )					 /* λ2��CH378�Զ����USB�豸/SD�������ӺͶϿ�ʹ�� */
#define  RB_HOST_SD_UDISK	       ( 1 << 3 )			 		 /* λ3������ģʽ��0---USB�洢�豸��1---SD�� */
#define  RB_USB_CONFIG	    	   ( 1 << 4 ) 					 /* λ4��������ʽ��USB�豸���ӱ�־ */

#define	VAR32_FILE_SIZE			   0x68							 /* ��ǰ�ļ��ĳ���(�ܳ���32λ,���ֽ���ǰ) */
#define	VAR32_DSK_START_LBA		   0x70							 /* �߼��̵���ʼ����������LBA(�ܳ���32λ,���ֽ���ǰ) */
#define	VAR32_DISK_ROOT			   0x71							 /* ����FAT16��Ϊ��Ŀ¼ռ��������,����FAT32��Ϊ��Ŀ¼��ʼ�غ�(�ܳ���32λ,���ֽ���ǰ) */
#define	VAR32_DSK_DAT_START		   0x72							 /* �߼��̵������������ʼLBA(�ܳ���32λ,���ֽ���ǰ) */
#define	VAR32_START_CLUSTER		   0x73							 /* ��ǰ�ļ�����Ŀ¼(�ļ���)����ʼ�غ�(�ܳ���32λ,���ֽ���ǰ) */
#define	VAR32_CURRENT_OFFSET	   0x74							 /* ��ǰ�ļ�ָ��,��ǰ��дλ�õ��ֽ�ƫ��(�ܳ���32λ,���ֽ���ǰ) */
#define	VAR_FAT_DIR_LBA			   0x75							 /* ��ǰ�ļ�Ŀ¼��Ϣ���ڵ�����LBA��ַ(�ܳ���32λ,���ֽ���ǰ) */
#define	VAR_FAT_OFFSET			   0x76							 /* ��ǰ�ļ�Ŀ¼��Ϣ��������ƫ�Ƶ�ַ(�ܳ���32λ,���ֽ���ǰ) */

#endif

/* ********************************************************************************************************************* */
/* ����USB���� */

/* USB�İ���ʶPID, ������ʽ�����õ� */
#ifndef	DEF_USB_PID_SETUP
#define	DEF_USB_PID_NULL		   0x00							 /* ����PID, δ���� */
#define	DEF_USB_PID_SOF		       0x05
#define	DEF_USB_PID_SETUP	       0x0D
#define	DEF_USB_PID_IN		       0x09
#define	DEF_USB_PID_OUT		  	   0x01
#define	DEF_USB_PID_ACK			   0x02
#define	DEF_USB_PID_NAK			   0x0A
#define	DEF_USB_PID_STALL		   0x0E
#define	DEF_USB_PID_DATA0		   0x03
#define	DEF_USB_PID_DATA1		   0x0B
#define	DEF_USB_PID_PRE			   0x0C
#endif

/* USB��������, ���ù̼�ģʽ�����õ� */
#ifndef	DEF_USB_REQ_TYPE
#define	DEF_USB_REQ_READ		   0x80							 /* ���ƶ����� */
#define	DEF_USB_REQ_WRITE		   0x00						  	 /* ����д���� */
#define	DEF_USB_REQ_TYPE		   0x60							 /* ������������ */
#define	DEF_USB_REQ_STAND		   0x00							 /* ��׼���� */
#define	DEF_USB_REQ_CLASS		   0x20						 	 /* �豸������ */
#define	DEF_USB_REQ_VENDOR		   0x40							 /* �������� */
#define	DEF_USB_REQ_RESERVE		   0x60							 /* �������� */
#endif

/* USB��׼�豸����, RequestType��λ6λ5=00(Standard), ���ù̼�ģʽ�����õ� */
#ifndef	DEF_USB_GET_DESCR
#define	DEF_USB_CLR_FEATURE	 	   0x01
#define	DEF_USB_SET_FEATURE		   0x03
#define	DEF_USB_GET_STATUS		   0x00
#define	DEF_USB_SET_ADDRESS		   0x05
#define	DEF_USB_GET_DESCR		   0x06
#define	DEF_USB_SET_DESCR		   0x07
#define	DEF_USB_GET_CONFIG		   0x08
#define	DEF_USB_SET_CONFIG		   0x09
#define	DEF_USB_GET_INTERF		   0x0A
#define	DEF_USB_SET_INTERF		   0x0B
#define	DEF_USB_SYNC_FRAME		   0x0C
#endif

/* ********************************************************************************************************************* */

#ifdef __cplusplus
}
#endif

#endif
