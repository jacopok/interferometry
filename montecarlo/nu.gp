#!/gnuplot

p(x) = (gamma*(abs(x) - N0) + n - 1)
f(x) = t0 + sgn(x)*acos((n*n - 1 - p(x)*p(x))/(2*p(x)))
GNUTERM = "wxt"
gamma = 2.67e-05
t0 = 0.005
N0 = 0.1
n = 1.336
