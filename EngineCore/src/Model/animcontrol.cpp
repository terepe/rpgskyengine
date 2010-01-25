#include "animcontrol.h"
#include "util.h"

IMPLEMENT_CLASS(AnimControl, wxWindow)

BEGIN_EVENT_TABLE(AnimControl, wxWindow)
	EVT_COMBOBOX(ID_ANIM, AnimControl::OnAnim)
	EVT_COMBOBOX(ID_ANIM_SECONDARY, AnimControl::OnAnim)
	EVT_COMBOBOX(ID_ANIM_MOUTH, AnimControl::OnAnim)

	EVT_COMBOBOX(ID_LOOPS, AnimControl::OnLoop)
	EVT_COMBOBOX(ID_SKIN, AnimControl::OnSkin)
	EVT_COMBOBOX(ID_ITEMSET, AnimControl::OnItemSet)

	EVT_CHECKBOX(ID_OLDSTYLE, AnimControl::OnCheck)
	EVT_CHECKBOX(ID_ANIM_LOCK, AnimControl::OnCheck)

	EVT_BUTTON(ID_PLAY, AnimControl::OnButton)
	EVT_BUTTON(ID_PAUSE, AnimControl::OnButton)
	EVT_BUTTON(ID_STOP, AnimControl::OnButton)
	EVT_BUTTON(ID_ADDANIM, AnimControl::OnButton)
	EVT_BUTTON(ID_CLEARANIM, AnimControl::OnButton)
	EVT_BUTTON(ID_PREVANIM, AnimControl::OnButton)
	EVT_BUTTON(ID_NEXTANIM, AnimControl::OnButton)

	EVT_SLIDER(ID_SPEED, AnimControl::OnSliderUpdate)
	EVT_SLIDER(ID_SPEED_MOUTH, AnimControl::OnSliderUpdate)
	EVT_SLIDER(ID_FRAME, AnimControl::OnSliderUpdate)
END_EVENT_TABLE()

