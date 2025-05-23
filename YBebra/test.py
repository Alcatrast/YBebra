import socket
import math
import time
import random
import numpy as np

# Настройки UDP
UDP_IP = "127.0.0.1"
UDP_PORT = 8151
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

# Параметры параболы
DURATION = 10.0  # Длительность в секундах
INTERVAL = 0.05  # Интервал отправки (сек)
FREQ = 1.0  # Частота основной параболы (Гц)


# Случайные параметры для вариаций
def random_variation(base, variation=0.1):
    return base * (1 + variation * (random.random() - 0.5))


# Параболическая траектория с вариациями
def parabolic_trajectory(t):
    # Основные параметры параболы (случайные вариации ±10%)
    a = random_variation(-2.0)
    b = random_variation(15.0)
    c = random_variation(0.0)

    # Высота по параболе
    z = a * t ** 2 + b * t + c

    # Горизонтальное движение с небольшими случайными отклонениями
    x = random_variation(10.0) * t
    y = random_variation(0.5) * math.sin(t * random_variation(2.0))

    # Углы ориентации с небольшими колебаниями
    pitch = random_variation(0.5) * math.sin(t * random_variation(1.0))
    roll = random_variation(0.3) * math.sin(t * random_variation(1.5))
    yaw = random_variation(0.2) * math.sin(t * random_variation(0.8))

    return x, y, z, pitch, roll, yaw


# Основной цикл
start_time = time.time()
t = 0.0

print("Starting parabolic trajectory simulation...")

while t < DURATION:
    # Вычисляем позицию и углы
    x, y, z, pitch, roll, yaw = parabolic_trajectory(t)

    # Формируем сообщение
    message = f"{x:.3f} {y:.3f} {z:.3f} {pitch:.3f} {roll:.3f} {yaw:.3f}"

    # Отправляем по UDP
    sock.sendto(message.encode('utf-8'), (UDP_IP, UDP_PORT))
    print(f"t = {t:.2f}s: {message}")

    # Ожидаем следующий интервал
    t += INTERVAL
    time.sleep(max(0, start_time + t - time.time()))

print("Simulation completed.")