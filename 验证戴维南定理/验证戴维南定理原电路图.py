import PySpice.Logging.Logging as Logging
logger = Logging.setup_logging()

from PySpice.Spice.Netlist import Circuit
from PySpice.Unit import *

# ==========================================
# 1. 仿真原始电路 (参数: 12V/6Ω, 6V/3Ω, 负载2Ω)
# ==========================================
print("--- 1. 仿真原始电路 ---")
circuit_orig = Circuit('Original Circuit')

# 定义节点: n_top1, n_top2 分别为上下支路电源正极; n_mid 为公共输出节点
circuit_orig.V('1', 'n_top1', circuit_orig.gnd, 18@u_V)
circuit_orig.R('1', 'n_top1', 'n_mid', 6@u_Ohm)

circuit_orig.V('2', 'n_top2', circuit_orig.gnd, 9@u_V)
circuit_orig.R('2', 'n_top2', 'n_mid', 3@u_Ohm)

# 负载电阻 R = 2Ω
circuit_orig.R('load', 'n_mid', circuit_orig.gnd, 2@u_Ohm)

# 执行直流工作点分析
simulator_orig = circuit_orig.simulator()
analysis_orig = simulator_orig.operating_point()

# 提取节点电压
v_load_orig = float(analysis_orig['n_mid'][0])

# 根据欧姆定律计算电流：I = U / R (电阻为2Ω)
i_load_orig = v_load_orig / 2.0

print(f"原始电路 - 负载两端电压: {v_load_orig:.4f} V")
print(f"原始电路 - 流过负载的电流: {i_load_orig:.4f} A")


# ==========================================
# 2. 仿真戴维南等效电路 (Vth=12V, Rth=2Ω)
# ==========================================
print("\n--- 2. 仿真戴维南等效电路 ---")
V_th =12.0
R_th = 2.0

circuit_thev = Circuit('Thevenin Equivalent')

# 戴维南等效电压源
circuit_thev.V('th', 'n_th_pos', circuit_thev.gnd, V_th@u_V)
# 戴维南等效电阻
circuit_thev.R('th', 'n_th_pos', 'n_load', R_th@u_Ohm)
# 负载电阻 R = 2Ω
circuit_thev.R('load', 'n_load', circuit_thev.gnd, 2@u_Ohm)

simulator_thev = circuit_thev.simulator()
analysis_thev = simulator_thev.operating_point()

# 提取节点电压
v_load_thev = float(analysis_thev['n_load'][0])

# 根据欧姆定律计算电流
i_load_thev = v_load_thev / 2.0

print(f"戴维南电路 - 负载两端电压: {v_load_thev:.4f} V")
print(f"戴维南电路 - 流过负载的电流: {i_load_thev:.4f} A")

# 验证两者是否一致
if abs(i_load_orig - i_load_thev) < 1e-6 and abs(v_load_orig - v_load_thev) < 1e-6:
    print("\n✅ 验证成功：戴维南等效电路与原始电路结果完全一致！")
    print(f"   预期整数结果: 负载电压 = 6.0000 V, 负载电流 = 3.0000 A")
else:
    print("\n❌ 验证失败：结果不一致，请检查电路模型。")