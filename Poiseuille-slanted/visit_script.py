OpenDatabase("localhost:Z:\\home\\michael\\simulation\\ibamr-run\\BScompisite\\iim-ex2-Hagen-Poiseuille\\viz_IB_4\\dumps.visit", 0)
AddPlot("Pseudocolor", "U_magnitude", 1, 1)
DrawPlots()
SetActiveWindow(2)
SetActivePlots(0)
LineoutAtts = LineoutAttributes()
LineoutAtts.point1 = (2.5, 0, 0)
LineoutAtts.point2 = (2.5, 5, 0)
LineoutAtts.interactive = 0
LineoutAtts.ignoreGlobal = 0
LineoutAtts.samplingOn = 0
LineoutAtts.numberOfSamplePoints = 100
LineoutAtts.reflineLabels = 0
SetOperatorOptions(LineoutAtts, 0, 1)
SaveWindowAtts = SaveWindowAttributes()
SaveWindowAtts.outputToCurrentDirectory = 0
SaveWindowAtts.outputDirectory = "Z:\home\michael\simulation\ibamr-run\BScompisite\iim-ex2-Hagen-Poiseuille"
SaveWindowAtts.fileName = "IB_4.U"
SaveWindowAtts.family = 1
SaveWindowAtts.format = SaveWindowAtts.CURVE  # BMP, CURVE, JPEG, OBJ, PNG, POSTSCRIPT, POVRAY, PPM, RGB, STL, TIFF, ULTRA, VTK, PLY, EXR
SaveWindowAtts.width = 1024
SaveWindowAtts.height = 1024
SaveWindowAtts.screenCapture = 0
SaveWindowAtts.saveTiled = 0
SaveWindowAtts.quality = 80
SaveWindowAtts.progressive = 0
SaveWindowAtts.binary = 0
SaveWindowAtts.stereo = 0
SaveWindowAtts.compression = SaveWindowAtts.NONE  # NONE, PackBits, Jpeg, Deflate, LZW
SaveWindowAtts.forceMerge = 0
SaveWindowAtts.resConstraint = SaveWindowAtts.ScreenProportions  # NoConstraint, EqualWidthHeight, ScreenProportions
SaveWindowAtts.pixelData = 1
SaveWindowAtts.advancedMultiWindowSave = 0
SaveWindowAtts.subWindowAtts.win1.position = (0, 0)
SaveWindowAtts.subWindowAtts.win1.size = (128, 128)
SaveWindowAtts.subWindowAtts.win1.layer = 0
SaveWindowAtts.subWindowAtts.win1.transparency = 0
SaveWindowAtts.subWindowAtts.win1.omitWindow = 0
SaveWindowAtts.subWindowAtts.win2.position = (0, 0)
SaveWindowAtts.subWindowAtts.win2.size = (128, 128)
SaveWindowAtts.subWindowAtts.win2.layer = 0
SaveWindowAtts.subWindowAtts.win2.transparency = 0
SaveWindowAtts.subWindowAtts.win2.omitWindow = 0
SaveWindowAtts.subWindowAtts.win3.position = (0, 0)
SaveWindowAtts.subWindowAtts.win3.size = (128, 128)
SaveWindowAtts.subWindowAtts.win3.layer = 0
SaveWindowAtts.subWindowAtts.win3.transparency = 0
SaveWindowAtts.subWindowAtts.win3.omitWindow = 0
SaveWindowAtts.subWindowAtts.win4.position = (0, 0)
SaveWindowAtts.subWindowAtts.win4.size = (128, 128)
SaveWindowAtts.subWindowAtts.win4.layer = 0
SaveWindowAtts.subWindowAtts.win4.transparency = 0
SaveWindowAtts.subWindowAtts.win4.omitWindow = 0
SaveWindowAtts.subWindowAtts.win5.position = (0, 0)
SaveWindowAtts.subWindowAtts.win5.size = (128, 128)
SaveWindowAtts.subWindowAtts.win5.layer = 0
SaveWindowAtts.subWindowAtts.win5.transparency = 0
SaveWindowAtts.subWindowAtts.win5.omitWindow = 0
SaveWindowAtts.subWindowAtts.win6.position = (0, 0)
SaveWindowAtts.subWindowAtts.win6.size = (128, 128)
SaveWindowAtts.subWindowAtts.win6.layer = 0
SaveWindowAtts.subWindowAtts.win6.transparency = 0
SaveWindowAtts.subWindowAtts.win6.omitWindow = 0
SaveWindowAtts.subWindowAtts.win7.position = (0, 0)
SaveWindowAtts.subWindowAtts.win7.size = (128, 128)
SaveWindowAtts.subWindowAtts.win7.layer = 0
SaveWindowAtts.subWindowAtts.win7.transparency = 0
SaveWindowAtts.subWindowAtts.win7.omitWindow = 0
SaveWindowAtts.subWindowAtts.win8.position = (0, 0)
SaveWindowAtts.subWindowAtts.win8.size = (128, 128)
SaveWindowAtts.subWindowAtts.win8.layer = 0
SaveWindowAtts.subWindowAtts.win8.transparency = 0
SaveWindowAtts.subWindowAtts.win8.omitWindow = 0
SaveWindowAtts.subWindowAtts.win9.position = (0, 0)
SaveWindowAtts.subWindowAtts.win9.size = (128, 128)
SaveWindowAtts.subWindowAtts.win9.layer = 0
SaveWindowAtts.subWindowAtts.win9.transparency = 0
SaveWindowAtts.subWindowAtts.win9.omitWindow = 0
SaveWindowAtts.subWindowAtts.win10.position = (0, 0)
SaveWindowAtts.subWindowAtts.win10.size = (128, 128)
SaveWindowAtts.subWindowAtts.win10.layer = 0
SaveWindowAtts.subWindowAtts.win10.transparency = 0
SaveWindowAtts.subWindowAtts.win10.omitWindow = 0
SaveWindowAtts.subWindowAtts.win11.position = (0, 0)
SaveWindowAtts.subWindowAtts.win11.size = (128, 128)
SaveWindowAtts.subWindowAtts.win11.layer = 0
SaveWindowAtts.subWindowAtts.win11.transparency = 0
SaveWindowAtts.subWindowAtts.win11.omitWindow = 0
SaveWindowAtts.subWindowAtts.win12.position = (0, 0)
SaveWindowAtts.subWindowAtts.win12.size = (128, 128)
SaveWindowAtts.subWindowAtts.win12.layer = 0
SaveWindowAtts.subWindowAtts.win12.transparency = 0
SaveWindowAtts.subWindowAtts.win12.omitWindow = 0
SaveWindowAtts.subWindowAtts.win13.position = (0, 0)
SaveWindowAtts.subWindowAtts.win13.size = (128, 128)
SaveWindowAtts.subWindowAtts.win13.layer = 0
SaveWindowAtts.subWindowAtts.win13.transparency = 0
SaveWindowAtts.subWindowAtts.win13.omitWindow = 0
SaveWindowAtts.subWindowAtts.win14.position = (0, 0)
SaveWindowAtts.subWindowAtts.win14.size = (128, 128)
SaveWindowAtts.subWindowAtts.win14.layer = 0
SaveWindowAtts.subWindowAtts.win14.transparency = 0
SaveWindowAtts.subWindowAtts.win14.omitWindow = 0
SaveWindowAtts.subWindowAtts.win15.position = (0, 0)
SaveWindowAtts.subWindowAtts.win15.size = (128, 128)
SaveWindowAtts.subWindowAtts.win15.layer = 0
SaveWindowAtts.subWindowAtts.win15.transparency = 0
SaveWindowAtts.subWindowAtts.win15.omitWindow = 0
SaveWindowAtts.subWindowAtts.win16.position = (0, 0)
SaveWindowAtts.subWindowAtts.win16.size = (128, 128)
SaveWindowAtts.subWindowAtts.win16.layer = 0
SaveWindowAtts.subWindowAtts.win16.transparency = 0
SaveWindowAtts.subWindowAtts.win16.omitWindow = 0
SaveWindowAtts.opts.types = (5)
SaveWindowAtts.opts.help = ""
SetSaveWindowAttributes(SaveWindowAtts)
SaveWindow()
