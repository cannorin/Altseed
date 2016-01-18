﻿#include "asd.GeometryObject2D.h"

namespace asd
{
	extern ObjectSystemFactory* g_objectSystemFactory;

	CoreObject2D* GeometryObject2D::GetCoreObject() const
	{
		return m_coreObject.get();
	}

	CoreDrawnObject2D* GeometryObject2D::GetCoreDrawnObject() const
	{
		return m_coreObject.get();
	}

	GeometryObject2D::GeometryObject2D()
		: m_coreObject(nullptr)
		, texturePtr(nullptr)
		, shapePtr(nullptr)
	{
		m_coreObject = CreateSharedPtrWithReleaseDLL(g_objectSystemFactory->CreateGeometryObject2D());
	}

	GeometryObject2D::~GeometryObject2D()
	{

	}

	std::shared_ptr<Shape> GeometryObject2D::GetShape() const
	{
		return shapePtr;
	}

	void GeometryObject2D::SetShape(std::shared_ptr<Shape> shape)
	{
		shapePtr = shape;
		m_coreObject->SetShape(shape.get()->GetCoreShape().get());
	}

	void GeometryObject2D::SetTexture(std::shared_ptr<Texture2D> texture)
	{
		texturePtr = texture;
		m_coreObject->SetTexture(texture.get());
	}

	std::shared_ptr<Texture2D> GeometryObject2D::GetTexture() const
	{
		return texturePtr;
	}

	AlphaBlendMode GeometryObject2D::GetAlphaBlendMode() const
	{
		return m_coreObject->GetAlphaBlendMode();
	}

	void GeometryObject2D::SetAlphaBlendMode(AlphaBlendMode alphaBlend)
	{
		m_coreObject->SetAlphaBlendMode(alphaBlend);
	}

	void GeometryObject2D::SetCenterPosition(Vector2DF centerPosition)
	{
		m_coreObject->SetCenterPosition(centerPosition);
	}

	Vector2DF GeometryObject2D::GetCenterPosition() const
	{
		return m_coreObject->GetCenterPosition();
	}

	void GeometryObject2D::SetTextureFilterType(TextureFilterType textureFilterType)
	{
		m_coreObject->SetTextureFilterType(textureFilterType);
	}

	TextureFilterType GeometryObject2D::GetTextureFilterType() const
	{
		return m_coreObject->GetTextureFilterType();
	}
}