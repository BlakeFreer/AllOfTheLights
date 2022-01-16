# Author: Blake Freer
# Date: January 15, 2022
# Convert RGB, fade, repeat and duration into a 32 bit strong for the Arduino

def clamp(x, lower, upper):
    # restrict a number to be within a range
    return max(lower, min(x, upper))

def createPacket(red, green, blue, fade, repeat, duration):
    # r,g,b are 0-255
    # fade, repeat are 0 (off) or 1 (on)
    # duration is in milliseconds

    # check input parameters
    red = clamp(red, 0, 255)        # r,g,b in [0,255]
    green = clamp(green, 0, 255)
    blue = clamp(blue, 0, 255)
    fade = int(fade >= 1)           # fade, repeat = 0 or 1
    repeat = int(fade >= 1)
    duration = max(duration, 10)    # duration >= 10

    # create a 32 bit binary packet
    byte1 = (red >> 1) << 1 + fade
    byte2 = (green >> 1) << 1 + repeat
    byte3 = (blue >> 1) << 1

    # convert duration into custom floating point int
    exp = 0
    base = duration // 10 # arduino works in 10ms intervals

    while base > 32:
        exp += 1
        base = base // 2
    if exp > 15:
        exp = 15
        base = 32
    base -= 1
    byte3 += base >> 4
    byte4 = ((base & 0b1111) << 4) + (exp & 0b1111)

    print(str(duration // 10)+ " ~ (" + str(base) + "+1)*2^" + str(exp) + " = " + str((base+1) * 2 ** exp))
    # print("Base = {:08b}+1 = {}".format(base, base+1))
    # print("Exp = {:08b} = {}".format(exp, exp))
    # print("Duration binary: {:05b} {:04b}".format(base, exp))
    print("{{0b{:08b},0b{:08b},0b{:08b},0b{:08b}}}".format(byte1, byte2, byte3, byte4))

    return (byte1, byte2, byte3, byte4)



if __name__ == "__main__":
    # for testing only, won't be imported
    while True:
        r = int(input("R: "))
        g = int(input("G: "))
        b = int(input("B: "))
        dur = int(input("Enter duration in ms: "))
        createPacket(r,g,b,0,0,dur)