AnimControl::AnimControl(wxWindow* parent, wxWindowID id)
{
	
	if(Create(parent, id, wxDefaultPosition, wxSize(700,90), 0, _T("AnimControlFrame")) == false) {
		wxMessageBox(_T("Failed to create a window for our AnimControl!"), _T("ERROR"));
		wxLogMessage(_T("Error: Failed to create a window for our AnimControl!"));
		return;
	}

	const wxString strLoops[10] = {_T("0"), _T("1"), _T("2"), _T("3"), _T("4"), _T("5"), _T("6"), _T("7"), _T("8"), _T("9")};
	
	animList = new wxComboBox(this, ID_ANIM, _("Animation"), wxPoint(10,10), wxSize(144,16), 0, NULL, wxCB_READONLY|wxCB_SORT, wxDefaultValidator, _("Animation")); //|wxCB_SORT); //wxPoint(66,10)
	animList2 = new wxComboBox(this, ID_ANIM_SECONDARY, _("Secondary"), wxPoint(154,10), wxSize(144,16), 0, NULL, wxCB_READONLY|wxCB_SORT, wxDefaultValidator, _("Secondary")); //|wxCB_SORT); //wxPoint(66,10)
	animList2->Enable(false);
	animList2->Show(false);

	// Our hidden head/mouth related controls
	animList3 = new wxComboBox(this, ID_ANIM_MOUTH, _("Mouth"), wxPoint(10,100), wxSize(144,16), 0, NULL, wxCB_READONLY|wxCB_SORT, wxDefaultValidator, _("Secondary")); //|wxCB_SORT); //wxPoint(66,10)
	animList3->Enable(false);
	animList3->Show(false);

	//btnPauseMouth = new wxButton(this, ID_PAUSE_MOUTH, _("Pause"), wxPoint(160,100), wxSize(45,20));
	//btnPauseMouth->Show(false);

	speedMouthSlider = new wxSlider(this, ID_SPEED_MOUTH, 10, 0, 40, wxPoint(255,100), wxSize(100,38), wxSL_AUTOTICKS);
	speedMouthSlider->SetTickFreq(10, 1);
	speedMouthSlider->Show(false);

	speedMouthLabel = new wxStaticText(this, -1, _("Speed: 1.0x"), wxPoint(180,100), wxDefaultSize);
	speedMouthLabel->Show(false);
	// ---

	loopList = new wxComboBox(this, ID_LOOPS, wxString("0"), wxPoint(298, 10), wxSize(40,16), 10, strLoops, wxCB_READONLY, wxDefaultValidator, wxString("Loops")); //|wxCB_SORT); //wxPoint(66,10)
	btnAdd = new wxButton(this, ID_ADDANIM, _("Add"), wxPoint(338, 10), wxSize(36,20));

	skinList = new wxComboBox(this, ID_SKIN, _("Skin"), wxPoint(154,10), wxSize(144,16), 0, NULL, wxCB_READONLY);
	skinList->Show(FALSE);
	randomSkins = true;
	defaultDoodads = true;

	wmoList = new wxComboBox(this, ID_ITEMSET, _("Item set"), wxPoint(220,10), wxSize(128,16), 0, NULL, wxCB_READONLY);
	wmoList->Show(FALSE);
	wmoLabel = new wxStaticText(this, -1, _T(""), wxPoint(10,15), wxSize(192,16));
	wmoLabel->Show(FALSE);

	speedSlider = new wxSlider(this, ID_SPEED, 10, 1, 40, wxPoint(420,56), wxSize(100,38), wxSL_AUTOTICKS);
	speedSlider->SetTickFreq(10, 1);
	speedLabel = new wxStaticText(this, -1, _("Speed: 1.0x"), wxPoint(420,40), wxDefaultSize);

	frameLabel = new wxStaticText(this, -1, _("Frame: 0"), wxPoint(250,40), wxDefaultSize);
	frameSlider = new wxSlider(this, ID_FRAME, 1, 1, 10, wxPoint(250,56), wxSize(160,38), wxSL_AUTOTICKS);
	frameSlider->SetTickFreq(2, 1);

	btnPlay = new wxButton(this, ID_PLAY, _("Play"), wxPoint(10,40), wxSize(45,20));
	btnPause = new wxButton(this, ID_PAUSE, _("Pause"), wxPoint(53,40), wxSize(45,20));
	btnStop = new wxButton(this, ID_STOP, _("Stop"), wxPoint(96,40), wxSize(45,20));
	
	btnClear = new wxButton(this, ID_CLEARANIM, _("Clear"), wxPoint(10,64), wxSize(45,20));
	btnPrev = new wxButton(this, ID_PREVANIM, _T("<<"), wxPoint(53,64), wxSize(45,20));
	btnNext = new wxButton(this, ID_NEXTANIM, _T(">>"), wxPoint(96,64), wxSize(45,20));
	
	lockAnims = new wxCheckBox(this, ID_ANIM_LOCK, _("Lock Animations"), wxPoint(150,40), wxDefaultSize, 0);
	lockAnims->SetValue(true);
	bLockAnims = true;
	oldStyle = new wxCheckBox(this, ID_OLDSTYLE, _("Auto Animate"), wxPoint(150,60), wxDefaultSize, 0);
	bOldStyle = true;
	oldStyle->SetValue(true);

	model = NULL;
	wmo = NULL;
}

AnimControl::~AnimControl()
{
	// Free the memory the was allocated (fixed: memory leak)
	for (int i=0; i<skinList->GetCount(); i++) {
		TextureGroup *grp = (TextureGroup *)skinList->GetClientData(i);
		wxDELETE(grp);
	}

	texturemanager.clear();

	animList->Clear();
	skinList->Clear();
	
	animList->Destroy();
	skinList->Destroy();
}

