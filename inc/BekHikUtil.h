#pragma once

#ifndef __BEK_HIK_UTIL_H__
#define __BEK_HIK_UTIL_H__



#ifndef DLL_HIKUTIL_API
#define DLL_HIKUTIL_API __declspec(dllexport)
#endif


/**
* @class CBekHikUtil
* @brief �����豸������װ��
*
* ���ڲ��������豸������������������NVR��DVR �ȣ�
*/

class CBekHikUtil
{
public:
	
	/** @brief ���� sdk ��ʼ��
	*	@return  ���ز������
	*	 - false ��ʼ��ʧ��
	*	 - true	��ʼ���ɹ�
	*/
	static DLL_HIKUTIL_API bool WINAPI InitSDK();

	/** @brief ��¼�����豸
	*	@param[in]	ip		�豸IP
	*	@param[in]	username 	�û���
	*	@param[in]	password 	����
	*	@param[in]	port	 	�˿�
	*	@param[out] userId	�����¼id
	*	@return  ���ز������
	*	 - false ��¼ʧ��
	*	 - true	��¼�ɹ�
	*/
	static DLL_HIKUTIL_API bool WINAPI LoginDevice(wstring ip, wstring username, wstring password, int port, int &userId);
};


#endif