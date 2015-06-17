
bl_info = {
		"name":         "Scene mech exporter",
		"author":       "DezerteR",
		"blender":      (2,6,7),
		"version":      (0,0,1),
		"location":     "File > Export > Scene mech exporter",
		"description":  "Export Scene mech",
		"category":     "Import-Export"
}

import bpy
import mathutils
import string
import os
from bpy_extras.io_utils import ExportHelper

def tstr (number):
	# return str(round(number, 6))
	return str("%.4f" %number)

def vecToString(vec):
	str = '['+tstr(vec.x)+', '+tstr(vec.y)+', '+tstr(vec.z)+']'
	return str

def getVectTo(a, b):
	Pa = a.matrix_world*mathutils.Vector((0.0, 0.0, 0.0, 1.0))
	Pb = b.matrix_world*mathutils.Vector((0.0, 0.0, 0.0, 1.0))
	return Pb - Pa

def writePosition(file, object, offset):
	position = object.matrix_world*mathutils.Vector((0.0, 0.0, 0.0, 1.0))
	file.write(offset + '  Position: ' + vecToString(position))
def writeQuaternion(file, object, offset):
	quat = object.rotation_euler.to_quaternion()
	str = '['+tstr(quat[0])+', '+tstr(quat[1])+', '+tstr(quat[2])+', '+tstr(quat[3])+']'
	file.write(offset + '  Quaternion: ' + str)
def writeMaterial(file, object, offset):
	# write color
	mat = object.active_material
	if mat:
		color = mat.diffuse_color
		str = '['+tstr(color[0])+', '+tstr(color[1])+', '+tstr(color[2])+']'
		file.write(offset + '  Color: ' + str)
	else:
		file.write(offset + '  Color: [0.1473, 0.1473, 0.1473]')
def writeLampColor(file, object, offset):
	color = object.color
	str = '['+tstr(color[0])+', '+tstr(color[1])+', '+tstr(color[2])+']'
	file.write(offset + '  Color: ' + str)

def writeScene(file, objects, offset, dirname):
	file.write(offset + 'Meshes:')
	offset = offset + ' '
	for mesh in objects:
		if mesh.type == 'MESH':
			file.write(offset + '- Name: ' + mesh.name)

			writePosition(file, mesh, offset)
			writeQuaternion(file, mesh, offset)
			writeMaterial(file, mesh, offset)
			writeProperties(file, mesh, offset)
			bpy.ops.object.select_pattern(pattern = mesh.name, extend = False)
			bpy.ops.export_scene.obj(check_existing=True, filepath=dirname+"/"+mesh.name+'.obj', axis_forward='X', axis_up='Z', filter_glob="*.obj;*.mtl", use_selection=True, use_animation=False, use_mesh_modifiers=True, use_edges=True, use_smooth_groups=False, use_smooth_groups_bitflags=False, use_normals=True, use_uvs=True, use_materials=False, use_triangles=True, use_nurbs=False, use_vertex_groups=False, use_blen_objects=True, group_by_object=False, group_by_material=False, keep_vertex_order=False, global_scale=1.0, path_mode='AUTO')

def writeLamps(file, objects, offset):
	file.write(offset + 'Lamps:')
	offset = offset + ' '
	for lamp in objects:
		file.write(offset + '- Name: ' + lamp.name)
		object = bpy.data.objects[lamp.name]

		writePosition(file, object, offset)
		writeQuaternion(file, object, offset)
		writeLampColor(file, object, offset)

		file.write(offset + '  Falloff_distance: ' + tstr(lamp.distance))
		file.write(offset + '  Energy: ' + tstr(lamp.energy))
		file.write(offset + '  Type: ' + lamp.type)

		if lamp.type == 'POINT':
			file.write(offset + '  falloff_type: ' + lamp.falloff_type)

		if lamp.type == 'SPOT':
			file.write(offset + '  falloff_type: ' + lamp.falloff_type)
			file.write(offset + '  spot_size: ' + tstr(lamp.spot_size))

		if lamp.type == 'AREA':
			file.write(offset + '  Size: ' + tstr(lamp.size))

def writeChain(file, mesh, offset):
	o = offset;
	file.write('\n - Name: '+mesh.name)

	parent = mesh.parent
	axis = parent.matrix_local*mathutils.Vector((0.0, 0.0, 1.0, 0.0))
	axis.normalize()
	file.write(o + '\n   ParentJoint: '+parent.name)
	file.write(o + '\n     Name: '+parent.name)
	file.write(o + '\n     Vec: '+vecToString(getVectTo(mesh, parent)))
	file.write(o + '\n     Axis: '+vecToString(axis))
	file.write(o + '\n     Min: '+parent['Min'])
	file.write(o + '\n     Max: '+parent['Max'])
	child = mesh.children[0]
	file.write(o + '\n   ChildJoint: '+child.name)
	file.write(o + '\n     Vec: '+vecToString(getVectTo(child, mesh)))
	if len(child.children) > 0:
		writeChain(file, child.children[0], o)

def writeMech(file, obj):
	file.write('\n - Name: '+obj.name)
	for child in obj.children:
		if child['Type'] and child['Type'] == 'hinge':
			file.write('\n   Chain:')
			writeChain(file, child.children[0], '  ')
		else :
			writeMech(file, child)



#---------------------
class ExportMyFormat(bpy.types.Operator, ExportHelper):
	bl_idname       = "scene.yml";
	bl_label        = "Scene mech exporter";
	bl_options      = {'PRESET'};

	filename_ext    = ".yml";

	def execute(self, context):
		file = open(self.filepath, "w", encoding='utf8')

		offset = '\n'
		file.write('dirname: '+os.path.dirname(self.filepath))
		file.write('\nMech:')
		writeMech(file, bpy.data.objects['Base'].children[0])
		writeScene(file, bpy.data.objects, offset, os.path.dirname(self.filepath))
		writeLamps(file, bpy.data.lamps, offset)

		file.close()
		return {'FINISHED'};

def menu_func(self, context):
	self.layout.operator(ExportMyFormat.bl_idname, text="Scene MECH(.yml)");
def register():
	bpy.utils.register_module(__name__);
	bpy.types.INFO_MT_file_export.append(menu_func);
def unregister():
	bpy.utils.unregister_module(__name__);
	bpy.types.INFO_MT_file_export.remove(menu_func);
if __name__ == "__main__":
	register()
