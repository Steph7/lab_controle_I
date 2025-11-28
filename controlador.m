% Arquivo do Matlab

clear all; clc;

G = tf(6.715e04,[1 680 2.2247e05]);

Gbar = c2d(G, 0.01, 'zoh')
numC = [1 -0.0928];
denC = [1 -1];
C = tf(numC, denC, 0.01)

sisotool(Gbar,C);