void AnimControl::UpdateModel(Model *m)
{
	if (m == NULL)
		return;
	
	model = m;
	wmo = 0;

	selectedAnim = 0;
	selectedAnim2 = -1;
	selectedAnim3 = -1;

	animList->Clear();
	animList2->Clear();
	animList3->Clear();
	skinList->Clear();

	int useanim = -1;

	// Find any textures that exist for the model
	bool res = false;

	wxString fn(m->name);
	fn = fn.Lower();
	if (fn.substr(0,4) != "char") {

		if (fn.substr(0,8) == "creature")
			res = UpdateCreatureModel(m);

		if (!res) {
			if (fn.substr(0,4) == "item")
				res = UpdateItemModel(m);
		}
	}

	skinList->Show(res);

	// A small attempt at keeping the 'previous' animation that was selected when changing
	// the selected model via the model control.
	if (model->currentAnim > 0)
		useanim = model->currentAnim;

	if (model->charModelDetails.isChar) { // only display the "secondary" animation list if its a character
		animList2->Select(useanim);
		animList2->Show(true);
		lockAnims->Show(true);
		loopList->Show(true);
		btnAdd->Show(true);
	} else {
		animList2->Show(false);
		lockAnims->Show(false);
		loopList->Show(false);
		btnAdd->Show(false);
	}

	//
	if (model->animated && model->anims) {
		wxString strName;
		for (unsigned int i=0; i<model->header.nAnimations; i++) {
			const char *name=NULL;
			
			try {
				AnimDB::Record rec = animdb.getByAnimID(model->anims[i].animID);
				name = rec.getString(AnimDB::Name);
			} catch (AnimDB::NotFound) {
				name = "???";
			}
			
			strName = name;
			strName += wxString::Format(" [%i]", i);

			animList->Append( strName);
			if (model->charModelDetails.isChar) {
				animList2->Append(strName);
				animList3->Append(strName);
			}
			
			if ((useanim==-1) && (wxString(name)=="Stand")) 
				useanim = i;
		}

		if (useanim==-1)
			useanim = 0;
			//return;

		model->currentAnim = useanim;
		animList->Select(useanim);
		animList->Show(true);

		frameSlider->SetRange(model->anims[useanim].timeStart, model->anims[useanim].timeEnd);
		frameSlider->SetTickFreq(model->anims[useanim].playSpeed, 1);
		
		model->animManager->Set(0, useanim, 0);
		model->animManager->Play();
	}

	wmoList->Show(false);
	wmoLabel->Show(false);
}

void AnimControl::UpdateWMO(WMO *w, int group)
{
	if (!w || w->name.size()==0)
		return;

	bool newwmo = (oldname != w->name);
	oldname = w->name;

	model = 0;
	wmo = w;

	//Model *m = static_cast<Model*>(canvas->root->children[0]);

	//if (!m || m->anims==NULL)
	//	return;

	//m->animManager->Reset();

	frameSlider->SetRange(0, 10);
	frameSlider->SetTickFreq(2, 1);
	
	animList->Show(false);
	skinList->Show(false);
	loopList->Show(false);
	btnAdd->Show(false);
	
	if (newwmo) {
		// build itemset list
		wmoList->Clear();
		wmoList->Append(_T("(No doodads)"));

		for (size_t i=0; i<wmo->doodadsets.size(); i++) {
			wmoList->Append(wxString(wmo->doodadsets[i].name, *wxConvCurrent));
		}

		int sel = defaultDoodads ? 1 : 0;
		wmo->includeDefaultDoodads = defaultDoodads;
		wmoList->Select(sel);
		wmo->showDoodadSet(sel-1);
	}
	wmoList->Show(TRUE);

	// get wmo name or current wmogroup name/descr
	if (group>=-1 && group<wmo->nGroups) {
		std::string label;
		label = w->name.substr(w->name.find_last_of('\\')+1);
		if (group>=0) {
			label += " - " + wmo->groups[group].name;
			if (wmo->groups[group].desc.length()) {
				label += " - " + wmo->groups[group].desc;
			}
		}
		wmoLabel->SetLabel(wxString(label.c_str(), *wxConvCurrent));
	} else {
		wmoLabel->SetLabel(_("This group has been removed from the WMO"));
	}
	wmoLabel->Show(TRUE);
}


