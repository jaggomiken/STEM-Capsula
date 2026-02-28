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
#include "stemcapsulax_box2d_proxy.h"
#include "stemcapsulax_box2d_fromimage.h"
#include "stemcapsulax_system.h"
#include "TaskScheduler.h"
#include <map>

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * MACROS
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
#define STEMCAPSULAX_TASKEXEC_MAX_TASKS                                  256
#define STEMCAPSULAX_SYSTEM_THREADS_NUM                                    2
#define STEMCAPSULAX_MAX_THREADS_NUM   \
  (std::thread::hardware_concurrency() \
     - (STEMCAPSULAX_SYSTEM_THREADS_NUM))

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * CLASS/STRUCT DECLARATION
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
namespace stemcapsulax {
  struct BodyInfo {
    b2BodyId bodyid;
    bool bIsGround;
  };
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * CLASS/STRUCT DECLARATION
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
class TaskForB2 : public enki::ITaskSet
{
public:
	void ExecuteRange(enki::TaskSetPartition range
    , uint32_t threadIndex) override {
		m_task(range.start, range.end, threadIndex, m_taskContext);
	}

	b2TaskCallback* m_task = nullptr;
	void* m_taskContext = nullptr;
};

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * CLASS/STRUCT DECLARATION
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
namespace stemcapsulax {
  struct TaskExecutionContext {
    enki::TaskScheduler scheduler;
    TaskForB2 aTasks[STEMCAPSULAX_TASKEXEC_MAX_TASKS];
    u32 uCurrentTaskNum;

    TaskExecutionContext() 
      : uCurrentTaskNum { 0 } 
    {
      std::printf("[BOX2DPROXY]: TaskExecutionContext constructor.\n");
      scheduler.Initialize(STEMCAPSULAX_MAX_THREADS_NUM);
    }

    ~TaskExecutionContext() {
      std::printf("[BOX2DPROXY]: TaskExecutionContext destructor.\n");
    }
  };
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * PRIVATE IMPLEMENTATION CLASS
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
class stemcapsulax::Box2DProxy::Impl {
public:
  Impl();
  
  b2WorldId m_wid;
  f32 m_fPixelPerMeter;
  f32 m_fWorldWidth;
  f32 m_fWorldHeight;
  uint32_t m_uUserCommand;
  std::map<u64,BodyInfo> m_mapBodies;
  Box2DBodyFromImage m_bfromimg;
  TaskExecutionContext m_ctxTaskExec;

