import csv
import random
import os


def generate_data(filename, num_records=5000):
    os.makedirs(os.path.dirname(filename), exist_ok=True)

    with open(filename, mode='w', newline='') as file:
        writer = csv.writer(file)
        writer.writerow(['speed_kmh', 'engine_rpm', 'throttle_pos',
                        'coolant_temp', 'fuel_level', 'intake_air_temp', 'label'])

        for _ in range(num_records):
            label = random.choices(
                ['SLOW', 'NORMAL', 'AGGRESSIVE'], weights=[0.2, 0.6, 0.2])[0]

            if label == 'SLOW':
                speed = round(random.uniform(0, 40), 1)
                rpm = round(random.uniform(800, 2000), 1)
                throttle = round(random.uniform(0, 20), 1)
            elif label == 'NORMAL':
                speed = round(random.uniform(40, 90), 1)
                rpm = round(random.uniform(1500, 3000), 1)
                throttle = round(random.uniform(15, 40), 1)
            else:  # AGGRESSIVE
                speed = round(random.uniform(80, 160), 1)
                rpm = round(random.uniform(3000, 6000), 1)
                throttle = round(random.uniform(50, 100), 1)

            coolant = round(random.uniform(70, 105), 1)
            fuel = round(random.uniform(5, 100), 1)
            intake = round(random.uniform(10, 50), 1)

            writer.writerow(
                [speed, rpm, throttle, coolant, fuel, intake, label])


if __name__ == '__main__':
    generate_data('data/obd_data.csv')
    print("Файл успешно сгенерирован")
