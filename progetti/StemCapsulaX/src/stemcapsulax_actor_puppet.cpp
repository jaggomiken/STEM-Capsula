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
  Impl(b2WorldId wid, const b2Vec2& center, f32 scale
    , Options, const std::vector<Color>& vcolors);
 ~Impl();

   b2BodyId m_CreateGround(b2WorldId wid, f32 x, f32 y
    , const b2Vec2& size, f32 dens, Color);
   b2BodyId m_CreateGroundWWalls(b2WorldId wid, f32 x, f32 y
    , const b2Vec2& size, f32 dens, f32 wallh, Color);
  b2BodyId m_CreateBox(b2WorldId wid, f32 x, f32 y
    , const b2Vec2& size, f32 dens, Color);
  b2BodyId m_CreateCapsule(b2WorldId wid, f32 x, f32 y
    , const b2Vec2& p0, const b2Vec2& p1, f32 radius, f32 dens, Color);
  b2JointId m_CreateJoint(b2WorldId wid, f32 x, f32 y
    , b2BodyId b0, b2BodyId b1, b2Vec2 angles);

  std::vector<b2BodyId> vbodies;
  std::vector<b2JointId> vjoints;
  Options m_opt;
};

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
stemcapsulax::ActorPuppet::ActorPuppet(b2WorldId wid, const b2Vec2& center
  , f32 scale, const std::string& name, Options opt
  , const std::vector<Color>& vcolors)