bool AnimControl::UpdateCreatureModel(Model *m)
{
	wxString fn(m->name);

	// replace M2 with MDX
	if (fn.Last() == '2') {
		fn[fn.Length()-1] = 'd';
		fn.Append("x");
	}

	TextureSet skins;

	// see if this model has skins
	try {
		CreatureModelDB::Record rec = modeldb.getByFilename(fn);
		// for character models, don't use skins
		if (rec.getUInt(CreatureModelDB::Type) != 4) {
			//TextureSet skins;
			unsigned int modelid = rec.getUInt(CreatureModelDB::ModelID);

			for (CreatureSkinDB::Iterator it = skindb.begin();  it!=skindb.end();  ++it) {
				if (it->getUInt(CreatureSkinDB::ModelID) == modelid) {
					TextureGroup grp;
					for (int i=0; i<TextureGroup::num; i++) {
						//const char *skin = it->getString(CreatureSkinDB::Skin + i);
						std::string skin(it->getString(CreatureSkinDB::Skin + i));
						
						grp.tex[i] = skin;
					}
					grp.base = 11;
					grp.count = 3;
					if (grp.tex[0].length() > 0) 
						skins.insert(grp);
				}
			}
			
			// Hard coded skin additions - missing from DBC ?
			if (fn == "Creature\\Furbolg\\furbolg.mdx") {
				TextureGroup grp;
				grp.tex[0] = "FurbolgSkinPanda";
				grp.tex[1] = "FurbolgStuffWhite";
				grp.base = 11;
				grp.count = 2;
				skins.insert(grp);
			} else if (fn == "Creature\\Murloc\\babymurloc.mdx") {
				TextureGroup grp;
				grp.tex[0] = "MurlocBabyBlue";
				grp.tex[1] = "TopHat";
				grp.tex[2] = "MurlocBabyReflect";
				grp.base = 11;
				grp.count = 3;
				skins.insert(grp);
				grp.tex[0] = "MurlocBabyGreen";
				skins.insert(grp);
				grp.tex[0] = "MurlocBabyOrange";
				skins.insert(grp);
				grp.tex[0] = "MurlocBabyPink";
				skins.insert(grp);
				grp.tex[0] = "MurlocBabyPurple";
				skins.insert(grp);
				grp.tex[0] = "MurlocBabyWhite";
				skins.insert(grp);
			} else if (fn == "Creature\\Felbeast\\felbeast.mdx") {
				TextureGroup grp;
				grp.tex[0] = "FelBeastSkinGreenBlack";
				grp.base = 11;
				grp.count = 1;
				skins.insert(grp);
			} else if (fn == "Creature\\Gryphon\\gryphon.mdx") {
				TextureGroup grp;
				grp.tex[0] = "Gryphon_Skin01Black";
				grp.tex[1] = "Gryphon_Skin02Black";
				grp.base = 11;
				grp.count = 2;
				skins.insert(grp);
				grp.tex[0] = "Gryphon_Skin01White";
				grp.tex[1] = "Gryphon_Skin02White";
				skins.insert(grp);
			} else if (fn == "Creature\\Lasher\\lasher.mdx") {
				TextureGroup grp;
				grp.tex[0] = "LasherSkinBlue";
				grp.base = 11;
				grp.count = 1;
				skins.insert(grp);
				grp.tex[0] = "LasherSkinGreen";
				skins.insert(grp);
				grp.tex[0] = "LasherSkinPurple";
				skins.insert(grp);
			} else if (fn == "Creature\\Minespider\\minespider.mdx") {
				TextureGroup grp;
				grp.tex[0] = "MineSpiderSkinViolet";
				grp.base = 11;
				grp.count = 1;
				skins.insert(grp);
			} else if (fn == "Creature\\Owl\\owl.mdx") {
				TextureGroup grp;
				grp.tex[0] = "OwlSpirit";
				grp.base = 11;
				grp.count = 1;
				skins.insert(grp);
			} else if (fn == "Creature\\Parrot\\parrot.mdx") {
				TextureGroup grp;
				grp.tex[0] = "ParrotSkinBrown";
				grp.base = 11;
				grp.count = 1;
				skins.insert(grp);
				grp.tex[0] = "ParrotSkinCrimson";
				skins.insert(grp);
				grp.tex[0] = "ParrotSkinlavender";
				skins.insert(grp);
			} else if (fn == "Creature\\Pterrordax\\pterrordax.mdx") {
				TextureGroup grp;
				grp.tex[0] = "PterrordaxSkinBrown";
				grp.tex[1] = "PterrordaxWingSkinBrown";
				grp.base = 11;
				grp.count = 2;
				skins.insert(grp);
			} else if (fn == "Creature\\Silithidscarab\\silithidscarab.mdx") {
				TextureGroup grp;
				grp.tex[0] = "SilithidScarabSkinGreen";
				grp.base = 11;
				grp.count = 1;
				skins.insert(grp);
			} else if (fn == "Creature\\Silithidflyer\\silithidflyer.mdx") {
				TextureGroup grp;
				grp.tex[0] = "SilithidFlyerSkinBlack";
				grp.base = 11;
				grp.count = 1;
				skins.insert(grp);
				grp.tex[0] = "SilithidFlyerSkinOrange";
				skins.insert(grp);
			}
			
			// hard-coded fixes.
			if (skins.empty() == true) {
				if (fn == "Creature\\Dwarfmalewarriorlight\\dwarfmalewarriorlight.mdx") {
					TextureGroup grp;
					grp.tex[0] = "DwarfMaleWarriorLightSkin";
					grp.base = 11;
					grp.count = 1;
					skins.insert(grp);
					grp.tex[0] = "DwarfMaleWarriorLightSkin2";
					grp.base = 11;
					grp.count = 1;
					skins.insert(grp);
				} else if (fn == "Creature\\Faeriedragon\\faeriedragon_ghost.mdx") {
					TextureGroup grp;
					grp.tex[0] = "FaerieDragon_Ghost";
					grp.tex[1] = "faeriewing_Ghost";
					grp.base = 11;
					grp.count = 2;
					skins.insert(grp);
				} else if (fn == "Creature\\Felbat\\felbat.mdx") {
					TextureGroup grp;
					grp.tex[0] = "BatSkin01";
					grp.tex[1] = "BatSkin02";
					grp.base = 11;
					grp.count = 2;
					skins.insert(grp);
					grp.tex[0] = "BatSkinBrown01";
					grp.tex[1] = "BatSkinBrown02";
					grp.base = 11;
					grp.count = 2;
					skins.insert(grp);
					grp.tex[0] = "BatSkinViolet01";
					grp.tex[1] = "BatSkinViolet02";
					grp.base = 11;
					grp.count = 2;
					skins.insert(grp);
					grp.tex[0] = "BatSkinWhite01";
					grp.tex[1] = "BatSkinWhite02";
					grp.base = 11;
					grp.count = 2;
					skins.insert(grp);
				} else if (fn == "Creature\\Highelf\\highelfmale_priest.mdx") {
					TextureGroup grp;
					grp.tex[0] = "HighElfMalePriest";
					grp.base = 11;
					grp.count = 1;
					skins.insert(grp);
				} else if (fn == "Creature\\Orcmalewarriorheavy\\orcmalewarriorheavy.mdx") {
					TextureGroup grp;
					grp.tex[1] = "OrcMaleSamuHairSkin";
					grp.tex[0] = "OrcNPCSkin";
					grp.base = 11;
					grp.count = 2;
					skins.insert(grp);
				} else if (fn == "Creature\\Seagiant\\seagiant.mdx") {
					TextureGroup grp;
					grp.tex[0] = "SeaGiantSkin01";
					grp.tex[1] = "SeaGiantSkin02";
					grp.base = 11;
					grp.count = 2;
					skins.insert(grp);
				} else if (fn == "Creature\\Sealion\\sealion.mdx") {
					TextureGroup grp;
					grp.tex[0] = "SeaLionSkin";
					grp.base = 11;
					grp.count = 1;
					skins.insert(grp);
				} else if (fn == "Creature\\Stormcrow\\stormcrow.mdx") {
					TextureGroup grp;
					grp.tex[0] = "StormCrowSkin";
					grp.base = 11;
					grp.count = 1;
					skins.insert(grp);
				} else if(fn == "Creature\\Humanmalewizard\\humanmalewizard.mdx") {
					TextureGroup grp;
					grp.tex[0] = "HumanMaleMerchant01Skin";
					grp.base = 11;
					grp.count = 1;
					skins.insert(grp);
					grp.tex[0] = "HumanMaleMerchant02Skin";
					grp.base = 11;
					grp.count = 1;
					skins.insert(grp);
				} 


			}
			return FillSkinSelector(skins);

		} else 
			return false;

	} catch (CreatureModelDB::NotFound) {
		// Try hardcoding some fixes for missing model info from the DBC
		if(fn == "Creature\\Dwarfmalewarriorlight\\dwarfmalewarriorlight_ghost.mdx") {
			TextureGroup grp;
			grp.tex[0] = "DwarfMaleWarriorLightSkin_Ghost";
			grp.base = 11;
			grp.count = 1;
			skins.insert(grp);
			return FillSkinSelector(skins);
		} else if(fn == "Creature\\Mounteddemonknight\\mounteddemonknight.mdx") {
			TextureGroup grp;
			grp.tex[0] = "MountedDemonKnightBlack_01";
			grp.tex[1] = "MountedDemonKnightBlack_02";
			grp.tex[2] = "MountedDemonKnightBlack_03";
			grp.base = 11;
			grp.count = 3;
			skins.insert(grp);
			return FillSkinSelector(skins);
		} else if(fn == "Creature\\Orcfemalewarriorlight\\orcfemale.mdx") {
			TextureGroup grp;
			grp.tex[0] = "OrcFemaleSkin";
			//grp.tex[1] = "OrcFemaleSkin";
			grp.base = 11;
			grp.count = 1;
			skins.insert(grp);
			return FillSkinSelector(skins);
		} else if(fn == "Creature\\Tigon\\tigon.mdx") {
			TextureGroup grp;
			grp.tex[0] = "TIGONSKIN_ORANGE";
			grp.tex[1] = "TIGONEYEGLOW";
			grp.base = 11;
			grp.count = 2;
			skins.insert(grp);
			grp.tex[0] = "CatManSkin_Orange";
			grp.tex[1] = "TIGONEYEGLOW";
			grp.base = 11;
			grp.count = 2;
			skins.insert(grp);
			grp.tex[0] = "CatManSkin_Black";
			grp.tex[1] = "TIGONEYEGLOW";
			grp.base = 11;
			grp.count = 2;
			skins.insert(grp);
			return FillSkinSelector(skins);
		} else if(fn == "Creature\\Humanmalepiratecaptain\\humanmalepiratecaptain_ghost.mdx") {
			TextureGroup grp;
			grp.tex[0] = "PirateCaptain02_Ghost";
			grp.base = 11;
			grp.count = 1;
			skins.insert(grp);
			return FillSkinSelector(skins);
		} else if(fn == "Creature\\Humanmalepiratecrewman\\humanmalepiratecrewman_ghost.mdx") {
			TextureGroup grp;
			grp.tex[0] = "PirateCrewman01_Ghost";
			grp.base = 11;
			grp.count = 1;
			skins.insert(grp);
			grp.tex[0] = "PirateCrewman02_Ghost";
			grp.base = 11;
			grp.count = 1;
			skins.insert(grp);
			grp.tex[0] = "PirateCrewman03_Ghost";
			grp.base = 11;
			grp.count = 1;
			skins.insert(grp);
			return FillSkinSelector(skins);
		}

		return false;
	}

}

