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
#include "stemcapsulax_actor_puppet.h"
#include "stemcapsulax_status.h"
#include <vector>
#define _USE_MATH_DEFINES 
#include <cmath>

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * PRIVATE IMPLEMENTATION CLASS
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
class stemcapsulax::ActorPuppet::Impl {
public:
  Impl(b2WorldId wid, const b2Vec2& center);
 ~Impl();

  b2BodyId m_CreateBox(b2WorldId wid, f32 x, f32 y
    , const b2Vec2& size, f32 dens = 1.0f);
  b2BodyId m_CreateCapsule(b2WorldId wid, f32 x, f32 y
    , const b2Vec2& p0, const b2Vec2& p1, f32 radius, f32 dens = 1.0f);
  b2JointId m_CreateJoint(b2WorldId wid, f32 x, f32 y
    , b2BodyId b0, b2BodyId b1);

  std::vector<b2BodyId> vbodies;
  std::vector<b2JointId> vjoints;
};

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
stemcapsulax::ActorPuppet::ActorPuppet(b2WorldId wid, const b2Vec2& center
  , const std::string& name)
: ActorBox2D  { wid, name }
, m_pImpl     {   nullptr }
{
  m_pImpl = new(std::nothrow) Impl{wid, center };
  STEMCAPSULAX_CAPTURE_CPU(nullptr == m_pImpl, "Cannot allocate");
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
stemcapsulax::ActorPuppet::~ActorPuppet()
{
  delete m_pImpl;
  m_pImpl = nullptr;
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
void stemcapsulax::ActorPuppet::update()
{
  auto& st = Status::GetInstance();
  f32 t = st.data().sysinf.uSecondsElapsed;
  b2RevoluteJoint_SetMotorSpeed(m_pImpl->vjoints[3], std::cosf(0.5f * t));
  b2RevoluteJoint_SetMotorSpeed(m_pImpl->vjoints[4], std::sinf(0.5f * t));
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
stemcapsulax::ActorPuppet::Impl::Impl(b2WorldId wid, const b2Vec2& c)
{
  auto bidUHead = m_CreateCapsule(wid, c.x +  .0f, c.y - 5.5f
    , { .0f, 1.0f }, { .0f, -1.0f }, 1.0f);
  auto bidTorso = m_CreateCapsule(wid, c.x +  .0f, c.y +  .0f
    , { .0f, 3.0f }, { .0f, -2.0f }, 2.0f, 1.2);
  auto bidLeArT = m_CreateCapsule(wid, c.x - 3.0f, c.y +  .0f
    , { .0f, 2.0f }, { .0f, -2.0f }, 1.0f);
  auto bidLeArB = m_CreateCapsule(wid, c.x - 3.0f, c.y + 4.0f
    , { .0f, 2.0f }, { .0f, -2.0f }, 1.0f);
  auto bidRaArT = m_CreateCapsule(wid, c.x + 3.0f, c.y +  .0f
    , { .0f, 2.0f }, { .0f, -2.0f }, 1.0f);
  auto bidRaArB = m_CreateCapsule(wid, c.x + 3.0f, c.y + 4.0f
    , { .0f, 2.0f }, { .0f, -2.0f }, 1.0f);
  auto bidLeLeT = m_CreateCapsule(wid, c.x - 1.0f, c.y +  8.0f
    , { .0f, 4.0f }, { .0f, -4.0f }, 1.0f, 10.0f);
  auto bidLeLeB = m_CreateCapsule(wid, c.x - 1.0f, c.y + 14.5f
    , { .0f, 3.5f }, { .0f, -3.5f }, 1.0f, 20.0f);
  auto bidRaLeT = m_CreateCapsule(wid, c.x + 1.0f, c.y +  8.0f
    , { .0f, 4.0f }, { .0f, -4.0f }, 1.0f, 10.0f);
  auto bidRaLeB = m_CreateCapsule(wid, c.x + 1.0f, c.y + 14.5f
    , { .0f, 3.5f }, { .0f, -3.5f }, 1.0f, 20.0f);
  auto bidFootL = m_CreateBox(wid, c.x - 1.5f, c.y + 19.0f
    , { 3.0f, 1.0f }, 100.0f);
  auto bidFootR = m_CreateBox(wid, c.x + 1.5f, c.y + 19.0f
    , { 3.0f, 1.0f }, 100.0f);

  vbodies.push_back(bidUHead); // testa
  vbodies.push_back(bidTorso); // corpo
  vbodies.push_back(bidLeArT); // braccio sx superiore
  vbodies.push_back(bidLeArB); // braccio sx inferiore
  vbodies.push_back(bidRaArT); // braccio dx superiore
  vbodies.push_back(bidRaArB); // braccio dx inferiore
  vbodies.push_back(bidLeLeT); // gamba sx superiore
  vbodies.push_back(bidLeLeB); // gamba sx inferiore
  vbodies.push_back(bidRaLeT); // gamba dx superiore
  vbodies.push_back(bidRaLeB); // gamba dx inferiore
  vbodies.push_back(bidFootL); // piede sinistro
  vbodies.push_back(bidFootR); // piede destro

  auto jidHTO = m_CreateJoint(wid, c.x +  .0f, c.y -  4.0f, bidUHead, bidTorso);
  auto jidLTT = m_CreateJoint(wid, c.x - 3.0f, c.y -  2.0f, bidTorso, bidLeArT);
  auto jidLTB = m_CreateJoint(wid, c.x - 3.0f, c.y +  2.0f, bidLeArT, bidLeArB);
  auto jidRTT = m_CreateJoint(wid, c.x + 3.0f, c.y -  2.0f, bidTorso, bidRaArT);
  auto jidRTB = m_CreateJoint(wid, c.x + 3.0f, c.y +  2.0f, bidRaArT, bidRaArB);
  auto jidLLT = m_CreateJoint(wid, c.x - 1.0f, c.y +  5.0f, bidTorso, bidLeLeT);
  auto jidLLB = m_CreateJoint(wid, c.x - 1.0f, c.y + 12.0f, bidLeLeT, bidLeLeB);
  auto jidRLT = m_CreateJoint(wid, c.x + 1.0f, c.y +  5.0f, bidTorso, bidRaLeT);
  auto jidRLB = m_CreateJoint(wid, c.x + 1.0f, c.y + 12.0f, bidRaLeT, bidRaLeB);
  auto jidFTL = m_CreateJoint(wid, c.x - 1.0f, c.y + 19.0f, bidLeLeB, bidFootL);
  auto jidFTR = m_CreateJoint(wid, c.x + 1.0f, c.y + 19.0f, bidRaLeB, bidFootR);

  vjoints.push_back(jidHTO);
  vjoints.push_back(jidLTT);
  vjoints.push_back(jidLTB);
  vjoints.push_back(jidRTT);
  vjoints.push_back(jidRTB);
  vjoints.push_back(jidLLT);
  vjoints.push_back(jidLLB);
  vjoints.push_back(jidRLT);
  vjoints.push_back(jidRLB);
  vjoints.push_back(jidFTL);
  vjoints.push_back(jidFTR);
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
stemcapsulax::ActorPuppet::Impl::~Impl()
{

}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
b2BodyId stemcapsulax::ActorPuppet::Impl::m_CreateBox(b2WorldId wid
  , f32 x, f32 y, const b2Vec2& size, f32 density)
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
  b2CreatePolygonShape(bodyId, &sd, &box);
  return bodyId;
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
b2BodyId stemcapsulax::ActorPuppet::Impl::m_CreateCapsule(b2WorldId wid
  , f32 x, f32 y, const b2Vec2& p0, const b2Vec2& p1, f32 radius
  , f32 density)
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
  b2CreateCapsuleShape(bodyId, &sd, &capsule);
  return bodyId;
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
b2JointId stemcapsulax::ActorPuppet::Impl::m_CreateJoint(b2WorldId wid
  , f32 x, f32 y, b2BodyId b0, b2BodyId b1)
{
  b2Vec2 pivot = { x, y }; // in coordiante mondo
  b2RevoluteJointDef jd = b2DefaultRevoluteJointDef();
  jd.bodyIdA = b0;
  jd.bodyIdB = b1;
  jd.localAnchorA = b2Body_GetLocalPoint(b0, pivot);
  jd.localAnchorB = b2Body_GetLocalPoint(b1, pivot);
  jd.lowerAngle = -0.10f * M_PI;
  jd.upperAngle =  0.10f * M_PI;
  jd.enableLimit = true;
  jd.maxMotorTorque = 50.0f;
  jd.motorSpeed = 2.0f;
  jd.enableMotor = true;
  b2JointId jid = b2CreateRevoluteJoint(wid, &jd);
  return jid;
}
