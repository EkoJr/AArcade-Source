#include "cbase.h"

#include "c_prop_shortcut.h"
//#include "vgui/ISystem.h"
//#include "boost/regex.h"
//#include "baseviewport.h"
//#include "ienginevgui.h"
//#include "inetchannelinfo.h"
//#include "ICreateHotlink.h"
//#include "ienginevgui.h"
//#include <cassert>
//#include <string>

#include "c_anarchymanager.h"

#include <KeyValues.h>
#include "Filesystem.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

IMPLEMENT_CLIENTCLASS_DT(C_PropShortcutEntity, DT_PropShortcutEntity, CPropShortcutEntity)
	RecvPropBool(RECVINFO(m_bSlave)),
	RecvPropString(RECVINFO(m_itemId)),
END_RECV_TABLE()

C_PropShortcutEntity::C_PropShortcutEntity()
{
	m_bInitialized = false;
}

C_PropShortcutEntity::~C_PropShortcutEntity()
{
}

void C_PropShortcutEntity::Initialize()
{
	Vector absOrigin = this->GetAbsOrigin();
	object_t* pSpawningObject = g_pAnarchyManager->GetMetaverseManager()->GetSpawningObject();
	if (pSpawningObject)
		DevMsg("Abs origin (dist %f) is: %f %f %f vs %f %f %f\n", pSpawningObject->origin.DistTo(absOrigin), absOrigin.x, absOrigin.y, absOrigin.z, absOrigin.x, absOrigin.y, absOrigin.z);

	if (pSpawningObject->origin.DistTo(absOrigin) < 1.0)// GEEZ! not even position is set yet.  defer //&& !Q_strcmp(pSpawningObject->itemId.c_str(), m_itemId) )	// FIXME: accounts for rounding errors, but can produce false-positives!! // FIXME2: m_itemId isn't set until the 1st data update, der.  so this check is probably very unqualified.  too many false positive posibilities.
	{
		// THIS IS THE OBJECT THE CURRENT USER IS SPAWNING!!
		//Precache();
		SetModel("models\\cabinets\\two_player_arcade.mdl");// VarArgs("%s", this->GetModelName()));
		SetSolid(SOLID_NONE);
		SetSize(-Vector(100, 100, 100), Vector(100, 100, 100));
		//SetRenderMode(kRenderTransTexture);
		SetRenderMode(kRenderTransColor);
		SetRenderColorA(160);
		g_pAnarchyManager->ActivateObjectPlacementMode(this);
	}
	else
	{
		// This is a regular object that already existed or somebody else spawned
		//Precache();
		SetModel("models\\cabinets\\two_player_arcade.mdl");// VarArgs("%s", this->GetModelName()));
		//SetSolid(SOLID_NONE);
		SetSolid(SOLID_VPHYSICS);
		SetSize(-Vector(100, 100, 100), Vector(100, 100, 100));
		SetMoveType(MOVETYPE_VPHYSICS);

		SetRenderColorA(255);
		SetRenderMode(kRenderNormal);

		if (CreateVPhysics())
		{
			IPhysicsObject *pPhysics = this->VPhysicsGetObject();
			if (pPhysics)
			{
				pPhysics->EnableMotion(false);
			}
		}
	}

	m_bInitialized = true;
}

void C_PropShortcutEntity::OnPreDataChanged(DataUpdateType_t updateType)
{
	BaseClass::OnPreDataChanged(updateType);
}

void C_PropShortcutEntity::OnDataChanged(DataUpdateType_t updateType)
{
	//DevMsg("PreDataChangedUpdate: ")//DATA_UPDATE_DATATABLE_CHANGED
	if (!m_bInitialized )
		this->Initialize();
	else
	{
		// do nada
	}

	BaseClass::OnDataChanged(updateType);
}

void C_PropShortcutEntity::Release()
{
	BaseClass::Release();
}

