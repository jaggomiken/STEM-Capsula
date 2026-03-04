/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * STEMCAPSULAX
 * (C) 2025 Copyright by Michele Iacobellis
 * A project for enjoying computer graphics in C++1x.
 * 
 * This file is part of STEMCAPSULAX.
 *
 * STEMCAPSULAX is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * STEMCAPSULAX is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with STEMCAPSULAX. If not, see <http://www.gnu.org/licenses/>.
 * 
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
#include "stemcapsulax_actor_damper.h"
#include "stemcapsulax_status.h"
#include <vector>
#define _USE_MATH_DEFINES 
#include <cmath>

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * PRIVATE IMPLEMENTATION CLASS
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
class stemcapsulax::ActorDamper::Impl {
public:
  Impl(b2WorldId wid, const b2Vec2& center, f32 scalex, f32 scaley);
 ~Impl();

  b2BodyId m_CreateBox(b2WorldId wid, f32 x, f32 y
    , const b2Vec2& size, f32 dens, Color);
  b2BodyId m_CreateCapsule(b2WorldId wid, f32 x, f32 y
    , const b2Vec2& p0, const b2Vec2& p1, f32 radius, f32 dens, Color);
  b2JointId m_CreateJoint(b2WorldId wid, f32 x, f32 y
    , b2BodyId b0, b2BodyId b1, b2Vec2 angles);

  std::vector<b2BodyId> vbodies;
  std::vector<b2JointId> vjoints;
};

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
stemcapsulax::ActorDamper::ActorDamper(b2WorldId wid, const b2Vec2& center
  , f32 scalex, f32 scaley, const std::string& name)