bool AnimControl::UpdateItemModel(Model *m)
{
	//std::string fn = m->name;
	wxString fn = m->name;

	// change M2 to mdx
	if (fn[fn.Length()-1]=='2') {
		fn[fn.Length()-1] = 'd';
		fn.Append("x");
	}
	// just get the file name, exclude the path.
	fn = fn.AfterLast('\\');
	
	TextureSet skins;

	for (ItemDisplayDB::Iterator it=itemdb.begin(); it!=itemdb.end(); ++it) {
		if (fn.IsSameAs(it->getString(ItemDisplayDB::Model), false)) {
            TextureGroup grp;
			grp.base = 2;
			grp.count = 1;
			const char *skin = it->getString(ItemDisplayDB::Skin);
			grp.tex[0] = skin;
			if (grp.tex[0].length() > 0) 
				skins.insert(grp);
		}
		
		//if (!strcmp(it->getString(ItemDisplayDB::Model2), fn.c_str())) {
		if (fn.IsSameAs(it->getString(ItemDisplayDB::Model2), false)) {
            TextureGroup grp;
			grp.base = 2;
			grp.count = 1;
			const char *skin = it->getString(ItemDisplayDB::Skin2);
			grp.tex[0] = skin;
			if (grp.tex[0].length() > 0) 
				skins.insert(grp);
		}
	}

	return FillSkinSelector(skins);
}