: ActorBox2D  { wid, name }
, m_pImpl     {   nullptr }
{
  m_pImpl = new(std::nothrow) Impl{wid, center, scale, opt, vcolors};
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
void stemcapsulax::ActorPuppet::moveRelative(f32 x, f32 y)
{
  if (Options::kNOGROUNDBASE != m_pImpl->m_opt) {
    b2Body_SetLinearVelocity(m_pImpl->vbodies.back(), {x,y});
  } else {
    std::fprintf(stdout
      , "[PUPPET]: moveRelative() not supported for NOGROUNDBASE.\n");
  }
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
void stemcapsulax::ActorPuppet::behave(u64 what, const std::vector<f32>& v)
{
  f32 force_x = v.size() >= 1 ? v.at(0) : 0.0f;
  f32 force_y = v.size() >= 2 ? v.at(1) : 0.0f;
  if (what & u64(Behaviour::kMOVELEGS)) {
    b2Body_ApplyLinearImpulseToCenter(m_pImpl->vbodies[ 6], { -30000.0f * force_x, .0f }, true);
    b2Body_ApplyLinearImpulseToCenter(m_pImpl->vbodies[ 8], { +30000.0f * force_x, .0f }, true);
  }
  if (what & u64(Behaviour::kJUMP)) {
    b2Body_ApplyLinearImpulseToCenter(m_pImpl->vbodies[10], { .0f, -force_y }, true);
    b2Body_ApplyLinearImpulseToCenter(m_pImpl->vbodies[11], { .0f, -force_y }, true);
  }
  if (what & u64(Behaviour::kOPENLEGS)) {
    b2Body_ApplyLinearImpulseToCenter(m_pImpl->vbodies[10], { -20000.0f, .0f }, true);
    b2Body_ApplyLinearImpulseToCenter(m_pImpl->vbodies[11], { +20000.0f, .0f }, true);
  }
  if (what & u64(Behaviour::kLEFTARMUP)) {
    b2Body_ApplyLinearImpulseToCenter(m_pImpl->vbodies[ 2], { -100.0f, 100.0f * force_x }, true);
    b2Body_ApplyLinearImpulseToCenter(m_pImpl->vbodies[ 3], { -100.0f, 100.0f * force_x }, true);
  }
  if (what & u64(Behaviour::kRIGHTARMUP)) {
    b2Body_ApplyLinearImpulseToCenter(m_pImpl->vbodies[ 4], { +100.0f, 100.0f * force_x }, true);
    b2Body_ApplyLinearImpulseToCenter(m_pImpl->vbodies[ 5], { +100.0f, 100.0f * force_x }, true);
  }
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
void stemcapsulax::ActorPuppet::update()
{

}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
stemcapsulax::ActorPuppet::Impl::Impl(b2WorldId wid
  , const b2Vec2& c, f32 s /* scala */, Options opt
  , const std::vector<Color>& vcolors_in)
{
  m_opt = opt;

  auto vcolors = vcolors_in;
  if (vcolors.size() < 12) {
    std::fprintf(stderr
      , "[PUPPET]: Vector of colors shall have 12 colors!\n");
    for (size_t k = vcolors.size();k < 12;++k) {
      vcolors.push_back(Color{142,115, 25});
    }
  }

  auto bidUHead = m_CreateCapsule(wid, c.x + s*.0f, c.y - s*5.5f
    , { s*.0f, s*1.0f }, { s*.0f, s*-1.0f }, s*1.0f, 1.0f, vcolors.at( 0));
  auto bidTorso = m_CreateCapsule(wid, c.x + s*.0f, c.y + s*.0f
    , { s*.0f, s*3.0f }, { s*.0f, s*-2.0f }, s*2.0f, 1.2f, vcolors.at( 1));
  auto bidLeArT = m_CreateCapsule(wid, c.x - s*3.0f, c.y + s*.0f
    , { s*.0f, s*2.0f }, { s*.0f, s*-2.0f }, s*1.0f, 1.0f, vcolors.at( 2));
  auto bidLeArB = m_CreateCapsule(wid, c.x - s*3.0f, c.y + s*4.0f
    , { s*.0f, s*2.0f }, { s*.0f, s*-2.0f }, s*1.0f, 1.0f, vcolors.at( 3));
  auto bidRaArT = m_CreateCapsule(wid, c.x + s*3.0f, c.y + s*.0f
    , { s*.0f, s*2.0f }, { s*.0f, s*-2.0f }, s*1.0f, 1.0f, vcolors.at( 4));
  auto bidRaArB = m_CreateCapsule(wid, c.x + s*3.0f, c.y + s*4.0f
    , { s*.0f, s*2.0f }, { s*.0f, s*-2.0f }, s*1.0f, 1.0f, vcolors.at( 5));
  auto bidLeLeT = m_CreateCapsule(wid, c.x - s*1.0f, c.y + s*8.0f
    , { s*.0f, s*4.0f }, { s*.0f, s*-4.0f }, s*1.0f, 2.0f, vcolors.at( 6));
  auto bidLeLeB = m_CreateCapsule(wid, c.x - s*1.0f, c.y + s*14.5f
    , { s*.0f, s*3.5f }, { s*.0f, s*-3.5f }, s*1.0f, 2.0f, vcolors.at( 7));
  auto bidRaLeT = m_CreateCapsule(wid, c.x + s*1.0f, c.y + s*8.0f
    , { s*.0f, s*4.0f }, { s*.0f, s*-4.0f }, s*1.0f, 2.0f, vcolors.at( 8));
  auto bidRaLeB = m_CreateCapsule(wid, c.x + s*1.0f, c.y + s*14.5f
    , { s*.0f, s*3.5f }, { s*.0f, s*-3.5f }, s*1.0f, 2.0f, vcolors.at( 9));
  auto bidFootL = m_CreateBox(wid, c.x - s*1.5f, c.y + s*19.0f
    , { s*3.0f, s*1.0f }, 1000.0f, vcolors.at(10));
  auto bidFootR = m_CreateBox(wid, c.x + s*1.5f, c.y + s*19.0f
    , { s*3.0f, s*1.0f }, 1000.0f, vcolors.at(11));

  vbodies.push_back(bidUHead); //  0 testa
  vbodies.push_back(bidTorso); //  1 corpo
  vbodies.push_back(bidLeArT); //  2 braccio sx superiore
  vbodies.push_back(bidLeArB); //  3 braccio sx inferiore
  vbodies.push_back(bidRaArT); //  4 braccio dx superiore
  vbodies.push_back(bidRaArB); //  5 braccio dx inferiore
  vbodies.push_back(bidLeLeT); //  6 gamba sx superiore
  vbodies.push_back(bidLeLeB); //  7 gamba sx inferiore
  vbodies.push_back(bidRaLeT); //  8 gamba dx superiore
  vbodies.push_back(bidRaLeB); //  9 gamba dx inferiore
  vbodies.push_back(bidFootL); // 10 piede sinistro
  vbodies.push_back(bidFootR); // 11 piede destro

  // notare l'ordine dei corpi nelle joint per ottenere le dovute rotazioni relative
  auto jidHTO = m_CreateJoint(wid, c.x + s* .0f, c.y - s* 4.0f, bidUHead, bidTorso, {-0.10f,+0.10f});
  auto jidLTT = m_CreateJoint(wid, c.x - s*3.0f, c.y - s* 2.0f, bidLeArT, bidTorso, {-0.30f,+0.30f});
  auto jidLTB = m_CreateJoint(wid, c.x - s*3.0f, c.y + s* 2.0f, bidLeArB, bidLeArT, {-0.50f,+0.50f});
  auto jidRTT = m_CreateJoint(wid, c.x + s*3.0f, c.y - s* 2.0f, bidTorso, bidRaArT, {-0.30f,+0.30f});
  auto jidRTB = m_CreateJoint(wid, c.x + s*3.0f, c.y + s* 2.0f, bidRaArT, bidRaArB, {-0.50f,+0.50f});
  auto jidLLT = m_CreateJoint(wid, c.x - s*1.0f, c.y + s* 5.0f, bidLeLeT, bidTorso, {-0.20f,+0.20f});
  auto jidLLB = m_CreateJoint(wid, c.x - s*1.0f, c.y + s*12.0f, bidLeLeB, bidLeLeT, {-0.01f,+0.25f});
  auto jidRLT = m_CreateJoint(wid, c.x + s*1.0f, c.y + s* 5.0f, bidTorso, bidRaLeT, {-0.20f,+0.20f});
  auto jidRLB = m_CreateJoint(wid, c.x + s*1.0f, c.y + s*12.0f, bidRaLeT, bidRaLeB, {-0.01f,+0.25f});
  auto jidFTL = m_CreateJoint(wid, c.x - s*1.0f, c.y + s*19.0f, bidFootL, bidLeLeB, {-0.10f,+0.10f});
  auto jidFTR = m_CreateJoint(wid, c.x + s*1.0f, c.y + s*19.0f, bidRaLeB, bidFootR, {-0.10f,+0.10f});

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

  if (Options::kGROUNDBASENOWALLS == opt) {
    auto gndid = m_CreateGround(wid, c.x + s*.0f, c.y + s*20.0f
      , { s*1.5f*8.0f, 1.0f }, 10000.0f, {124, 76, 22});
    vbodies.push_back(gndid);
  }
  else if (Options::kGROUNDBASEWWALLS == opt) {
    auto gndid = m_CreateGroundWWalls(wid, c.x + s*.0f, c.y + s*20.0f
      , { s*2.0f*8.0f, 1.0f }, 10000.0f, s*9.0f, {124, 76, 22});
    vbodies.push_back(gndid);      
  }
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
b2BodyId stemcapsulax::ActorPuppet::Impl::m_CreateGround(b2WorldId wid
  , f32 x, f32 y, const b2Vec2& size, f32 density, Color c)
{
  b2BodyDef bd = b2DefaultBodyDef();
  bd.type = b2_kinematicBody;
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
  sd.material.friction = 0.1f;
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
b2BodyId stemcapsulax::ActorPuppet::Impl::m_CreateGroundWWalls(b2WorldId wid
  , f32 x, f32 y, const b2Vec2& size, f32 density, f32 wall_h, Color c)
{
  b2BodyDef bd = b2DefaultBodyDef();
  bd.type = b2_kinematicBody;
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
  sd.material.friction = 0.1f;
  sd.material.customColor =
      u32(c.a) << 24
    | u32(c.r) << 16
    | u32(c.g) <<  8
    | u32(c.b);
  b2CreatePolygonShape(bodyId, &sd, &box);
  b2Polygon leftwall = b2MakeOffsetBox(
      .5f, wall_h / 2.0f
    , { - (size.x / 2.0f) + .5f, -wall_h / 2.0f }, b2MakeRot(.0f));
  b2CreatePolygonShape(bodyId, &sd, &leftwall);
  b2Polygon rightwall = b2MakeOffsetBox(
      .5f, wall_h / 2.0f
    , { + (size.x / 2.0f) - .5f, -wall_h / 2.0f }, b2MakeRot(.0f));
  b2CreatePolygonShape(bodyId, &sd, &rightwall);
  return bodyId;
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
b2BodyId stemcapsulax::ActorPuppet::Impl::m_CreateBox(b2WorldId wid
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
b2BodyId stemcapsulax::ActorPuppet::Impl::m_CreateCapsule(b2WorldId wid
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
b2JointId stemcapsulax::ActorPuppet::Impl::m_CreateJoint(b2WorldId wid
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
  b2JointId jid = b2CreateRevoluteJoint(wid, &jd);
  return jid;
}
