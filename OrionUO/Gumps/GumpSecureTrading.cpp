/***********************************************************************************
**
** GumpSecureTrading.cpp
**
** Copyright (C) August 2016 Hotride
**
************************************************************************************
*/
//----------------------------------------------------------------------------------
#include "GumpSecureTrading.h"
#include "../SelectedObject.h"
#include "../ClickObject.h"
#include "../OrionUO.h"
#include "../Game objects/GameWorld.h"
#include "../Game objects/GamePlayer.h"
#include "../Game objects/ObjectOnCursor.h"
#include "../Managers/FontsManager.h"
#include "../Managers/MouseManager.h"
#include "../Network/Packets.h"
#include "../PressedObject.h"
//----------------------------------------------------------------------------------
CGumpSecureTrading::CGumpSecureTrading(uint serial, short x, short y, uint id, uint id2)
: CGump(GT_TRADE, serial, x, y), m_ID2(id2), m_StateMy(false), m_StateOpponent(false),
m_TextRenderer(), m_MyCheck(NULL), m_OpponentCheck(NULL), m_MyDataBox(NULL),
m_OpponentDataBox(NULL)
{
	m_ID = id;
}
//----------------------------------------------------------------------------------
CGumpSecureTrading::~CGumpSecureTrading()
{
}
//----------------------------------------------------------------------------------
void CGumpSecureTrading::CalculateGumpState()
{
	CGump::CalculateGumpState();

	if (g_GumpPressed && g_PressedObject.LeftObject() != NULL && g_PressedObject.LeftObject()->IsText())
	{
		g_GumpMovingOffset.Reset();

		if (m_Minimized)
		{
			g_GumpTranslate.X = (float)m_MinimizedX;
			g_GumpTranslate.Y = (float)m_MinimizedY;
		}
		else
		{
			g_GumpTranslate.X = (float)m_X;
			g_GumpTranslate.Y = (float)m_Y;
		}
	}
}
//----------------------------------------------------------------------------------
void CGumpSecureTrading::PrepareContent()
{
	if (m_MyCheck != NULL)
	{
		if (m_StateMy)
		{
			if (m_MyCheck->Graphic != 0x0869)
			{
				m_MyCheck->Graphic = 0x0869;
				m_MyCheck->GraphicSelected = 0x086A;
				m_MyCheck->GraphicPressed = 0x086A;
				m_WantRedraw = true;
			}
		}
		else if (m_MyCheck->Graphic != 0x0867)
		{
			m_MyCheck->Graphic = 0x0867;
			m_MyCheck->GraphicSelected = 0x0868;
			m_MyCheck->GraphicPressed = 0x0868;
			m_WantRedraw = true;
		}
	}

	if (m_OpponentCheck != NULL)
	{
		if (m_StateOpponent)
		{
			if (m_OpponentCheck->Graphic != 0x0869)
			{
				m_OpponentCheck->Graphic = 0x0869;
				m_WantRedraw = true;
			}
		}
		else if (m_OpponentCheck->Graphic != 0x0867)
		{
			m_OpponentCheck->Graphic = 0x0867;
			m_WantRedraw = true;
		}
	}
}
//----------------------------------------------------------------------------------
void CGumpSecureTrading::UpdateContent()
{
	CGameObject *selobj = g_World->FindWorldObject(m_Serial);

	if (selobj == NULL)
		return; //������, � �������� �������� ���� - �����

	if (m_Items == NULL)
	{
		Add(new CGUIGumppic(0x0866, 0, 0)); //Trade Gump

		if (m_StateMy)
			m_MyCheck = (CGUIButton*)Add(new CGUIButton(ID_GST_CHECKBOX, 0x0869, 0x086A, 0x086A, 52, 29));
		else
			m_MyCheck = (CGUIButton*)Add(new CGUIButton(ID_GST_CHECKBOX, 0x0867, 0x0868, 0x0868, 52, 29));

		CGUIText *text = (CGUIText*)Add(new CGUIText(0x0386, 84, 40));
		text->CreateTextureA(1, g_Player->Name);

		if (m_StateOpponent)
			m_OpponentCheck = (CGUIGumppic*)Add(new CGUIGumppic(0x0869, 266, 160));
		else
			m_OpponentCheck = (CGUIGumppic*)Add(new CGUIGumppic(0x0867, 266, 160));

		int fontWidth = 260 - g_FontManager.GetWidthA(1, m_Text.c_str(), m_Text.length());

		text = (CGUIText*)Add(new CGUIText(0x0386, fontWidth, 170));
		text->CreateTextureA(1, m_Text);

		Add(new CGUIShader(g_ColorizerShader, true));

		Add(new CGUIScissor(true, 0, 0, 45, 70, 110, 80));
		m_MyDataBox = (CGUIDataBox*)Add(new CGUIDataBox());
		Add(new CGUIScissor(false));

		Add(new CGUIScissor(true, 0, 0, 192, 70, 110, 80));
		m_OpponentDataBox = (CGUIDataBox*)Add(new CGUIDataBox());
		Add(new CGUIScissor(false));

		Add(new CGUIShader(g_ColorizerShader, false));
	}
	else
	{
		m_MyDataBox->Clear();
		m_OpponentDataBox->Clear();
	}

	uint ignoreSerial = 0;

	if (g_ObjectInHand != NULL)
		ignoreSerial = g_ObjectInHand->Serial;

	//��������� ������ ������ (��� ������� ������)
	CGameObject *container = g_World->FindWorldObject(m_ID);

	if (container != NULL && container->m_Items != NULL)
	{
		QFOR(item, container->m_Items, CGameItem*)
		{
			if (item->Serial == ignoreSerial)
				continue;

			bool doubleDraw = false;
			ushort graphic = item->GetDrawGraphic(doubleDraw);

			CGUITilepicHightlighted *dataObject = (CGUITilepicHightlighted*)m_MyDataBox->Add(new CGUITilepicHightlighted(item->Serial, graphic, item->Color, 0x0035, 45 + item->X, 70 + item->Y, doubleDraw));
			dataObject->PartialHue = IsPartialHue(g_Orion.GetStaticFlags(graphic));
		}
	}

	//��������� ������ �������� (��� ������� ������)
	container = g_World->FindWorldObject(m_ID2);
	if (container != NULL && container->m_Items != NULL)
	{
		QFOR(item, container->m_Items, CGameItem*)
		{
			bool doubleDraw = false;
			ushort graphic = item->GetDrawGraphic(doubleDraw);

			CGUITilepicHightlighted *dataObject = (CGUITilepicHightlighted*)m_OpponentDataBox->Add(new CGUITilepicHightlighted(item->Serial, graphic, item->Color, 0x0035, 192 + item->X, 70 + item->Y, doubleDraw));
			dataObject->PartialHue = IsPartialHue(g_Orion.GetStaticFlags(graphic));
		}
	}
}
//----------------------------------------------------------------------------------
void CGumpSecureTrading::Draw()
{
	CGameObject *selobj = g_World->FindWorldObject(m_Serial);

	if (selobj == NULL)
		return; //������, � �������� �������� ���� - �����
	
	CalculateGumpState();

	if (g_GumpPressed)
		m_FrameCreated = false;

	//���������� ���������, ������������ � ��������� ������
	CRenderTextObject *rto = m_TextRenderer.m_TextItems;
	m_TextRenderer.ClearRect();

	//���������� ��������� � ������������ ������
	for (; rto != NULL; rto = rto->m_NextDraw)
	{
		if (!rto->IsText())
		{
			if (rto->m_NextDraw == NULL)
				break;

			continue;
		}

		CTextData *td = (CTextData*)rto;

		if (td->Type == TT_OBJECT && td->Timer >= g_Ticks)
		{
			CGameObject *go = g_World->FindWorldObject(td->Serial);

			if (go != NULL && go->Graphic < 0x4000)
			{
				CGLTextTexture &tth = td->m_Texture;
				int drawX = td->X - go->GetTextOffsetX(td);
				int drawY = td->Y - go->GetTextOffsetY(td);

				CTextImageBounds ib(drawX, drawY, tth.Width, tth.Height, td);

				td->Transparent = m_TextRenderer.InRect(ib, go);

				m_TextRenderer.AddRect(ib);
			}
		}

		if (rto->m_NextDraw == NULL)
			break;
	}

	CGump::Draw();

	glTranslatef(g_GumpTranslate.X, g_GumpTranslate.Y, 0.0f);

	//��������� ������
	for (; rto != NULL; rto = rto->m_PrevDraw)
	{
		if (!rto->IsText())
			continue;

		CTextData *td = (CTextData*)rto;

		if (td->Type == TT_OBJECT && td->Timer >= g_Ticks)
		{
			CGLTextTexture &tth = td->m_Texture;

			int drawX = td->X - (tth.Width / 2);
			int drawY = td->Y;
			bool transparent = td->Transparent;

			while (td != NULL)
			{
				drawY -= td->m_Texture.Height;

				td = (CTextData*)td->m_Next;
			}

			if (transparent)
			{
				glEnable(GL_BLEND);
				glBlendFunc(GL_ONE, GL_DST_COLOR);

				tth.Draw(drawX, drawY);

				glDisable(GL_BLEND);
			}
			else
				tth.Draw(drawX, drawY);
		}
	}

	glTranslatef(-g_GumpTranslate.X, -g_GumpTranslate.Y, 0.0f);
}
//----------------------------------------------------------------------------------
CRenderObject *CGumpSecureTrading::Select()
{
	CGameObject *selobj = g_World->FindWorldObject(m_Serial);

	if (selobj == NULL)
		return NULL; //������, � �������� �������� ���� - �����

	//���������� ���������, ������������ � ��������� ������
	CRenderTextObject *rto = m_TextRenderer.m_TextItems;

	//���������� ��������� � ������������ ������
	for (; rto != NULL; rto = rto->m_NextDraw)
	{
		if (!rto->IsText())
		{
			if (rto->m_NextDraw == NULL)
				break;

			continue;
		}

		if (rto->m_NextDraw == NULL)
			break;
	}

	CRenderObject *selected = CGump::Select();

	WISP_GEOMETRY::CPoint2Di oldPos = g_MouseManager.Position;
	g_MouseManager.Position = WISP_GEOMETRY::CPoint2Di(oldPos.X - (int)g_GumpTranslate.X, oldPos.Y - (int)g_GumpTranslate.Y);

	//�������� ������
	for (; rto != NULL; rto = rto->m_PrevDraw)
	{
		if (!rto->IsText())
			continue;

		CTextData *td = (CTextData*)rto;

		if (td->Type == TT_OBJECT)
		{
			if (td->Timer >= g_Ticks)
			{
				CGLTextTexture &tth = td->m_Texture;

				int drawX = td->X - (tth.Width / 2);
				int drawY = td->Y;

				while (td != NULL)
				{
					drawY -= td->m_Texture.Height;

					td = (CTextData*)td->m_Next;
				}

				if (tth.UnderMouse(drawX, drawY))
				{
					g_SelectedObject.Init(rto, this);
					g_SelectedObject.Serial = rto->Serial;
				}
			}
		}
	}

	g_MouseManager.Position = oldPos;

	return selected;
}
//----------------------------------------------------------------------------------
void CGumpSecureTrading::GUMP_BUTTON_EVENT_C
{
	if (serial == ID_GST_CHECKBOX) //��������� ��������� ��������
	{
		m_StateMy = !m_StateMy;

		SendTradingResponse(2);
	}
	else
	{
		if (!g_ClickObject.Enabled)
		{
			CGameObject *clickTarget = g_World->FindWorldObject(serial);

			if (clickTarget == NULL)
				return;

			g_ClickObject.Init(clickTarget);
			g_ClickObject.Timer = g_Ticks + DCLICK_DELAY;
			g_ClickObject.X = g_MouseManager.Position.X - m_X;
			g_ClickObject.Y = g_MouseManager.Position.Y - m_Y;
		}
	}
}
//----------------------------------------------------------------------------------
void CGumpSecureTrading::OnLeftMouseButtonUp()
{
	CGump::OnLeftMouseButtonUp();

	if (g_ObjectInHand != NULL)
	{
		int x = m_X;
		int y = m_Y;

		if (g_Orion.PolygonePixelsInXY(x + 45, y + 70, 110, 80))
		{
			if (GetTopObjDistance(g_Player, g_World->FindWorldObject(m_ID2)) < 3)
			{
				x = g_MouseManager.Position.X - x - 45;
				y = g_MouseManager.Position.Y - y - 70;

				bool doubleDraw = false;
				ushort graphic = g_ObjectInHand->GetDrawGraphic(doubleDraw);

				CGLTexture *th = g_Orion.ExecuteStaticArt(graphic);

				if (th != NULL)
				{
					x -= (th->Width / 2);
					y -= (th->Height / 2);

					if (x + th->Width > 110)
						x = 110 - th->Width;

					if (y + th->Height > 80)
						y = 80 - th->Height;
				}

				if (x < 0)
					x = 0;

				if (y < 0)
					y = 0;

				g_Orion.DropItem(m_ID, x, y, 0);
			}
			else
				g_Orion.PlaySoundEffect(0x0051);
		}
	}
}
//----------------------------------------------------------------------------------
void CGumpSecureTrading::SendTradingResponse(int code)
{
	//����� �� ����� ����
	CPacketTradeResponse(this, code).Send();

	if (code == 1) //��������� ����
		m_RemoveMark = true;
}
//----------------------------------------------------------------------------------
