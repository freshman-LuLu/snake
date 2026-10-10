import numpy as np
import matplotlib.pyplot as plt
import PySpice.Logging.Logging as Logging
logger = Logging.setup_logging()

from PySpice.Spice.Netlist import Circuit
from PySpice.Unit import *

# ================= 解决中文乱码 =================
plt.rcParams['font.sans-serif'] = ['SimHei']  # Windows 用黑体
plt.rcParams['axes.unicode_minus'] = False    # 解决负号显示问题

# ================= 元件参数 =================
R_val = 1e3      # 1 kΩ
C_val = 100e-9   # 100 nF
fc = 1 / (2 * np.pi * R_val * C_val)
tau = R_val * C_val

print(f"理论计算: 时间常数 τ = {tau:.2e} s, 截止频率 fc = {fc:.2f} Hz")

# ================= 1. AC 扫描分析 (幅频/相频) =================
print("正在进行 AC 扫描分析...")
circuit = Circuit('RC Low Pass Filter')
# 注意：将节点名从 'in' 改为 'vin'
circuit.V('input', 'vin', circuit.gnd, 'DC 0V AC 1V')
circuit.R(1, 'vin', 'out', R_val @ u_Ω)
circuit.C(1, 'out', circuit.gnd, C_val @ u_F)

simulator = circuit.simulator(temperature=25, nominal_temperature=25)
ac = simulator.ac(
    start_frequency=1 @ u_Hz,
    stop_frequency=1 @ u_MHz,
    number_of_points=100,
    variation='dec'
)

freq = np.array(ac.frequency)
gain_sim = np.abs(np.array(ac['out'])) / np.abs(np.array(ac['vin']))
phase_sim = np.angle(np.array(ac['out']), deg=True)

# 理论值计算
gain_theory = 1 / np.sqrt(1 + (freq / fc) ** 2)
phase_theory = -np.arctan(freq / fc) * 180 / np.pi

# 画图：幅频特性
plt.figure(figsize=(10, 4))
plt.subplot(1, 2, 1)
plt.semilogx(freq, 20*np.log10(gain_sim), label='仿真值')
plt.semilogx(freq, 20*np.log10(gain_theory), '--', label='理论值')
plt.axvline(fc, color='r', linestyle=':', label=f'fc={fc:.1f} Hz')
plt.xlabel('频率 / Hz')
plt.ylabel('增益 / dB')
plt.title('幅频特性')
plt.grid(True, which='both')
plt.legend()

# 画图：相频特性
plt.subplot(1, 2, 2)
plt.semilogx(freq, phase_sim, label='仿真值')
plt.semilogx(freq, phase_theory, '--', label='理论值')
plt.axvline(fc, color='r', linestyle=':', label=f'fc={fc:.1f} Hz')
plt.xlabel('频率 / Hz')
plt.ylabel('相位 / °')
plt.title('相频特性')
plt.grid(True, which='both')
plt.legend()

plt.tight_layout()
plt.savefig('rc_ac.png', dpi=300)
plt.show()
plt.close()

# ================= 2. 瞬态分析 (看波形) =================
print("正在进行瞬态分析...")
circuit2 = Circuit('RC Low Pass Filter Transient')
# 注意：将节点名从 'in' 改为 'vin'
circuit2.SinusoidalVoltageSource(
    'input', 'vin', circuit2.gnd,
    amplitude=1 @ u_V,
    frequency=1 @ u_kHz  # 1kHz 输入
)
circuit2.R(1, 'vin', 'out', R_val @ u_Ω)
circuit2.C(1, 'out', circuit2.gnd, C_val @ u_F)

sim2 = circuit2.simulator(temperature=25, nominal_temperature=25)
tr = sim2.transient(step_time=1 @ u_us, end_time=5 @ u_ms)

t = np.array(tr.time)
vin = np.array(tr['vin'])
vout = np.array(tr['out'])

plt.figure(figsize=(10, 4))
plt.plot(t * 1000, vin, label='输入 Vi (1kHz)')
plt.plot(t * 1000, vout, label='输出 Vo')
plt.xlabel('时间 / ms')
plt.ylabel('电压 / V')
plt.title('瞬态波形 (1kHz 正弦波)')
plt.grid(True)
plt.legend()
plt.savefig('rc_transient.png', dpi=300)
plt.show()
plt.close()

print("仿真结束！图片已保存为 rc_ac.png 和 rc_transient.png")