class Sensor:

    def __init__(self, id, name, location):
        self.id = id
        self.name = name
        self.location = location
        self.code = location + '.' + id

    def full_description(self):
        return '{}.{} - {}'.format(self.location, self.id, self.name)


sensor_1 = Sensor('1', 'Ard1', 'AL1')
sensor_2 = Sensor('2', 'Ras1', 'AL1')

print(sensor_1.code)
print(sensor_1.full_description())