bool AnimControl::FillSkinSelector(TextureSet &skins)
{
	if (skins.size() > 0) {
		int num = 0;
		// fill our skin selector
		for (TextureSet::iterator it = skins.begin(); it != skins.end(); ++it) {
			skinList->Append(wxString(it->tex[0].c_str(), *wxConvCurrent));
			TextureGroup *grp = new TextureGroup(*it);
			skinList->SetClientData(num++, grp);
		}

		int mySkin = randomSkins ? randint(0,(int)skins.size()-1) : 0;
		SetSkin(mySkin);

		bool ret = (skins.size() > 1);
		skins.clear();
		return ret;
	} else return false;
}


void AnimControl::OnButton(wxCommandEvent &event)
{
	if (!model)
		return;

	int id = event.GetId();

	if (id == ID_PLAY) {
		canvas->model->currentAnim = canvas->model->animManager->GetAnim();
		canvas->model->animManager->Play();
	} else if (id == ID_PAUSE) {
		canvas->model->animManager->Pause();
	} else if (id == ID_STOP) {
		canvas->model->animManager->Stop();
	} else if (id == ID_CLEARANIM) {
		canvas->model->animManager->Clear();
	} else if (id == ID_ADDANIM) {
		canvas->model->animManager->AddAnim(selectedAnim, loopList->GetSelection());
	} else if (id == ID_PREVANIM) {
		canvas->model->animManager->PrevFrame();
		SetAnimFrame(canvas->model->animManager->GetFrame());
	} else if (id == ID_NEXTANIM) {
		canvas->model->animManager->NextFrame();
		SetAnimFrame(canvas->model->animManager->GetFrame());
	}
}

