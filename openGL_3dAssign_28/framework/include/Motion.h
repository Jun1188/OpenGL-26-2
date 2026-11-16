#pragma once
#include "Transform.h"
#include <map>
#include <initializer_list>

namespace graphics {
using MotionId = std::uint64_t;
using LayerId = std::uint64_t;
enum class TransformSpace { World, Local };
enum class Playback { Once, Loop, PingPong };

struct MotionSample {
    glm::mat4 parent{1.0f};
    glm::mat4 local{1.0f};
};

using MotionAction = std::function<bool(double, const MotionSample&)>;

struct RotationMotion {
    AxisLine axis;
    double degreesPerSecond = 45;
    double initialDegrees = 0;
    TransformSpace space = TransformSpace::Local;
};

struct PathMotion {
    PathFunction path;
    InterpolationFunction interpolation = PathSystem::linear;
    double durationSeconds = 1;
    double rate = 1;
    double phase = 0;
    Playback playback = Playback::Loop;
    bool relativeToStart = false;
    TransformSpace space = TransformSpace::World;
};

struct LinearMotion {
    glm::vec3 from{0}, to{1, 0, 0};
    double durationSeconds = 1;
    Playback playback = Playback::Once;
    TransformSpace space = TransformSpace::Local;
};

struct PendulumMotion {
    AxisLine axis{{0, 0, 0}, {1, 0, 0}};
    double amplitudeDegrees = 25;
    double periodSeconds = 3;
    double phaseDegrees = 0;
    double offsetDegrees = 0;
    TransformSpace space = TransformSpace::Local;
};

struct MotionStep {
    // The callback receives seconds since this step started, not frame delta time.
    std::function<glm::mat4(double)> transform;
    TransformSpace space = TransformSpace::World;
    double startSeconds = 0;
    std::optional<double> durationSeconds;
    bool enabled = true;
    MotionAction action; // A step contains either transform or action.
};

class Motion {
    std::vector<MotionStep> steps_;


    static bool validStep(const MotionStep& step);


    std::optional<MotionSample> evaluate(double seconds, bool actions) const;


public:
    /// [API] 모션 단계 배열의 끝에 유효한 단계를 추가한다. 자동 순차 배치는 then을 사용한다.
    bool append(const MotionStep& step);


    /// [API] 주어진 인덱스에 모션 단계를 삽입한다.
    bool insert(std::size_t index, const MotionStep& step);


    /// [API] 변환 사이의 인덱스에 의미 동작 또는 람다·함수 포인터를 삽입한다.
    bool insertAction(std::size_t index, MotionAction action, double startSeconds = 0,
                      std::optional<double> durationSeconds = std::nullopt);


    /// [API] 주어진 인덱스의 모션 단계를 새 단계로 교체한다.
    bool replace(std::size_t index, const MotionStep& step);


    /// [API] 주어진 인덱스의 모션 단계를 삭제한다.
    bool erase(std::size_t index);


    /// [API] 모든 앞 단계의 유한 종료 시각을 구한 뒤 새 단계의 시작을 그 뒤로 이동해 순차 실행을 만든다.
    bool then(MotionStep step); // Requires all previous steps to have finite durations.


    /// [API] 등록된 모션 단계 배열을 읽기 전용 참조로 반환한다.
    const std::vector<MotionStep>& getSteps() const {
        return steps_;
    }


    /// [API] 절대 시간(초)에서 활성 단계의 월드/로컬 행렬을 합성한다. 의미 동작은 실행하지 않는다.
    std::optional<MotionSample> sample(double seconds) const;


    /// [API] 변환을 평가하며 동작을 순서대로 실행한다. 실패한 외부 부수 효과는 복구하지 않는다.
    std::optional<MotionSample> execute(double seconds) const;


    /// [API] 기존 append/insert/then/replace와 조립하는 비변환 동작 단계를 만든다.
    static MotionStep action(MotionAction action, double startSeconds = 0,
                             std::optional<double> durationSeconds = std::nullopt);


    /// [API] 고정 행렬을 반환하는 모션 단계를 만든다. 합성 공간을 함께 지정한다.
    static MotionStep fixed(const glm::mat4& matrix, TransformSpace space = TransformSpace::World);


    /// [API] 축·초당 각속도·초기 각도·합성 공간으로 회전 모션 단계를 만든다.
    static MotionStep rotation(const RotationMotion& config);


    /// [API] 경로·보간·재생 설정으로 시간에 따른 위치 이동 모션 단계를 만든다.
    static MotionStep followPath(const PathMotion& config);


    /// [API] 시작·끝 위치를 지정 시간 동안 선형보간한다. Once·Loop·PingPong을 지원한다.
    static MotionStep linear(const LinearMotion& config);


