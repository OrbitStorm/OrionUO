/***********************************************************************************
**
** GLFrameBuffer.h
**
** Copyright (C) August 2016 Hotride
**
************************************************************************************
*/
//----------------------------------------------------------------------------------
#ifndef GLFRAMEBUFFER_H
#define GLFRAMEBUFFER_H
//----------------------------------------------------------------------------------
#include "../Globals.h"
//----------------------------------------------------------------------------------
//!����� ��� ������ � ������������
class CGLFrameBuffer
{
	//!�������� ������
	SETGET(int, Width);
	SETGET(int, Height);

private:
	//!������ ����� � ����� � ������������� ��� ���
	bool m_Ready;

	//!��������� �� ���������� ����������
	GLint m_OldFrameBuffer;

	//!��������� �� ����������
	GLuint m_FrameBuffer;

	//!��������� �� �������� �����������
	GLuint m_Texture;

public:
	CGLFrameBuffer();
	~CGLFrameBuffer();

	/*!
	�������������� ������
	@param [__in] width ������ ������
	@param [__in] height ������ ������
	@return true � ������ ������
	*/
	bool Init(int width, int height);

	/*!
	������� �����������
	@return 
	*/
	void Free();

	/*!
	���������� ������������� �����������
	@return 
	*/
	void Release();

	/*!
	����� ��� ��� �����
	@return true � ������ ����������
	*/
	bool Ready() const { return m_Ready; }

	/*!
	�������� ���������� ������ � ������������� �������������
	@param [__in] width ������ ������
	@param [__in] height ������ ������
	@return true � ������ ����������
	*/
	bool Ready(int &width, int &height);

	/*!
	������������� ������
	@return true � ������ ������
	*/
	bool Use();

	/*!
	���������� �������� ������
	@param [__in] x �������� ���������� X
	@param [__in] y �������� ���������� Y
	@return 
	*/
	void Draw(int x, int y);
};
//----------------------------------------------------------------------------------
#endif
