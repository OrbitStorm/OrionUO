/***********************************************************************************
**
** GumpScreenSelectProfession.cpp
**
** Copyright (C) August 2016 Hotride
**
************************************************************************************
*/
//----------------------------------------------------------------------------------
#include "GumpScreenSelectProfession.h"
#include "../Screen stages/SelectProfessionScreen.h"
#include "../OrionUO.h"
#include "../OrionWindow.h"
#include "../Managers/ConfigManager.h"
#include "../Managers/ConnectionManager.h"
#include "../Managers/ProfessionManager.h"
#include "../Managers/ClilocManager.h"
#include "../Skills.h"
//----------------------------------------------------------------------------------
CGumpScreenSelectProfession::CGumpScreenSelectProfession()
: CGump(GT_NONE, 0, 0, 0)
{
	m_NoMove = true;
	m_NoClose = true;

	IFOR(i, 0, 3)
	{
		m_StatsSliders[i] = NULL;
		m_SkillsSliders[i] = NULL;
	}

	m_SkillsSliders[3] = NULL;
}
//----------------------------------------------------------------------------------
CGumpScreenSelectProfession::~CGumpScreenSelectProfession()
{
}
//----------------------------------------------------------------------------------
void CGumpScreenSelectProfession::UpdateContent()
{
	Clear();

	if (g_ProfessionManager.Selected == NULL)
	{
		g_OrionWindow.ShowMessage("No items in profession manager!", "Profession error!");
		return;
	}

	if (g_ConnectionManager.ClientVersion >= CV_308Z)
		UpdateContentNew();
	else
		UpdateContentOld();
}
//----------------------------------------------------------------------------------
void CGumpScreenSelectProfession::UpdateContentOld()
{
	CBaseProfession *obj = g_ProfessionManager.Selected;

	if (obj == NULL)
	{
		g_OrionWindow.ShowMessage("No items in profession manager!", "Profession error!");
		return;
	}

	IFOR(i, 0, 3)
	{
		m_StatsSliders[i] = NULL;
		m_SkillsSliders[i] = NULL;
	}

	m_SkillsSliders[3] = NULL;

	Add(new CGUIGumppicTiled(0x0588, 0, 0, 640, 480));
	Add(new CGUIGumppic(0x157C, 0, 0));
	Add(new CGUIGumppic(0x15A0, 0, 4));
	Add(new CGUIButton(ID_SPS_QUIT, 0x1589, 0x158A, 0x158B, 555, 4));
	Add(new CGUIButton(ID_SPS_ARROW_PREV, 0x15A1, 0x15A2, 0x15A3, 586, 445));
	Add(new CGUIResizepic(0, 0x0A28, 80, 80, 546, 352));
	Add(new CGUIGumppic(0x058B, 145, 57));
	Add(new CGUIGumppic(0x0589, 222, 44));
	Add(new CGUIButton(ID_SPS_ARROW_BACK_PROFESSION, 0x119C, 0x119D, 0x119E, 200, 356));

	CGUIHTMLGump *htmlGump = new CGUIHTMLGump(ID_SPS_HTMLGUMP, 0x0BB8, 120, 137, 221, 214, true, true);
	Add(htmlGump);

	if (!g_SelectProfessionScreen.SkillSelection)
	{
		if (obj != NULL)
		{
			CGUIText *text = new CGUIText(0, 3, 3);
			text->CreateTextureW(0, obj->Description, 30, 195);
			htmlGump->Add(text);
		}
	}
	else
	{
		int yPtr = 4; // 20;

		IFOR(i, 0, g_SkillsCount)
		{
			int skillIndex = g_SkillSort.m_Skills[i];

			CGUITextEntry *entry = (CGUITextEntry*)htmlGump->Add(new CGUITextEntry(ID_SPS_SKILLS_LIST + i, 1, 0x0035, 0x0035, 3, yPtr, 0, false, 9));
			entry->m_Entry.SetText(g_Skills[skillIndex].Name);
			entry->m_Entry.CreateTextureA(9, g_Skills[skillIndex].Name, 1, 0, TS_LEFT, 0);
			entry->CheckOnSerial = true;
			entry->ReadOnly = true;

			htmlGump->Add(new CGUIHitBox(ID_SPS_SKILLS_LIST + i, 3, yPtr, 195, entry->m_Entry.m_Texture.Height));

			yPtr += entry->m_Entry.m_Texture.Height;
		}
	}

	htmlGump->CalculateDataSize();

	if (obj->Type == PT_CATEGORY) //category
	{
		Add(new CGUIButton(ID_SPS_LABEL_BACK_PROFESSION, obj->Gump, obj->Gump, obj->Gump + 1, 231, 53));

		int offsY = 0;

		int index = 0;

		QFOR(child, obj->m_Items, CBaseProfession*)
		{
			Add(new CGUIGumppic(0x0589, 500, 100 + offsY));

			Add(new CGUIButton(ID_SPS_LABEL + index, child->Gump, child->Gump, child->Gump + 1, 509, 109 + offsY));

			CGUIText *text = new CGUIText(1, 350, 135 + offsY);
			text->CreateTextureA(9, child->GetName());
			Add(text);

			offsY += 79;

			index++;
		}
	}
	else if (obj->Type == PT_PROFESSION) //profession
	{
		Add(new CGUIButton(ID_SPS_LABEL_BACK_PROFESSION, obj->Gump, obj->Gump, obj->Gump + 1, 231, 53));

		const float SphereListWidth = 95.0f;
		float ValPer = 0.0f;

		CProfession *profession = (CProfession*)obj;
		char val[15] = { 0 };
		int statVal[3] = { profession->Str, profession->Dex, profession->Int };

		const string statName[3] = { "Strength", "Dexterity", "Intelligence" };

		int yPtr = 136;

		IFOR(i, 0, 3)
		{
			CGUIText *text = new CGUIText(1, 360, yPtr);
			text->CreateTextureA(1, statName[i]);
			Add(text);

			m_StatsSliders[i] = (CGUISlider*)Add(new CGUISlider(ID_SPS_STATS_SPHERE + i, 0x00D8, 0x00D8, 0x00D8, 0x00D5, true, false, 496, yPtr, 95, 10, 45, statVal[i]));
			m_StatsSliders[i]->DefaultTextOffset = -10;
			m_StatsSliders[i]->SetTextParameters(true, STP_LEFT, 1, 1, false);

			yPtr += 30;
		}

		if (profession->DescriptionIndex >= 0)
		{
			yPtr = 260;

			IFOR(i, 0, 3)
			{
				int skillID = profession->GetSkillIndex(i);

				if (skillID >= g_SkillsCount)
					skillID = 0;

				CGUIText *text = new CGUIText(1, 360, yPtr);
				text->CreateTextureA(1, g_Skills[skillID].Name, 90, TS_LEFT, UOFONT_FIXED);
				Add(text);

				yPtr += 32;
			}
		}
		else //advanced
		{
			yPtr = 256;

			IFOR(i, 0, 3)
			{
				Add(new CGUIResizepic(ID_SPS_SKILLS_FILED + i, 0x0BB8, 350, yPtr, 105, 25));

				int skillID = profession->GetSkillIndex(i);

				CGUITextEntry *entry = (CGUITextEntry*)Add(new CGUITextEntry(ID_SPS_SKILLS_FILED + i, 0x0386, 0, 0x0021, 354, yPtr + 5, 90, false, 9, TS_LEFT, UOFONT_FIXED));

				if (skillID >= g_SkillsCount)
					entry->m_Entry.SetText("Click here");
				else
					entry->m_Entry.SetText(g_Skills[skillID].Name);

				entry->CheckOnSerial = true;
				entry->ReadOnly = true;

				yPtr += 32;
			}
		}

		IFOR(i, 0, 3)
		{
			m_SkillsSliders[i] = (CGUISlider*)Add(new CGUISlider(ID_SPS_SKILLS_SPHERE + i, 0x00D8, 0x00D8, 0x00D8, 0x00D5, true, false, 496, 258 + (i * 32), 95, 0, 50, profession->GetSkillValue(i)));
			m_SkillsSliders[i]->DefaultTextOffset = -10;
			m_SkillsSliders[i]->SetTextParameters(true, STP_LEFT, 1, 1, false);
		}

		Add(new CGUIButton(ID_SPS_ARROW_NEXT, 0x15A4, 0x15A5, 0x15A6, 610, 445));
	}
}
//----------------------------------------------------------------------------------
void CGumpScreenSelectProfession::UpdateContentNew()
{
	CBaseProfession *obj = g_ProfessionManager.Selected;

	Add(new CGUIGumppicTiled(0x0588, 0, 0, 640, 480));
	Add(new CGUIGumppic(0x157C, 0, 0));
	Add(new CGUIGumppic(0x15A0, 0, 4));
	Add(new CGUIButton(ID_SPS_QUIT, 0x1589, 0x158A, 0x158B, 555, 4));
	Add(new CGUIButton(ID_SPS_ARROW_PREV, 0x15A1, 0x15A2, 0x15A3, 586, 445));
	Add(new CGUIResizepic(0, 0x0A28, 100, 80, 470, 372));
	Add(new CGUIGumppic(0x058B, 213, 57));
	Add(new CGUIGumppic(0x0589, 290, 44));

	string str = g_ClilocManager.Cliloc(g_Language)->GetA(3000326, "Choose a Trade for Your Character");
	CGUIText *text = new CGUIText(0x0386, 120, 126);
	text->CreateTextureA(2, str, 432, TS_CENTER);
	Add(text);

	Add(new CGUIGumppic(obj->Gump, 299, 53));

	if (obj->Type == PT_CATEGORY) //category
	{
		int offsX = 0;
		int offsY = 0;
		int index = 0;

		QFOR(child, obj->m_Items, CBaseProfession*)
		{
			Add(new CGUIResizepic(0, 0x0BB8, 145 + offsX, 168 + offsY, 175, 34));

			text = new CGUIText(0x0386, 151 + offsX, 174 + offsY);
			text->CreateTextureW(0, ToWString(child->Name), 30, 185, TS_LEFT, UOFONT_SOLID);
			Add(text);

			Add(new CGUIButton(ID_SPS_LABEL + index, child->Gump, child->Gump, child->Gump + 1, 265 + offsX, 155 + offsY));

			if (offsX)
			{
				offsX = 0;
				offsY += 70;
			}
			else
				offsX = 195;

			index++;
		}
	}

	else if (obj->Type == PT_PROFESSION) //profession
	{
		CProfession *profession = (CProfession*)obj;
		char val[15] = { 0 };
		int statVal[3] = { profession->Str, profession->Dex, profession->Int };
		const string statName[3] = { "Strength", "Dexterity", "Intelligence" };

		int yPtr = 171;

		//Stats
		IFOR(i, 0, 3)
		{
			text = new CGUIText(1, 160, yPtr);
			text->CreateTextureA(1, statName[i]);
			Add(text);

			m_StatsSliders[i] = (CGUISlider*)Add(new CGUISlider(ID_SPS_STATS_SPHERE + i, 0x00D8, 0x00D8, 0x00D8, 0x00D5, true, false, 96, yPtr + 20, 95, 10, 50, statVal[i]));
			m_StatsSliders[i]->SetTextParameters(true, STP_LEFT, 1, 1, false);

			yPtr += 80;
		}

		yPtr = 171;

		//Skills

		if (g_SelectProfessionScreen.SkillSelection)
		{
			CGUIHTMLGump *htmlGump = new CGUIHTMLGump(ID_SPS_HTMLGUMP, 0x0BB8, 320, 168, 197, 215, true, true);
			Add(htmlGump);

			yPtr = 4;

			IFOR(i, 0, g_SkillsCount)
			{
				int skillIndex = g_SkillSort.m_Skills[i];

				CGUITextEntry *entry = new CGUITextEntry(ID_SPS_SKILLS_LIST + i, 1, 0x0035, 0x0035, 326, yPtr, 0, false, 9);
				entry->m_Entry.SetText(g_Skills[skillIndex].Name);
				entry->m_Entry.CreateTextureA(9, g_Skills[skillIndex].Name.c_str(), 1, 0, TS_LEFT, 0);
				entry->CheckOnSerial = true;
				entry->ReadOnly = true;
				htmlGump->Add(entry);

				yPtr += entry->m_Entry.m_Texture.Height;
			}

			htmlGump->CalculateDataSize();
		}
		else
		{
			IFOR(i, 0, 3)
			{
				Add(new CGUIResizepic(ID_SPS_SKILLS_FILED + i, 0x0BB8, 340, yPtr, 175, 25));

				int skillID = profession->GetSkillIndex(i);

				CGUITextEntry *entry = (CGUITextEntry*)Add(new CGUITextEntry(ID_SPS_SKILLS_FILED + i, 0x0386, 0, 0x0021, 346, yPtr + 5, 90, false, 9, TS_LEFT, UOFONT_FIXED));

				if (skillID >= g_SkillsCount)
					entry->m_Entry.SetText("Click here");
				else
					entry->m_Entry.SetText(g_Skills[skillID].Name);

				entry->CheckOnSerial = true;
				entry->ReadOnly = true;
				Add(entry);

				m_SkillsSliders[i] = (CGUISlider*)Add(new CGUISlider(ID_SPS_SKILLS_SPHERE + i, 0x00D8, 0x00D8, 0x00D8, 0x00D5, true, false, 276, yPtr + 30, 95, 0, 50, profession->GetSkillValue(i)));
				m_SkillsSliders[i]->SetTextParameters(true, STP_LEFT, 1, 1, false);

				yPtr += 80;
			}
		}

		Add(new CGUIButton(ID_SPS_ARROW_NEXT, 0x15A4, 0x15A5, 0x15A6, 610, 445));
	}
}
//----------------------------------------------------------------------------------
void CGumpScreenSelectProfession::InitToolTip()
{
	if (!g_ConfigManager.UseToolTips)
		return;

	uint id = g_SelectedObject.Serial;

	switch (id)
	{
		case ID_SPS_QUIT:
		{
			g_ToolTip.Set(L"Quit Ultima Online", g_SelectedObject.Object(), 80);
			break;
		}
		case ID_SPS_ARROW_NEXT:
		{
			g_ToolTip.Set(L"Next screen", g_SelectedObject.Object());
			break;
		}
		case ID_SPS_ARROW_PREV:
		{
			g_ToolTip.Set(L"Preveous screen", g_SelectedObject.Object());
			break;
		}
		case ID_SPS_ARROW_BACK_PROFESSION:
		case ID_SPS_LABEL_BACK_PROFESSION:
		{
			g_ToolTip.Set(L"Back to select profession category", g_SelectedObject.Object(), 150);
			break;
		}
		default:
			break;
	}

	if (id >= ID_SPS_LABEL && id < ID_SPS_SKILLS_LIST)
	{
		CBaseProfession *obj = g_ProfessionManager.Selected;

		int index = 0;

		QFOR(child, obj->m_Items, CBaseProfession*)
		{
			LOG("%i %i child->DescriptionClilocID = %i\n", id, ID_SPS_LABEL + index, child->DescriptionClilocID);

			if (id == ID_SPS_LABEL + index)
			{
				if (child->DescriptionClilocID)
					g_ToolTip.Set(child->DescriptionClilocID, "Description", g_SelectedObject.Object(), 350);

				break;
			}

			index++;
		}
	}
}
//----------------------------------------------------------------------------------
void CGumpScreenSelectProfession::OnButton(const uint &serial)
{
	CBaseProfession *obj = g_ProfessionManager.Selected;
	CProfession *profession = (CProfession*)obj;

	if (serial == ID_SPS_QUIT) //x button
		g_SelectProfessionScreen.CreateSmoothAction(CSelectProfessionScreen::ID_SMOOTH_SPS_QUIT);
	else if (serial == ID_SPS_ARROW_PREV) //< button
	{
		if (g_ConnectionManager.ClientVersion >= CV_308Z && g_ProfessionManager.Selected->Type == PT_PROFESSION && g_ProfessionManager.Selected->DescriptionIndex == -1 /*Advanced*/)
		{
			g_ProfessionManager.Selected = g_ProfessionManager.GetParent(g_ProfessionManager.Selected);
			g_SelectProfessionScreen.SkillSelection = 0;

			return;
		}
		else
			g_SelectProfessionScreen.CreateSmoothAction(CSelectProfessionScreen::ID_SMOOTH_SPS_GO_SCREEN_CHARACTER);
	}
	else if (serial == ID_SPS_ARROW_NEXT) //> button
	{
		if (obj->Type == PT_PROFESSION)
		{
			bool passed = true;

			if (profession->DescriptionIndex == -1) //Advanced
			{
				IFOR(i, 0, 3)
				{
					IFOR(j, 0, 3)
					{
						if (i != j)
						{
							if (profession->GetSkillIndex(i) == 0xFF || profession->GetSkillIndex(i) == profession->GetSkillIndex(j))
							{
								passed = false;

								g_SelectProfessionScreen.CreateSmoothAction(CSelectProfessionScreen::ID_SMOOTH_SPS_GO_SCREEN_GAME_CONNECT);

								break;
							}
						}
					}
				}
			}

			if (passed)
				g_SelectProfessionScreen.CreateSmoothAction(CSelectProfessionScreen::ID_SMOOTH_SPS_GO_SCREEN_CREATE);
		}
	}
	else if (serial == ID_SPS_ARROW_BACK_PROFESSION || serial == ID_SPS_LABEL_BACK_PROFESSION) //Arrow < or General Label gump
	{
		g_ProfessionManager.Selected = g_ProfessionManager.GetParent(g_ProfessionManager.Selected);
		g_SelectProfessionScreen.SkillSelection = 0;

		return;
	}
	else if (obj->Type == PT_CATEGORY)
	{
		int index = 0;

		QFOR(child, obj->m_Items, CBaseProfession*)
		{
			if (serial == ID_SPS_LABEL + index)
			{
				g_ProfessionManager.Selected = child;
				g_SelectProfessionScreen.SkillSelection = 0;

				if (g_ConnectionManager.ClientVersion >= CV_308Z && child->Type == PT_PROFESSION && child->DescriptionIndex != -1)
					g_SelectProfessionScreen.CreateSmoothAction(CSelectProfessionScreen::ID_SMOOTH_SPS_GO_SCREEN_CREATE);

				return;
			}

			index++;
		}
	}
	else if (obj->Type == PT_PROFESSION)
	{
		if (serial >= ID_SPS_SKILLS_LIST)
		{
			g_SelectProfessionScreen.DecSkillSelection();
			int index = serial - ID_SPS_SKILLS_LIST;
			index = g_SkillSort.m_Skills[index];

			profession->SetSkillIndex(g_SelectProfessionScreen.SkillSelection, index);

			g_SelectProfessionScreen.SkillSelection = 0;
		}
		else
		{
			IFOR(i, 0, 3)
			{
				if (serial == ID_SPS_SKILLS_FILED + i)
				{
					g_SelectProfessionScreen.SkillSelection = i + 1;

					break;
				}
			}
		}
	}
}
//----------------------------------------------------------------------------------
void CGumpScreenSelectProfession::OnSliderClick(const uint &serial)
{
	OnSliderMove(serial);
}
//----------------------------------------------------------------------------------
void CGumpScreenSelectProfession::OnSliderMove(const uint &serial)
{
	//Stats
	if (serial >= ID_SPS_STATS_SPHERE && serial <= ID_SPS_STATS_SPHERE + 2)
	{
		if (g_ConnectionManager.ClientVersion >= CV_308Z)
			ShuffleStats(serial - ID_SPS_STATS_SPHERE, 80, 60);
		else
			ShuffleStats(serial - ID_SPS_STATS_SPHERE, 65, 45);
	}

	//Skills
	if (serial >= ID_SPS_SKILLS_SPHERE && serial <= ID_SPS_SKILLS_SPHERE + 2)
		ShuffleSkills(serial - ID_SPS_SKILLS_SPHERE);
}
//----------------------------------------------------------------------------------
void CGumpScreenSelectProfession::ShuffleStats(const int &id, const int &maxSum, const int &maxVal)
{
	CProfession *profession = (CProfession*)g_ProfessionManager.Selected;
	int stats[3] = { m_StatsSliders[0]->Value, m_StatsSliders[1]->Value, m_StatsSliders[2]->Value };

	int used_stat = id;
	int others_stat[2] = { 0 };

	others_stat[0] = 0;
	if (others_stat[0] == used_stat)
		others_stat[0]++;

	others_stat[1] = others_stat[0] + 1;
	if (others_stat[1] == used_stat)
		others_stat[1]++;

	stats[used_stat] = m_StatsSliders[id]->Value;

	int stat_sum = maxSum - (stats[0] + stats[1] + stats[2]);

	if (stat_sum > 0) //stat will decrease
	{
		while (stat_sum > 0)
		{
			if (stats[others_stat[0]] < maxVal)
			{
				stat_sum--;
				stats[others_stat[0]]++;
			}
			else if (stats[others_stat[1]] < maxVal)
			{
				stat_sum--;
				stats[others_stat[1]]++;
			}
			else
				break;
		}
	}
	else if (stat_sum < 0) //stat will increase
	{
		while (stat_sum < 0)
		{
			if (stats[others_stat[0]] > 10)
			{
				stat_sum++;
				stats[others_stat[0]]--;
			}
			else if (stats[others_stat[1]] > 10)
			{
				stat_sum++;
				stats[others_stat[1]]--;
			}
			else
				break;
		}
	}

	IFOR(i, 0, 3)
	{
		m_StatsSliders[i]->Value = stats[i];
		m_StatsSliders[i]->CalculateOffset();
		m_StatsSliders[i]->UpdateText();
	}

	profession->Str = stats[0];
	profession->Dex = stats[1];
	profession->Int = stats[2];
}
//----------------------------------------------------------------------------------
void CGumpScreenSelectProfession::ShuffleSkills(const int &id)
{
	CProfession *profession = (CProfession*)g_ProfessionManager.Selected;
	int skills[3] = { m_SkillsSliders[0]->Value, m_SkillsSliders[1]->Value, m_SkillsSliders[2]->Value };

	int used_skill = id;
	int others_skills[2] = { 0 };

	others_skills[0] = 0;
	if (others_skills[0] == used_skill)
		others_skills[0]++;

	others_skills[1] = others_skills[0] + 1;
	if (others_skills[1] == used_skill)
		others_skills[1]++;

	skills[used_skill] = m_SkillsSliders[id]->Value;

	int skills_sum = 100 - (skills[0] + skills[1] + skills[2]);

	if (skills_sum > 0) //skill will decrease
	{
		while (skills_sum > 0)
		{
			if (skills[others_skills[0]] < 50)
			{
				skills_sum--;
				skills[others_skills[0]]++;
			}
			else if (skills[others_skills[1]] < 50)
			{
				skills_sum--;
				skills[others_skills[1]]++;
			}
			else
				break;
		}
	}
	else if (skills_sum < 0) //skill will increase
	{
		while (skills_sum < 0)
		{
			if (skills[others_skills[0]] > 0)
			{
				skills_sum++;
				skills[others_skills[0]]--;
			}
			else if (skills[others_skills[1]] > 0)
			{
				skills_sum++;
				skills[others_skills[1]]--;
			}
			else
				break;
		}
	}


	IFOR(i, 0, 3)
	{
		m_SkillsSliders[i]->Value = skills[i];
		m_SkillsSliders[i]->CalculateOffset();
		m_SkillsSliders[i]->UpdateText();
	}

	profession->SetSkillValue(0, skills[0]);
	profession->SetSkillValue(1, skills[1]);
	profession->SetSkillValue(2, skills[2]);
}
//----------------------------------------------------------------------------------
