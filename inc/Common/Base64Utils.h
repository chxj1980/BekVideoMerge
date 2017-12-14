#pragma once

#ifndef __BASE64_UTILS_H__
#define __BASE64_UTILS_H__

#include "DllExport.h"

/**
* @class CBase64Utils
* @brief Base64编解码静态类
*
* 用于Base64编解码，根据输入的string,wtring的base64编码文本，转换为对应的二进制内容。将输入的二进制编码，	\
* 转换为string,wstring的base64编码文本 
*/

class CBase64Utils
{
public:
	/** @brief 将二进制流编码为base64
	*	@param[in]	indata		指定二进制流
	*	@param[in]	indatalen	指定二进制流长度
	*	@param[out] outbase64	输出base64编码
	*	@return  返回操作结果
	*	 - false 转换失败
	*	 - true	 转换成功
	*/
	static DLL_COMMONLIB_API bool WINAPI Encode(const void *indata, int indatalen, std::wstring &outbase64);

	/** @brief 将二进制流编码为base64
	*	@param[in]	indata		指定二进制流
	*	@param[in]	indatalen	指定二进制流长度
	*	@param[out] outbase64	输出base64编码
	*	@return  返回操作结果
	*	 - false 转换失败
	*	 - true	 转换成功
	*/
	static DLL_COMMONLIB_API bool WINAPI Encode(const void *indata, int indatalen, std::string &outbase64);

	/** @brief 将二进制流编码为base64
	*	@param[in]	inbase64str		指定二进制流
	*	@param[in,out]	outlen		指定二进制流长度
	*	@param[out] outdata			输出base64编码
	*	@return  返回操作结果
	*	 - false 转换失败，输出数据所开的空间大小太小
	*	 - true	 表示成功
	*	@note 当outlen 的大小小于 getCipherLen(instr.size())时，函数会修改outlen的长度。表示需要申请的输出的空间大小。
	*	@par 示例
	*	@code
	*		int len = 0;
	*		Decode(instr,len,NULL);//len = getCipherLen(instr.size());
	*		char *outdata = new char[len];
	*		Decode(instr,len,outdata);
	*	 @endcode
	*/
	static DLL_COMMONLIB_API bool WINAPI Decode(const std::wstring &inbase64str, int &outlen, void *outdata);


	/** @brief 将二进制base64
	*	@param[in]	inbase64str		指定二进制流
	*	@param[in,out]	outlen		指定二进制流长度
	*	@param[out] outdata			输出base64编码
	*	@return  返回操作结果
	*	 - false 转换失败，输出数据所开的空间大小太小
	*	 - true	 表示成功
	*	@note 当outlen 的大小小于 getCipherLen(instr.size())时，函数会修改outlen的长度。表示需要申请的输出的空间大小。
	*	@par 示例
	*	@code
	*		int len = 0;
	*		Decode(instr,len,NULL);//len = getCipherLen(instr.size());
	*		char *outdata = new char[len];
	*		Decode(instr,len,outdata);
	*	 @endcode
	*/
	static DLL_COMMONLIB_API bool WINAPI Decode(const std::string &inbase64str, int &outlen, void *outdata);



	/** @brief 预测经过base64编码后的数据长度
	*	@param[in]	len	待编码的数据的长度
	*	@return  返回预测的编码后的数据长度
	*/
	static DLL_COMMONLIB_API int WINAPI getCipherLen(int len);


	/** @brief 预测经过base64解码后的数据长度
	*	@param[in]	len	待解码的数据的长度
	*	@return  返回预测的解码后的数据长度
	*/
	static DLL_COMMONLIB_API int WINAPI getPlainLen(int len);
};


#endif