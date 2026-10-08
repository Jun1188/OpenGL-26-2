#pragma once

// 기존 과제 코드의 include와 이름을 유지하는 호환 헤더.
// 새 코드는 Graphics.h와 graphics 네임스페이스를 사용한다.
#include "Graphics.h"

using graphics::WINDOW_WIDTH;
using graphics::WINDOW_HEIGHT;
using graphics::MAX_ENTITIES;
using graphics::EntityId;
using graphics::GlColor;
using graphics::GLShape;
using graphics::GLFace;
using graphics::Transform3D;
using graphics::PlaneFigure;
using graphics::Polyhedron;
using graphics::Component_Poly;
using graphics::FaceId;
using graphics::DiagramSystem;
using graphics::Camera;
using graphics::CameraSystem;
using graphics::readTextFile;
using Entity = graphics::Scene;
using ShaderHandler = graphics::Renderer;