    /// [API] 지정 축에서 offset + amplitude*sin(위상)의 도 단위 회전을 반복한다. 물리 적분은 아니다.
    static MotionStep pendulum(const PendulumMotion& config);
};

struct MotionState {
    Motion motion;
    double timeSeconds = 0;
    bool playing = true;
    bool enabled = true;
    MotionSample sampled;
};

struct LayerConfig {
    glm::mat4 transform{1.0f};
    TransformSpace space = TransformSpace::World;
    std::optional<MotionId> motion;
    bool enabled = true;
};

struct LayerState {
    LayerConfig config;
    std::optional<LayerId> parent;
    std::vector<LayerId> children;
};

class MotionSystem {
    std::map<MotionId, MotionState> motions_;
    MotionId nextMotion_ = 1;
    std::map<LayerId, LayerState> layers_{{0, LayerState{}}};
    LayerId nextLayer_ = 1;


    bool validLayer(const LayerConfig& config) const;


public:
    /// [API] 삭제할 수 없는 루트 레이어의 ID 0을 반환한다.
    static constexpr LayerId rootLayer() {
        return 0;
    }


    /// [API] 모션을 시스템에 등록하고 새 MotionId를 반환한다.
    std::optional<MotionId> createMotion(const Motion& motion);


    /// [API] 의미 단계 목록을 검사·조립하여 하나의 모션으로 등록한다.
    std::optional<MotionId> createMotion(std::initializer_list<MotionStep> steps);


    /// [API] 기존 MotionId의 모션 데이터를 교체하고 현재 시각의 샘플을 갱신한다.
    bool setMotion(MotionId id, const Motion& motion);


    /// [API] 모션 상태의 읽기 전용 비소유 포인터를 반환한다. 없는 ID면 nullptr이다.
    const MotionState* getMotion(MotionId id) const;


    /// [API] 모션의 시간 진행을 정지하거나 재개한다. 모션 기여 자체의 사용 여부와 구분한다.
    bool setMotionPlaying(MotionId id, bool playing);


    /// [API] 모션 샘플의 기여 여부를 설정한다. playing과 별도 상태다.
    bool setMotionEnabled(MotionId id, bool enabled);


    /// [API] 지정 시각을 샘플링하고 유효할 때 모션의 시간과 샘플을 함께 바꾼다.
    bool seekMotion(MotionId id, double seconds);


    /// [API] 모션을 제거하고 연결된 레이어의 해당 모션 연결을 해제한다.
    bool removeMotion(MotionId id);


    /// [API] 모든 샘플 성공 후 시간·샘플을 확정하고 활성 모션의 동작을 실행한다. 동작 실패는 외부 상태를 복구하지 않는다.
    bool update(double deltaSeconds);


    /// [API] 모션과 레이어를 비우고 루트 레이어를 복구한다. 다음 ID 카운터는 되돌리지 않는다.
    void clear(); // Identifiers are never reused by this system, including after clear.


    /// [API] 유효한 부모 아래 새 자식 레이어를 추가한다. 복사한 임시 맵에 반영한 뒤 swap하여 상태를 확정한다.
    std::optional<LayerId> pushLayer(LayerId parent, const LayerConfig& config = LayerConfig{});


    /// [API] 기존 child와 그 parent 사이에 새 레이어를 끼운다. root 위 삽입은 허용하지 않는다.
    std::optional<LayerId> insertLayer(LayerId child, const LayerConfig& config = LayerConfig{});


    /// [API] 자식이 없는 비root leaf만 제거하고 그 부모 ID를 반환한다.
    std::optional<LayerId> popLayer(LayerId top); // Only removes a leaf; returns its parent.


    /// [API] root 이외 레이어를 제거하며 자식들을 부모의 해당 위치로 승격한다. 월드 자세 보존을 자동 약속하지 않는다.
    bool removeLayer(LayerId id); // Promotes children in place; root cannot be removed.


    /// [API] 레이어의 부모를 바꾸며 자신의 자손으로 연결되는 순환을 거부한다.
    bool reparentLayer(LayerId id, LayerId parent);


    /// [API] 기존 레이어의 고정 변환·합성 공간·모션 연결·활성 상태를 바꾼다.
    bool setLayer(LayerId id, const LayerConfig& config);


    /// [API] 레이어 상태의 읽기 전용 비소유 포인터를 반환한다. 없는 ID면 nullptr이다.
    const LayerState* getLayer(LayerId id) const;


    /// [API] 기존 레이어에 모션 ID를 연결하거나 해제한다. 모션과 레이어가 존재하는지 검사한다.
    bool bindMotion(LayerId layer, std::optional<MotionId> motion);


    /// [API] leaf까지의 계층을 따라 transform과 motion 샘플을 World/Local 공간에 맞게 parent/local 행렬로 합성한다.
    std::optional<MotionSample> layerMatrices(LayerId leaf) const;


    /// [API] 로컬 공간 회전 모션을 등록하여 자전에 사용한다.
    std::optional<MotionId> createSpin(RotationMotion config);


    /// [API] 월드 공간 회전 모션을 등록하여 축 주위 공전에 사용한다.
    std::optional<MotionId> createOrbit(RotationMotion config);

};
} // namespace graphics