: ActorBox2D  { wid, name }
, m_pImpl     {   nullptr }
{
  m_pImpl = new(std::nothrow) Impl{wid, center, scalex, scaley };
  STEMCAPSULAX_CAPTURE_CPU(nullptr == m_pImpl, "Cannot allocate");
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
stemcapsulax::ActorDamper::~ActorDamper()
{
  delete m_pImpl;
  m_pImpl = nullptr;
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
void stemcapsulax::ActorDamper::behave(u64, const std::vector<f32>& v)
{
  f32 f = v.size() >= 1 ? v.at(0) : 1.0f;
  f32 fscale = 50000.0f, val = -std::logf(f);
  b2Body_ApplyLinearImpulseToCenter(m_pImpl->vbodies[0]
    , { .0f, -val*fscale }, true);
  // std::fprintf(stdout, "[DAMPER %s]: %f\n", name().c_str(), f);
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
void stemcapsulax::ActorDamper::update()
{

}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
stemcapsulax::ActorDamper::Impl::Impl(b2WorldId wid
  , const b2Vec2& c, f32 sx, f32 sy)
{
  f32 dens = 5.0f;
  auto bidTopPad =     m_CreateBox(wid, c.x + sx* .0f, c.y - sy* 4.5f
    , { sx* 6.0f, sy*1.0f }, dens, Color{124, 76, 22});
  auto bidLeDn01 = m_CreateCapsule(wid, c.x - sx* .0f, c.y - sy* 3.0f
    , { sx*-2.5f, sy*-1.5f }, { sx* 2.5f, sy* 1.5f }, sx* .5f, dens, Color{124, 76, 22});
  auto bidLeDn02 = m_CreateCapsule(wid, c.x - sx* .0f, c.y - sy*  .0f
    , { sx*-2.5f, sy*-1.5f }, { sx* 2.5f, sy* 1.5f }, sx* .5f, dens, Color{124, 76, 22});
  auto bidMidPad =     m_CreateBox(wid, c.x + sx* .0f, c.y - sy* 1.5f
    , { sx* 6.0f, sy*1.0f }, dens, Color{124, 76, 22});
  auto bidRaUp01 = m_CreateCapsule(wid, c.x - sx* .0f, c.y - sy* 3.0f
    , { sx*-2.5f, sy* 1.5f }, { sx* 2.5f, sy*-1.5f }, sx* .5f, dens, Color{124, 76, 24});
  auto bidRaUp02 = m_CreateCapsule(wid, c.x - sx* .0f, c.y - sy*  .0f
    , { sx*-2.5f, sy* 1.5f }, { sx* 2.5f, sy*-1.5f }, sx* .5f, dens, Color{124, 76, 22});
  auto bidDowPad =     m_CreateBox(wid, c.x + sx* .0f, c.y + sy* 1.5f
    , { sx* 6.0f, sy*1.0f }, 200000.0f, Color{124, 76, 22});

  vbodies.push_back(bidTopPad); //  0 pad superiore
  vbodies.push_back(bidLeDn01); //  1 fascia alta da sx a dx verso il basso
  vbodies.push_back(bidRaUp01); //  2 fascia alta da dx a sx verso il basso
  vbodies.push_back(bidMidPad); //  3 pad superiore
  vbodies.push_back(bidLeDn02); //  4 fascia bassa da sx a dx verso il basso
  vbodies.push_back(bidRaUp02); //  5 fascia bassa da dx a sx verso il basso
  vbodies.push_back(bidDowPad); //  6 pad inferiore

  // notare l'ordine dei corpi nelle joint per ottenere le dovute rotazioni relative
  auto jidTPLD1 = m_CreateJoint(wid, c.x - sx*2.5f, c.y - sy* 4.5f, bidLeDn01, bidTopPad, {-0.00f,+0.25f});
  auto jidTPRU1 = m_CreateJoint(wid, c.x + sx*2.5f, c.y - sy* 4.5f, bidTopPad, bidRaUp01, {-0.00f,+0.25f});
  auto jidRU1MD = m_CreateJoint(wid, c.x - sx*2.5f, c.y - sy* 1.5f, bidMidPad, bidRaUp01, {-0.00f,+0.25f});
  auto jidLD1MD = m_CreateJoint(wid, c.x + sx*2.5f, c.y - sy* 1.5f, bidLeDn01, bidMidPad, {-0.00f,+0.25f});
  auto jidLD2MD = m_CreateJoint(wid, c.x - sx*2.5f, c.y - sy* 1.5f, bidLeDn02, bidMidPad, {-0.00f,+0.25f});
  auto jidLD2DN = m_CreateJoint(wid, c.x + sx*2.5f, c.y + sy* 1.5f, bidLeDn02, bidDowPad, {-0.00f,+0.25f});
  auto jidRU2MD = m_CreateJoint(wid, c.x + sx*2.5f, c.y - sy* 1.5f, bidMidPad, bidRaUp02, {-0.00f,+0.25f});
  auto jidRU2DN = m_CreateJoint(wid, c.x - sx*2.5f, c.y + sy* 1.5f, bidDowPad, bidRaUp02, {-0.00f,+0.25f});

  vjoints.push_back(jidTPLD1);
  vjoints.push_back(jidTPRU1);
  vjoints.push_back(jidRU1MD);
  vjoints.push_back(jidLD1MD);
  vjoints.push_back(jidLD2MD);
  vjoints.push_back(jidLD2DN);
  vjoints.push_back(jidRU2MD);
  vjoints.push_back(jidRU2DN);
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
stemcapsulax::ActorDamper::Impl::~Impl()
{

}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
b2BodyId stemcapsulax::ActorDamper::Impl::m_CreateBox(b2WorldId wid
  , f32 x, f32 y, const b2Vec2& size, f32 density, Color c)
{
  b2BodyDef bd = b2DefaultBodyDef();
  bd.type = b2_dynamicBody;
  bd.angularDamping = 0.6f;
  bd.position = b2Vec2{ x, y };
  bd.rotation = b2MakeRot(0.0f * (B2_PI / 180.0f));
  b2BodyId bodyId = b2CreateBody(wid, &bd);
  b2Polygon box = b2MakeBox(size.x / 2.0f, size.y / 2.0f);
  b2ShapeDef sd = b2DefaultShapeDef();
  sd.enableContactEvents = true;
  sd.enableHitEvents = true;
  sd.density = density;
  sd.material.restitution = 0.8f;
  sd.material.friction = 0.2f;
  sd.material.customColor =
      u32(c.a) << 24
    | u32(c.r) << 16
    | u32(c.g) <<  8
    | u32(c.b);
  b2CreatePolygonShape(bodyId, &sd, &box);
  return bodyId;
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
b2BodyId stemcapsulax::ActorDamper::Impl::m_CreateCapsule(b2WorldId wid
  , f32 x, f32 y, const b2Vec2& p0, const b2Vec2& p1, f32 radius
  , f32 density, Color c)
{
  b2BodyDef bd = b2DefaultBodyDef();
  bd.type = b2_dynamicBody;
  bd.angularDamping = 0.6f;
  bd.position = b2Vec2{ x, y };
  bd.rotation = b2MakeRot(0.0f * (B2_PI / 180.0f));
  b2BodyId bodyId = b2CreateBody(wid, &bd);
  b2Capsule capsule = { p0, p1, radius };
  b2ShapeDef sd = b2DefaultShapeDef();
  sd.enableContactEvents = true;
  sd.enableHitEvents = true;
  sd.density = density;
  sd.material.restitution = 0.8f;
  sd.material.friction = 0.2f;
  sd.material.customColor =
      u32(c.a) << 24
    | u32(c.r) << 16
    | u32(c.g) <<  8
    | u32(c.b);
  b2CreateCapsuleShape(bodyId, &sd, &capsule);
  return bodyId;
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
b2JointId stemcapsulax::ActorDamper::Impl::m_CreateJoint(b2WorldId wid
  , f32 x, f32 y, b2BodyId b0, b2BodyId b1, b2Vec2 angles)
{
  b2Vec2 pivot = { x, y }; // in coordiante mondo
  b2RevoluteJointDef jd = b2DefaultRevoluteJointDef();
  jd.bodyIdA = b0;
  jd.bodyIdB = b1;
  jd.localAnchorA = b2Body_GetLocalPoint(b0, pivot);
  jd.localAnchorB = b2Body_GetLocalPoint(b1, pivot);
  jd.lowerAngle = angles.x * M_PI;
  jd.upperAngle = angles.y * M_PI;
  jd.enableLimit = true;
  jd.maxMotorTorque = 10.0f;
  jd.motorSpeed = 0.0f;
  jd.enableMotor = true;
  jd.enableSpring = true;
  jd.hertz = 100;
  b2JointId jid = b2CreateRevoluteJoint(wid, &jd);
  return jid;
}
