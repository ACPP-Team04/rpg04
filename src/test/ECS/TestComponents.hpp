#include "Abstract/ECS/Component/Component.hpp"

#include <vector>
template <int N, typename U>
struct TestComponent : public Component<TestComponent<N, U>> {
  public:
	U member;
};

using Intcomp1 = TestComponent<1, int>;
using Intcomp2 = TestComponent<2, int>;
using Intcomp3 = TestComponent<3, int>;
using Doublecomp4 = TestComponent<4, double>;
using Doublecomp5 = TestComponent<5, double>;
using Doublecomp6 = TestComponent<6, double>;
using Veccomp7 = TestComponent<7, std::vector<double>>;
using Veccomp8 = TestComponent<8, std::vector<double>>;
using Veccomp9 = TestComponent<9, std::vector<double>>;