void AnimControl::OnCheck(wxCommandEvent &event)
{
	if (event.GetId() == ID_OLDSTYLE)
		bOldStyle = event.IsChecked();
	else if (event.GetId() == ID_ANIM_LOCK) {
		bLockAnims = event.IsChecked();
		animList2->Enable(bLockAnims==false);
		if (bLockAnims)
			canvas->model->animManager->ClearSecondary();

		if (wxGetKeyState(WXK_SHIFT) && event.IsChecked()==false) {
			animList3->Enable(true);
			animList3->Show(true);
			speedMouthSlider->Show(true);
			speedMouthLabel->Show(true);
			//btnPauseMouth->Show(true);
		} else {
			animList3->Enable(false);
			animList3->Show(false);
			speedMouthSlider->Show(false);
			speedMouthLabel->Show(false);
			//btnPauseMouth->Show(false);
		}
	}
}

void AnimControl::OnAnim(wxCommandEvent &event)
{
	if (event.GetId() == ID_ANIM) {
		if (model) {
			wxString val = animList->GetValue();
			int first = val.Find('[')+1;
			int last = val.Find(']');
			selectedAnim = atoi(val.Mid(first, last-first).c_str());
			
			if (bLockAnims) {
				//selectedAnim2 = -1;
				animList2->SetSelection(event.GetSelection());
			}

			if (bOldStyle == true) {
				model->currentAnim = selectedAnim;
				model->animManager->Stop();
				model->animManager->Set(0, selectedAnim, loopList->GetSelection());
				model->animManager->Play();
				
				frameSlider->SetRange(model->anims[selectedAnim].timeStart, model->anims[selectedAnim].timeEnd);
				frameSlider->SetTickFreq(model->anims[selectedAnim].playSpeed, 1);
			}
		}

		//canvas->resetTime();
	} else if (event.GetId() == ID_ANIM_SECONDARY) {
		wxString val = animList2->GetValue();
		int first = val.Find('[')+1;
		int last = val.Find(']');
		selectedAnim2 = atoi(val.Mid(first, last-first).c_str());

		model->animManager->SetSecondary(selectedAnim2);
	} else if (event.GetId() == ID_ANIM_MOUTH) {
		wxString val = animList3->GetValue();
		int first = val.Find('[')+1;
		int last = val.Find(']');
		selectedAnim3 = atoi(val.Mid(first, last-first).c_str());

		//canvas->model->animManager->SetSecondary(selectedAnim2);
		model->animManager->SetMouth(event.GetSelection());
	}
}

