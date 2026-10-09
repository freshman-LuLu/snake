import PySpice.Logging.Logging as Logging
logger = Logging.setup_logging()

from PySpice.Spice.Netlist import Circuit
from PySpice.Unit import *

# ==========================================
# 仿真戴维南等效电路 (12V, 2Ω内阻, 2Ω负载)
# ==========================================
print("--- 仿真等效电路 ---")
circuit = Circuit('Thevenin Equivalent Circuit')

# 节点定义:
# 'n_pos': 电压源正极与等效内阻的连接点
# 'n_load': 等效内阻与负载的连接点 (即输出端)
# circuit.gnd: 接地端

# 1. 电压源 (12V)
circuit.V('th', 'n_pos', circuit.gnd, 12@u_V)

# 2. 戴维南等效电阻 (2Ω)
circuit.R('th', 'n_pos', 'n_load', 2@u_Ohm)

# 3. 负载电阻 (2Ω)
circuit.R('load', 'n_load', circuit.gnd, 2@u_Ohm)

# 执行直流工作点分析
simulator = circuit.simulator()
analysis = simulator.operating_point()

# 提取结果
v_load = float(analysis['n_load'][0])
i_load = v_load / 2.0  # 用欧姆定律计算流过负载的电流

print(f"等效电路 - 负载两端电压: {v_load:.4f} V")
print(f"等效电路 - 流过负载的电流: {i_load:.4f} A")



