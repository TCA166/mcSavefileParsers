from PIL import Image
import sys
import os

#a python script for generating mtl files

if __name__ == "__main__":
    s = False #simplified mode
    t = False #top correction
    c = False #color correction
    path = sys.argv[1]
    for el in sys.argv:
        if el == "-s":
            s = True
        if el == "-t":
            t = True
        if el == "-c":
            c = True
    with open("out.mtl", "w") as o:
        for filename in os.listdir(path):
            f = os.path.join(path, filename)
            #simplified functioning that doesn't require textures
            if os.path.isfile(f) and filename[-4:] == ".png" and s:
                rgb = [0, 0, 0, 0]
                img = Image.open(f).convert('RGBA')
                for x in range(img.size[0]):
                    for y in range(img.size[1]):
                        pixel = img.getpixel((x, y))
                        rgb[0] += pixel[0]
                        rgb[1] += pixel[1]
                        rgb[2] += pixel[2]
                        rgb[3] += pixel[3]
                amt = img.size[0] * img.size[1]
                rgb[0] /= (amt * 255)
                rgb[1] /= (amt * 255)
                rgb[2] /= (amt * 255)
                rgb[3] /= (amt * 255)
                if "grass" in filename or "leaves" in filename:
                    # #8eb971
                    rgb[0] = 142 / 255
                    rgb[1] = 185 / 255
                    rgb[2] = 113 / 255
                if "top" in filename and t:
                    filename = filename.replace("_top", "", 1)
                o.write("newmtl %s\n" % filename[:-4])
                o.write("Ka %.3f %.3f %.3f\n" % (rgb[0], rgb[1], rgb[2]))
                o.write("Kd %.3f %.3f %.3f\n" % (rgb[0], rgb[1], rgb[2]))
                o.write("Ks 0.000 0.000 0.000\n")
                o.write("d %.1f\n" % rgb[3])
                o.write("illum 1\n")


            