void AnimControl::OnSkin(wxCommandEvent &event)
{
	if (model) {
		int sel = event.GetSelection();
		SetSkin(sel);
	}
}

void AnimControl::OnItemSet(wxCommandEvent &event)
{
	if (wmo) {
		int sel = event.GetSelection();
		// -1 for no doodads
		wmo->showDoodadSet(sel - 1);
	}
}

void AnimControl::OnSliderUpdate(wxCommandEvent &event)
{
	if (event.GetId() == ID_SPEED) {
		SetAnimSpeed(speedSlider->GetValue() / 10.0f);

	} else if (event.GetId() == ID_SPEED_MOUTH) {
		if (!model || !model->animManager)
			return;
		
		float speed = speedMouthSlider->GetValue() / 10.0f;
		model->animManager->SetMouthSpeed(speed);
		speedMouthLabel->SetLabel(wxString::Format(_T("Speed: %.1fx"), speed));

	} else if (event.GetId() == ID_FRAME)
		SetAnimFrame(frameSlider->GetValue());

}

void AnimControl::OnLoop(wxCommandEvent &event)
{
	if (bOldStyle == true) {
		canvas->model->animManager->Stop();
		canvas->model->animManager->Set(0, selectedAnim, loopList->GetSelection());
		canvas->model->animManager->Play();
	} 
}

void AnimControl::SetSkin(int num)
{
	TextureGroup *grp = (TextureGroup*) skinList->GetClientData(num);
	for (int i=0; i<grp->count; i++) {
		if (model->useReplaceTextures[grp->base+i]) {
			texturemanager.del(model->replaceTextures[grp->base+i]);
			TextureID def = texturemanager.add(makeSkinTexture(model->name.c_str(), grp->tex[i].c_str()));
			model->replaceTextures[grp->base+i] = def;
		}
	}

	skinList->Select(num);
}

wxString AnimControl::makeSkinTexture(const char *texfn, const char *skin)
{
	wxString res = texfn;
	size_t i = res.find_last_of('\\');
	res = res.substr(0,i+1);
	res.append(skin);
	res.append(_T(".blp"));
	return res;
}

int AnimControl::AddSkin(TextureGroup grp)
{
	skinList->Append(wxString(_T("Custom"), *wxConvCurrent));
	int count = skinList->GetCount() - 1;
	TextureGroup *group = new TextureGroup(grp);
	skinList->SetClientData(count, group);
	SetSkin(count);
	return count;
}

void AnimControl::SetAnimSpeed(float speed)
{
	if (!model || !model->animManager)
		return;

	model->animManager->SetSpeed(speed);
	
	speedLabel->SetLabel(wxString::Format(_T("Speed: %.1fx"), speed));
}


void AnimControl::SetAnimFrame(int frame)
{
	if (!canvas || !canvas->model || !canvas->model->animManager)
		return;

	canvas->model->animManager->SetFrame((unsigned int) frame);
	
	int frameNum = (frame - model->anims[model->currentAnim].timeStart);

	frameLabel->SetLabel(wxString::Format(_T("Frame: %i"), frameNum));
	frameSlider->SetValue(frame);
}

