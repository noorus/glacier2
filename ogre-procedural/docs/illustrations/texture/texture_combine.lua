buffer = Procedural.TextureBuffer(128)
Procedural.Cloud(buffer):process()
Procedural.Combine(buffer):addImage(bufferGradient, Procedural.Combine_METHOD_ADD_CLAMP):process()
tests:addTextureBuffer(buffer)
dotfile = tests:getDotFile("texture_08", "Combine_Demo")
dotfile:set("Cloud", "texture_cloud", "Gradient", "texture_gradient", "Combine", "texture_combine")
