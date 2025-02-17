#pragma once
#include <Transform.h>
#include <UnorderedVector.h>
#include "GameObject.h"

class CQuad
{
public:
	enum ENUM_QUADTYPE
	{
		ROOT,
		E_UPL,
		E_UPR,
		E_BOTL,
		E_BOTR,
	};

private:
	glm::vec3 m_origin;
	float m_width, m_depth;

	CQuad* m_parent;
	CQuad* UpL;
	CQuad* UpR;
	CQuad* BotL;
	CQuad* BotR;

	UnorderedVector<GameObject*> GOList;
	ENUM_QUADTYPE m_quadType;

public:
	CQuad(void);
	CQuad(glm::vec3 origin, float width = 0.0f, float depth = 0.0f, CQuad* parent = nullptr);
	~CQuad(void);

	size_t goCount(void) const;

	void UpLSetter(CQuad* newQuad);
	CQuad* UpLGetter(void);

	void UpRSetter(CQuad* newQuad);
	CQuad* UpRGetter(void);

	void BotLSetter(CQuad* newQuad);
	CQuad* BotLGetter(void);

	void BotRSetter(CQuad* newQuad);
	CQuad* BotRGetter(void);

	UnorderedVector<GameObject*> GOListGetter(void);

	void insertGO(GameObject* newObj);
	void removeGO(GameObject* obj2Remove);

	glm::vec3 OriginGetter(void) const;
	void DimensionGetter(int& width, int& height) const;

	bool isGOWithin(GameObject* obj) const;

	void TypeSetter(ENUM_QUADTYPE type);
	ENUM_QUADTYPE TypeGetter(void) const;

	CQuad* GetParent(void);
};

