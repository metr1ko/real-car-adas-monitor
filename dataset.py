import csv
import random
import os

# Функция для добавления небольшого "шума" (чтобы стрелки чуть-чуть дрожали, как в реальной жизни)
def noise(val, amount):
    return val + random.uniform(-amount, amount)

rows = []
speed = 0.0
rpm = 800.0
throttle = 0.0
coolant = 85.0
fuel = 50.0

print("Generating realistic driving data...")

for i in range(600):
    t = i / 10.0 # Время в секундах (1 шаг = 100 мс)

    if t < 5.0:
        # Стоим
        target_speed, target_throttle, target_rpm = 0, 0, 800
        label = "SLOW"
    elif t < 20.0:
        # Плавный разгон до 60
        target_speed = 60 * ((t - 5) / 15.0)
        target_throttle = 35
        gear_ratio = 1.0 if target_speed < 20 else (0.6 if target_speed < 40 else 0.4)
        target_rpm = 1200 + target_speed * 60 * gear_ratio
        label = "NORMAL"
    elif t < 35.0:
        # Круиз 60 км/ч
        target_speed, target_throttle = 60, 15
        target_rpm = 2200
        label = "NORMAL"
    elif t < 45.0:
        # Агрессивный разгон (педаль в пол)
        target_speed = 60 + 60 * ((t - 35) / 10.0)
        target_throttle = 100
        # Обороты сильно выше, уверенно заходят в красную зону (от 3500 до 5900)
        target_rpm = 3500 + (target_speed - 60) * 40
        label = "AGGRESSIVE"
    elif t < 55.0:
        # Торможение
        target_speed = 120 * (1.0 - (t - 45) / 10.0)
        target_throttle = 0
        target_rpm = max(800, target_speed * 20)
        label = "NORMAL"
    else:
        # Стоим
        target_speed, target_throttle, target_rpm = 0, 0, 800
        label = "SLOW"

    # Плавное приближение текущих значений к целевым (эффект инерции машины)
    speed += (target_speed - speed) * 0.2
    rpm += (target_rpm - rpm) * 0.3
    throttle += (target_throttle - throttle) * 0.3

    # Медленные изменения
    if coolant < 95: coolant += 0.01 
    fuel -= 0.001
    intake = noise(35, 1.0)

    # Записываем строку с добавлением микро-шума и меткой (label)
    rows.append([
        round(max(0, noise(speed, 0.5)), 2),
        round(max(800, noise(rpm, 20)), 2),
        round(max(0, min(100, noise(throttle, 1.0))), 2),
        round(coolant, 2),
        round(fuel, 2),
        round(intake, 2),
        label
    ])

# Определяем абсолютный путь к папке data рядом со скриптом
script_dir = os.path.dirname(os.path.abspath(__file__))
data_dir = os.path.join(script_dir, 'data')
os.makedirs(data_dir, exist_ok=True)

file_path = os.path.join(data_dir, 'obd_data.csv')

# Сохраняем поверх старого файла
with open(file_path, 'w', newline='') as f:
    writer = csv.writer(f)
    writer.writerow(['speed_kmh', 'engine_rpm', 'throttle_pos', 'coolant_temp', 'fuel_level', 'intake_air_temp', 'label'])
    writer.writerows(rows)

print(f"File {file_path} generated successfully!")