  void m_HandleStartTouching(b2ShapeId, b2ShapeId);
  void   m_HandleEndTouching(b2ShapeId, b2ShapeId);
  void           m_handleHit(b2ShapeId, b2ShapeId);
};

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * FUNCTION
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
static u64 B2TOU64(b2BodyId b) {
  return *reinterpret_cast<const u64*>(&b);
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * FUNCTIONS (for task scheduler)
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
static void* b2CallbackEnqueueTask(b2TaskCallback* task
  , int32_t itemCount, int32_t minRange, void* pTaskCtx, void* pUserCtx);
static void  b2CallbackFinishTask(void* pTask, void* pUserCtx);

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
stemcapsulax::Box2DProxy::Box2DProxy()
: m_pImpl{ nullptr }
{
  m_pImpl = new(std::nothrow) Impl{};
  STEMCAPSULAX_CAPTURE_CPU(nullptr == m_pImpl, "Cannot allocate");
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
stemcapsulax::Box2DProxy::~Box2DProxy()
{
  delete m_pImpl;
  m_pImpl = nullptr;
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
void stemcapsulax::Box2DProxy::setUserCommand(uint32_t val)
{
  m_pImpl->m_uUserCommand = val;
  // QUESTA E' DA COMPLETARE
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
uint32_t stemcapsulax::Box2DProxy::currentUserCommand() const
{
  return m_pImpl->m_uUserCommand;
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
b2WorldId stemcapsulax::Box2DProxy::worldId() const
{
  return m_pImpl->m_wid;
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
f32 stemcapsulax::Box2DProxy::pixelPerMeter() const
{
  return m_pImpl->m_fPixelPerMeter;
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
void stemcapsulax::Box2DProxy::setWidth(f32 value)
{
  m_pImpl->m_fWorldWidth = value;
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
f32 stemcapsulax::Box2DProxy::width() const
{
  return m_pImpl->m_fWorldWidth;
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
void stemcapsulax::Box2DProxy::setHeight(f32 value)
{
  m_pImpl->m_fWorldHeight = value;
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
f32 stemcapsulax::Box2DProxy::height() const
{
  return m_pImpl->m_fWorldHeight;
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
void stemcapsulax::Box2DProxy::update()
{
  // dai il controllo a Box2D per simulare il mondo fisico
  b2World_Step(m_pImpl->m_wid, 0.0167 /* 60FPS */, 4);
  m_pImpl->m_ctxTaskExec.uCurrentTaskNum = 0; // reset contatore task

  // rileva le collisioni e dà il controllo al metodo (begin)
  const auto& ce = b2World_GetContactEvents(worldId());
  for (size_t i = 0;i < ce.beginCount;++i) {
    b2ContactBeginTouchEvent* be = ce.beginEvents + i;
    m_pImpl->m_HandleStartTouching(be->shapeIdA, be->shapeIdB);
  }

  // rileva le collisioni e dà il controllo al metodo (end)
  for (size_t i = 0;i < ce.endCount;++i) {
    b2ContactEndTouchEvent* ee = ce.endEvents + i;
    if (b2Shape_IsValid(ee->shapeIdA) && b2Shape_IsValid(ee->shapeIdB)) {
      m_pImpl->m_HandleEndTouching(ee->shapeIdA, ee->shapeIdB);
    }
  }
  
  // rileva gli hit e dà il controllo al metodo (end)
  for (size_t i = 0;i < ce.hitCount;++i) {
    b2ContactHitEvent* he = ce.hitEvents + i;
    m_pImpl->m_handleHit(he->shapeIdA, he->shapeIdB);
  }
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
void stemcapsulax::Box2DProxy::destroyInactiveBodies()
{
  std::printf("[BOX2DPROXY]: DESTROY INACTIVE BODIES...\n");
  std::vector<u64> vbodytorem; // da completare...
  
  for (auto item : m_pImpl->m_mapBodies) {
    if (!b2Body_IsAwake(item.second.bodyid) && !item.second.bIsGround) {
      std::printf(
        "[BOX2DPROXY]: Found (NON-GROUND) sleeping body, destroying...\n");
      b2DestroyBody(item.second.bodyid);
      vbodytorem.push_back(item.first);
    }
  }
  
  for (auto bid : vbodytorem) {
    m_pImpl->m_mapBodies.erase(bid);
  }
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
void stemcapsulax::Box2DProxy::createBodyGroundNoWalls(Color color)
{
  f32 fgroundsizw =  width() * 3.0f;
  f32 fgroundsizh = 0.5f;
  f32 fgroundposy = height() - fgroundsizh;
  
  b2BodyDef groundBodyDef = b2DefaultBodyDef();
  groundBodyDef.rotation = b2MakeRot(0.0f * (B2_PI / 180.0f));
  groundBodyDef.position = b2Vec2{
       fgroundsizw / 2.0f - width()
    ,  fgroundposy };

  // l'estensione di una forma box2d da specificare è metà dell'effettiva
    b2BodyId groundId = b2CreateBody(worldId(), &groundBodyDef);
   b2Polygon groundBox = b2MakeBox(fgroundsizw / 2.0f, fgroundsizh / 2.0f);
  b2ShapeDef groundShapeDef = b2DefaultShapeDef();
  groundShapeDef.density = 1000.0f;
  groundShapeDef.enableContactEvents = true;
  groundShapeDef.enableHitEvents = true;
  groundShapeDef.material.customColor =
      u32(color.a) << 24
    | u32(color.r) << 16
    | u32(color.g) <<  8
    | u32(color.b);
  b2CreatePolygonShape(groundId, &groundShapeDef, &groundBox);
  std::printf("[BOX2DPROXY]: <GROUNDNOWALLS>: Put shape @ %.2f,%.2f\n"
    , groundBodyDef.position.x, groundBodyDef.position.y);
  m_pImpl->m_mapBodies.insert({ B2TOU64(groundId), { groundId, true }});
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
void stemcapsulax::Box2DProxy::createBodyConcaveGround(Color color)
{
  f32 ffractionofground = 8.0f, foffright = .0f;
  f32 fgroundsizw =
    (width() - 0.03f) * (ffractionofground / 8.0f);
  f32 fgroundsizh = 0.5f;
  f32 fgroundposy = height() - fgroundsizh;
  
  b2BodyDef groundBodyDef = b2DefaultBodyDef();
  groundBodyDef.rotation = b2MakeRot(0.0f * (B2_PI / 180.0f));
  groundBodyDef.position = b2Vec2{
      fgroundsizw / 2.0f + foffright
    , fgroundposy };

  // l'estensione di una forma box2d da specificare è metà dell'effettiva
    b2BodyId groundId = b2CreateBody(worldId(), &groundBodyDef);
  b2ShapeDef groundShapeDef = b2DefaultShapeDef();
  groundShapeDef.density = 1000.0f;
  groundShapeDef.enableContactEvents = true;
  groundShapeDef.enableHitEvents = true;
  groundShapeDef.material.customColor =
      u32(color.a) << 24
    | u32(color.r) << 16
    | u32(color.g) <<  8
    | u32(color.b);
  b2Polygon dnleft = b2MakeOffsetBox(
      .5f, 20.0f
    , { -20.5f, 0.0f }, b2MakeRot(45.0f * DEG2RAD));
  b2CreatePolygonShape(groundId, &groundShapeDef, &dnleft);
  b2Polygon dnrght = b2MakeOffsetBox(
      .5f, 20.0f
    , {  +20.5f, 0.0f }, b2MakeRot(-45.0f * DEG2RAD));
  b2CreatePolygonShape(groundId, &groundShapeDef, &dnrght);
  b2Polygon leftwall = b2MakeOffsetBox(
      .5f, 80.0f
    , { - (width() / 2.0f) - 20.5f, -80.0f }, b2MakeRot(-45.0f * DEG2RAD));
  b2CreatePolygonShape(groundId, &groundShapeDef, &leftwall);
  b2Polygon rightwall = b2MakeOffsetBox(
      .5f, 80.0f
    , { + (width() / 2.0f) + 20.5f, -80.0f }, b2MakeRot(+45.0f * DEG2RAD));
  b2CreatePolygonShape(groundId, &groundShapeDef, &rightwall);
  std::printf("[BOX2DPROXY]: <CONCAVEGROUND>: Put shape @ %.2f,%.2f\n"
    , groundBodyDef.position.x, groundBodyDef.position.y);
  m_pImpl->m_mapBodies.insert({ B2TOU64(groundId), { groundId, true }});
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
void stemcapsulax::Box2DProxy::createBodyTestGround(Color color)
{
  f32 ffractionofground = 8.0f, foffright = .0f;
  f32 fgroundsizw =
    (width() - 0.03f) * (ffractionofground / 8.0f);
  f32 fgroundsizh = 0.5f;
  f32 fgroundposy = height() - fgroundsizh;
  
  b2BodyDef groundBodyDef = b2DefaultBodyDef();
  groundBodyDef.rotation = b2MakeRot(0.0f * (B2_PI / 180.0f));
  groundBodyDef.position = b2Vec2{
      fgroundsizw / 2.0f + foffright
    , fgroundposy };

  // l'estensione di una forma box2d da specificare è metà dell'effettiva
    b2BodyId groundId = b2CreateBody(worldId(), &groundBodyDef);
   b2Polygon groundBox = b2MakeBox(fgroundsizw / 2.0f, fgroundsizh / 2.0f);
  b2ShapeDef groundShapeDef = b2DefaultShapeDef();
  groundShapeDef.density = 1000.0f;
  groundShapeDef.enableContactEvents = true;
  groundShapeDef.enableHitEvents = true;
  groundShapeDef.material.customColor =
      u32(color.a) << 24
    | u32(color.r) << 16
    | u32(color.g) <<  8
    | u32(color.b);
  b2CreatePolygonShape(groundId, &groundShapeDef, &groundBox);
  b2Polygon leftwall = b2MakeOffsetBox(
      .5f, 80.0f
    , { - (width() / 2.0f) + .5f, -80.0f }, b2MakeRot(.0f));
  b2CreatePolygonShape(groundId, &groundShapeDef, &leftwall);
  b2Polygon rightwall = b2MakeOffsetBox(
      .5f, 80.0f
    , { + (width() / 2.0f) - .5f, -80.0f }, b2MakeRot(.0f));
  b2CreatePolygonShape(groundId, &groundShapeDef, &rightwall);
#if 0
  b2Polygon platformup = b2MakeOffsetBox(
      2.0f, .5f
    , { -8.0f, -7.5f }, b2MakeRot(.0f));
  b2CreatePolygonShape(groundId, &groundShapeDef, &platformup);
#endif
  std::printf("[BOX2DPROXY]: <TESTGROUND>: Put shape @ %.2f,%.2f\n"
    , groundBodyDef.position.x, groundBodyDef.position.y);
  m_pImpl->m_mapBodies.insert({ B2TOU64(groundId), { groundId, true }});
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
void stemcapsulax::Box2DProxy::createBodyGroundRect(
  f32 x, f32 y, f32 w, f32 h, Color color)
{
  b2BodyDef groundBodyDef = b2DefaultBodyDef();
  groundBodyDef.rotation = b2MakeRot(0.0f * (B2_PI / 180.0f));
  groundBodyDef.position = b2Vec2{ x, y };

  // l'estensione di una forma box2d da specificare è metà dell'effettiva
    b2BodyId groundId = b2CreateBody(worldId(), &groundBodyDef);
   b2Polygon groundBox = b2MakeBox(w / 2.0f, h / 2.0f);
  b2ShapeDef groundShapeDef = b2DefaultShapeDef();
  groundShapeDef.density = 1000.0f;
  groundShapeDef.enableContactEvents = true;
  groundShapeDef.enableHitEvents = true;
  groundShapeDef.material.customColor =
      u32(color.a) << 24
    | u32(color.r) << 16
    | u32(color.g) <<  8
    | u32(color.b);
  b2CreatePolygonShape(groundId, &groundShapeDef, &groundBox);
  std::printf("[BOX2DPROXY]: <GROUND>: Put shape @ %.2f,%.2f\n"
    , groundBodyDef.position.x, groundBodyDef.position.y);
  m_pImpl->m_mapBodies.insert({ B2TOU64(groundId), { groundId, true }});
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
void stemcapsulax::Box2DProxy::createBodyRectangle(
  f32 x, f32 y, f32 w, f32 h, Color color)
{
  f32 bodyw = w, bodyh = h;
  b2BodyDef bodyDef = b2DefaultBodyDef();
  bodyDef.type      = b2_dynamicBody;
  bodyDef.angularDamping = 0.6f;
  bodyDef.position  = b2Vec2{ x, y };
  bodyDef.rotation  = b2MakeRot(.0f * (B2_PI / 180.0f));
    b2BodyId     bodyId = b2CreateBody(worldId(), &bodyDef);
   b2Polygon dynamicBox = b2MakeBox(bodyw / 2.0f, bodyh / 2.0f);
  b2ShapeDef   shapeDef = b2DefaultShapeDef();
  shapeDef.enableContactEvents = true;
  shapeDef.enableHitEvents = true;
  shapeDef.density = .2f;
  shapeDef.material.restitution = 0.6f;
  shapeDef.material.friction = 0.3f;
  shapeDef.material.customColor =
      u32(color.a) << 24
    | u32(color.r) << 16
    | u32(color.g) <<  8
    | u32(color.b);
  b2CreatePolygonShape(bodyId, &shapeDef, &dynamicBox);
  
  std::printf("[BOX2DPROXY]: <RECT>: Put shape @ %.2f,%.2f\n"
    , bodyDef.position.x, bodyDef.position.y);
  m_pImpl->m_mapBodies.insert({ B2TOU64(bodyId), { bodyId, false }});
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
void stemcapsulax::Box2DProxy::createBodyCircle(f32 x, f32 y, f32 r, Color color)
{
  b2BodyDef bodyDef = b2DefaultBodyDef();
  bodyDef.type      = b2_dynamicBody;
  bodyDef.angularDamping = 0.6f;
  bodyDef.position  = b2Vec2{ x, y };
  bodyDef.rotation  = b2MakeRot(.0f * (B2_PI / 180.0f));
    b2BodyId     bodyId = b2CreateBody(worldId(), &bodyDef);
    b2Circle     circle = { {.0f, .0f}, r};
  b2ShapeDef   shapeDef = b2DefaultShapeDef();
  shapeDef.enableContactEvents = true;
  shapeDef.enableHitEvents = true;
  shapeDef.density = 1.0f;
  shapeDef.material.restitution = 0.3f;
  shapeDef.material.friction = 0.5f;
  shapeDef.material.customColor =
      u32(color.a) << 24
    | u32(color.r) << 16
    | u32(color.g) <<  8
    | u32(color.b);
  b2CreateCircleShape(bodyId, &shapeDef, &circle);
  
  std::printf("[BOX2DPROXY]: <CIRCLE>: Put shape @ %.2f,%.2f\n"
    , bodyDef.position.x, bodyDef.position.y);
  m_pImpl->m_mapBodies.insert({ B2TOU64(bodyId), { bodyId, false }});
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
void stemcapsulax::Box2DProxy::createBodyPolygon(
  f32 x, f32 y, const b2Vec2* v, size_t count, Color color)
{
  b2BodyDef bodyDef = b2DefaultBodyDef();
  bodyDef.type      = b2_dynamicBody;
  bodyDef.angularDamping = 0.6f;
  bodyDef.position  = b2Vec2{ x, y };
  bodyDef.rotation  = b2MakeRot(0.0f * (B2_PI / 180.0f));
    b2BodyId     bodyId = b2CreateBody(worldId(), &bodyDef);
      b2Hull       hull = b2ComputeHull(v, int(count));
   b2Polygon       poly = b2MakePolygon(&hull, .0f);
  b2ShapeDef   shapeDef = b2DefaultShapeDef();
  shapeDef.enableContactEvents = true;
  shapeDef.enableHitEvents = true;
  shapeDef.density = 2.0f;
  shapeDef.material.restitution = 0.8f;
  shapeDef.material.friction = 0.5f;
  shapeDef.material.customColor =
      u32(color.a) << 24
    | u32(color.r) << 16
    | u32(color.g) <<  8
    | u32(color.b);
  b2CreatePolygonShape(bodyId, &shapeDef, &poly);

  std::printf("[BOX2DPROXY]: <POLY>: Put shape @ %.2f,%.2f\n"
    , bodyDef.position.x, bodyDef.position.y);
  m_pImpl->m_mapBodies.insert({ B2TOU64(bodyId), { bodyId, false }});
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
void stemcapsulax::Box2DProxy::createBodyCapsule(f32 x, f32 y
  , const b2Vec2& p0, const b2Vec2& p1, f32 r, Color color)
{
  b2BodyDef bodyDef = b2DefaultBodyDef();
  bodyDef.type      = b2_dynamicBody;
  bodyDef.angularDamping = 0.6f;
  bodyDef.position  = b2Vec2{ x, y };
  bodyDef.rotation  = b2MakeRot(0.0f * (B2_PI / 180.0f));
    b2BodyId     bodyId = b2CreateBody(worldId(), &bodyDef);
   b2Capsule    capsule = { p0, p1, r };
  b2ShapeDef   shapeDef = b2DefaultShapeDef();
  shapeDef.enableContactEvents = true;
  shapeDef.enableHitEvents = true;
  shapeDef.density = 3.0f;
  shapeDef.material.restitution = 0.8f;
  shapeDef.material.friction = 0.2f;
  shapeDef.material.customColor =
      u32(color.a) << 24
    | u32(color.r) << 16
    | u32(color.g) <<  8
    | u32(color.b);
  b2CreateCapsuleShape(bodyId, &shapeDef, &capsule);

  std::printf("[BOX2DPROXY]: <CAPSULE>: Put shape @ %.2f,%.2f\n"
    , bodyDef.position.x, bodyDef.position.y);
  m_pImpl->m_mapBodies.insert({ B2TOU64(bodyId), { bodyId, false }});
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
void stemcapsulax::Box2DProxy::createExplosion(f32 x, f32 y, f32 energy)
{
  b2ExplosionDef edef = b2DefaultExplosionDef();
  edef.position = { x, y };
  edef.impulsePerLength = energy;
  edef.radius = 10.0f;
  edef.falloff = 12.0f;
  b2World_Explode(worldId(), &edef);
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
void stemcapsulax::Box2DProxy::createBodyFromImage(const std::string& fn)
{
  auto& cnv = Conv::GetInstance();
  auto xh = cnv.fWorldWidth / 2.0f;
  auto yh = .0f;

  std::vector<b2BodyId> vBodies;
  if (m_pImpl->m_bfromimg.loadImage(fn)) {
    if (!m_pImpl->m_bfromimg.bodyCreate(worldId(), xh, yh, vBodies)) {
      std::fprintf(stderr, "[BOX2DPROXY]: Failed to create body from image\n");
    } else {
      for (const auto& b : vBodies) {
        m_pImpl->m_mapBodies.insert({ B2TOU64(b), { b, false }});
      }
    }
  }
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
void stemcapsulax::Box2DProxy::removeBodiesOutsideRect(
  f32 x0, f32 y0, f32 x1, f32 y1)
{
  std::vector<b2BodyId> vtoremove;
  for (auto& b : m_pImpl->m_mapBodies) {
    auto pos = b2Body_GetPosition(b.second.bodyid);
    if ((pos.x < x0) || (pos.x > x1) || (pos.y < y0) || (pos.y > y1)) {
      vtoremove.push_back(b.second.bodyid);
    }
  }
  for (auto& b : vtoremove) {
    b2DestroyBody(b);
    m_pImpl->m_mapBodies.erase(B2TOU64(b));
  }
#if 0  
  if (!vtoremove.empty()) {
    std::fprintf(stdout
      , "[BOX2DPROXY]: Body destroyed %zu\n",vtoremove.size());
  }
#endif  
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
void stemcapsulax::Box2DProxy::removeBodiesYGreaterThan(f32 y)
{
  std::vector<b2BodyId> vtoremove;
  for (auto& b : m_pImpl->m_mapBodies) {
    auto pos = b2Body_GetPosition(b.second.bodyid);
    if (pos.y > y) { vtoremove.push_back(b.second.bodyid); }
  }
  for (auto& b : vtoremove) {
    b2DestroyBody(b);
    m_pImpl->m_mapBodies.erase(B2TOU64(b));
  }
#if 0  
  if (!vtoremove.empty()) {
    std::fprintf(stdout
      , "[BOX2DPROXY]: Body destroyed %zu\n",vtoremove.size());
  }
#endif  
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
void stemcapsulax::Box2DProxy::removeBodiesXGreaterThan(f32 x)
{

}


/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
stemcapsulax::Box2DProxy::Impl::Impl()
: m_fPixelPerMeter{    1.0f }
, m_fWorldWidth   {    1.0f }
, m_fWorldHeight  {    1.0f }
, m_uUserCommand  {      0  }
{
  /*
   * Questo codice stabilisce che il numero di pixel per metro usati dal
   * simulatore fisico Box2D. In genere conviene dare a questo parametro
   * l'altezza del personaggio principale. Ma la documentazione suggerisce
   * di lasciare il valore predefinito di 1.0f.
   */
  f32 fPixelPerMeter = 1.0f;
	b2SetLengthUnitsPerMeter(fPixelPerMeter);
  m_fPixelPerMeter = fPixelPerMeter;

  /*
   * Box2D 3.x supporta il multithreading ma si affida al software in cui
   * deve integrarsi per schedulare i task di volta in volta. Questo viene
   * fatto mediante delle callback che vanno configurate nel world.
   */
      b2Vec2  gravity = {0.0f, 9.8f * fPixelPerMeter};
  b2WorldDef worldDef = b2DefaultWorldDef();
  worldDef.gravity         = gravity;
  worldDef.enableSleep     = true;
  worldDef.enqueueTask     = b2CallbackEnqueueTask;
  worldDef.finishTask      = b2CallbackFinishTask;
  worldDef.workerCount     = STEMCAPSULAX_MAX_THREADS_NUM;
  worldDef.userTaskContext = &m_ctxTaskExec;
  m_wid = b2CreateWorld(&worldDef);
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
void stemcapsulax::Box2DProxy::Impl::m_HandleStartTouching(
  b2ShapeId sa, b2ShapeId sb)
{
  auto ba = b2Shape_GetBody(sa);
  auto bb = b2Shape_GetBody(sb);
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
void stemcapsulax::Box2DProxy::Impl::m_HandleEndTouching(
  b2ShapeId sa, b2ShapeId sb)
{
  auto ba = b2Shape_GetBody(sa);
  auto bb = b2Shape_GetBody(sb);
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
void stemcapsulax::Box2DProxy::Impl::m_handleHit(b2ShapeId sa, b2ShapeId sb)
{
  auto ba = b2Shape_GetBody(sa);
  auto bb = b2Shape_GetBody(sb);
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
stemcapsulax::Conv& stemcapsulax::Conv::GetInstance()
{
  static Conv c; return c;
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * STATIC FUNCTION DEF
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
static void* b2CallbackEnqueueTask(b2TaskCallback* pTaskCb
  , int32_t itemCount, int32_t minRange, void* pTaskCtx, void* pUserCtx)
{
  auto* pCtx = static_cast<stemcapsulax::TaskExecutionContext*>(pUserCtx);
  if (pCtx->uCurrentTaskNum < STEMCAPSULAX_TASKEXEC_MAX_TASKS) {
    auto& t = pCtx->aTasks[pCtx->uCurrentTaskNum];
    t.m_SetSize     = itemCount;
    t.m_MinRange    = minRange;
    t.m_task        = pTaskCb;
    t.m_taskContext = pTaskCtx;
    pCtx->scheduler.AddTaskSetToPipe(&t);
    pCtx->uCurrentTaskNum++;
    return &t;
  }
  else {
    pTaskCb(0, itemCount, 0, pTaskCtx);
    return nullptr;
  }
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * STATIC FUNCTION DEF
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
static void b2CallbackFinishTask(void* pTask, void* pUserCtx)
{
	if (nullptr != pTask)	{
		auto* pT = static_cast<TaskForB2*>(pTask);
		auto* pCtx = static_cast<stemcapsulax::TaskExecutionContext*>(pUserCtx);
		pCtx->scheduler.WaitforTask(pT);
	}
}
