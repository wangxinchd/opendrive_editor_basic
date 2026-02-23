import numpy as np
import matplotlib.pyplot as plt
import math


# =========================================================
# 1. Hermite world cubic
# =========================================================
def generate_world_cubic(P0, P1, yaw0, yaw1):
    x0, y0 = P0
    x1, y1 = P1

    L = math.hypot(x1 - x0, y1 - y0)

    T0 = np.array([math.cos(yaw0), math.sin(yaw0)]) * L
    T1 = np.array([math.cos(yaw1), math.sin(yaw1)]) * L

    d = np.array([x0, y0])
    c = T0
    b = -3*np.array([x0, y0]) - 2*T0 + 3*np.array([x1, y1]) - T1
    a = 2*np.array([x0, y0]) + T0 - 2*np.array([x1, y1]) + T1

    return a, b, c, d


# =========================================================
# 2. 弧长重参数化
# =========================================================
def reparameterize_arc_length(a, b, c, d, n=2000):
    ts = np.linspace(0, 1, n)

    dx = 3*a[0]*ts**2 + 2*b[0]*ts + c[0]
    dy = 3*a[1]*ts**2 + 2*b[1]*ts + c[1]

    speed = np.sqrt(dx**2 + dy**2)

    s_vals = np.zeros_like(ts)
    s_vals[1:] = np.cumsum(0.5*(speed[:-1] + speed[1:]) * np.diff(ts))

    total_length = s_vals[-1]

    return ts, s_vals, total_length


# =========================================================
# 3. 世界 → 局部
# =========================================================
def world_to_local(x, y, P0, hdg):
    x0, y0 = P0
    cos_h = math.cos(hdg)
    sin_h = math.sin(hdg)

    dx = x - x0
    dy = y - y0

    U =  cos_h*dx + sin_h*dy
    V = -sin_h*dx + cos_h*dy

    return U, V


# =========================================================
# 4. 拟合三次多项式 U(s), V(s)
# =========================================================
def fit_cubic(s, values):
    return np.polyfit(s, values, 3)


# =========================================================
# 5. ===== 输入 =====
# =========================================================
x0, y0, yaw0 = 100.0, 0.0, 0.0
x1, y1, yaw1 = 320.0, 3.7, 0.0

P0 = (x0, y0)
P1 = (x1, y1)

# =========================================================
# 6. 生成 world 曲线
# =========================================================
a, b, c, d = generate_world_cubic(P0, P1, yaw0, yaw1)

# =========================================================
# 7. arcLength 参数化
# =========================================================
ts, s_vals, total_length = reparameterize_arc_length(a, b, c, d)

# 计算 world 点
xs = a[0]*ts**3 + b[0]*ts**2 + c[0]*ts + d[0]
ys = a[1]*ts**3 + b[1]*ts**2 + c[1]*ts + d[1]

# =========================================================
# 8. 转为 OpenDRIVE 局部
# =========================================================
hdg = yaw0

U_vals, V_vals = world_to_local(xs, ys, P0, hdg)

# =========================================================
# 9. 拟合 arcLength 版本 paramPoly3
# =========================================================
coef_U = fit_cubic(s_vals, U_vals)
coef_V = fit_cubic(s_vals, V_vals)

# polyfit 返回 [a,b,c,d]
aU, bU, cU, dU = coef_U
aV, bV, cV, dV = coef_V

# =========================================================
# 10. 输出 OpenDRIVE
# =========================================================
print("\n===== OpenDRIVE planView =====")
print('<planView>')
print(f'    <geometry s="0.0" '
      f'x="{x0:.6f}" '
      f'y="{y0:.6f}" '
      f'hdg="{hdg:.12e}" '
      f'length="{total_length:.12e}">')

print(f'        <paramPoly3 '
      f'aU="{dU:.12e}" '
      f'bU="{cU:.12e}" '
      f'cU="{bU:.12e}" '
      f'dU="{aU:.12e}" '
      f'aV="{dV:.12e}" '
      f'bV="{cV:.12e}" '
      f'cV="{bV:.12e}" '
      f'dV="{aV:.12e}" '
      f'pRange="arcLength"/>')

print('    </geometry>')
print('</planView>')


# =========================================================
# 11. 可视化验证
# =========================================================
plt.figure(figsize=(7,5))
plt.plot(xs, ys, label="world cubic")
plt.scatter([x0, x1], [y0, y1])
plt.axis("equal")
plt.grid(True)
plt.legend()
plt.show()