void C_PropShortcutEntity::Precache(void)
{
	PrecacheModel("models\\cabinets\\two_player_arcade.mdl");
	BaseClass::Precache();
}

void C_PropShortcutEntity::Spawn()
{
	m_bInitialized = false;
	Precache();

	//	DevMsg("SPAWNING!!\n");
	/*
	object_t* pSpawningObject = g_pAnarchyManager->GetMetaverseManager()->GetSpawningObject();
	if (false && pSpawningObject->origin.DistTo(this->GetAbsOrigin()) < 1.0 )// GEEZ! not even position is set yet.  defer //&& !Q_strcmp(pSpawningObject->itemId.c_str(), m_itemId) )	// FIXME: accounts for rounding errors, but can produce false-positives!! // FIXME2: m_itemId isn't set until the 1st data update, der.  so this check is probably very unqualified.  too many false positive posibilities.
	{
		// THIS IS THE OBJECT THE CURRENT USER IS SPAWNING!!
		Precache();
		SetModel("models\\cabinets\\two_player_arcade.mdl");// VarArgs("%s", this->GetModelName()));
		SetSolid(SOLID_NONE);
		SetSize(-Vector(100, 100, 100), Vector(100, 100, 100));
		SetRenderMode(kRenderTransTexture);
		g_pAnarchyManager->ActivateObjectPlacementMode(this);
		BaseClass::Spawn();
	}
	else
	{
		// This is a regular object that already existed or somebody else spawned
		Precache();
		SetModel("models\\cabinets\\two_player_arcade.mdl");// VarArgs("%s", this->GetModelName()));
		//SetSolid(SOLID_NONE);
		SetSolid(SOLID_VPHYSICS);
		SetSize(-Vector(100, 100, 100), Vector(100, 100, 100));
		SetMoveType(MOVETYPE_VPHYSICS);

		if (CreateVPhysics())
		{
			IPhysicsObject *pPhysics = this->VPhysicsGetObject();
			if (pPhysics)
			{
				pPhysics->EnableMotion(false);
			}
		}
	}
	*/
	//	SetRenderMode(kRenderNormal);
	//SetUse(&CPropHotlinkEntity::UseFunc);

	BaseClass::Spawn();
}

std::string C_PropShortcutEntity::GetItemId()
{
	return std::string(m_itemId);
}

bool C_PropShortcutEntity::GetSlave()
{
	//DevMsg("Returning %i\n", m_bSlave);
	return m_bSlave;
}

void C_PropShortcutEntity::GetEmbeddedInstances(std::vector<C_EmbeddedInstance*>& embeddedInstances)
{
	// DETECT DYNAMIC TEXTURES
	const model_t* model = this->GetModel();

	IMaterial* pMaterials[1024];
	for (int x = 0; x < 1024; x++)
		pMaterials[x] = NULL;

	modelinfo->GetModelMaterials(model, 1024, &pMaterials[0]);

	//auto it;
	std::vector<C_EmbeddedInstance*>::iterator it;
	IMaterial* pMaterial;
	C_EmbeddedInstance* pEmbeddedInstance;
	for (int x = 0; x < 1024; x++)
	{
		if (pMaterials[x] && pMaterials[x]->HasProxy())
		{
			pMaterial = pMaterials[x];
			pEmbeddedInstance = g_pAnarchyManager->GetCanvasManager()->FindEmbeddedInstance(pMaterial);

			if (pEmbeddedInstance)
			{
				// make sure it isn't already on the list
				it = std::find(embeddedInstances.begin(), embeddedInstances.end(), pEmbeddedInstance);
				if (it == embeddedInstances.end())
					embeddedInstances.push_back(pEmbeddedInstance);
			}
		}
		//	Material: vgui/websurfacealt2
		//	Material: vgui/websurfacealt
		//	Material: vgui/websurfacealt5
		//	Material: vgui/websurfacealt5
		//	Material: vgui/websurfacealt7
	}
}