#pragma once
#include "TecLibs/3D/3DObject/BBox.h"
#include "CollisionBaseObject.h"
#include "CoreModifiable.h"
#include "CoreItem.h"
#include "JSonFileParser.h"


namespace Kigs
{

	namespace Collide
	{
		using namespace Kigs::Core;

		enum class SimpleShapeType
		{
			ST_Uninit = -1,
			ST_AABBox = 0,
			ST_AACylinder = 1,
			ST_Compound = 2,
			ST_AABBTree = 3,
			ST_Transform = 4
		};

		class SimpleShapeBase : public CollisionBaseObject
		{
		public:

			virtual SimpleShapeType getType() const = 0;
			virtual CoreItemSP	getCoreItemDesc() const = 0;

			static SimpleShapeBase* createFromDesc(CoreItemSP desc);

			/*template<typename PacketStream>
			bool Serialize(PacketStream& stream);*/

			virtual std::vector<SimpleShapeBase*> GetLeafShapes() { return { this }; }

			int mFaceIndex = 0;

		protected:
			virtual void initFromCoreItemDesc(CoreItemSP init) = 0;
			virtual CoreItemSP getBaseCoreItemDesc() const
			{
				CoreItemSP map = MakeCoreMap();
				map->set("Type", (int)getType());
				map->set("FaceIndex", mFaceIndex);
				return map;
			}
		};

		class CompoundShape : public SimpleShapeBase
		{
		public:
			SimpleShapeType getType() const override
			{
				return SimpleShapeType::ST_Compound;
			}

			~CompoundShape();

#ifdef KIGS_TOOLS
			void DrawDebug(const Hit& h, const Matrix3x4& mat) override;
#endif
			/*template<typename PacketStream>
			bool Serialize(PacketStream& stream)
			{
				SERIALIZE_VERSION(stream, 1);
				CHECK_SERIALIZE(serialize_object(stream, mTriangleIndex));

				size_t count = mShapes.size();
				CHECK_SERIALIZE(serialize_object(stream, count));
				if (!PacketStream::IsWriting)
					mShapes.resize(count);
				for (auto shape : mShapes)
				{
					std::string desc;
					if (PacketStream::IsWriting)
					{
						desc = CoreItemToJSon(shape->getCoreItemDesc());
					}
					CHECK_SERIALIZE(serialize_object(stream, desc));
					if (!PacketStream::IsWriting)
					{
						shape = SimpleShapeBase::createFromDesc(JSonToCoreItem(desc));
					}
				}
			}*/

			std::vector<SimpleShapeBase*> GetLeafShapes() override
			{
				std::vector<SimpleShapeBase*> result;
				for (auto& ss : mShapes)
				{
					auto vec = ss->GetLeafShapes();
					for (auto s : vec)
					{
						result.push_back(s);
					}
				}
				return result;
			}

			CoreItemSP	getCoreItemDesc() const override;

			virtual bool CallLocalRayIntersection(Hit& hit, const Point3D& start, const Vector3D& dir)  const override;
			virtual bool CallLocalRayIntersection(std::vector<Hit>& hit, const Point3D& start, const Vector3D& dir)  const override;

			std::vector<SP<SimpleShapeBase>> mShapes;

		protected:
			void initFromCoreItemDesc(CoreItemSP init) override;
		};

		class TransformShape : public SimpleShapeBase
		{
		public:
			SimpleShapeType getType() const override
			{
				return SimpleShapeType::ST_Transform;
			}

			~TransformShape();

			std::vector<SimpleShapeBase*> GetLeafShapes() override { if (mShape) return mShape->GetLeafShapes(); return {}; }

			/*template<typename PacketStream>
			bool Serialize(PacketStream& stream)
			{
				SERIALIZE_VERSION(stream, 1);
				CHECK_SERIALIZE(serialize_object(stream, mTriangleIndex));

				std::string desc;
				if (PacketStream::IsWriting)
				{
					desc = CoreItemToJSon(mShape->getCoreItemDesc());
				}
				CHECK_SERIALIZE(serialize_object(stream, desc));
				if (!PacketStream::IsWriting)
				{
					mShape = SimpleShapeBase::createFromDesc(JSonToCoreItem(desc));
				}
				CHECK_SERIALIZE(serialize_object(stream, mTransform));
				return true;
			}*/

#ifdef KIGS_TOOLS
			void DrawDebug(const Hit& h, const Matrix3x4& mat) override;
#endif

			CoreItemSP	getCoreItemDesc() const override;

			virtual bool CallLocalRayIntersection(Hit& hit, const Point3D& start, const Vector3D& dir)  const override;
			virtual bool CallLocalRayIntersection(std::vector<Hit>& hit, const Point3D& start, const Vector3D& dir)  const override;

			SP<SimpleShapeBase> mShape;
			mat3x4 mTransform{ mat3x4::Identity{} };
			mat3x4 mInvertTransform{ mat3x4::Identity{} };
		protected:
			void initFromCoreItemDesc(CoreItemSP init) override;
		};


		class AABBoxSimpleShape : public SimpleShapeBase
		{
		public:
			SimpleShapeType getType() const override
			{
				return SimpleShapeType::ST_AABBox;
			}

			/*template<typename PacketStream>
			bool Serialize(PacketStream& stream)
			{
				SERIALIZE_VERSION(stream, 1);
				CHECK_SERIALIZE(serialize_object(stream, mTriangleIndex));

				CHECK_SERIALIZE(serialize_object(stream, mBBox.m_Min));
				CHECK_SERIALIZE(serialize_object(stream, mBBox.m_Max));
				return true;
			}*/


			CoreItemSP	getCoreItemDesc() const override;

#ifdef KIGS_TOOLS
			void DrawDebug(const Hit& h, const Matrix3x4& mat) override;
#endif


			virtual bool CallLocalRayIntersection(Hit& hit, const Point3D& start, const Vector3D& dir)  const override;
			virtual bool CallLocalRayIntersection(std::vector<Hit>& hit, const Point3D& start, const Vector3D& dir)  const override;

			BBox mBBox;

		protected:
			void initFromCoreItemDesc(CoreItemSP init) override;
		};

		class AACylinderSimpleShape : public SimpleShapeBase
		{
		public:
			SimpleShapeType getType() const override
			{
				return SimpleShapeType::ST_AACylinder;
			}

			/*template<typename PacketStream>
			bool Serialize(PacketStream& stream)
			{
				SERIALIZE_VERSION(stream, 1);
				CHECK_SERIALIZE(serialize_object(stream, mTriangleIndex));

				CHECK_SERIALIZE(serialize_object(stream, mAxis[0]));
				CHECK_SERIALIZE(serialize_object(stream, mAxis[1]));
				CHECK_SERIALIZE(serialize_object(stream, mRadius));
				return true;
			}*/

#ifdef KIGS_TOOLS
			void DrawDebug(const Hit& h, const Matrix3x4& mat) override;
#endif

			CoreItemSP	getCoreItemDesc() const override;

			virtual bool CallLocalRayIntersection(Hit& hit, const Point3D& start, const Vector3D& dir)  const override;
			virtual bool CallLocalRayIntersection(std::vector<Hit>& hit, const Point3D& start, const Vector3D& dir)  const override;

			v3f			mAxis[2];
			float		mRadius;

		protected:
			void initFromCoreItemDesc(CoreItemSP init) override;
		};

	}
}