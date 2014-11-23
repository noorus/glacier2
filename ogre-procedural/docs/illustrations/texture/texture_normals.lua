buffer = Procedural.TextureBuffer(128)
Procedural.Cell(buffer):setDensity(4):setRegularity(234):process()
Procedural.Normals(buffer):process()
tests:addTextureBuffer(buffer)
dotfile = tests:getDotFile("texture_21a", "Normals_1_Demo")
dotfile:set("Cell", "texture_cell_smooth", "Normals", "texture_normals")
